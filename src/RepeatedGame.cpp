#include "RepeatedGame.h"

/*
 * RepeatedGame.cpp
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 */

/*
 * This class has core functions in this program.
 * It has players, environment, and payoff.
 * It has some methods to require invariant distribution, initial belief division, and target belief division. And it holds their values.
 * It also has some methods to make equations and alphavector. And it holds their values.
 * It has two core function named checkGlobalResilent ands checkResilent. They check whether it is a Global RFPE and RFPE.
 */

/**
 * TODO　プレイヤーごとに対応しているが，プレイヤー1以外にはちゃんと計算できてない様．
 * Joint FSAのEquationを構成する部分が問題あると推測．
 */

RepeatedGame::RepeatedGame() {
   playerStates = 0;
   playerActions = 0;
   playerSignals = 0;
   opponentStates = 0;
   opponentSignals = 0;

   numPlayers = 0;
   numJointStates = 0;
   numJointActions = 0;
   numJointSignals = 0;
   setAllFlag(false);
}

RepeatedGame::RepeatedGame(const Environment &env, const Payoff &po, const Players &pls) {
   configure(env, po, pls);
}

RepeatedGame::~RepeatedGame() {

}

void RepeatedGame::setVariables(map<string, PreciseNumber> &vars) {
   globalVar = vars;
}

/*
 * 機能: 各種クリアを行う
 */
void RepeatedGame::configure(const Environment &env, const Payoff &po, const Players &pls) {
   environment = env;
   payoff = po;
   numPlayers = pls.getNumberOfPlayers();
   players = pls;
   equations.clear();
   equations.resize(numPlayers);

   playerStates = players.getNumberOfStates(PLAYER);
   playerActions = players.getNumberOfActions(PLAYER);
   playerSignals = players.getNumberOfSignals(PLAYER);

   opponentStates = players.getNumberOfOpponentStates();
   opponentSignals = players.getNumberOfOpponentSignals();

   numJointStates = players.getNumberOfJointStates();
   numJointActions = players.getNumberOfJointActions();
   numJointSignals = players.getNumberOfJointSignals();

   alphaVectors.clear();
   alphaVectors.resize(playerStates);

   failure.clear();

   maxIteration = 0;
}
/*
 * 割引期待利得を計算
 */
void RepeatedGame::solveJointFSA() {
   setRewardEquation(PLAYER);
   solveRewardEquation(PLAYER);
}
void RepeatedGame::solveRewardEquation(const int player) {
   equations[player].solve();
}
void RepeatedGame::setRewardEquation(const int player) {
   const vector< vector<PreciseNumber> > rewardEquation = makeRewardEquation(player);
   equations[player].configure(numJointStates, rewardEquation);
}
/*
 * 機能: equation に割引期待利得を解かせるためにセットする
 * メモ: v=u+dAv が割引期待利得の式-> (E-dA)v=u を equation で解く
 */
vector< vector<PreciseNumber> > RepeatedGame::makeRewardEquation(const int player) {
   vector< vector<PreciseNumber> > equation(numJointStates, vector<PreciseNumber>(numJointStates + 1));
   for (int states = 0; states < numJointStates; states++) {
      const int actions = players.actionsOfJointStates(states);
      equation[states][numJointStates] = payoff.getPayoff(player, actions);
      for (int signals = 0; signals < numJointSignals; signals++) {
         const int transitions = players.transitionsOfJointStates(states, signals);
         equation[states][transitions] += environment.getSignalDistribution(actions, signals);
      }
   }
   for (int i = 0; i < numJointStates; i++) {
      for (int j = 0; j < numJointStates; j++) {
         equation[i][j] = environment.getDiscountRate() * equation[i][j] * PreciseNumber("-1");
      }
      equation[i][i] = ONE + equation[i][i];
   }

   return equation;
}
PreciseNumber RepeatedGame::getReward(const int player, const StateProfile &sp) {
   return equations[player].getResult(players.getState(sp));
}
PreciseNumber RepeatedGame::getReward(const int player, const int state) {
   return equations[player].getResult(state);
}
void RepeatedGame::makeAlphaVectors() {
   for (int i = 0; i < playerStates; i++) {
      vector<PreciseNumber> aV(opponentStates);
      for (int j = 0; j < opponentStates; j++) {
         const StateProfile sp = players.getStateProfile(i, j);
         aV[j] = getReward(PLAYER, sp);
      }
      alphaVectors[i] = aV;
   }
}
vector< vector<PreciseNumber> > RepeatedGame::getAlphaVectors() const {
   return alphaVectors;
}

