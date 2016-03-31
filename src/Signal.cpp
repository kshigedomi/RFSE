#include "Signal.h"

int main(int argc, char *argv[]) {
  if (argc < 5) {
	cout << "USAGE : ./Signal tri numPlayers numActions numSignals [actionStrings]" << endl;
	cout << "USAGE : ./Signal multi numPlayers numActions numSignals numMarket [actionsStrings]" << endl;
	exit(1);
  }
  const string option(argv[1]);
  const int numPlayers = MyUtil::toInteger(string(argv[2]));
  const int numActions = MyUtil::toInteger(string(argv[3]));
  const int numSignals = MyUtil::toInteger(string(argv[4]));
  
  Signal signal(numPlayers, numActions, numSignals);
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

