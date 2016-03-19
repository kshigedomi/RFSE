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

#ifndef PLAYERS_H_
#include "Players.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

class Environment {
	PreciseNumber discountRate;

	int numPlayers;
	int totalNumOfActions;
	int totalNumOfSignals;

	vector<int> numActionsOfPlayer;
	vector<int> numSignalsOfPlayer;

	vector< vector<PreciseNumber> > signalDistribution;
	vector< vector<string> > rawSignalDistribution;

        //   bool existCDOption;
    //	vector<string> rawCorrelationDevice;
    //	vector<PreciseNumber> correlationDevice;

public:
	Environment();
	Environment(const PreciseNumber &dR, const vector<int> &nA, const vector<int> &nS);
	virtual ~Environment();
	void configure(const PreciseNumber &dR, const vector<int> &nA, const vector<int> &nS);
	void setDiscountRate(const PreciseNumber &dR);

	bool setRawSignalDistribution(const ActionProfile& ap, const SignalProfile& sp, const string &pr);
	bool setRawSignalDistribution(int ap, int sp, const string &pr);

	void setSignalDistribution(const ActionProfile &ap, const SignalProfile &sp, const PreciseNumber &pr);
	void setSignalDistribution(int ap, int sp, const PreciseNumber &pr);

	void setEnvironmentFromEnvironment(const Environment &env, const bool transition, const int me);

	bool checkEnvironment(const Players &pls) const;

    //     void setOptionOfCorrelationDevice(bool exists);
    //	bool existCorrelationDevice();
    /*
	void setRawCorrelationDevice(const StateProfile &sp, const string &v);
	void setRawCorrelationDevice(int sp, const string &v);
	void setCorrelationDevice(const StateProfile &sp, const PreciseNumber &v);
	void setCorrelationDevice(int sp, const PreciseNumber &v);
	void setCorrelationDeviceFromRawCorrelationDevice(map<string, PreciseNumber> &variables);
    */
    /*
	vector<string> getRawCorrelationDevice() const;
	vector<PreciseNumber> getCorrelationDevice() const;
	PreciseNumber getCorrelationDevice(const StateProfile &sp) const;
	PreciseNumber getCorrelationDevice(int sp) const;
    */
	PreciseNumber getDiscountRate() const;
	PreciseNumber getSignalDistribution(const ActionProfile &ap, const SignalProfile &sp) const;
	PreciseNumber getSignalDistribution(int ap, int sp) const;
	string getRawSignalDistribution(const ActionProfile &ap, const SignalProfile &sp) const;
	string getRawSignalDistribution(int ap, int sp) const;

	int getNumberOfActionProfiles() const;
	int getNumberOfSignalProfiles() const;
	vector<int> getNumberOfActionsOfPlayer() const;
	vector<int> getNumberOfSignalsOfPlayer() const;

    string toString(const Players &players) const;
	void view(const Players &players) const;
	string signalDistributionToString(const Players &players) const;
	string signalDistributionToString(const ActionProfile &ap, const Players &players) const;

	string rawSignalDistributionToString(const Players &players) const;
	string rawSignalDistributionToString(const ActionProfile &ap, const Players &players) const;

	void viewCorrelationDevice() const;
	void viewRawCorrelationDevice() const;

	void setSignalDistributionFromRawSignalDistribution(map<string, PreciseNumber> &variables);

	const static bool modeDebug = false;
	template<typename T> static void debugTitle(T msg) { if (modeDebug) { cout << "************ " << msg << " **************" << "\n"; } }
	template<typename T> static void debug(T msg) { if (modeDebug) { cout << msg << "\n"; } }
	template<typename T> static void displayTitle(T msg) { cout << "************ " << msg << " **************" << "\n"; }
	template<typename T> static void display(T msg) { cout << msg << "\n"; }
	template<typename T> static void error(T msg) { cerr << msg << "\n"; }
};

#endif /* ENVIRONMENT_H_ */