void RepeatedGame::solve() {
   beliefTransition.make(environment, payoff, players);
   if (beliefTransition_view_flag)
      Message::display(beliefTransition.toString(environment, players));
   solveJointFSA();
   makeAlphaVectors();
   if (!nobd_flag)
      makeBeliefDivision();
   if (target_flag) {
      makeOneShotExtension();
      makeTargetBeliefDivision();
   }
}

/*
 * Name: makeBeliefDivision
 * Function: make candidate of compatible covering belief division named initial beleif division
 */
void RepeatedGame::makeBeliefDivision() {
   initialBeliefDivision.configure(playerStates);

   vector< vector<PreciseNumber> > input(playerStates + opponentStates, vector<PreciseNumber>(opponentStates + 1, 0));
   vector<bool> il(playerStates + opponentStates, false);

   int row = 0;
   // alpha vector
   for (int i = 0; i < playerStates; i++) {
      const int pivot = opponentStates - 1;
      int col = 0;
      const PreciseNumber pivotValue = getReward(PLAYER, players.getStateProfile(i, pivot));
      input[row][col++] = -1 * pivotValue;
      for (int j = 0; j < opponentStates - 1; j++) {
         const PreciseNumber value = getReward(PLAYER, players.getStateProfile(i, j));
         input[row][col++] = -(value - pivotValue);
      }
      input[row][col++] = 1;
      row++;
   }

   // belief range
   for (int i = 0; i < opponentStates; i++) {
      if (i == opponentStates - 1) {
         input[row][0] = 1;
         for (int j = 0; j < opponentStates - 1; j++) input[row][j + 1] = -1;
      }
      else {
         input[row][i + 1] = 1;
      }
      row++;
   }

   Inequality inequalities = Inequality(input, il);
   pair< vector< vector<PreciseNumber> >, vector< vector<int> > > res = convertInequalityToPointWithIncidence(inequalities);
   vector< vector<PreciseNumber> > beliefPoints = res.first;
   vector< vector<int> > belongDivisions = res.second;

   vector<bool> isPoint(beliefPoints.size(), false);
   vector<Belief> rawExtremePoints = convertPointToBeliefWithReward(beliefPoints, isPoint);

   vector< vector<Belief> > polytope(playerStates);
   for (vector<vector<int> >::size_type i = 0; i < belongDivisions.size(); ++i) {
      if (!isPoint[i]) continue;
      for (vector<int>::size_type j = 0; j < belongDivisions[i].size(); j++) {
         if (belongDivisions[i][j] < playerStates) {
            polytope[belongDivisions[i][j]].push_back(rawExtremePoints[i]);
         }
      }
   }

   for (int i = 0; i < playerStates; i++) {
      if (polytope[i].size() > 0) {
         sort(polytope[i].begin(), polytope[i].end());
         initialBeliefDivision.setPolytopeToDivision(i, polytope[i]);
         initialBeliefDivision.makeInequalities(i);
      }
   }
}

