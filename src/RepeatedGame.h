#ifndef REPEATEDGAME_H_
#define REPEATEDGAME_H_

/*
 * RepeatedGame.h
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#ifndef EQUATION_H_
#include "Equation.h"
#endif

#ifndef BELIEFDIVISION_H_
#include "BeliefDivision.h"
#endif

#ifndef ENVIRONMENT_H_
#include "Environment.h"
#endif

#ifndef PARSER_H_
#include "Parser.h"
#endif

#ifndef PAYOFF_H_
#include "Payoff.h"
#endif

#ifndef REWARDEQUATION_H_
#include "RewardEquation.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

#ifndef JOINTFSA_H_
#include "JointFSA.h"
#endif

#ifndef BELIERTRANSITION_H_
#include "BeliefTransition.h"
#endif

#ifndef CDDHANDLER_H_
#include "CddHandler.h"
#endif

class MissCreateInvariantDistribution{};
class MissIteration{};

enum PreProcess {
  TRUE,NOFIX,FALSE,
};

struct IterationInfo {
  pair<int, int> lastOb; //last observation
  BeliefMatrix bMatrix;
};
  

class RepeatedGame {
  bool history_flag;
  bool preprocess_view_flag;
  bool target_flag;
  bool preprocess_flag;
  bool pure_flag;
  bool timeover_flag;
  bool beliefTransition_view_flag;
  bool no_fixed_flag;
  bool global_flag;
  bool nobd_flag;
  
  int numPlayers;
  int playerStates;
  int playerActions;
  int playerSignals;
  int opponentStates;
  int opponentSignals;

  int numJointStates;
  int numJointActions;
  int numJointSignals;

  Environment environment;
  Payoff payoff;
  vector<Automaton> automatons;
  RewardEquation rewardEquation;

  vector<Belief> invariantDistribution;
  BeliefDivision initialBeliefDivision;
  BeliefDivision targetBeliefDivision;

  vector<Belief> failure;
  vector<vector<PreciseNumber> > alphaVectors;

  int maxIteration;

  BeliefTransition beliefTransition;

  Belief fixedPoint;

 public:

  long count_test ;

  map<string, PreciseNumber> globalVar;

  RepeatedGame();
  RepeatedGame(const Environment &env, const Payoff &po, const vector<Automaton> &ms, const RewardEquation& eq);
  virtual ~RepeatedGame();

  void configure(const Environment &env, const Payoff &po, const vector<Automaton> &ms, const RewardEquation& eq);

  vector<vector<PreciseNumber> > getAlphaVectors() const;

  PreciseNumber getReward(const int sp) const;

  void solve();

  vector<OneShotExtension> makeOneShotExtensions();
  void makeBeliefDivision();
  void makeTargetBeliefDivision();

  BeliefDivision getBeliefDivision() const;
  BeliefDivision getTargetBeliefDivision() const;

  void viewBeliefTransition();

  bool checkSeminalBelief(const Belief &b) const;
  bool checkSeminalBeliefSet(const vector<Belief> &b) const;
  bool checkSeminalBeliefDivision(const BeliefDivision &closed) const;
  pair<OneShotExtension, PreciseNumber> getMostProfitableOneShotWithAction(const int myAction, const Belief &b) const;

  Result checkGlobalResilent();
  Result checkNoEquilibrium();
  Result checkNoEquilibriumWithInvariant();
  Result checkResilent();
  Result checkResilentFromBelief(const Belief &initial);
  Result checkResilentFromPureStrategy();

  vector<Belief> getInvariantDistribution() const;
  vector<PreciseNumber> makeInvariantDistribution();

  bool isInnerPointOfInitialDivision(const Belief &b, const int division) const;
  bool isInnerPointOfDivision(const Belief &b, const int division) const;
    
  int getMaxIteration() const;
  vector<Belief> getFailure() const;
  PreProcess makeFixedPoint();
  Belief getFixedPoint() const;

  void test();

  bool preProcess(const Belief &b);

  bool isPolytopeInTargetBeliefDivision(const vector<Belief> &polytope);
  bool isPolytopeInTargetBeliefDivisionWithBeliefDivision(const vector<Belief> &polytope);

  void set_history_flag(const bool flag);
  void set_preprocess_flag(const bool flag);
  void set_preprocess_view_flag(const bool flag);
  void set_target_flag(const bool flag);
  void set_pure_flag(const bool flag);
  void set_beliefTransition_view_flag(const bool flag);
  void set_timeover_flag(const bool flag);
  void set_no_fixed_flag(const bool flag);
  void set_global_flag(const bool flag);
  void set_nobd_flag(const bool flag);
  void setAllFlag(const bool flag);
  
  string optionsToString() const;
  vector<PreciseNumber> getOneShotAlphaVector(const OneShotExtension&);
  PreciseNumber getRewardFromOneShotAndBelief(const OneShotExtension&, const Belief&);
  //  Belief getNextBelief(const Belief &b, const int myAction, const int mySignal) const ;
};

string oneShotToString(const vector<Automaton> &pls, const OneShotExtension& oneShot);

#endif /* REPEATEDGAME_H_ */
