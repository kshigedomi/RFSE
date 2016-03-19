#include "rfpe.h"
/*
 * メインクラス．
 * 入力ファイルを読み，RFSE or global RFSE かを判定して，出力ファイルに書き出す．
 */

/*
 * 機能: 均衡判定前の下準備を行う
 */
void setRepeatedGame(RepeatedGame &rg,map<string, PreciseNumber> &values, const Environment &env, const Payoff &pd, const Players &pls) {
   rg.setVariables(values); // 変数セット
   rg.configure(env, pd, pls); // 各種設定をセット
   rg.solve(); // 割引期待利得などを出す
}

/*
 * 機能: environment と payoff に変数をセットする
 */
void setVariablesToEnvironmentAndPayoff(map<string, PreciseNumber> &values, Environment &env, Payoff &pd) {
   env.setSignalDistributionFromRawSignalDistribution(values);
   pd.setPayoffFromRawPayoff(values);
}

/*
 * 機能: 平均利得をだす
 */
void checkReward(map<string,PreciseNumber> &variables, RepeatedGame rg, Environment environment, Payoff payoff, Players players, const string &out) {
   Message::display("checkReward");
   Writer writer(out);
   writer.writeEnvironment(rg, environment, payoff, players, variables);
   // 変数の始まり，精度，終わり
   const PreciseNumber start = PreciseNumber("1/1000");
   const PreciseNumber interval = PreciseNumber("1/1000");
   const PreciseNumber end = PreciseNumber("2/10");

   const bool discount = false;  // 割引因子ごとの平均利得
   const bool no_error = false;  // no error ごとの平均利得
   const bool one_error = true; // one error ごとの平均利得

   for (PreciseNumber value = start; value < end; value += interval) {
	 cout << rationalToString(value) << endl;
      // value set
      if (discount)
         environment.setDiscountRate(value);
      else if (no_error)
         variables["p"] = value;
      else if (one_error)
         variables["e"] = value;
      if (!environment.checkEnvironment(players)) {
         break;
      }
      // 平均利得を書き出す
      setVariablesToEnvironmentAndPayoff(variables, environment, payoff);
      setRepeatedGame(rg, variables, environment, payoff, players);
      const Result result = rg.checkNoEquilibrium();
      PreciseNumber reward = -1;
      if ((result == SUCCESS || result == GLOBAL)) {
         vector<vector<PreciseNumber> > ex = rg.getAlphaVectors();
         reward = ex[0][0];
      }
      writer.put(rationalToString(value) + "\t" +  rationalToString((1-environment.getDiscountRate())*reward));
   }
   cout << endl;
}  

/*
 * 機能: 均衡判定をパラメータごとにする
 */
void checkParameter(map<string,PreciseNumber> &variables, RepeatedGame rg, Environment environment, Payoff payoff, Players players, const string &out) {
   //parameter
   PreciseNumber pStart = PreciseNumber("5000/10000");
   PreciseNumber pInterval = PreciseNumber("1/100");
   PreciseNumber pEnd = ONE;
   PreciseNumber qStart = ZERO;
   PreciseNumber qInterval = PreciseNumber("1/100");
   PreciseNumber qEnd = PreciseNumber("2/100");
   
   Message::display("checkParmeter");
   Writer writer(out);
   setVariablesToEnvironmentAndPayoff(variables, environment, payoff);
   writer.writeEnvironment(rg, environment, payoff, players, variables);
   
   PreciseNumber QOver = ZERO;
   PreciseNumber QUnder = ZERO;
   writer.put("PARAMETER CHECK");
   setVariablesToEnvironmentAndPayoff(variables, environment, payoff);
   for (PreciseNumber p = pStart; p <= pEnd; p += pInterval) {
	 cout << rationalToString(p) << endl;
      bool isEq = false;
      QOver = ZERO;
      QUnder = ZERO;
      for (PreciseNumber q = qStart; q <= p; q += qInterval) {
         variables["p"] = p;
         variables["q"] = q;
         setVariablesToEnvironmentAndPayoff(variables, environment, payoff);
         if (!environment.checkEnvironment(players)) {
            if (isEq)
               QOver = q - qInterval;
            isEq = false;
            continue;
         }
         setRepeatedGame(rg, variables, environment, payoff, players);
         const Result result = rg.checkNoEquilibrium();

         if ((result == SUCCESS || result == GLOBAL) && !isEq) {//first success
            cout << resultToString(result) << endl;
            QUnder = q;
            isEq = true;
         } else if ((result != SUCCESS && result != GLOBAL) && isEq) {//last success
            QOver = q - qInterval;
            break;
         }
      }
      if (QOver == ZERO && isEq)
         QOver = qEnd;
      writer.writeEqRange(p, QUnder, QOver);
   }
}


