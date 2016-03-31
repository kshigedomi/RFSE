#include "MultiSignal.h"

/*
 * 多市場のシグナル分布を出力する．
 */

int main(int argc, char *argv[]) {
   if (argc < 5) {
      cout << "USAGE : ./MultiSignal numPlayers numMarkets numActions numSignals [actionsStrings]" << endl;
      exit(1);
   }
   const string option(argv[1]);
   const int numPlayers = MyUtil::toInteger(string(argv[1]));
   const int numMarkets = MyUtil::toInteger(string(argv[2]));
   const int numActions = MyUtil::toInteger(string(argv[3]));
   const int numSignals = MyUtil::toInteger(string(argv[4]));
  
   MultiSignal multiSignal(numPlayers, numMarkets, numActions, numSignals);
   vector<string> actionsString(argc - 5);
   for (int i = 0; i < argc - 5; ++i)
      actionsString[i] = string(argv[5 + i]);
   cout << "Title : Prisoner's Dilemma 1MP-1MP\n\
Discount Rate : 0.9\n\
Variables : p=0.7 q=0.01 x=0.1 y=1 e=0.0001\n\
Players : Player1 Player2\n\
\n\
Automaton Player1\n";
   multiSignal.viewAutomaton(actionsString);
   cout << endl << "Automaton Player2" << endl;
   multiSignal.viewAutomaton(actionsString);
   cout << endl;
   multiSignal.multiMarket(actionsString);
}

MultiSignal::MultiSignal(const int nPl, const int nMa, const int nAc, const int nSg) {
   numPlayers = nPl;
   numMarkets = nMa;
   numActions = nAc;
   numSignals = nSg;
   numAllActions = pow(numActions, numMarkets);
   numAllSignals = pow(numSignals, numMarkets);
}

/*
 * 多市場用
 * オートマトンを生成
 */