void RepeatedGame::makeOneShotExtension() {
   vector<int> nextTransition(playerSignals);	//transition of one-shot extention
   Automaton automaton = players.getAutomaton(PLAYER);
   // reset
   oneShotExtensions.clear();
  
   for (int next = 0; next < pow(playerStates, playerSignals); next++) {
      for (int i = 0, t = next; i < playerSignals; i++) {
         nextTransition[i] = t % playerStates;
         t /= playerStates;
      }

      for (int myAction = 0; myAction < playerActions; ++myAction) {
		bool exist = false;
		for (int myState = 0; myState < playerStates; ++myState) {
            if (automaton.getActionChoice(myState) == myAction && automaton.getStateTransition(myState) == nextTransition) {
			  exist = true;
			  break;
            }
		}
		if (!exist) {
		  oneShotExtensions.push_back(OneShotExtension(myAction, nextTransition));
		}
      }
   }
}

// it is not used. return normal condition which means above
vector<PreciseNumber> RepeatedGame::makeInvariantDistribution() {
   vector< vector<PreciseNumber> > nc(numJointStates, vector<PreciseNumber>(numJointStates));
   for (int states = 0; states < numJointStates; states++) {
      const ActionProfile actions = players.actionsOfJointStates(states);
      for (int signals = 0; signals < numJointSignals; signals++) {
         const int transitions = players.transitionsOfJointStates(states, signals);
         nc[states][transitions] += environment.getSignalDistribution(actions, signals);
      }
   }
   
   while (true) {
      vector< vector<PreciseNumber> > nc2(numJointStates, vector<PreciseNumber>(numJointStates, ZERO));
      bool check = true;
      for (int i = 0; i < numJointStates; i++) {
         for (int j = 0; j < numJointStates; j++) {
            for (int k = 0; k < numJointStates; k++) {
               nc2[i][j] += nc[i][k] * nc[k][j];
            }
            if (!equal(nc[i][j], nc2[i][j])) check = false;
         }
      }
      nc = nc2;
      if (check) break;
   }
   vector<PreciseNumber> normalConditions(numJointStates);

   for (int i = 0; i < numJointStates; i++)
      normalConditions[i] = nc[i][i];
   return normalConditions;
}

/*
 * 機能: Target Belief Division を作る
 */
void RepeatedGame::makeTargetBeliefDivision() {
   targetBeliefDivision.configure(playerStates);
   vector< vector<PreciseNumber> > extendedAlphaVectors = alphaVectors;

   for (vector<vector<PreciseNumber> >::size_type i=0; i<oneShotExtensions.size(); ++i) {
	 extendedAlphaVectors.push_back(getOneShotAlphaVector(oneShotExtensions[i]));
   }
   //extendedAlphaVectors = alphaVectors + rewards of one-shot extendtion
   const int n = extendedAlphaVectors.size();

   vector<vector<PreciseNumber> > input(n + opponentStates, vector<PreciseNumber>(opponentStates + 1, ZERO));
   vector<bool> il(n + opponentStates, false);

   int row = 0;
   // alpha vector
   for (int i = 0; i < n; i++) {
      int pivot = opponentStates - 1;
      int col = 0;
      input[row][col++] = -1 * extendedAlphaVectors[i][pivot];
      for (int j = 0; j < opponentStates - 1; j++) {
         input[row][col++] = -(extendedAlphaVectors[i][j] - extendedAlphaVectors[i][pivot]);
      }
      input[row][col++] = 1;
      row++;
   }

   // belief range
   for (int i = 0; i < opponentStates; i++) {
      if (i == opponentStates - 1) {
         input[row][0] = 1;
         for (int j = 0; j < opponentStates - 1; j++) input[row][j + 1] = -1;
      }
      else {
         input[row][i + 1] = 1;
      }
      row++;
   }

   Inequality inequalities = Inequality(input, il);
   pair< vector< vector<PreciseNumber> >, vector< vector<int> > > res = convertInequalityToPointWithIncidence(inequalities);
   vector< vector<PreciseNumber> > beliefPoints = res.first;
   vector< vector<int> > belongDivisions = res.second;

   vector<bool> isPoint(beliefPoints.size(), false);
   vector<Belief> rawExtremePoints = convertPointToBeliefWithReward(beliefPoints, isPoint);

   vector< vector<Belief> > polytope(playerStates);

   for (vector<vector<int> >::size_type i = 0; i < belongDivisions.size(); ++i) {
      if (!isPoint[i]) continue;
      for (vector<int>::size_type j = 0; j < belongDivisions[i].size(); j++) {
         if (belongDivisions[i][j] < playerStates) {
            polytope[belongDivisions[i][j]].push_back(rawExtremePoints[i]);
         }
      }
   }

   for (int i = 0; i < playerStates; i++) {
      sort(polytope[i].begin(), polytope[i].end());
      if (polytope[i].size() > 0) {
         targetBeliefDivision.setPolytopeToDivision(i, polytope[i]);
         targetBeliefDivision.makeInequalities(i);
      }
   }
}
// Belief DivisionはTarget Belief Divisionの中？
bool RepeatedGame::checkSeminalBeliefDivision(const BeliefDivision &closed) const {
   for (int i = 0; i < closed.getNumberOfDivisions(); ++i) {
      if (!checkSeminalBeliefSet(closed.getDivision(i)))
         return false;
   }
   return true;
}
// Belief SetはTarget Belief Divisionの中？
bool RepeatedGame::checkSeminalBeliefSet(const vector<Belief> &b) const {
   for (vector<Belief>::const_iterator it = b.begin(); it != b.end(); ++it) {
      if (!checkSeminalBelief(*it))
         return false;
   }
   return true;
}
// BeliefはTarget Belief Divisionの中？
bool RepeatedGame::checkSeminalBelief(const Belief &b) const {
   const PreciseNumber myReward = getMaxReward(b, getAlphaVectors()); // オンパスの利得を取得
   for (int ac = 0; ac < players.getNumberOfActions(PLAYER); ++ac) {
      const pair<OneShotExtension, PreciseNumber> maxOneShot = getMostProfitableOneShotWithAction(ac, b);
      if (myReward < maxOneShot.second)
         return false;
   }
   return true;
}