/*
 * 機能: GA を動かす
 */
/*
void checkGeneticAlgorithm(map<string,PreciseNumber> &variables, RepeatedGame rg, Environment environment, Payoff payoff, Players players, const string &out) {
   Message::display("check Genetic");
   Writer writer(out);
   writer.writeEnvironment(rg, environment, payoff, players, variables);
   srand((unsigned int)time(NULL)); // 乱数セット
   const Automaton m = players.getAutomaton(PLAYER); // 本来のオートマトンをゲット
   // GA start
   GeneticAlgorithm ga(100, m.getNumberOfStates() + m.getNumberOfStates() * m.getNumberOfSignals(), 0.5, m);
   ga.createGenes(); // 最初の世代を作る
   const vector<Belief> samples = getSampleBeliefs(m.getNumberOfStates()); // sample を作る
   while (true) {
      Message::display("next generation");
      while (ga.nextGene()) {
         // 現在のオートマトンをチェック
         players.setAllAutomaton(ga.getNowGeneAutomaton()); // GAが作ったオートマトンをセット
         // 均衡判定
         setRepeatedGame(rg, variables, environment, payoff, players);
         const Result result = rg.checkNoEquilibrium();
         if (result == SUCCESS || result == TIMEISOVER || result == GLOBAL) { // 成功
            // 書き込み
            writer.writeResult(result);
            writer.put(ga.getNowGeneAutomaton());
            // スコアセット
            ga.nowGeneSetScore(ZERO);
         } else if (result == NOTEXIST) {
            // スコアセット
            ga.nowGeneSetScore(calcAveGapBetweenRewards(samples, rg.getAlphaVectors(), rg.getOneShotExtensions()));
         } else if (result == PREPROFAILURE || result == FAILURE) {
            // スコアセット
            rg.makeTargetBeliefDivision();
            const BeliefDivision target = rg.getTargetBeliefDivision();
            if (target.existEmptyDivision()) {
               ga.nowGeneSetScore(calcAveGapBetweenRewards(samples, rg.getAlphaVectors(), rg.getOneShotExtensions()));
            } else {
               ga.nowGeneSetScore(calcAveManhattanDistance(rg.getFailure(), rg.getTargetBeliefDivision()));
            }
         }
         cout << "end" << endl;
      }
      ga.nextGeneration(); // 次の世代を生成
   }
}
*/
/*
 * 機能: regular なオートマトンを filename in から受け取って両方向から均衡判定
 * 注意: プレイヤ1から見たシグナル分布/利得表は同じでないと判定方向からの均衡判定が失敗する
 */
