/*
 * Reader.cpp
 *
 *  Created on: 2012. 12. 4.
 *       Author: chaerim
 */

/*
 * This class reads input file and sets environment, payoff, and so on.
 * Even if correation device is not found in input file, it does not return error code.
 */
#include "Reader.h"


Reader::Reader() {
   numPlayers = 0;
}

Reader::Reader(const string &nameDataFile) {
   numPlayers = 0;
   fin.open(nameDataFile.c_str(), ifstream::in);
}

Reader::~Reader() {
   if (fin) {
      fin.close();
   }
}

/*
 * 機能: 一行読み込み改行コードを削除したものを返す
 * メモ: ウインドウズとユニックスで改行コードが違うため使用
 */
ifstream &Reader::readLine(ifstream &fin, string &s) {
  getline(fin,s);
  if (s[s.size()-1] == '\n') {
    s.erase(s.size()-1);
  } else if (s[s.size()-1] == '\r') {
    s.erase(s.size()-1);
  }
  return fin;
}


/*
 * 機能: player が何番目のプレイヤか調べる
 * 引数: player
 * 返り値: 何番目か
 */
int Reader::getIndexOfPlayers(const string &player) {
   return getIndex(players, player);
}

/*
 * 機能: タイトルを読み込む
 */
READSTATUS Reader::readTitle() {
   READSTATUS result;
   string buf;
   boost::regex expr("Title *: *(.+)");
   boost::smatch what;
   
   fin.seekg(0, ios_base::beg);
   result = ITEMNOTFOUND;
   while (!fin.eof()) {
      readLine(fin, buf);
      
      if (boost::regex_match(buf, what, expr)) {
         title = what[1];
         result = NOERROR;
         break;
      }
   }
   return result;
}

/*
 * 機能: 割引因子を読み込む
 */
READSTATUS Reader::readDiscountRate() {
   READSTATUS result;

   string buf;
   boost::regex expr("Discount Rate *: *(.+)");
   boost::smatch what;

   fin.seekg(0, ios_base::beg);
   result = ITEMNOTFOUND;
   while (!fin.eof()) {
      readLine(fin, buf);
      if (boost::regex_match(buf, what, expr)) {
         discountRate = numberToRational(what[1]);
         result = NOERROR;
         break;
      }
   }
   return result;
}

/*
 * 機能: tag で指定されるデータを読み込む
 * 引数: tag，読み込んだデータ data, 初めから読むかどうか startFirst
 */
READSTATUS Reader::readItem(const string &tag, vector<string> &data, bool startFirst) {
   READSTATUS result = ITEMNOTFOUND;

   string buf;
   boost::regex expr(tag + " *: *(.+)");
   boost::smatch what;

   if (startFirst) {
      fin.seekg(0, ios_base::beg);
      while (!fin.eof()) {
         readLine(fin, buf);
         if (boost::regex_match(buf, what, expr)) { // tag が見つかった
            data = splitAtSpace(what[1]); // スペースごとに分割して，dataに入れる
            result = NOERROR;
            break;
         }
      }
   }
   else {
      readLine(fin, buf);
      if (boost::regex_match(buf, what, expr)) {
         data = splitAtSpace(what[1]); // スペースごとに分割して，dataに入れる
         result = NOERROR;
      }
   }

   return result;
}

/*
 * 機能: プレイヤ名を読み込む
 */
READSTATUS Reader::readPlayers() {
   READSTATUS result = readItem("Players", players, true);
   if (result == NOERROR) {
      numPlayers = players.size();
   }
   return result;
}

/*
 * 機能: 変数を読み込む
 */
READSTATUS Reader::readVariables() {
   vector<string> vars;
   READSTATUS result = readItem("Variables", vars, true);

   if (result == NOERROR) {
      boost::regex expr("(.+)=(.+)");
      boost::smatch what;

      PreciseNumber value;
      for (vector<string>::size_type i = 0; i < vars.size(); i++) {
         if (boost::regex_match(vars[i], what, expr)) {
            value = numberToRational(what[2]);
            if (variables.find(what[1]) == variables.end()) {
               variables.insert(make_pair(what[1], value));
            }
         }
         else {
            result = SYNTAXERROR;
            break;
         }
      }
   }

   return result;
}

/*
 * 機能: FSA を読み込む
 * 引数: 読み込むプレイヤの名前 namePlayer, 読み込むオートマトン am
 */
