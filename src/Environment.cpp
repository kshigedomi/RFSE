/*
 * Environment.cpp
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

/*
 * This class holds some factors of the situation.
 * It has discount rate.
 * It also has the number of players, total states, total actions, and total signals.
 * It also has each player's number of states, actions, and signals.
 * It also has signal distribution corresponding to actions and signals.
 * It also has correlation device. If it is not defined by input file, invariant distribution is used instead. So, it has boolean whether correalation device exists or not.
 * It has a method to reverse index named setEnvironmentFromEnvironment, because we need to run twice in same environment if player1's automaton is different from player2's automaton.
 */

#include "Environment.h"

Environment::Environment() {
	numPlayers = 0;
	totalNumOfActions = 0;
	totalNumOfSignals = 0;
    // existCDOption = false;
}

Environment::Environment(const PreciseNumber &dR, const vector<int> &nA, const vector<int> &nS) {
	configure(dR, nA, nS);
}

Environment::~Environment() {
}

/*
* check validity at signal distribution and correlation devise if exists.
*/
bool Environment::checkEnvironment(const Players& pls) const {
	PreciseNumber sum = ZERO;
    for (int apIndex = 0; apIndex < getNumberOfActionProfiles(); ++apIndex) {
       sum = ZERO;
       for (int spIndex = 0; spIndex < getNumberOfSignalProfiles(); ++spIndex) {
          const PreciseNumber prob = getSignalDistribution(apIndex, spIndex);
		  if (prob < ZERO  || prob > ONE ) {
			 cout << "ERROR: Signal Distribution Error " << endl;
			 cout << "Action: " + pls.actionProfileToString(pls.getActionProfile(apIndex)) << endl;
			 cout << "Signal: " + pls.signalProfileToString(pls.getSignalProfile(spIndex)) << endl;
			 cout << "Prob: " + rationalToString(prob)<< endl;
			 cout << "RawProb: " + getRawSignalDistribution(apIndex, spIndex) << endl;
			return false;
          }
          sum += prob;
       }
       if (!equal(sum, ONE)) {
           cout << "ERROR: Signal Distribution Error" << endl;
           cout << "Action : " + pls.actionProfileToString(pls.getActionProfile(apIndex)) << endl;
           cout << "Sum : " + rationalToString(sum) << endl;
		cout << "RawProb: " + getRawSignalDistribution(0,0) << endl;
          return false;
       }
    }
    return true;
}

void Environment::configure(const PreciseNumber &dR, const vector<int> &nA, const vector<int> &nS) {
	discountRate = dR;
	numPlayers = nA.size();
	numActionsOfPlayer = nA;
	numSignalsOfPlayer = nS;

	totalNumOfActions = accumulate(numActionsOfPlayer.begin(), numActionsOfPlayer.end(), 1, multiplies<int>());
	totalNumOfSignals = accumulate(numSignalsOfPlayer.begin(), numSignalsOfPlayer.end(), 1, multiplies<int>());

	signalDistribution.resize(totalNumOfActions);
	rawSignalDistribution.resize(totalNumOfActions);

	for (int i = 0; i < totalNumOfActions; i++) {
		signalDistribution[i].resize(totalNumOfSignals);
		rawSignalDistribution[i].resize(totalNumOfSignals);
	}
}

void Environment::setDiscountRate(const PreciseNumber &dR) {
	discountRate = dR;
}

void Environment::setSignalDistribution(const ActionProfile &ap, const SignalProfile &sp, const PreciseNumber &pr) {
	setSignalDistribution(ap.getIndex(numActionsOfPlayer), sp.getIndex(numSignalsOfPlayer), pr);
}

void Environment::setSignalDistribution(int ap, int sp, const PreciseNumber &pr) {
	signalDistribution[ap][sp] = pr;
}

bool Environment::setRawSignalDistribution(const ActionProfile &ap, const SignalProfile &sp, const string &pr) {
	return setRawSignalDistribution(ap.getIndex(numActionsOfPlayer), sp.getIndex(numSignalsOfPlayer), pr);
}

