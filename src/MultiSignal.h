#ifndef MULTISIGNAL_H_
#define MULTISIGNAL_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

class MultiSignal {
  int numPlayers;
  int numMarkets;
  int numActions;
  int numSignals;
  int numAllActions;
  int numAllSignals;
  vector<string> nameActions;


 public:
  MultiSignal(const int nPl, const int nMa, const int nAc, const int nSg);
  void multiMarket(const vector<string> &actionsString);
  void multiMarketPublic(const vector<string> &actionsString);
  void multiMarketPrivateWithPublic(const vector<string> &actionsString);
  void viewAutomaton(const vector<string> &actionsString);
  void viewProb(const vector<int> &myActions, const vector<int> &opActions, const vector<int> &mySignals, const vector<int> &opSignals);
  void viewProbPublic(const vector<int> &myActions, const vector<int> &opActions, const vector<int> &mySignals, const vector<int> &opSignals);
  void viewProbPrivateWithPublic(const vector<int> &myActions, const vector<int> &opActions, const vector<int> &mySignals, const vector<int> &opSignals, const int myMark, const int opMark);
  void viewPayoff(const vector<int> &myActions, const vector<int> &opActions);
};

vector<int> toProfile(int index, const int radix, const int size);
#endif