/*
 * 機能: 入力の信念と行動から，遷移を決定した最も高い利得とその One Shot Extension を返す
 * 入力: 行動 myAction, 信念 b
 * 出力: One Shot Extension とその利得
 */
pair<OneShotExtension, PreciseNumber> RepeatedGame::getMostProfitableOneShotWithAction(const int myAction, const Belief &b) const {
   OneShotExtension res(players.getNumberOfSignals(PLAYER));
   res.action=myAction;
   PreciseNumber reward = ZERO;
   // future loss
   for (int mySignal = 0; mySignal < players.getNumberOfSignals(PLAYER); ++mySignal) {
      // (myAction, mySignal) 後の利得が最大となるような遷移を行う
      const Belief nextBelief = beliefTransition.nextBelief(b, myAction, mySignal); // 事後信念を計算
      const pair<int, PreciseNumber> maxInfo = getMaxIndexAndReward(nextBelief, alphaVectors);
      res.transition[mySignal] = maxInfo.first;
      // 相手の信念が b, 自分の行動が ac の元で mySignal が発生する確率を計算
      PreciseNumber prob = ZERO;
      for (int opState = 0; opState < players.getNumberOfOpponentStates(); ++opState) {
         const int opAction = players.opponentActionsOfJointStates(opState);
         const ActionProfile ap = players.getActionProfile(myAction, opAction);
         for (int opSignal = 0; opSignal < players.getNumberOfOpponentSignals(); ++opSignal) {
            const SignalProfile sgp = players.getSignalProfile(mySignal, opSignal);
            prob += b.get(opState) * environment.getSignalDistribution(ap, sgp);
         }
      }
      reward += prob * maxInfo.second;
   }
   reward *= environment.getDiscountRate();
   // deviation gain
   for (int opState = 0; opState < opponentStates; ++opState) {
      const int opAction = players.opponentActionsOfJointStates(opState);
      const ActionProfile ap = players.getActionProfile(myAction, opAction);
      reward += payoff.getPayoff(PLAYER, ap) * b.get(opState);
   }
   return make_pair(res,reward);
}


