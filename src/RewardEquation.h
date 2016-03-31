/*
 * Profile.h
 *
 */

#ifndef REWARDEQUATION_H_
#define REWARDEQUATION_H_

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef EQUATION_H_
#include "Equation.h"
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

#ifndef JOINTFSA_H_
#include "JointFSA.h"
#endif

class RewardEquation {
 private:
    int numPlayers;
    int playerStates;
    int opponentStates;
    int numJointStates;
    vector<Automaton> automatons;
    Environment environment;
    Payoff payoff;
    Equation equation;
    vector<vector<PreciseNumber> > alphaVectors;
    
 public:
    RewardEquation();
    RewardEquation(const Environment& env, const Payoff& po, const vector<Automaton>& ms);
	virtual ~RewardEquation();

    void configure(const Environment& env, const Payoff& po, const vector<Automaton>& ms);
    void solve() ;
    void setRewardEquation() ;
    vector<vector<PreciseNumber> > makeRewardEquation(const int player);
    void makeAlphaVectors() ;
    PreciseNumber getReward(const int state) const ;
    vector< vector<PreciseNumber> > getAlphaVectors() const ;
    
};

#endif /* REWARDEQUATION_H_ */