READSTATUS Reader::readAutomaton(const string &namePlayer, Automaton &am) {
   READSTATUS result = NOERROR;

   if (!fin) {
      result = FILENOTFOUND;
   }
   else {
      string buf;

      fin.seekg(0, ios_base::beg);
      result = ITEMNOTFOUND;
      while (!fin.eof()) {
         readLine(fin, buf);
         if (buf == "Automaton " + namePlayer) {
            // States
            vector<string> nameStates;
            result = readItem("States", nameStates);
            if (result != NOERROR)
               return result;
            am.setNumberOfStates(nameStates.size());
            if (!am.setNameOfStates(nameStates)) {
               Message::alert("Number Of States");
               return ERROR;
            }
            // Actions
            vector<string> nameActions;
            result = readItem("Actions", nameActions);
            if (result != NOERROR)
               return result;
            am.setNumberOfActions(nameActions.size());
            if (!am.setNameOfActions(nameActions)) {
               Message::alert("Number Of Actions");
               return ERROR;
            }
            am.setNameOfActions(nameActions);

            // Signals
            vector<string> nameSignals;
            result = readItem("Signals", nameSignals);
            if (result != NOERROR)
               return result;
            am.setNumberOfSignals(nameSignals.size());
            if (!am.setNameOfSignals(nameSignals)) {
               Message::alert("Number Of Signals");
               return ERROR;
            }

            // Action choice
            string buf1, buf2, buf3;
            for (int i = 0; i < am.getNumberOfStates(); i++) {
               fin >> buf1 >> buf2;
               if (!am.setActionChoice(am.getIndexOfStates(buf1), am.getIndexOfActions(buf2))) {
				 Message::alert("set Action Choce : " + buf1 + " to " + buf2);
				 return ERROR;
			   }
            }

            // State transition
            for (int i = 0; i < am.getNumberOfStates() * am.getNumberOfSignals(); i++) {
               fin >> buf1 >> buf2 >> buf3;
               if (!am.setStateTransition(am.getIndexOfStates(buf1), am.getIndexOfSignals(buf2), am.getIndexOfStates(buf3))) {
				 Message::alert("set State Transition : " +
								::toString(am.getIndexOfSignals(buf2)) + "(" + buf2 + ")" + 
								" from " + ::toString(am.getIndexOfStates(buf1)) + "(" + buf1 + ")" +
								" to " + ::toString(am.getIndexOfStates(buf3)) + "(" + buf3 + ")");
				 return ERROR;
			   }
            }

            result = NOERROR;
            break;
         }
      }
   }

   return result;
}

/*
 * 機能: シグナル分布を読み込む
 * 引数: 読み込む環境クラス env, pls
 */
READSTATUS Reader::readSignalDistribution(Environment &env, const Players &pls) {
  READSTATUS result = NOERROR;

  string buf;
  fin.seekg(0, ios_base::beg);
  result = ITEMNOTFOUND;
  while (!fin.eof()) {
	readLine(fin, buf);
	if (buf == "Signal Distribution") {
	  for (int i = 0; i < env.getNumberOfActionProfiles(); i++) {
		readLine(fin, buf);
		boost::regex expr("(\\S+) *: *(.*)");
		boost::smatch what;

		if (boost::regex_match(buf, what, expr)) {
		  // どの行動に対するものか
		  vector<string> actionStr = split(what[1], ',');
		  string stbuf;
		  const int numPlayers = pls.getNumberOfPlayers();
		  vector<int> actions(numPlayers);
		  for (int pl = 0; pl < numPlayers; ++pl)
			actions[pl] = pls.getIndexOfActions(pl, actionStr[pl]);
		  const int action = pls.getAction(ActionProfile(actions));
		  // 確率をセット
		  vector<string>::size_type sg = 0;
		  // 最初の行
		  vector<string> distributions = splitAtSpace(what[2]);
		  for (; sg < distributions.size(); ++sg) {
			if (!env.setRawSignalDistribution(action, sg, numberToRational(distributions[sg]))) {
			  Message::alert("set Raw Signal Distribution : " +
							 pls.actionProfileToString(ActionProfile(actions)) + " " +
							 pls.signalProfileToString(pls.getSignalProfile(sg)) + " " +
							 distributions[sg]);
			  return ERROR;
			}
		  }
		  // 次の行
		  for (; sg < pls.getNumberOfJointSignals(); ++sg) { // 型違い warning は仕方ない
			string distribution;
			if (!nextWord(fin, distribution)) // 次の確率読み込み
			  return ITEMNOTFOUND;
			if (!env.setRawSignalDistribution(ActionProfile(actions), SignalProfile(sg, numSignalsOfPlayer), numberToRational(distribution))) {
			  Message::alert("set Raw Signal Distribution : " +
							 pls.actionProfileToString(ActionProfile(actions)) + " " +
							 pls.signalProfileToString(pls.getSignalProfile(sg)) + " " +
							 distribution);
			  return ERROR;
			}
		  }
		}
	  }
	  result = NOERROR;
	  break;
	}
  }

  return result;
}