BeliefDivision RepeatedGame::getBeliefDivision() const {
   return initialBeliefDivision;
}
BeliefDivision RepeatedGame::getTargetBeliefDivision() const {
   return targetBeliefDivision;
}

/*
 * 機能: polytope が Target Belief Division に含まれるかどうか調べる
 * 入力: polytope（事後信念）
 * メモ: Initial Belief Division を計算し終わっていることを前提とする
 */
bool RepeatedGame::isPolytopeInTargetBeliefDivisionWithBeliefDivision(const vector<Belief> &polytope) {
   Inequality IneNextPolytope = convertBeliefToInequality(polytope); // polytope を不等式に
   for (int i = 0; i < playerStates; i++) {
      Inequality cover = initialBeliefDivision.getDivisionInequality(i); // D_i^{\theta} を不等式に
      vector<Belief> points = convertPointToBelief(intersection(IneNextPolytope, cover)); // 共通部分の端点を見つける
      if (!checkSeminalBeliefSet(points))
         return false;
   }
   return true;
}

/*
 * 機能: polytope が Target Belief Division に含まれるかどうか調べる
 * 入力: polytope（事後信念）
 */
bool RepeatedGame::isPolytopeInTargetBeliefDivision(const vector<Belief> &polytope) {
   vector<OneShotExtension> oneShots;
   // 各 Belief で最も強い One Shot を抜き出す
   for (vector<Belief>::const_iterator b = polytope.begin(); b != polytope.end(); ++b) {
      for (int action = 0; action < playerActions; ++action) {
         oneShots.push_back(getMostProfitableOneShotWithAction(action, *b).first);
      }
   }
   // すべての OneShot を支配しているか見る
   for (vector<OneShotExtension>::const_iterator oneShot = oneShots.begin(); oneShot != oneShots.end(); ++oneShot) {
      bool checkDom = false;
      for (int state = 0; state < playerStates; ++state) { // ある状態が oneShot を支配している
         bool checkAllBelief = true;
         for (vector<Belief>::const_iterator b = polytope.begin(); b != polytope.end(); ++b) {
            const PreciseNumber onPath = getRewardFromAlphaVectorAndBelief(alphaVectors[state], *b);
            const PreciseNumber offPath = getRewardFromOneShotAndBelief(*oneShot, *b);
            if (onPath < offPath) {
               checkAllBelief = false;
               break;
            }
         }
         if (checkAllBelief) {
            checkDom = true;
            break;
         }
      }
      if (!checkDom)
         return false;
   }
   return true;
}