bool Environment::setRawSignalDistribution(int ap, int sp, const string &pr) {
   if (ap < rawSignalDistribution.size() && sp < rawSignalDistribution[ap].size()) { // 型違い warning は仕方ない
	rawSignalDistribution[ap][sp] = pr;
	return true;
  } else {
     cout << ap << endl;
     cout << rawSignalDistribution.size() << endl;
     cout << sp << endl;
     cout << rawSignalDistribution[ap].size() << endl;
	return false;
  }
}

void Environment::setEnvironmentFromEnvironment(const Environment &env, const bool transition, const int me) {
	*this = env;
	if (transition) {
       swap(numActionsOfPlayer[PLAYER], numActionsOfPlayer[me]);
       swap(numSignalsOfPlayer[PLAYER], numSignalsOfPlayer[me]);
       for (int apIndex = 0; apIndex < getNumberOfActionProfiles(); ++apIndex) {
          ActionProfile pastAp(apIndex, numActionsOfPlayer);
          pastAp.swap(PLAYER, me);
          for (int spIndex = 0; spIndex < getNumberOfSignalProfiles(); ++spIndex) {
             SignalProfile pastSp(spIndex, numSignalsOfPlayer);
             pastSp.swap(PLAYER, me);
             PreciseNumber prob = env.getSignalDistribution(pastAp, pastSp);
             string rawProb = env.getRawSignalDistribution(pastAp, pastSp);
             setSignalDistribution(apIndex, spIndex, prob);
             setRawSignalDistribution(apIndex, spIndex, rawProb);
          }
       }
    }
}
             

vector<int> Environment::getNumberOfActionsOfPlayer() const {
	return numActionsOfPlayer;
}

vector<int> Environment::getNumberOfSignalsOfPlayer() const {
	return numSignalsOfPlayer;
}


PreciseNumber Environment::getDiscountRate() const {
	return discountRate;
}

PreciseNumber Environment::getSignalDistribution(const ActionProfile &ap, const SignalProfile &sp) const {
	return getSignalDistribution(ap.getIndex(numActionsOfPlayer), sp.getIndex(numSignalsOfPlayer));
}

PreciseNumber Environment::getSignalDistribution(int ap, int sp) const {
	return signalDistribution[ap][sp];
}

string Environment::getRawSignalDistribution(const ActionProfile &ap, const SignalProfile &sp) const {
	return getRawSignalDistribution(ap.getIndex(numActionsOfPlayer), sp.getIndex(numSignalsOfPlayer));
}

string Environment::getRawSignalDistribution(int ap, int sp) const {
	return rawSignalDistribution[ap][sp];
}

int Environment::getNumberOfActionProfiles() const {
	return totalNumOfActions;
}

int Environment::getNumberOfSignalProfiles() const {
	return totalNumOfSignals;
}
/*
int Environment::getNumberOfStateProfiles() const {
	return totalNumOfStates;
}
vector<int> Environment::getNumberOfStatesOfPlayer() const {
	return numStatesOfPlayer;
}
*/
string Environment::toString(const Players &players) const {
   string s;
   s += "* Discount rate : " + rationalToString(discountRate) + "\n";
   s += "* Raw signal distribution\n";
   s += rawSignalDistributionToString(players);
   s += "* Signal distribution\n";
   s += signalDistributionToString(players);
   return s;
}

void Environment::view(const Players &players) const {
   cout << toString(players) << endl;
}

string Environment::rawSignalDistributionToString(const Players &players) const{
   string res;
   for (int apIndex = 0; apIndex < getNumberOfActionProfiles(); ++apIndex) {
      res += rawSignalDistributionToString(ActionProfile(apIndex, numActionsOfPlayer), players);
   }
   return res;
}

