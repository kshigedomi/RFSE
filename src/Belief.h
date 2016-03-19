/*
 * Belief.h
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#ifndef BELIEF_H_
#define BELIEF_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

class Belief {
public:
	int dimension;
	vector<PreciseNumber> belief;

	Belief();
	Belief(const int dimension);
    Belief(const int dimension, const PreciseNumber &value);
    Belief(const vector<PreciseNumber> &vec);
	virtual ~Belief();

	int getDimension() const;
	void configure(const int dimension);
	PreciseNumber get(const int index) const;
	void set(int index, PreciseNumber value);
    vector<PreciseNumber> getVector() const;
	bool checkValidity() const;
    string toString() const;
};

PreciseNumber getRewardFromAlphaVectorAndBelief(const vector<PreciseNumber> &alphaVector, const Belief &b);

pair<int, PreciseNumber> getMaxIndexAndReward(const Belief &b, const vector<vector<PreciseNumber> > &ex);
PreciseNumber getMaxReward(const Belief &b, const vector<vector<PreciseNumber> > &ex);

bool operator < (const Belief& lhs, const Belief& rhs);
bool operator > (const Belief& lhs, const Belief& rhs);
bool operator == (const Belief& lhs, const Belief& rhs);
bool operator != (const Belief& lhs, const Belief& rhs);
PreciseNumber operator - (const Belief& lhs, const Belief& rhs);
ostream& operator<<(ostream &os, const Belief& b);
vector<Belief> makeBeliefSpace(const int opStates);
vector<string> gnuplotString(const vector<Belief> &poly);
vector<string> gnuplotString2(const vector<Belief> &poly);
//PreciseNumber dist(const Belief &lhs, const Belief& rhs) ;
#endif /* BELIEF_H_ */