void checkRegularAsymAutomatons(map<string,PreciseNumber> &variables, RepeatedGame rg, Environment environment, Payoff payoff, Players players, const string &out,const string &in) {
   Message::display("check regular start");
   Writer writer(out);
   writer.writeEnvironment(rg, environment, payoff, players, variables);
   Loader myLoader(players.getAutomaton(PLAYER),in); // 自分のオートマトンのロード
   int count = 0; // もう既に調べたオートマトン
   vector<Belief> e;
   for (int state = 0; state < players.getNumberOfOpponentStates(); ++state) {
      Belief b(players.getNumberOfOpponentStates());
      b.set(state, ONE);
      e.push_back(b);
   }
   while (myLoader.setNextAutomatonFromRawString()) {
      Loader opLoader(players.getAutomaton(OPPONENT),in); // 相手のオートマトンのロード
      opLoader.skipAutomaton(count++);
      // 自分のオートマトンをセット
      const Automaton me = myLoader.getAutomaton();
      players.setAutomaton(PLAYER, me);
      while (opLoader.setNextAutomatonFromRawString()) {
         // 相手のオートマトンをセット
         const Automaton you = opLoader.getAutomaton();
         players.setAutomaton(OPPONENT, you);
         // 均衡判定
         setRepeatedGame(rg, variables, environment, payoff, players);
         if (rg.checkSeminalBeliefSet(e)) {
            players.setAutomaton(PLAYER,you);
            players.setAutomaton(OPPONENT,me);
            // 均衡判定
            setRepeatedGame(rg, variables, environment, payoff, players);
            if (rg.checkSeminalBeliefSet(e)) {
               // 書き込み
               writer.put("-----------------------------------------");
               writer.put("my automaton");
               writer.put(me);
               writer.put("op automaton");
               writer.put(you);
            }
         }
      }
   }
   Message::display("End gracufully");
   writer.put("End gracufully");
}

/*
 * 機能: regular なオートマトンを filename in から受け取って均衡判定
 * メモ: public 用
 */
void checkRegularAutomatonWithPublic(map<string,PreciseNumber> &variables, RepeatedGame rg, Environment environment, Payoff payoff, Players players, const string &out,const string &in) {
   Message::display("check regular start");
   Writer writer(out);
   writer.writeEnvironment(rg, environment, payoff, players, variables);
   Loader loader(players.getAutomaton(PLAYER),in); // 元のオートマトンをセット
   long count=0;
   variables["q"]=0;
   setVariablesToEnvironmentAndPayoff(variables, environment, payoff);
   while (loader.setNextAutomatonFromRawString()) {
      // オートマトンをセット
      Automaton m = loader.getAutomaton();
      players.setAllAutomaton(m);
      cout << ++count << endl;
      Message::display(m);
      // 均衡判定
      setRepeatedGame(rg, variables, environment, payoff, players);
      bool isEq=true;
      for (int st = 0; st < players.getNumberOfStates(PLAYER); ++st) {
         Belief b(players.getNumberOfOpponentStates());
         b.set(st, ONE);
         if (!rg.checkSeminalBelief(b)) {
            isEq = false;
            break;
         }
      }
      if (isEq) {
         // 書き込み
         writer.writeResult(SUCCESS);
         writer.put(m);
         vector<vector<PreciseNumber> > ex = rg.getAlphaVectors();
         writer.put(rationalToString((1-environment.getDiscountRate())*(ex[0][0]/(ONE))));
      }
   }
   Message::display("End gracufully");
   writer.put("End gracufully");
}

/*
 * 機能: regular なオートマトンを filename in から受け取って均衡判定
 */
void checkRegularAutomatons(map<string,PreciseNumber> &variables, RepeatedGame rg, Environment environment, Payoff payoff, Players players, const string &out,const string &in) {
   Message::display("check regular start");
   Writer writer(out);
   writer.writeEnvironment(rg, environment, payoff, players, variables);
   Loader loader(players.getAutomaton(PLAYER),in); // 元のオートマトンをセット
   long count=0;
   setVariablesToEnvironmentAndPayoff(variables, environment, payoff);
   while (loader.setNextAutomatonFromRawString()) {
      // オートマトンをセット
      Automaton m = loader.getAutomaton();
      players.setAllAutomaton(m);
      cout << ++count << endl;
      Message::display(m);
      // 均衡判定
      setRepeatedGame(rg, variables, environment, payoff, players);
      const Result result = rg.checkNoEquilibrium();
      cout << resultToString(result) << endl;
      if (result == SUCCESS || result == TIMEISOVER || result == GLOBAL || result == NOTFIXED) {
         // 書き込み
         writer.writeResult(result);
         writer.put(m);
         vector<vector<PreciseNumber> > ex = rg.getAlphaVectors();
         writer.put(rationalToString((1-environment.getDiscountRate())*(ex[0][0]/(ONE+ONE))));
      }
   }
   Message::display("End gracufully");
   writer.put("End gracufully");
}

