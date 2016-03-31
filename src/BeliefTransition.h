#ifndef BELIEFTRANSITION_H_
#define BELIEFTRANSITION_H_

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#ifndef CDDHANDLER_H_
#include "CddHandler.h"
#endif

#ifndef ENVIRONMENT_H_
#include "Environment.h"
#endif

#ifndef PAYOFF_H_
#include "Payoff.h"
#endif

#ifndef JOINTFSA_H_
#include "JointFSA.h"
#endif

class MissNextBelief{};

typedef vector<vector<PreciseNumber> > BeliefMatrix;

class BeliefTransition {
 private:
    int playerActions;
    int playerSignals;
    int opponentStates;
    int opponentSignals;

    vector<vector<vector<vector<PreciseNumber> > > > beliefTransition;

 public:
    void make(const Environment &env, const Payoff &pd, const vector<Automaton> &pls);
    Belief nextBelief(const Belief &b, const int myAction, const int mySignal) const;
    vector<Belief> nextBeliefSet(const vector<Belief> &bd, const int action, const int signal) const;
    vector<Belief> nextBeliefDivision(const vector<Belief> &bd, const int action, const int signal) const;

    BeliefMatrix createBeliefMatrix(const int opponentStates) const;
    BeliefMatrix nextBeliefMatrix(const BeliefMatrix &bMatrix, const pair<int, int> &lastOb) const;
    Belief nextBeliefFromBeliefMatrix(const Belief &b, const BeliefMatrix &bMatrix) const;

    Belief oldNextBelief(const Environment &env, const vector<Automaton> &ms, const Belief &bp, const int myAction, const int mySignal) const;
    
    void test(const Environment &env, const vector<Automaton> &ms, const Belief &bp) const;
    void test(const Environment &env, const vector<Automaton> &ms, const Belief &bp, const int myAction, const int mySignal) const;
    string toString(const Environment &environment, const vector<Automaton> &ms) const;
};
ostream& operator<<(ostream &os, const Belief& b);
#endif

