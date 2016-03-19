#ifndef READER_H_
#define READER_H_

/*
 * IO.h
 *
 *  Created on: 2012. 12. 4.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef ENVIRONMENT_H_
#include "Environment.h"
#endif

#ifndef PAYOFF_H_
#include "Payoff.h"
#endif

#ifndef PLAYERS_H_
#include "Players.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

enum READSTATUS {
	NOERROR = 0,
	FILENOTFOUND = 1,
	ITEMNOTFOUND = 2,
	SYNTAXERROR = 3,
    ERROR = 4,
};

class Reader {
private:
	ifstream fin;

	string title;
	PreciseNumber discountRate;
	vector<string> players;
	map<string, PreciseNumber> variables;
	vector<string> rawCorrelationDevice;
	int numPlayers;

	vector<int> numStatesOfPlayer;
	vector<int> numActionsOfPlayer;
	vector<int> numSignalsOfPlayer;

public:
	Reader();
	Reader(const string &nameDataFile);
	virtual ~Reader();

	READSTATUS read(string &tit, Environment &env, Payoff &po, Players &pls);
	ifstream &readLine(ifstream &fin, string &s);

	READSTATUS readTitle();
	READSTATUS readDiscountRate();
	READSTATUS readPlayers();
	READSTATUS readVariables();
	void setVariable(const string &name, const PreciseNumber &value);
	READSTATUS readAutomaton(const string &namePlayer, Automaton &am);
	READSTATUS readItem(const string &tag, vector<string> &data, bool startFirst = false);
	//	READSTATUS readPayoffMatrix(const Players &pls, Payoff &po);
	READSTATUS readPayoffMatrix(const Players &pls, Payoff &po);
	READSTATUS readSignalDistribution(Environment &env, const Players &pls);
	READSTATUS readExcelSignalDistribution(Environment &env, const Players &pls);
	READSTATUS readCorrelationDevice(Environment &env);
	int getIndexOfPlayers(const string &player);

	map<string, PreciseNumber> getVariables() const;

};

#endif /* READER_H_ */
