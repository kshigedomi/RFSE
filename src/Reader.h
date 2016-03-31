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

#ifndef UTIL_H_
#include "Util.h"
#endif

class Reader {
 public:

    class ReaderException : public ios_base::failure {
 public:
 ReaderException(const string& tmp) : ios_base::failure(tmp){}
 };
 private:
    
 ifstream fin;

 vector<string> rawCorrelationDevice;
 int numPlayers;

 vector<int> numStatesOfPlayer;
 vector<int> numActionsOfPlayer;
 vector<int> numSignalsOfPlayer;

 public:
 Reader();
 Reader(const string &nameDataFile);
 virtual ~Reader();

 void read(string &tit, Environment &env, Payoff &po, vector<Automaton>& ms, map<string, PreciseNumber>& variables);
 ifstream &readLine(ifstream &fin, string &s);

 void readTitle(string &title);
 void readDiscountRate(Environment &env);
 void readPlayers(vector<string> &playerNames);
 void readVariables(map<string, PreciseNumber> &varaibles);
 void setVariable(const string &name, const PreciseNumber &value);
 void readAutomaton(const string &namePlayer, Automaton &am);
 void readItem(const string &tag, vector<string> &data, bool startFirst = false);
 //	void readPayoffMatrix(const Players &pls, Payoff &po);
 void readPayoffMatrix(Payoff &po, const vector<Automaton> &ms);
 void readSignalDistribution(Environment &env, const vector<Automaton> &pls);
 void readExcelSignalDistribution(Environment &env, const vector<Automaton> &pls);
 void readCorrelationDevice(Environment &env);

 map<string, PreciseNumber> getVariables() const;

};

#endif /* READER_H_ */
