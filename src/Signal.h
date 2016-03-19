#ifndef SIGNAL_H_
#define SIGNAL_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef PROFILE_H_
#include "Profile.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

class Signal {
  int numPlayers;
  int numActions;
  int numSignals;
  vector<string> nameActions;

  vector<int> numActionsOfPlayers;
  vector<int> numSignalsOfPlayers;

  int numJointActions;
  int numJointSignals;

 public:
  Signal(const int nPl, const int nAc, const int nSg);
  string actionsToString(const ActionProfile &ap) const;
  int getOpponent(const int pl) const;
  // 三角取引
  void tri(const vector<string> &actionsString);
  void tri_co(const vector<string> &actionsString);
  void tri_payoff(const vector<string> &actionsString);

};
#endif
