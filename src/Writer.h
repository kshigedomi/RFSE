#ifndef WRITER_H_
#define WRITER_H_

/*
 * Writer.h
 *
 *  Created on: 2012. 12. 6.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef BELIEF_H_
#include "Belief.h"
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

#ifndef REPEATEDGAME_H_
#include "RepeatedGame.h"
#endif

class Writer {
	ofstream fout;
	string filename;
public:
	Writer();
	Writer(const string &filename);
	void openAddFileWithFilename(const string &filename);
	void openFileWithFilename(const string &filename);
	virtual ~Writer();
	template<typename T>
	void put(T val) {
		fout << val << endl;
	}

    // write
    void writeVariables(map<string,PreciseNumber> &values);
    void writeEnvironment(const RepeatedGame &rg, const Environment &env,const Payoff &pd,const Players &pls, map<string,PreciseNumber> &values);
    void writePlayers(const Players &pls);
    void writeResult(const Result &result);
    void writeInvariantDistribution(const vector<Belief> &id, Players &pls);
    void writeAveragePayoff(const Environment &env, const RepeatedGame &rg, const Players &pls);
    void writeProfitableOneShot(const vector<Belief> &set, const RepeatedGame &rg, Players &pls);
    void writeOffPathMaxReward(const Belief &b, const RepeatedGame &rg, Players &pls);
    void writeOffPath(const Belief &b, const OneShotExtension &oneShot, const Players &pls, const PreciseNumber &oneShotReward);
    void writeBeliefSpace(const RepeatedGame &rg, Players &pls);
    void writeProfitableOneShot(const Belief &b,const RepeatedGame &rg, Players &pls);
    void writeMaxIteration(const RepeatedGame &rg);
    void writeChangeVariables(const char &start, const char &end);
    void writeEqRange(const PreciseNumber &x, const PreciseNumber &under, const PreciseNumber &over);
	void writeAverageSignal(const Environment &env, const Players &pls, const PreciseNumber &value);
    void writeOptions(const RepeatedGame &rg);
};

#endif /* WRITER_H_ */


