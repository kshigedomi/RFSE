#ifndef SCORE_H_
#define SCORE_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#ifndef BELIEFDIVISION_H_
#include "BeliefDivision.h"
#endif

#include "setoper.h"
#include "cdd.h"

PreciseNumber calcAveManhattanDistance(const vector<Belief> &points, const BeliefDivision &target);
PreciseNumber calcManhattanDistance(const Belief &point, const BeliefDivision &target);
PreciseNumber calcManhattanDistanceWithOneDivision(const Belief &point,const Inequality &polytope);
PreciseNumber calcDistance(const Belief &point,const Inequality &polytope);

vector<Belief> getSampleBeliefs(const int numOpStates);
PreciseNumber calcAveGapBetweenRewards(const vector<Belief> &samples, const vector<vector<PreciseNumber> > &alphaVectors, const vector<OneShotExtension> &oneShotExtensions);
PreciseNumber maxRewardWithBelief(const vector < vector <PreciseNumber> > &vectors,const Belief &b);

#endif
