#include "Signal.h"

int main(int argc, char *argv[]) {
  if (argc < 5) {
	cout << "USAGE : ./Signal tri numPlayers numActions numSignals [actionStrings]" << endl;
	cout << "USAGE : ./Signal multi numPlayers numActions numSignals numMarket [actionsStrings]" << endl;
	exit(1);
  }
  const string option(argv[1]);
  const int numPlayers = toInteger(string(argv[2]));
  const int numActions = toInteger(string(argv[3]));
  const int numSignals = toInteger(string(argv[4]));
  
  Signal signal(numPlayers, numActions, numSignals);
  if (option == "tri") {
	vector<string> actionsString(argc - 5);
	for (int i = 0; i < argc - 5; ++i)
	  actionsString[i] = string(argv[5 + i]);
	signal.tri(actionsString);
	signal.tri_payoff(actionsString);
  }
  if (option == "tri_co") {
	vector<string> actionsString(argc - 5);
	for (int i = 0; i < argc - 5; ++i)
	  actionsString[i] = string(argv[5 + i]);
	signal.tri_co(actionsString);
	signal.tri_payoff(actionsString);
  }
}

Signal::Signal(const int nPl, const int nAc, const int nSg) {
  numPlayers = nPl;
  numActions = nAc;
  numSignals = nSg;
  numActionsOfPlayers.resize(numPlayers);
  numSignalsOfPlayers.resize(numPlayers);
  for (int pl = 0; pl < numPlayers; ++pl) {
	numActionsOfPlayers[pl] = nAc;
	numSignalsOfPlayers[pl] = nSg;
  }
  numJointActions = accumulate(numActionsOfPlayers.begin(), numActionsOfPlayers.end(), 1, multiplies<int>());
  numJointSignals = accumulate(numSignalsOfPlayers.begin(), numSignalsOfPlayers.end(), 1, multiplies<int>());
  nameActions.resize(numActions);
}


  
string Signal::actionsToString(const ActionProfile &ap) const {
  string res;
  for (int pl = 0; pl < numPlayers; ++pl) {
	if (pl != 0)
	  res += ",";
	res += nameActions[ap[pl]];
  }
  return res;
}

/*
 * 三角取引用
 * 誰のアクションに対するシグナルかを知らせる
 */
int Signal::getOpponent(const int pl) const {
  if (pl == 0)
	return numPlayers - 1;
  return pl - 1;
}

/*
 * 三角取引用
 * 正しいならp,間違っているなら1-p
 */
void Signal::tri(const vector<string> &actionsString) {
  // 行動の文字セット
  nameActions = actionsString;
  const string cor = "p";
  const string miss = "(1-p)";
  for (int ac = 0; ac < numJointActions; ++ac) {
	const ActionProfile ap(ac, numActionsOfPlayers);
	cout << actionsToString(ap) << " :";
	for (int sg = 0; sg < numJointSignals; ++sg) {
	  cout << " ";
	  const SignalProfile sgp(sg, numSignalsOfPlayers);
	  for (int pl = 0; pl < numPlayers; ++pl) {
		if (pl != 0)
		  cout << "*";
		if (ap[getOpponent(pl)] == sgp[pl])
		  cout << cor;
		else
		  cout << miss;
	  }
	}
	cout << endl;
  }
}

/*
 * 三角取引用
 * 利得表を生成
 */
void Signal::tri_payoff(const vector<string> &actionsString) {
  // 行動の文字セット
  nameActions = actionsString;
  vector<string> reward(numActions);
  reward[0] = "x";
  reward[1] = "0";
  reward[2] = "h";
  reward[3] = "u";
  vector<string> cost(numActions);
  cost[0] = "c";
  cost[1] = "0";
  cost[2] = "l";
  cost[3] = "k";
  
  for (int ac = 0; ac < numJointActions; ++ac) {
	const ActionProfile ap(ac, numActionsOfPlayers);
	cout << actionsToString(ap) << " :";
	for (int pl = 0; pl < numPlayers; ++pl) {
	  const int opAc = ap[getOpponent(pl)];
	  const int myAc = ap[pl];
	  cout << " " + reward[opAc] + "-" + cost[myAc];
	}
	cout << endl;
  }
}	  
  
  
/*
 * 三角取引用
 * No Error p, One Error q, Two Error r
 */
void Signal::tri_co(const vector<string> &actionsString) {
  // 行動の文字セット
  nameActions = actionsString;
  // エラーのときの文字をセット
  vector<string> error_string(numPlayers + 1);
  for (int i = 0; i < numPlayers + 1; ++i)
	error_string[i] = 'p' + i;

  for (int ac = 0; ac < numJointActions; ++ac) {
	const ActionProfile ap(ac, numActionsOfPlayers);
	cout << actionsToString(ap) << " :";
	for (int sg = 0; sg < numJointSignals; ++sg) {
	  const SignalProfile sgp(sg, numSignalsOfPlayers);
	  int error = 0;
	  for (int pl = 0; pl < numPlayers; ++pl) {
		if (ap[getOpponent(pl)] != sgp[pl])
		  error++;
	  }
	  cout << " " + error_string[error];
	}
	cout << endl;
  }
}

/*
 * 多市場用
 * a_i = (a_i^1,a_i^2,...,a_i^m) を生成
 */
vector<int> toProfile(int index, const int radix, const int size) {
  vector<int> res(size, 0);
  for (int i = size - 1; i >= 0; --i) {
	res[i] = index % radix;
	index /= radix;
  }
  return res;
}

/*
 * 三角多市場用
 * 
 */