Result RepeatedGame::checkResilentFromBelief(const Belief &condition) {
   // belief space
   vector<Belief> beliefSpace = makeBeliefSpace(opponentStates);
   
   //stack
   vector<BeliefMatrix> stack_bMatrix;
   stack_bMatrix.push_back(beliefTransition.createBeliefMatrix(opponentStates));
   vector<vector<pair<int, int> > > stack_history(1);

   int iteration = 0;
   time_t start = time(NULL);
   // step 1
   while (!stack_bMatrix.empty()) {
      // タイムオーバ判定
      if (timeover_flag && time(NULL) - start > 1000)
         return TIMEISOVER;
      ++iteration;
      // pop
      BeliefMatrix bMatrix = stack_bMatrix.back();
      stack_bMatrix.pop_back();
      vector<pair<int, int> > history;
      if (history_flag) {
         history = stack_history.back();
         stack_history.pop_back();
      }
      
      // step 2
      // set nextCondition
      Belief nextCondition = beliefTransition.nextBeliefFromBeliefMatrix(condition, bMatrix);
      
      // check nextCondition
      if (!checkSeminalBelief(nextCondition)) {
         maxIteration = max(maxIteration, iteration);
         failure.push_back(nextCondition);
         if (history_flag) {
            Belief b = condition;
			Message::display("Init : " + condition.toString());
            for (int i = history.size()-1; i >= 0; --i) {
               b = beliefTransition.nextBelief(b, history[i].first, history[i].second);
			   Message::display("t=" + toString(history.size()-1-i) + "(" + players.getNameOfActions(PLAYER, history[i].first) + ", " + players.getNameOfSignals(PLAYER, history[i].second) + " Belief : " + b.toString());
            }
         }
         return FAILURE;
      }
	  // step3
      // set nextPolytope
      vector<Belief> nextPolytope = beliefSpace;
      for (int i = 0; i < opponentStates; ++i) {
         nextPolytope[i] = beliefTransition.nextBeliefFromBeliefMatrix(nextPolytope[i], bMatrix);
      }
      sort(nextPolytope.begin(), nextPolytope.end());

      // check Win
      bool checkWin;
      if (!nobd_flag)
         checkWin = isPolytopeInTargetBeliefDivisionWithBeliefDivision(nextPolytope);
      else
         checkWin = isPolytopeInTargetBeliefDivision(nextPolytope);
      if (beliefTransition_view_flag && iteration % 100 == 0) {
         Message::display("t=" + toString(iteration) + " Belief: " + nextCondition.toString());
         for (vector<Belief>::const_iterator b = nextPolytope.begin(); b != nextPolytope.end(); ++b) {
            pair<int, PreciseNumber> maxInfo = getMaxIndexAndReward(*b, alphaVectors);
            cout << "isIn : " << maxInfo.first << " " << *b << endl;
         }
         cout << "checkWin : " << checkWin << endl;
         cout << "size : " << stack_bMatrix.size() << endl;
         cout << "length : " << stack_history.back().size() << endl;
      }
      if (!checkWin) {
         // update
         for (int a = 0; a < playerActions; a++) {
            for (int o = 0; o < playerSignals; o++) {
               // update bMatrix
               BeliefMatrix next = beliefTransition.nextBeliefMatrix(bMatrix, make_pair(a, o));
               // update stack
               stack_bMatrix.push_back(next);
               // update history
               if (history_flag) {
                  vector<pair<int, int> > his = history;
                  his.push_back(make_pair(a, o));
                  stack_history.push_back(his);
			   }
            }
         }
      }
   }
   maxIteration = max(maxIteration, iteration);
   return SUCCESS;
}

/*
 * 機能: fixed point の生成
 * 返り値: fixed point が TargetBeliefDivision から出てたら，false
 */
PreProcess RepeatedGame::makeFixedPoint() {
   PreProcess result = NOFIX;
   Belief init(opponentStates, ZERO);
   init.set(0, PreciseNumber("1"));
   for (int a = 0; a < playerActions; a++) {
      for (int o = 0; o < playerSignals; o++) {
         Belief b = init;
         BeliefMatrix BM = beliefTransition.createBeliefMatrix(opponentStates);
         BM = beliefTransition.nextBeliefMatrix(BM, make_pair(a, o));
         int count = 0;
         bool check_fix = true;
         while (true) {
            if (no_fixed_flag && ++count > 10) { // fixed point 見つからず
               check_fix = false;
               break;
            }
            if (preprocess_view_flag)
               Message::display("Belief:" + b.toString());
            BM = matrixMul(BM, BM);
            const Belief next = beliefTransition.nextBeliefFromBeliefMatrix(b, BM);
            if (b == next) {
               fixedPoint = b;
               result = TRUE;
               break; // fixed point が見つかった
            }
            b = next;
         }
         if (preprocess_view_flag) {
            Message::display("Fixed (" + players.getNameOfActions(PLAYER, a) + ", " + players.getNameOfSignals(PLAYER, o) + ") : " + b.toString());
            string line = "isIn :";
            for (int i = 0; i < playerStates; ++i) {
               if (isInnerPointOfDivision(b, i))
				 line += " " + players.getNameOfStates(PLAYER,i);
            }
            Message::display(line);
         }
         if (check_fix == false)
            continue;
         if (!preprocess_flag)
            return TRUE;
         // 逸脱しないかのチェックを以下でする
         if (!checkSeminalBelief(b)) {
            failure.push_back(b);
            return FALSE;
         }
      }
   }
   return result;
}

