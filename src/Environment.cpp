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
	this->numPlayers = 0;
	this->totalNumOfActions = 0;
	this->totalNumOfSignals = 0;
    // existCDOption = false;
}

Environment::Environment(const vector<int> &nA, const vector<int> &nS) {
	this->configure(nA, nS);
}

Environment::~Environment() {
}

bool Environment::isValidEnvironment( void ) const {
	PreciseNumber sum = ZERO;
    ActionProfile ap;
    
    for (int apIndex = 0; apIndex < this->getNumberOfActionProfiles(); ++apIndex) {
        sum = ZERO;
        for (int spIndex = 0; spIndex < this->getNumberOfSignalProfiles(); ++spIndex) {
            const PreciseNumber prob = this->getSignalDistribution(apIndex, spIndex);
            if (prob <= ZERO  || prob >= ONE ) {
                return false;
            }
            sum += prob;
        }
        if (!MyUtil::equal(sum, ONE)) {
            return false;
        }
    }
    return true;
}

/*
* check validity at signal distribution and correlation devise if exists.
*/
void Environment::checkEnvironment( void ) const {
	PreciseNumber sum = ZERO;
    ActionProfile ap;
    
    for (int apIndex = 0; apIndex < this->getNumberOfActionProfiles(); ++apIndex) {
        sum = ZERO;
        for (int spIndex = 0; spIndex < this->getNumberOfSignalProfiles(); ++spIndex) {
            const PreciseNumber prob = this->getSignalDistribution(apIndex, spIndex);
            if (prob <= ZERO  || prob >= ONE ) {
                throw EnvironmentException("Signal Distiribution Error\n"
                                           "Actions : " + ActionProfile(apIndex).toString() + "\n"
                                           "Signals : " + SignalProfile(spIndex).toString() + "\n"
                                           "Prob : " + MyUtil::rationalToString(prob) + "\n"
                                           "RawProb : " + this->getRawSignalDistribution(apIndex, spIndex));
            }
            sum += prob;
        }
        if (!MyUtil::equal(sum, ONE)) {
            throw EnvironmentException("Signal Distiribution Error\n"
                                       "Actions : " + ActionProfile(apIndex).toString() + "\n"
                                       "Sum : " + MyUtil::rationalToString(sum) + "\n"
                                       "RawProb : " + this->getRawSignalDistribution(0, 0));
        }
    }
}

void Environment::configure(const vector<int> &nA, const vector<int> &nS) {
	this->numPlayers = nA.size();
	this->numActionsOfPlayer = nA;
	this->numSignalsOfPlayer = nS;

	this->totalNumOfActions = accumulate(numActionsOfPlayer.begin(), numActionsOfPlayer.end(), 1, multiplies<int>());
	this->totalNumOfSignals = accumulate(numSignalsOfPlayer.begin(), numSignalsOfPlayer.end(), 1, multiplies<int>());

	this->signalDistribution.resize(totalNumOfActions);
    this->rawSignalDistribution.resize(totalNumOfActions);

	for (int i = 0; i < totalNumOfActions; i++) {
		this->signalDistribution[i].resize(this->totalNumOfSignals);
		this->rawSignalDistribution[i].resize(this->totalNumOfSignals);
	}
}

void Environment::setDiscountRate(const PreciseNumber &dR) {
	this->discountRate = dR;
}

void Environment::setSignalDistribution(int ap, int sp, const PreciseNumber &pr) {
	this->signalDistribution[ap][sp] = pr;
}

bool Environment::setRawSignalDistribution(const int ap, const int sp, const string &pr) {
    if (ap < this->rawSignalDistribution.size() && sp < this->rawSignalDistribution[ap].size()) { // 型違い warning は仕方ない
        this->rawSignalDistribution[ap][sp] = pr;
    } else {
        throw range_error("Environment::setRawSignalDistribution(" + MyUtil::toString(ap) + ", " + MyUtil::toString(sp) + ", " + pr + ")");
    }
}

PreciseNumber Environment::getDiscountRate() const {
	return this->discountRate;
}

PreciseNumber Environment::getSignalDistribution(const int ap, const int sp) const {
	return this->signalDistribution[ap][sp];
}

string Environment::getRawSignalDistribution(const int ap, const int sp) const {
	return this->rawSignalDistribution[ap][sp];
}

int Environment::getNumberOfActionProfiles() const {
	return this->totalNumOfActions;
}

int Environment::getNumberOfSignalProfiles() const {
	return this->totalNumOfSignals;
}

string Environment::toString( void ) const {
    string s("");
   s += "* Discount rate : " + MyUtil::rationalToString(discountRate) + "\n";
   s += "* Raw signal distribution\n";
   s += this->rawSignalDistributionToString();
   s += "* Signal distribution\n";
   s += this->signalDistributionToString();
   return s;
}

string Environment::rawSignalDistributionToString( void ) const{
   string res;
   ActionProfile ap;
   do {
      res += this->rawSignalDistributionToString(ap);
   } while (ap.next());
   return res;
}

string Environment::rawSignalDistributionToString(const ActionProfile &ap) const{
    const int space = 10;
    // Action
    string res = "Action : " + ap.toString() + "\n" ;
    // Raw Signal Distribution
    SignalProfile sp;
    do {
        const string rawProb = this->getRawSignalDistribution(ap.getIndex(), sp.getIndex());
        res += "** Signal : " + sp.toString() + " " + MyUtil::spaceToString(rawProb, space) + "\n";
    } while (sp.next());
    return res;
}

string Environment::signalDistributionToString( void ) const{
   string res;
   ActionProfile ap;
   do {
       res += this->signalDistributionToString(ap);
   } while (ap.next());
   return res;
}
string Environment::signalDistributionToString(const ActionProfile &ap) const{
   string res = "";
   // Action
   res += "Action : " + ap.toString() + "\n" ;
   // Signal Distribution
   SignalProfile sp;
   do {
       res += "** Signal : " + sp.toString()
           + MyUtil::rationalToString(getSignalDistribution(ap.getIndex(), sp.getIndex())) + "\n";
   } while (sp.next());
   return res;
}

/*
* Hint: rawSignalDistribution holds string, for example "1-p-2*q", corresponding action profile and signal distribution.
* Input:	variables: mappings string to number like p=0.95.
*/
void Environment::setSignalDistributionFromRawSignalDistribution(const map<string, PreciseNumber> &variables){
    for (int apIndex = 0; apIndex < this->getNumberOfActionProfiles(); ++apIndex) {
        for (int spIndex = 0; spIndex < this->getNumberOfSignalProfiles(); ++spIndex) {
            const string rawProb = this->getRawSignalDistribution(apIndex, spIndex);
            this->setSignalDistribution(apIndex, spIndex, Parser::parseEquation(rawProb, variables));
        }
    }
}