void MultiSignal::viewAutomaton(const vector<string> &actionsString) {
   vector<string> signalsString(numSignals);
   signalsString[0] = "g";
   signalsString[1] = "b";
   cout << "States : R P" << endl;
   cout << "Actions :";
   for (int ac = 0; ac < numAllActions; ++ac) {
      const vector<int> actions = toProfile(ac, numActions, numMarkets);
      cout << " ";
	  for (int m = 0; m < numMarkets; ++m)
		cout << actionsString[actions[m]];
   }
   cout << endl;
   cout << "Signals :";
   for (int sg = 0; sg < numAllSignals; ++sg) {
      const vector<int> signals = toProfile(sg, numSignals, numMarkets);
	  cout << " ";
	  for (int m = 0; m < numMarkets; ++m)
		cout << signalsString[signals[m]];
   }
   cout << endl;
   // オートマトンアクションチョイス
   cout << "R ";
   for (int mark = 0; mark < numMarkets; ++mark)
      cout << actionsString[0];
   cout << endl;
   cout << "P ";
   for (int mark = 0; mark < numMarkets; ++mark) {
      if (mark == 0)
         cout << actionsString[0];
      else
         cout << actionsString[1];
   }
   cout << endl;
   // オートマトン シグナル遷移 R
   for (int sg = 0; sg < numAllSignals; ++sg) {
      const vector<int> signals = toProfile(sg, numSignals, numMarkets);
      cout << "R ";
      for (int m = 0; m < numMarkets; ++m)
         cout << signalsString[signals[m]];
      cout << " R" << endl;
   }
   // オートマトン シグナル遷移 P
   for (int sg = 0; sg < numAllSignals; ++sg) {
      const vector<int> signals = toProfile(sg, numSignals, numMarkets);
      cout << "P ";
      for (int m = 0; m < numMarkets; ++m)
         cout << signalsString[signals[m]];
      cout << " P" << endl;
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

string toString(const vector<int> &vec) {
   string res;
   for (vector<int>::size_type i = 0; i < vec.size(); ++i)
      res += MyUtil::toString(vec[i]);
   return res;
}

/*
 * 多市場用
 * 市場間の相関はなし
 * No error p, one error q
 */
void MultiSignal::multiMarket(const vector<string> &actionsString) {
   cout << "Signal Distribution" << endl;
   for (int myAc = 0; myAc < numAllActions; ++myAc) {
      const vector<int> myActions = toProfile(myAc, numActions, numMarkets); // 自分の行動（市場ごと）
      for (int opAc = 0; opAc < numAllActions; ++opAc) {
         const vector<int> opActions = toProfile(opAc, numActions, numMarkets); // 相手の行動（市場ごと）
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[myActions[i]];
         cout << ",";
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[opActions[i]];
         cout << " : ";
         for (int mySg = 0; mySg < numAllSignals; ++mySg) {
            const vector<int> mySignals = toProfile(mySg, numSignals, numMarkets); // 自分のシグナル（市場ごと）
            for (int opSg = 0; opSg < numAllSignals; ++opSg) {
               const vector<int> opSignals = toProfile(opSg, numSignals, numMarkets); // 自分のシグナル（市場ごと）
               if (opSg == 0)
                  viewProb(myActions, opActions, mySignals, opSignals);
               else {
                  cout << " ";
                  viewProb(myActions, opActions, mySignals, opSignals);
               }
            }
            cout << endl;
         }
      }
   }
   cout << endl;
   cout << "Payoff Matrix" << endl;
   for (int myAc = 0; myAc < numAllActions; ++myAc) {
      const vector<int> myActions = toProfile(myAc, numActions, numMarkets); // 自分の行動（市場ごと）
      for (int opAc = 0; opAc < numAllActions; ++opAc) {
         const vector<int> opActions = toProfile(opAc, numActions, numMarkets); // 相手の行動（市場ごと）
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[myActions[i]];
         cout << ",";
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[opActions[i]];
         cout << " : ";
         viewPayoff(myActions, opActions);
         cout << " ";
         viewPayoff(opActions, myActions);
         cout << endl;
      }
   }
}


/*
 * 多市場用 ほぼ公的観測
 * 市場間の相関はなし
 * P(gg|CC)=p P(gb|CC)=q エラー確率 e
 */
void MultiSignal::multiMarketPublic(const vector<string> &actionsString) {
   cout << "Signal Distribution" << endl;
   for (int myAc = 0; myAc < numAllActions; ++myAc) {
      const vector<int> myActions = toProfile(myAc, numActions, numMarkets); // 自分の行動（市場ごと）
      for (int opAc = 0; opAc < numAllActions; ++opAc) {
         const vector<int> opActions = toProfile(opAc, numActions, numMarkets); // 相手の行動（市場ごと）
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[myActions[i]];
         cout << ",";
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[opActions[i]];
         cout << " : ";
         for (int mySg = 0; mySg < numAllSignals; ++mySg) {
            const vector<int> mySignals = toProfile(mySg, numSignals, numMarkets); // 自分のシグナル（市場ごと）
            for (int opSg = 0; opSg < numAllSignals; ++opSg) {
               const vector<int> opSignals = toProfile(opSg, numSignals, numMarkets); // 自分のシグナル（市場ごと）
               if (opSg == 0)
                  viewProbPublic(myActions, opActions, mySignals, opSignals);
               else {
                  cout << " ";
                  viewProbPublic(myActions, opActions, mySignals, opSignals);
               }
            }
            cout << endl;
         }
      }
   }

   cout << "Payoff Matrix" << endl;
   for (int myAc = 0; myAc < numAllActions; ++myAc) {
      const vector<int> myActions = toProfile(myAc, numActions, numMarkets); // 自分の行動（市場ごと）
      for (int opAc = 0; opAc < numAllActions; ++opAc) {
         const vector<int> opActions = toProfile(opAc, numActions, numMarkets); // 相手の行動（市場ごと）
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[myActions[i]];
         cout << ",";
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[opActions[i]];
         cout << " : ";
         viewPayoff(myActions, opActions);
         cout << " ";
         viewPayoff(opActions, myActions);
         cout << endl;
      }
   }
}


/*
 * 多市場用 ほぼ完全観測（公的装置つき）
 * 市場間の相関はなし
 * No error p, one error q, public signal l
 */
void MultiSignal::multiMarketPrivateWithPublic(const vector<string> &actionsString) {
   cout << endl;
   cout << "Signal Distribution" << endl;
   for (int myAc = 0; myAc < numAllActions; ++myAc) {
      const vector<int> myActions = toProfile(myAc, numActions, numMarkets); // 自分の行動（市場ごと）
      for (int opAc = 0; opAc < numAllActions; ++opAc) {
         const vector<int> opActions = toProfile(opAc, numActions, numMarkets); // 相手の行動（市場ごと）
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[myActions[i]];
         cout << ",";
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[opActions[i]];
         cout << " : ";
         for (int mySg = 0; mySg < numAllSignals; ++mySg) {
            const vector<int> mySignals = toProfile(mySg, numSignals, numMarkets); // 自分のシグナル（市場ごと）
            for (int myMark = 0; myMark < numMarkets; ++myMark) {
               for (int opSg = 0; opSg < numAllSignals; ++opSg) {
                  for (int opMark = 0; opMark < numMarkets; ++opMark) {
                     const vector<int> opSignals = toProfile(opSg, numSignals, numMarkets); // 自分のシグナル（市場ごと）
                     if (myMark == 0 && opSg == 0 && opMark == 0)
                        viewProbPrivateWithPublic(myActions, opActions, mySignals, opSignals, myMark, opMark);
                     else {
                        cout << " ";
                        viewProbPrivateWithPublic(myActions, opActions, mySignals, opSignals, myMark, opMark);
                     }
                  }
               }
            }
            cout << endl;
         }
      }
   }

   cout << endl;
   cout << "Payoff Matrix" << endl;
   for (int myAc = 0; myAc < numAllActions; ++myAc) {
      const vector<int> myActions = toProfile(myAc, numActions, numMarkets); // 自分の行動（市場ごと）
      for (int opAc = 0; opAc < numAllActions; ++opAc) {
         const vector<int> opActions = toProfile(opAc, numActions, numMarkets); // 相手の行動（市場ごと）
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[myActions[i]];
         cout << ",";
         for (int i = 0; i < numMarkets; ++i)
            cout << actionsString[opActions[i]];
         cout << " : ";
         viewPayoff(myActions, opActions);
         cout << " ";
         viewPayoff(opActions, myActions);
         cout << endl;
      }
   }
}

/*
 * 多市場用 ほぼ完全観測
 * 確率を出力
 */
void MultiSignal::viewProbPrivateWithPublic(const vector<int> &myActions, const vector<int> &opActions, const vector<int> &mySignals, const vector<int> &opSignals, const int myMark, const int opMark) {
   const int numMarket = myActions.size();
   // 確率の名前を決定
   vector<string> names(numPlayers + 1);
   for (int pl = 0; pl < numPlayers + 1; ++pl)
      names[pl] = 'p' + pl;
   names[numPlayers] = "(1-p-2*q)";
   // 確率を出力
   for (int mark = 0; mark < numMarket; ++mark) {
      if (mark != 0)
         cout << "*";
      int error = 0;
      if (opActions[mark] != mySignals[mark])
         error++;
      if (myActions[mark] != opSignals[mark])
         error++;
      cout << names[error];
   }
   if (myMark == opMark)
      cout << "*(1-2*e*(" << numMarkets << "-1))" << "/" << numMarkets;
   else
      cout << "*e";
}

/*
 * 多市場用 ほぼ公的観測
 * 確率を出力
 */
void MultiSignal::viewProbPublic(const vector<int> &myActions, const vector<int> &opActions, const vector<int> &mySignals, const vector<int> &opSignals) {
   const int numMarket = myActions.size();
   vector<string> names(numPlayers + 1);
   names[0] = "p";
   names[1] = "q";
   names[2] = "(1-p)";
   for (int mark = 0; mark < numMarket; ++mark) {
      if (mark != 0)
         cout << "*";
      if (mySignals[mark] != opSignals[mark])
         cout << "e";
      else {
         const int dNumber = myActions[mark]+opActions[mark];
         if (mySignals[mark] == 0) {
            cout << "("+names[dNumber];
            if (dNumber != 2)
               cout << ")";
            else
               cout << "-2*e)";
         } else if (mySignals[mark] == 1) {
            cout << "(1-" + names[dNumber];
            if (dNumber == 2)
               cout << ")";
            else
               cout << "-2*e)";
         }
      }
   }
}

/*
 * 多市場用
 * 確率を出力
 */
void MultiSignal::viewProb(const vector<int> &myActions, const vector<int> &opActions, const vector<int> &mySignals, const vector<int> &opSignals) {
   const int numMarket = myActions.size();
   vector<string> names(numPlayers + 1);
   for (int pl = 0; pl < numPlayers; ++pl)
      names[pl] = 'p' + pl;
   names[numPlayers] = "(1-p-2*q)";
   for (int mark = 0; mark < numMarket; ++mark) {
      if (mark != 0)
         cout << "*";
      int error = 0;
      if (opActions[mark] != mySignals[mark])
         error++;
      if (myActions[mark] != opSignals[mark])
         error++;
      cout << names[error];
   }
}

/*
 * 多市場用
 * 利得を出力
 */
void MultiSignal::viewPayoff(const vector<int> &myActions, const vector<int> &opActions) {
   const int numMarket = myActions.size();
   for (int mark = 0; mark < numMarket; ++mark) {
      if (mark != 0)
         cout << "+";
      if (myActions[mark] == 0 && opActions[mark] == 0)
         cout << "1";
      else if (myActions[mark] == 1 && opActions[mark] == 0)
         cout << "1+x";
      else if (myActions[mark] == 0 && opActions[mark] == 1)
         cout << "(-y)";
      else if (myActions[mark] == 1 && opActions[mark] == 1)
         cout << "0";
   }
}