/*
 * 機能: regular なオートマトンを filename in から受け取って（パラメータを変えながら）均衡判定
 */
void checkParameterRegular(map<string,PreciseNumber> &variables, RepeatedGame rg, Environment environment, Payoff payoff, Players players, const string &out, const string& in) {
   vector<PreciseNumber> vStart, vInterval, vEnd;
   /* パラメータセット */
   // 始まり
   vStart.push_back(ONE);
   vStart.push_back(PreciseNumber("1/10"));
   //  vStart.push_back(0.0005);
   // 途中
   vInterval.push_back(PreciseNumber("5/100"));
   vInterval.push_back(PreciseNumber("5/100"));
   //  vInterval.push_back(0.005);
   // 終了
   vEnd.push_back(PreciseNumber("5/10"));
   vEnd.push_back(ONE);
   //  vEnd.push_back(0.1);   
   
   Message::display("checkParameterRegular");
   Writer writer(out);
   setVariablesToEnvironmentAndPayoff(variables, environment, payoff);
   writer.writeEnvironment(rg, environment, payoff, players, variables);
   Loader loader(players.getAutomaton(PLAYER),in);
   vector<Automaton> automatons = loader.loadAllAutomatons();
   vector<Result> results(automatons.size(), NOTEXIST);

   vector<PreciseNumber> v = vStart;
   writer.writeChangeVariables('p', 'p' + v.size());
   
   do {
      // varaibles set
      char vChar = 'p';
      for (vector<PreciseNumber>::size_type i = 0; i < v.size(); ++i) {
         string vStr(1, vChar++);
         variables[vStr] = v[i];
      }
      setVariablesToEnvironmentAndPayoff(variables, environment, payoff);
      writer.writeVariables(variables);

      if (!environment.checkEnvironment(players))
         continue;

      for (vector<Automaton>::size_type i = 0; i < automatons.size(); ++i) {
         if (results[i] == TIMEISOVER || results[i] == SUCCESS || results[i] == GLOBAL || results[i] == NOTFIXED)
            continue;
         players.setAllAutomaton(automatons[i]);
         setRepeatedGame(rg, variables, environment, payoff, players);
         results[i] = rg.checkNoEquilibrium();
         //         Message::display(resultToString(results[i]));
         if (results[i] == SUCCESS || results[i] == TIMEISOVER || results[i] == GLOBAL || results[i] == NOTFIXED) {
            writer.writeResult(results[i]);
            writer.put(automatons[i]);
         }
      }
   } while (next(v, vStart, vInterval, vEnd));
}

/*
 * 機能: 他のことに使いたいときはこの関数を書き換える
 */
void checkSpecial(map<string,PreciseNumber> &variables, RepeatedGame rg,Environment environment, Payoff payoff, Players players, const string &out) {
   Message::display("checkSpecial");
   cout << parseEquation("-(x+1)",variables) << endl;
   return;
   PreciseNumber p = variables["p"];
   PreciseNumber s = ONE-p;
   PreciseNumber x = variables["x"];
   PreciseNumber y = variables["y"];
   PreciseNumber d = 0.99;
   for (int m=2; m < 1000; m+=2) {
	 PreciseNumber LTFD = m*(1-(1-p)*x/(2*p-1));
	 int mA = m/2;
	 int mB = m/2;
	 PreciseNumber pmA = pow(p, mA);
	 PreciseNumber smA = pow(s, mA);
	 PreciseNumber pmB = pow(p, mB);
	 PreciseNumber smB = pow(s, mB);
	 PreciseNumber vP = mA + p*mA*x/(p-s) + smB*mB*y/(pmB-smB);
	 PreciseNumber NTPD = m - (d*smA*(p-s)*(m-vP)+(1-d)*(s-smA)*mB*x)/((p-s)*(1-d*(1-smA)));
	 cout << m << "\t" << rationalToString((NTPD - LTFD)) << endl;
   }
}

