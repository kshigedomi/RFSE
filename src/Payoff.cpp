/*
 * Payoff.cpp
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

/*
 * This class has payoff. It associates with two value. First represents player. Second represents action profiles.
 * It also has the number of players and total actions.
 * It also has each player's number of actions.
 */

#include "Payoff.h"

Payoff::Payoff() {
	totalNumOfActions = 0;
	numPlayers = 0;
}

Payoff::~Payoff() {
}

void Payoff::setPayoffFromPayoff(const Payoff &po, const bool transition, const int me) {
   *this = po;

   if (transition) {
      swap(numActionsOfPlayer[me], numActionsOfPlayer[PLAYER]);
      for (int apIndex = 0; apIndex < getNumberOfActionProfiles(); ++apIndex) {
         // me == PLAYER(past)
         setPayoff(me, apIndex, po.getPayoff(PLAYER, apIndex));
         setRawPayoff(me, apIndex, po.getRawPayoff(PLAYER, apIndex));
         // PLAYER == me(past)
         setPayoff(PLAYER, apIndex, po.getPayoff(me, apIndex));
         setRawPayoff(PLAYER, apIndex, po.getRawPayoff(me, apIndex));
      }
   }
}

Payoff::Payoff(int nP, const vector<int> &nA) {
	configure(nP, nA);
}

void Payoff::configure(int nP, const vector<int> &nA) {
	numPlayers = nP;
	numActionsOfPlayer = nA;
	totalNumOfActions = accumulate(numActionsOfPlayer.begin(), numActionsOfPlayer.end(), 1, multiplies<int>());

	payoff.resize(numPlayers);
	rawPayoff.resize(numPlayers);
	for (int i = 0; i < numPlayers; i++) {
		rawPayoff[i].resize(totalNumOfActions);
		payoff[i].resize(totalNumOfActions);
	}
}

void Payoff::setPayoff(int player, int ap, const PreciseNumber &reward) {
	payoff[player][ap] = reward;
}

void Payoff::setPayoff(int player, const ActionProfile &ap, const PreciseNumber &reward) {
	setPayoff(player, ap.getIndex(numActionsOfPlayer), reward);
}

void Payoff::setRawPayoff(int player, int ap, const string &reward) {
	rawPayoff[player][ap] = reward;
}

void Payoff::setRawPayoff(int player, const ActionProfile &ap, const string &reward) {
	setRawPayoff(player, ap.getIndex(numActionsOfPlayer), reward);
}

PreciseNumber Payoff::getPayoff(int player, int ap) const{
	return payoff[player][ap];
}

PreciseNumber Payoff::getPayoff(int player, const ActionProfile &ap) const{
	return getPayoff(player, ap.getIndex(numActionsOfPlayer));
}

string Payoff::getRawPayoff(int player, const ActionProfile &ap) const{
	return getRawPayoff(player, ap.getIndex(numActionsOfPlayer));
}

string Payoff::getRawPayoff(int player, int ap) const{
	return rawPayoff[player][ap];
}

/*
* Hint: rawSignalDistribution holds string, for example "1+g", corresponding action profile and signal distribution.
* Input:	variables: mappings string to number like g=-0.5.
*/
void Payoff::setPayoffFromRawPayoff(map<string, PreciseNumber> &variables) {
   for (int player = 0; player < numPlayers; ++player) {
      for (int apIndex = 0; apIndex < getNumberOfActionProfiles(); ++apIndex) {
         setPayoff(player, apIndex, parseEquation(rawPayoff[player][apIndex], variables));
      }
   }
}

int Payoff::getNumberOfActionProfiles() const{
	return totalNumOfActions;
}

void Payoff::view(const Players &players) const{
   cout << toString(players) << endl;
}

string Payoff::toString(const Players &players) const {
   const int space = 10;
   string res;
   res += "* Raw Payoff Matrix\n";
   for (int apIndex = 0; apIndex < getNumberOfActionProfiles(); ++apIndex) {
      // action
      const ActionProfile ap(apIndex, numActionsOfPlayer);
      res += "Action : (" + players.getNameOfActions(PLAYER, ap[PLAYER]);
      for (int pl = OPPONENT; pl < numPlayers; ++pl)
         res += ", " + players.getNameOfActions(pl, ap[pl]);
      res += ")\n";
      // RawPayoffMatrix
      res += "(" + spaceToString(getRawPayoff(PLAYER, apIndex), space);
      for (int player = OPPONENT; player < numPlayers; ++player)
         res += ", " + spaceToString(getRawPayoff(player, apIndex), space);
      res += ")\n";
   }
   res += "* Payoff Matrix\n";
   for (int apIndex = 0; apIndex < getNumberOfActionProfiles(); ++apIndex) {
      // action
      const ActionProfile ap(apIndex, numActionsOfPlayer);
      res += "Action : (" + players.getNameOfActions(PLAYER, ap[PLAYER]);
      for (int pl = OPPONENT; pl < numPlayers; ++pl)
         res += ", " + players.getNameOfActions(pl, ap[pl]);
      res += ")\n";
      // PayoffMatrix
      res += "(" + rationalToString(getPayoff(PLAYER, apIndex));
      for (int player = OPPONENT; player < numPlayers; ++player)
         res += ", " + rationalToString(getPayoff(player, apIndex));
      res += ")\n";
   }
   return res;
}