/*
 * 機能: 純粋戦略均衡か確かめる
 * 返り値: 結果
 * メモ: すべてのプレイヤは R? から始める
 */
Result RepeatedGame::checkResilentFromPureStrategy() {
   // 自分の状態は 0
   const int myState = 0;
   // 相手の状態も 0
   Belief b(opponentStates);
   b.set(0, ONE);
   // Consistent チェック
   if (isInnerPointOfDivision(b, myState))
      return checkResilentFromBelief(b);
   else {
      failure.push_back(b);
      return NOTCONSISTENT;
   }
}

Belief RepeatedGame::getFixedPoint() const {
   return fixedPoint;
}

Result RepeatedGame::checkNoEquilibrium() {
   if ((!nobd_flag && initialBeliefDivision.existEmptyDivision()) || (target_flag && targetBeliefDivision.existEmptyDivision())) {
      return NOTEXIST;
   } else if (!nobd_flag && checkSeminalBeliefDivision(initialBeliefDivision)) {
      return GLOBAL;
   } else if (global_flag) {
      return NOTSURE;
   } else if (pure_flag) {
      return checkResilentFromPureStrategy();
   }
   const PreProcess result = makeFixedPoint();
   if (result == FALSE) {
      return PREPROFAILURE;
   } else if (result == NOFIX) {
      return NOTFIXED;
   }
   return checkResilentFromBelief(getFixedPoint());
}

/*
 * 機能: Belief が initial belief division の division に入っているか調べる
 * メモ: Belief.getMaxIndexAndReward を呼び，Index と division を比べてもほとんど同じことが言えるが，
 *       Belief Division の境目に信念があるときに，こちらは正しく動くが，あちらは知らない．
 */
bool RepeatedGame::isInnerPointOfInitialDivision(const Belief &b, const int division) const  {
   const PreciseNumber target = getRewardFromAlphaVectorAndBelief(alphaVectors[division], b);
   for (vector<vector<PreciseNumber> >::size_type i = 0; i < alphaVectors.size(); ++i) {
      const PreciseNumber tmp = getRewardFromAlphaVectorAndBelief(alphaVectors[i], b);
      if (tmp > target) {
         return false;
      }
   }
   return true;
}

/*
 * 機能: Belief が target belief division の division に入っているか調べる
 */
bool RepeatedGame::isInnerPointOfDivision(const Belief &b, const int division) const  {
   return isInnerPointOfInitialDivision(b, division) && checkSeminalBelief(b);
}

int RepeatedGame::getMaxIteration() const {
   return maxIteration;
}

vector<OneShotExtension> RepeatedGame::getOneShotExtensions() const {
   return oneShotExtensions;
}

/*
 * 機能: Target Belief Division から逸脱したときのBelief を取得
 */
vector<Belief> RepeatedGame::getFailure() const {
   return failure;
}

vector<Belief> RepeatedGame::getInvariantDistribution() const {
   return invariantDistribution;
}

void RepeatedGame::set_pure_flag(const bool flag) {
   pure_flag = flag;
}

void RepeatedGame::set_target_flag(const bool flag) {
   target_flag = flag;
}
void RepeatedGame::set_history_flag(const bool flag) {
   history_flag = flag;
}
void RepeatedGame::set_preprocess_flag(const bool flag) {
   preprocess_flag = flag;
}

void RepeatedGame::set_preprocess_view_flag(const bool flag) {
   preprocess_view_flag = flag;
}

void RepeatedGame::set_timeover_flag(const bool flag) {
   timeover_flag = flag;
}

void RepeatedGame::set_beliefTransition_view_flag(const bool flag) {
   beliefTransition_view_flag = flag;
}
void RepeatedGame::set_no_fixed_flag(const bool flag) {
   no_fixed_flag = flag;
}