/*
 * 機能: filename in の中のオートマトンを均衡判定
 */
void checkAutomatons(map<string,PreciseNumber> &variables, RepeatedGame rg, Environment environment, Payoff payoff, Players players, const string &out, const string &in) {
   Message::display("check autoamtons");
   Message::display("Input:" + in);
   Message::display("Output:" + out);
   Writer writer(out);
   writer.writeEnvironment(rg, environment, payoff, players, variables);
   Loader loader(players.getAutomaton(PLAYER), in, "NOTFIXED");
   while (loader.setNextAutomatonAndVariables(variables)) {
      Automaton m = loader.getAutomaton();
      Message::display(m);
      players.setAllAutomaton(m);
      // 均衡判定
      setRepeatedGame(rg, variables, environment, payoff, players);
      const Result result = rg.checkNoEquilibrium();
      Message::display(resultToString(result));
      if (result == SUCCESS || result == TIMEISOVER || result == GLOBAL || result == NOTFIXED) {
		// 書き込み
		writer.writeResult(result);
		writer.put(m);
		vector<vector<PreciseNumber> > ex = rg.getAlphaVectors();
		writer.put(rationalToString((1-environment.getDiscountRate())*(ex[0][0]/(ONE+ONE))));
	  }
   }
   /*
   //   sort(automatons.begin(),automatons.end());
   for (vector<Automaton>::size_type i = 0; i < automatons.size(); ++i) {
      writer.put("SUCCESS");
      writer.put(automatons[i].second);
      writer.put(rationalToString(automatons[i].first));
   }
   */
   Message::display("End gracufully");
   writer.put("End gracufully");
}

void checkAllInformations(map<string,PreciseNumber> &variables, RepeatedGame rg, Environment environment, Payoff payoff, Players players, const string &out) {
   Writer writer(out);
   Message::display("check start");
   Message::display(players.getAutomaton(PLAYER));
   writer.writeEnvironment(rg, environment, payoff, players, variables);

   // 均衡判定
   setRepeatedGame(rg, variables, environment, payoff, players);
   Message::display(rg.getBeliefDivision().toString());
   Message::display(rg.getTargetBeliefDivision().toString());

   vector<vector<PreciseNumber> > ex = rg.getAlphaVectors();
   cout << "平均利得 : " << rationalToString((1-environment.getDiscountRate())*(ex[0][0])/(ONE)) << endl;;

   const Result result = rg.checkNoEquilibrium();
   Message::display("Accomotability : " + resultToString(result));

   writer.writeResult(result);
   writer.writePlayers(players);
   if (result == FAILURE || result == NOTCONSISTENT) {
	 writer.writeProfitableOneShot(rg.getFailure(), rg, players);
   } else if (result == NOTEXIST) {
	 writer.writeBeliefSpace(rg, players);
   }
   writer.writeMaxIteration(rg);
   writer.writeAveragePayoff(environment, rg, players);
}

