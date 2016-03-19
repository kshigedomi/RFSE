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

#ifndef PLAYERS_H_
#include "Players.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
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
  
  int playerStates;
  int playerActions;
  int playerSignals;
  int opponentStates;
  int opponentSignals;

  int numPlayers;
  int numJointStates;
  int numJointActions;
  int numJointSignals;

  Players players;
  Environment environment;
  Payoff payoff;

  vector<Equation> equations;
  vector<Belief> invariantDistribution;
  BeliefDivision initialBeliefDivision;
  BeliefDivision targetBeliefDivision;

  vector<Belief> failure;

  vector<vector<PreciseNumber> > alphaVectors;

  vector<struct OneShotExtension> oneShotExtensions;
  int maxIteration;

  BeliefTransition beliefTransition;

  Belief fixedPoint;

 public:

  long count_test ;

  map<string, PreciseNumber> globalVar;
  void setVariables(map<string, PreciseNumber> &vars);

  RepeatedGame();
  RepeatedGame(const Environment &env, const Payoff &po, const Players &pls);
  virtual ~RepeatedGame();

  void configure(const Environment &env, const Payoff &po, const Players &pls);

  void solveJointFSA();

  void setRewardEquation(const int player);
  void solveRewardEquation(const int player);
  vector< vector<PreciseNumber> > makeRewardEquation(const int player);
  void makeAlphaVectors();
  vector<vector<PreciseNumber> > getAlphaVectors() const;
    
  PreciseNumber getReward(const int player, const StateProfile &sp);
  PreciseNumber getReward(const int player, const int state);

  void solve();

  void makeOneShotExtension();
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
  vector<OneShotExtension> getOneShotExtensions() const;
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

string oneShotToString(const Players &pls, const OneShotExtension& oneShot);

#endif /* REPEATEDGAME_H_ */
