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
	this->totalNumOfActions = 0;
	this->numPlayers = 0;
}

Payoff::~Payoff() {
}

Payoff::Payoff(int nP, const vector<int> &nA) {
	this->configure(nP, nA);
}

void Payoff::configure(int nP, const vector<int> &nA) {
	this->numPlayers = nP;
	this->numActionsOfPlayer = nA;
	this->totalNumOfActions = accumulate(this->numActionsOfPlayer.begin(), this->numActionsOfPlayer.end(), 1, multiplies<int>());

	this->payoff.resize(numPlayers);
	this->rawPayoff.resize(numPlayers);
	for (int i = 0; i < numPlayers; i++) {
		this->rawPayoff[i].resize(totalNumOfActions);
		this->payoff[i].resize(totalNumOfActions);
	}
}

void Payoff::setPayoff(const int player, const int ap, const PreciseNumber &reward) {
	this->payoff[player][ap] = reward;
}

void Payoff::setRawPayoff(const int player, const int ap, const string &reward) {
    if (player < this->numPlayers && ap < this->totalNumOfActions)
        this->rawPayoff[player][ap] = reward;
    else
        throw range_error("Payoff::setRawPayoff(" + MyUtil::toString(player) + ", " + MyUtil::toString(ap) + ", " + reward + ")");
}

PreciseNumber Payoff::getPayoff(const int player, const int ap) const{
	return this->payoff[player][ap];
}

string Payoff::getRawPayoff(const int player, const int ap) const{
	return this->rawPayoff[player][ap];
}

/*
* Hint: rawSignalDistribution holds string, for example "1+g", corresponding action profile and signal distribution.
* Input:	variables: mappings string to number like g=-0.5.
*/
void Payoff::setPayoffFromRawPayoff(map<string, PreciseNumber> &variables) {
   for (int player = 0; player < this->numPlayers; ++player) {
      for (int apIndex = 0; apIndex < this->getNumberOfActionProfiles(); ++apIndex) {
          this->setPayoff(player, apIndex, Parser::parseEquation(this->rawPayoff[player][apIndex], variables));
      }
   }
}

int Payoff::getNumberOfActionProfiles() const{
	return this->totalNumOfActions;
}

string Payoff::toString() const {
   const int space = 10;
   string res;
   res += "* Raw Payoff Matrix\n";
   for (int apIndex = 0; apIndex < this->getNumberOfActionProfiles(); ++apIndex) {
      // action
       res += "Action : " + ActionProfile(apIndex).toString() + "\n";
      // RawPayoffMatrix
      res += "(" + MyUtil::spaceToString(this->getRawPayoff(PLAYER, apIndex), space);
      for (int player = OPPONENT; player < this->numPlayers; ++player)
          res += ", " + MyUtil::spaceToString(this->getRawPayoff(player, apIndex), space);
      res += ")\n";
   }
   res += "* Payoff Matrix\n";
   for (int apIndex = 0; apIndex < this->getNumberOfActionProfiles(); ++apIndex) {
      // action
      res += "Action : " + ActionProfile(apIndex).toString() + "\n";
      // PayoffMatrix
      res += "(" + MyUtil::rationalToString(this->getPayoff(PLAYER, apIndex));
      for (int player = OPPONENT; player < this->numPlayers; ++player)
          res += ", " + MyUtil::rationalToString(this->getPayoff(player, apIndex));
      res += ")\n";
   }
   return res;
}