string Environment::rawSignalDistributionToString(const ActionProfile &ap, const Players &players) const{
   const int space = 10;
   const int apIndex = ap.getIndex(numActionsOfPlayer);
   // Action
   string res = "Action : (" + players.getNameOfActions(PLAYER, ap[0]);
   for (int pl = OPPONENT; pl < numPlayers; ++pl) 
      res += ", " + players.getNameOfActions(pl,ap[pl]);
   res += ")\n";
   // Raw Signal Distribution
   for (int spIndex = 0; spIndex < getNumberOfSignalProfiles(); ++spIndex) {
      const SignalProfile sp = players.getSignalProfile(spIndex);
      res += "** Signal : (" + players.getNameOfSignals(PLAYER, sp[PLAYER]);
      for (int pl = OPPONENT; pl < players.getNumberOfPlayers(); ++pl)
         res += ", " + players.getNameOfSignals(pl, sp[pl]);
      res += ")  ";
      const string rawProb = getRawSignalDistribution(apIndex, spIndex);
      res += spaceToString(rawProb, space) + "\n";
   }
   return res;
}

string Environment::signalDistributionToString(const Players &players) const{
   string res;
   for (int apIndex = 0; apIndex < getNumberOfActionProfiles(); ++apIndex) {
      res += signalDistributionToString(ActionProfile(apIndex, numActionsOfPlayer), players);
   }
   return res;
}
string Environment::signalDistributionToString(const ActionProfile &ap, const Players &players) const{
   string res;
   const int apIndex = ap.getIndex(numActionsOfPlayer);
   // Action
   res += "Action : " + players.getNameOfActions(PLAYER, ap[0]);
   for (int player = OPPONENT; player < numPlayers; ++player)
      res += ", " + players.getNameOfActions(player, ap[player]);
   res += "\n";
   // Signal Distribution
   for (int spIndex = 0; spIndex < getNumberOfSignalProfiles(); ++spIndex) {
      const SignalProfile sp = players.getSignalProfile(spIndex);
      res += "** Signal : (" + players.getNameOfSignals(PLAYER, sp[PLAYER]);
      for (int pl = OPPONENT; pl < players.getNumberOfPlayers(); ++pl)
         res += ", " + players.getNameOfSignals(pl, sp[pl]);
      res += ")  ";
      res += rationalToString(getSignalDistribution(apIndex, spIndex)) + "\n";
   }
   return res;
}

/*
* Hint: rawSignalDistribution holds string, for example "1-p-2*q", corresponding action profile and signal distribution.
* Input:	variables: mappings string to number like p=0.95.
*/
void Environment::setSignalDistributionFromRawSignalDistribution(map<string, PreciseNumber> &variables){
   for (int apIndex = 0; apIndex < getNumberOfActionProfiles(); ++apIndex) {
      for (int spIndex = 0; spIndex < getNumberOfSignalProfiles(); ++spIndex) {
         string rawProb = getRawSignalDistribution(apIndex, spIndex);
         setSignalDistribution(apIndex, spIndex, parseEquation(rawProb, variables));
      }
   }
}

/*
void Environment::setRawCorrelationDevice(StateProfile sp, string v) {
	setRawCorrelationDevice(sp.getIndex(numStatesOfPlayer), v);
}

void Environment::setRawCorrelationDevice(int sp, string v) {
	rawCorrelationDevice[sp] = v;
}

vector<string> Environment::getRawCorrelationDevice() {
	return rawCorrelationDevice;
}

void Environment::setCorrelationDevice(StateProfile sp, PreciseNumber v) {
	setCorrelationDevice(sp.getIndex(numStatesOfPlayer), v);
}

void Environment::setCorrelationDevice(int sp, PreciseNumber v) {
	correlationDevice[sp] = v;
}

vector<PreciseNumber> Environment::getCorrelationDevice() {
	return correlationDevice;
}
void Environment::setCorrelationDeviceFromRawCorrelationDevice(map<string, PreciseNumber> variables) {
	for (int i = 0; i < numStatesOfPlayer[0]; i++) {
		for (int j = 0; j < numStatesOfPlayer[1]; j++) {
			StateProfile sp(i, j);
			setCorrelationDevice(sp, parseEquation(rawCorrelationDevice[sp.getIndex(numStatesOfPlayer)], variables));
		}
	}
}
*/