/*
 * 機能: エクセルが出力したシグナル分布を読み込む
 * 引数: 読み込む環境クラス env, pls
 */
READSTATUS Reader::readExcelSignalDistribution(Environment &env, const Players &pls) {
  READSTATUS result = NOERROR;
  string buf;
  fin.seekg(0, ios_base::beg);
  result = ITEMNOTFOUND;
  while (!fin.eof()) {
	readLine(fin, buf);
	if (buf == "Signal Distribution") {
	  for (int i = 0; i < env.getNumberOfActionProfiles()*env.getNumberOfSignalProfiles(); i++) {
		readLine(fin, buf);
		boost::regex expr("(\\S+) *(\\S+) *(\\S+)");
		boost::smatch what;

		if (boost::regex_match(buf, what, expr)) {
		  // どの行動とシグナルに対するものか
		vector<string> actionStr = split(what[1], ',');
		  vector<string> signalStr = split(what[2], ',');
		  string stbuf;
		  const int numPlayers = pls.getNumberOfPlayers();
		  vector<int> actions(numPlayers);
		  for (int pl = 0; pl < numPlayers; ++pl)
			actions[pl] = pls.getIndexOfActions(pl, actionStr[pl]);
		  const int action = pls.getAction(ActionProfile(actions));
		  vector<int> signals(numPlayers);
		  for (int pl = 0; pl < numPlayers; ++pl)
			signals[pl] = pls.getIndexOfSignals(pl, signalStr[pl]);
		  const int signal = pls.getSignal(SignalProfile(signals));
		  // 最初の行
		  if (!env.setRawSignalDistribution(action, signal, numberToRational(what[3]))) {
			  Message::alert("set Raw Signal Distribution : " +
							 pls.actionProfileToString(ActionProfile(actions)) + " " +
							 pls.signalProfileToString(SignalProfile(signals)) + " " +
							 what[3]);
			  return ERROR;
		  }
		}
	  }
	  result = NOERROR;
	  break;
	}
  }

  return result;
}


/*
  READSTATUS Reader::readPayoffMatrix(Payoff &po) {
  READSTATUS result = NOERROR;

  if (!fin) {
  result = FILENOTFOUND;
  }
  else {
  string buf;
  fin.seekg(0, ios_base::beg);
  result = ITEMNOTFOUND;
  while (!fin.eof()) {
  readLine(fin, buf);
  if (buf == "Payoff Matrix") {
  for (int i = 0; i < numPlayers; i++) {
  readLine(fin, buf);
  boost::regex expr("([a-zA-Z0-9]+) *: *(.*)");
  boost::smatch what;
  if (boost::regex_match(buf, what, expr)) {
  const int player = getIndexOfPlayers(what[1]);
  istringstream iss(what[2]);
  string stbuf;
  int count = 0;
  while (!iss.eof()) {
  iss >> stbuf;
  po.setRawPayoff(player, ActionProfile(count++, numActionsOfPlayer), numberToRational(stbuf));
  }
  while (count < po.getNumberOfActionProfiles()) {
  readLine(fin, buf);
  istringstream iss2(buf);
  while (!iss2.eof()) {
  iss2 >> stbuf;
  po.setRawPayoff(player, ActionProfile(count++, numActionsOfPlayer), numberToRational(stbuf));
  }
  }
  }
  }
  result = NOERROR;
  break;
  }
  }
  }

  return result;
  }
*/
READSTATUS Reader::readPayoffMatrix(const Players &pls,Payoff &po) {
   READSTATUS result = NOERROR;

   if (!fin) {
      result = FILENOTFOUND;
   }
   else {
      string buf;
      fin.seekg(0, ios_base::beg);
      result = ITEMNOTFOUND;
      while (!fin.eof()) {
         result = NOERROR;
         readLine(fin, buf);
         if (buf == "Payoff Matrix") {
            boost::regex expr("(\\S+) *: *(.*)");
            boost::smatch what;
            for (int ac = 0; ac < po.getNumberOfActionProfiles(); ++ac) {
               readLine(fin, buf);
               if (boost::regex_match(buf, what, expr)) {
                  // どの行動か読み取る
                  vector<string> actions_s = split(what[1], ',');
                  vector<int> actions(pls.getNumberOfPlayers());
                  for (int pl = 0; pl < pls.getNumberOfPlayers(); ++pl) {
                     actions[pl] = pls.getIndexOfActions(pl, actions_s[pl]);
                  }
                  const ActionProfile ap(actions);
                  // 利得を読み取る
                  const vector<string> payoff_s = splitAtSpace(what[2]);
                  for (int pl = 0; pl < pls.getNumberOfPlayers(); ++pl) {
                     po.setRawPayoff(pl, ap, numberToRational(payoff_s[pl]));
                  }
               }
            }
         }
      }
   }
   return result;
}

