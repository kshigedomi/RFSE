#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

/*
 * Environment.h
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */


#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef PARSER_H_
#include "Parser.h"
#endif

#ifndef PROFILE_H_
#include "Profile.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

class Environment {
 public:
    class EnvironmentException : public runtime_error {
 public:
 EnvironmentException(const string& tmp) : runtime_error(tmp){};
 };
	PreciseNumber discountRate;

	int numPlayers;
	int totalNumOfActions;
	int totalNumOfSignals;

	vector<int> numActionsOfPlayer;
	vector<int> numSignalsOfPlayer;

	vector< vector<PreciseNumber> > signalDistribution;
	vector< vector<string> > rawSignalDistribution;

public:
    
	Environment();
	Environment(const vector<int> &nA, const vector<int> &nS);
	virtual ~Environment();
	void configure(const vector<int> &nA, const vector<int> &nS);
	void setDiscountRate(const PreciseNumber &dR);

	bool setRawSignalDistribution(const int ap, const int sp, const string &pr);
	void setSignalDistribution(const int ap, const int sp, const PreciseNumber &pr);

    bool isValidEnvironment( void ) const ;
	void checkEnvironment( void ) const;

	PreciseNumber getDiscountRate() const;
	string getRawSignalDistribution(const int ap, const int sp) const;
	PreciseNumber getSignalDistribution(const int ap, const int sp) const;

	int getNumberOfActionProfiles() const;
	int getNumberOfSignalProfiles() const;
	vector<int> getNumberOfActionsOfPlayer() const;
	vector<int> getNumberOfSignalsOfPlayer() const;

    string toString() const;
	string signalDistributionToString() const;
	string signalDistributionToString(const ActionProfile &ap) const;

	string rawSignalDistributionToString() const;
	string rawSignalDistributionToString(const ActionProfile &ap) const;

	void setSignalDistributionFromRawSignalDistribution(const map<string, PreciseNumber> &variables);

};

#endif /* ENVIRONMENT_H_ */
