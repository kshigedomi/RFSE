#ifndef PAYOFF_H_
#define PAYOFF_H_

/*
 * Payoff.h
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

class Payoff {
	int numPlayers;
	int totalNumOfActions;
	vector<int> numActionsOfPlayer;

	vector< vector<PreciseNumber> > payoff;
	vector< vector<string> > rawPayoff;

public:
	Payoff();
	Payoff(int nP, const vector<int> &nA);
	virtual ~Payoff();
	void configure(int nP, const vector<int> &nA);

	void setRawPayoff(int player, int ap, const string &reward);

	void setPayoff(int player, int ap, const PreciseNumber &reward);

	void setPayoffFromRawPayoff(map<string, PreciseNumber> &variables);
	void setPayoffFromPayoff(const Payoff &po, bool transition, const int me);

	PreciseNumber getPayoff(int player, int ap) const;

	string getRawPayoff(int player, int ap) const;

	int getNumberOfActionProfiles() const;

    string toString() const;

	const static bool modeDebug = true;
	template<typename T> static void debugTitle(T msg) { if (modeDebug) { cout << "************ " << msg << " **************" << endl; } }
	template<typename T> static void debug(T msg) { if (modeDebug) { cout << msg << endl; } }
	template<typename T> static void displayTitle(T msg) { cout << "************ " << msg << " **************" << endl; }
	template<typename T>	static void display(T msg) { cout << msg << endl; }
	template<typename T> static void error(T msg) { cerr << msg << endl; }
};

#endif /* PAYOFF_H_ */