/*
  READSTATUS Reader::readCorrelationDevice(Environment &env) {
  READSTATUS result = NOERROR;
  env.setOptionOfCorrelationDevice(false);

  if (!fin) {
  result = FILENOTFOUND;
  }
  else {
  string buf;
  fin.seekg(0, ios_base::beg);
  while (!fin.eof()) {
  readLine(fin, buf);
  if (buf == "Correlation Device") {
  env.setOptionOfCorrelationDevice(true);
  readLine(fin, buf);
  istringstream iss(buf);
  string stbuf;
  for (int i = 0; i < env.getNumberOfStateProfiles(); i++) {
  iss >> stbuf;
  env.setRawCorrelationDevice(i, numberToRational(stbuf));
  }
  break;
  }
  }
  }

  return result;
  }

*/
READSTATUS Reader::read(string &tit, Environment &env, Payoff &po, Players &pls) {
   READSTATUS result;
   if (!fin) {
      Message::alert("file is not found");
      return FILENOTFOUND;
   }      
   result = readTitle();
   if (result != NOERROR) {
      Message::alert("read titile error");
      return result;
   }
   tit = title;

   result = readDiscountRate();
   if (result != NOERROR) {
      Message::alert("read discountRate error");
      return result;
   }
   env.setDiscountRate(discountRate);

   result = readVariables();
   if (result != NOERROR) {
      Message::alert("read variables error");
      return result;
   }
   result = readPlayers();
   if (result != NOERROR) {
      Message::alert("read players error");
      return result;
   }

   pls.configure(numPlayers);
   numStatesOfPlayer.resize(numPlayers);
   numActionsOfPlayer.resize(numPlayers);
   numSignalsOfPlayer.resize(numPlayers);

   for (int i = 0; i < numPlayers; i++) {
	 Automaton am;
      result = readAutomaton(players[i], am);
      if (result != NOERROR) {
         Message::alert("read Automaton"+::toString(i)+" error");
         return result;
      }
      pls.setAutomaton(i, am);
      numStatesOfPlayer[i] = am.getNumberOfStates();
      numActionsOfPlayer[i] = am.getNumberOfActions();
      numSignalsOfPlayer[i] = am.getNumberOfSignals();
   }
   pls.setJointFSA();

   env.configure(discountRate, numActionsOfPlayer, numSignalsOfPlayer);
   if (title.find("EXCEL", 0) == string::npos)
	 result = readSignalDistribution(env, pls);
   else
	 result = readExcelSignalDistribution(env, pls);
   if (result != NOERROR) {
      Message::alert("read signalDistribution error");
      return result;
   }
   po.configure(numPlayers, numActionsOfPlayer);
   result = readPayoffMatrix(pls, po);
   if (result != NOERROR) {
      Message::alert("read payoffMatrix error");
      return result;
   }
   return result;
}

void Reader::setVariable(const string &name, const PreciseNumber &value) {
   variables[name] = value;
}

map<string, PreciseNumber> Reader::getVariables() const{
   return variables;
}