void RepeatedGame::set_global_flag(const bool flag) {
   global_flag = flag;
}

void RepeatedGame::set_nobd_flag(const bool flag) {
   nobd_flag = flag;
}

void RepeatedGame::setAllFlag(const bool flag) {
   set_history_flag(flag);
   set_preprocess_flag(flag);
   set_preprocess_view_flag(flag);
   set_target_flag(flag);
   set_pure_flag(flag);
   set_no_fixed_flag(flag);
   set_global_flag(flag);
   set_nobd_flag(flag);
}

string RepeatedGame::optionsToString() const {
   string res;
   res += "history : " + ::toString(history_flag) + "\n";
   res += "preprocess : " + ::toString(preprocess_flag) + "\n";
   res += "preprocess_view : " + ::toString(preprocess_view_flag) + "\n";
   res += "target : " + ::toString(target_flag) + "\n";   
   res += "pure : " + ::toString(pure_flag) + "\n";
   res += "timeover : " + ::toString(timeover_flag) + "\n";
   res += "belief : " + ::toString(beliefTransition_view_flag) + "\n";
   res += "no_fixed : " + ::toString(no_fixed_flag) + "\n";
   res += "global : " + ::toString(global_flag) + "\n";
   res += "nobd : " + ::toString(nobd_flag) + "\n";
   return res;
}

/*
 * 機能: OneShotExtension を文字に変換
 * 引数: oneShotExtension
 * 返り値: 文字
 */
string oneShotToString(const Players& pls,const OneShotExtension& oneShot) {
   string res;
   res += "OneShot Extension\n";
   res += "Action: " + pls.getNameOfActions(PLAYER, oneShot.action) + "\n";
   res += "Transition";
   for (int signal = 0; signal < pls.getNumberOfSignals(PLAYER); ++signal) {
      res += "\n";
      const string signalStr = pls.getNameOfSignals(PLAYER, signal);
      const int nextState = oneShot.transition[signal];
      const string nextStateStr = pls.getNameOfStates(PLAYER, nextState);
      res += signalStr + " -> " + nextStateStr;
   }
   return res;
}  

/*
 * 機能: OneShotExtension の alphaVector を作成
 * 引数: oneShotExtension
 */
vector<PreciseNumber> RepeatedGame::getOneShotAlphaVector(const OneShotExtension& oneShot) {
  vector<PreciseNumber> alphaVector(players.getNumberOfOpponentStates());
  for (int opState = 0; opState < opponentStates; ++opState) {
	PreciseNumber reward = ZERO;
	const int opActions = players.opponentActionsOfJointStates(opState);
	const ActionProfile ap = players.getActionProfile(oneShot.action, opActions);
	for (int mySignal = 0; mySignal < playerSignals; ++mySignal) {
	  for (int opSignal = 0; opSignal < opponentSignals; ++opSignal) {
		const int nextOpState = players.opponentTransitionsOfJointStates(opState, opSignal);
		const StateProfile nextState = players.getStateProfile(oneShot.transition[mySignal], nextOpState);
		const SignalProfile sp = players.getSignalProfile(mySignal, opSignal);
		reward += environment.getSignalDistribution(ap, sp) * getReward(PLAYER, nextState);
	  }
	}
	reward *= environment.getDiscountRate();
	reward += payoff.getPayoff(PLAYER, ap);
	alphaVector[opState] = reward; //aV[opState] is a reward when player do one-shot Extension and opponent is in opponetState.
  }
  return alphaVector;
}
  
/*
 * 機能: OneShotExtension の Reward を得る
 * 引数: oneShotExtension と信念 b
 */
PreciseNumber RepeatedGame::getRewardFromOneShotAndBelief(const OneShotExtension& oneShot,const Belief &b) {
   return getRewardFromAlphaVectorAndBelief(getOneShotAlphaVector(oneShot), b);
}
  