void checkRFPE(const string &datafile, const string &prefix, int argc, char **argv) {
   Reader reader(datafile);

   Environment env;
   Payoff pd;
   Players players;
   string title;

   READSTATUS result = reader.read(title, env, pd, players);
   if (result != NOERROR) {
      Message::alert("Fail to read datafile");
      Message::display("Abort");
      return;
   }
   players.setJointFSA();
   map<string, PreciseNumber> variables = reader.getVariables();
   
   // option
   int variableStart = 3;
   Option option = NONE;
   string filename;
   if (argc >= 4) {
      string tmp = argv[3];
      if(tmp.find("parameter",0) != string::npos) {
         variableStart = 4;
         option = PARA;
      } else if (tmp.find("special",0) != string::npos) {
         variableStart = 4;
         option = SPECIAL;
      } else if (tmp.find("reward",0) != string::npos) {
         variableStart = 4;
         option = REWARD;
      } else if (argc >= 5) {
         filename = argv[4];
         if (tmp.find("automaton",0) != string::npos){
            variableStart = 5;
            option = AUTOMATONS;
         } else if(tmp.find("regular",0) != string::npos) {
            variableStart = 5;
            option = REGULAR;
         } else if(tmp.find("all", 0) != string::npos) {
            variableStart = 5;
            option = PARA_REGULAR;
         } else if(tmp.find("asym", 0) != string::npos) {
            variableStart = 5;
            option = ASYM;
         } else if(tmp.find("public", 0) != string::npos) {
            variableStart = 5;
            option = PUBLIC;
         }
      }
   }

   // set variables
   boost::regex expr("([a-zA-Z]+)=(.+)");
   boost::smatch what;

   RepeatedGame rg;

   PreciseNumber value;
   for (int i = variableStart; i < argc; i++) {
      string exp = argv[i];
      if (exp[0] == '-') {
         if (exp == "-debug") {
            rg.set_history_flag(true);
            rg.set_preprocess_view_flag(true);
         } else if (exp == "-history") {
            rg.set_history_flag(true);
         } else if (exp == "-preprocessview") {
            rg.set_preprocess_view_flag(true);
         } else if (exp == "-target") {
            rg.set_target_flag(true);
         } else if (exp == "-preprocess") {
            rg.set_preprocess_flag(true);
         } else if (exp == "-pure") {
            rg.set_pure_flag(true);
         } else if (exp == "-timeover") {
            rg.set_timeover_flag(true);
         } else if (exp == "-belief") {
            rg.set_beliefTransition_view_flag(true);
         } else if (exp == "-nofixed") {
            rg.set_no_fixed_flag(true);
         } else if (exp == "-global") {
            rg.set_global_flag(true);
         } else if (exp == "-nobd") {
            rg.set_nobd_flag(true);
         } else {
            Message::alert("Fail to parse options in arguments");
            Message::display("Abort");
            return;
         }            
      } else if (boost::regex_match(exp, what, expr)) {
         value = numberToRational(what[2]);
         variables[what[1]] = value;
      }
      else {
         Message::alert("Fail to parse variables in arguments");
         Message::display("Abort");
         return;
      }
   }

   // set environment and payoff
   setVariablesToEnvironmentAndPayoff(variables, env, pd);
   if (!env.checkEnvironment(players)) {
      Message::alert("Player1 Signal distribution error");
      Message::display("Abort");
      return;
   }
   string out;
   switch (option) {
   case NONE:
      checkAllInformations(variables, rg, env, pd , players, prefix);
      break;
   case AUTOMATONS:
      checkAutomatons(variables, rg, env, pd, players, prefix, filename);
      break;
   case PARA:
      out = prefix + "-para";
      checkParameter(variables, rg, env, pd, players, out);
      break;
   case REGULAR:
      out = prefix + "-regular";
      checkRegularAutomatons(variables, rg, env, pd, players, out, filename);
      break;
   case PARA_REGULAR:
      out = prefix + "-all";
      checkParameterRegular(variables, rg, env, pd, players, out, filename);
      break; 
  case SPECIAL:
      out = prefix + "-special";
      checkSpecial(variables, rg, env, pd, players, out);
      break;
   case REWARD:
      out = prefix + "-reward";
      checkReward(variables, rg, env, pd, players, out);
      break;
   case ASYM:
      out = prefix + "-asym";
      checkRegularAsymAutomatons(variables, rg, env, pd, players, out, filename);
      break;
   case PUBLIC:
      out = prefix + "-pub";
      checkRegularAutomatonWithPublic(variables, rg, env, pd, players, out, filename);
      break;
   }
}      

int main(int argc, char **argv) {
   if (argc >= 3) {
      checkRFPE(string(argv[1]), string(argv[2]), argc, argv);
   } else {
      Message::display("Usage : rfpe [datafile] [output-prefix] {[variables], ...}");
      Message::display("OR : rfpe [datafile] [output-prefix] [option] {[input-data] [varables], ...}");
      Message::display("More Help : rfpe --help");
      Message::display("variables :");
      Message::display("	[name]=[value]    e.g) p=0.5 q=1/6");
   }
   return 0;
}
