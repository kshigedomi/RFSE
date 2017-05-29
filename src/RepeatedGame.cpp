#include "RepeatedGame.h"

/*
 * RepeatedGame.cpp
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 */

RepeatedGame::RepeatedGame() {
    setAllFlag(false);
}

RepeatedGame::RepeatedGame(const Environment &env, const Payoff &po, const vector<Automaton> &ms, const RewardEquation& eq) {
    configure(env, po, ms, eq);
}

RepeatedGame::~RepeatedGame() {}

/*
 * 機能: 各種初期設定を行う
 */
void RepeatedGame::configure(const Environment &env, const Payoff &po, const vector<Automaton>& ms, const RewardEquation &eq) {
    environment = env;
    payoff = po;
    automatons = ms;
    rewardEquation = eq;
    numPlayers = ms.size();
    playerStates = ms[PLAYER].getNumberOfStates();
    playerActions = ms[PLAYER].getNumberOfActions();
    playerSignals = ms[PLAYER].getNumberOfSignals();
    opponentStates = JointFSA::getMultipleAccumulate(ms.begin()+1, ms.end(), &Automaton::getNumberOfStates);
    opponentSignals = JointFSA::getMultipleAccumulate(ms.begin()+1, ms.end(), &Automaton::getNumberOfSignals);
    numJointStates = JointFSA::getMultipleAccumulate(ms.begin(), ms.end(), &Automaton::getNumberOfStates);
    numJointActions = JointFSA::getMultipleAccumulate(ms.begin(), ms.end(), &Automaton::getNumberOfActions);
    numJointSignals = JointFSA::getMultipleAccumulate(ms.begin(), ms.end(), &Automaton::getNumberOfSignals);
    alphaVectors = rewardEquation.getAlphaVectors();
    failure.clear();
    maxIteration = 0;
}

/*
 * 均衡判定の前処理を行う．
 */
void RepeatedGame::solve() {
    beliefTransition.make(environment, payoff, automatons);
    if (beliefTransition_view_flag)
        Message::display(beliefTransition.toString(environment, automatons));
    if (!nobd_flag)
        makeBeliefDivision();
    if (target_flag)
        makeTargetBeliefDivision();
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
    // プレイヤの利得を入れ込む．
    for (int myState = 0; myState < playerStates; myState++) {
        const int pivot = opponentStates - 1;
        int col = 0;
        const PreciseNumber pivotValue = getReward(StateProfile(myState, OpponentStateProfile(pivot)).getIndex());
        input[row][col++] = -1 * pivotValue;
        for (int opState = 0; opState < opponentStates - 1; opState++) {
            const PreciseNumber value = getReward(StateProfile(myState, opState).getIndex());
            input[row][col++] = -(value - pivotValue);
        }
        input[row][col++] = 1;
        row++;
    }

    // 信念の範囲を条件づける．
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

vector<OneShotExtension> RepeatedGame::makeOneShotExtensions() {
    vector<OneShotExtension> oneShotExtensions;
    vector<int> nextTransition(playerSignals);	//transition of one-shot extention
    Automaton automaton = automatons[PLAYER];
    // reset
    for (int next = 0; next < pow(playerStates, playerSignals); next++) {
        for (int i = 0, t = next; i < playerSignals; i++) {
            nextTransition[i] = t % playerStates;
            t /= playerStates;
        }
        for (int myAction = 0; myAction < playerActions; ++myAction) {
            bool exist = false;
            for (int myState = 0; myState < playerStates; ++myState) {
                if ((automaton.getActionChoice(myState) == myAction) && (automaton.getStateTransition(myState) == nextTransition)) {
                    exist = true;
                    break;
                }
            }
            if (!exist) {
                oneShotExtensions.push_back(OneShotExtension(myAction, nextTransition));
            }
        }
    }
    return oneShotExtensions;
}

// 不変分布を作る
// vector<PreciseNumber> RepeatedGame::makeInvariantDistribution() {
//    vector< vector<PreciseNumber> > nc(numJointStates, vector<PreciseNumber>(numJointStates));
//    for (int states = 0; states < numJointStates; states++) {
//       const ActionProfile actions = players.actionsOfJointStates(states);
//       for (int signals = 0; signals < numJointSignals; signals++) {
//          const int transitions = players.transitionsOfJointStates(states, signals);
//          nc[states][transitions] += environment.getSignalDistribution(actions, signals);
//       }
//    }
   
//    while (true) {
//       vector< vector<PreciseNumber> > nc2(numJointStates, vector<PreciseNumber>(numJointStates, ZERO));
//       bool check = true;
//       for (int i = 0; i < numJointStates; i++) {
//          for (int j = 0; j < numJointStates; j++) {
//             for (int k = 0; k < numJointStates; k++) {
//                nc2[i][j] += nc[i][k] * nc[k][j];
//             }
//             if (!equal(nc[i][j], nc2[i][j])) check = false;
//          }
//       }
//       nc = nc2;
//       if (check) break;
//    }
//    vector<PreciseNumber> normalConditions(numJointStates);

//    for (int i = 0; i < numJointStates; i++)
//       normalConditions[i] = nc[i][i];
//    return normalConditions;
// }

/*
 * 機能: Target Belief Division を作る
 */
void RepeatedGame::makeTargetBeliefDivision() {
    vector<OneShotExtension> oneShotExtensions = this->makeOneShotExtensions();
    this->targetBeliefDivision.configure(playerStates);
    //extendedAlphaVectors = alphaVectors + rewards of one-shot extendtion
    vector< vector<PreciseNumber> > extendedAlphaVectors(this->alphaVectors);
    for (vector<vector<PreciseNumber> >::size_type i=0; i<oneShotExtensions.size(); ++i) {
        extendedAlphaVectors.push_back(this->getOneShotAlphaVector(oneShotExtensions[i]));
    }
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
// Belief DivisionはTarget Belief Divisionの中か否か
bool RepeatedGame::checkSeminalBeliefDivision(const BeliefDivision &closed) const {
    for (int i = 0; i < closed.getNumberOfDivisions(); ++i) {
        if (!checkSeminalBeliefSet(closed.getDivision(i)))
            return false;
    }
    return true;
}
// Belief SetはTarget Belief Divisionの中か否か
bool RepeatedGame::checkSeminalBeliefSet(const vector<Belief> &b) const {
    for (vector<Belief>::const_iterator it = b.begin(); it != b.end(); ++it) {
        if (!checkSeminalBelief(*it))
            return false;
    }
    return true;
}
// BeliefはTarget Belief Divisionの中か否か
bool RepeatedGame::checkSeminalBelief(const Belief &b) const {
    const PreciseNumber myReward = getMaxReward(b, alphaVectors); // オンパスの利得を取得
    for (int ac = 0; ac < playerActions; ++ac) {
        const pair<OneShotExtension, PreciseNumber> maxOneShot = this->getMostProfitableOneShotWithAction(ac, b);
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
    OneShotExtension res(this->playerSignals);
    res.action=myAction;
    PreciseNumber reward = ZERO;
    // future loss
    for (int mySignal = 0; mySignal < playerSignals; ++mySignal) {
        // (myAction, mySignal) 後の利得が最大となるような遷移を行う
        const Belief nextBelief = beliefTransition.nextBelief(b, myAction, mySignal);
        const pair<int, PreciseNumber> maxInfo = ::getMaxIndexAndReward(nextBelief, alphaVectors);
        res.transition[mySignal] = maxInfo.first;
        // 信念が b, 自分の行動が myAction の元で mySignal が発生する確率を計算
        PreciseNumber prob = ZERO;
        OpponentStateProfile opState;
        do {
            const OpponentActionProfile opAction = JointFSA::actionProfileOfStateProfile(opState, this->automatons);
            const ActionProfile ap(myAction, opAction);
            OpponentSignalProfile opSignal;
            do {
                const SignalProfile sgp(mySignal, opSignal);
                prob += b.get(opState.getIndex()) * environment.getSignalDistribution(ap.getIndex(), sgp.getIndex());
            } while (opSignal.next());
        } while (opState.next());
        reward += prob * maxInfo.second;
    }
    reward *= environment.getDiscountRate();
    // deviation gain
    OpponentStateProfile opState;
    do {
        const OpponentActionProfile opAction = JointFSA::actionProfileOfStateProfile(opState, automatons);
        const ActionProfile ap(myAction, opAction);
        reward += payoff.getPayoff(PLAYER, ap.getIndex()) * b.get(opState.getIndex());
    } while (opState.next());
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
    // すべての OneShot を polytope 内でどれかのオンパスが支配しているか調べる
    vector<OneShotExtension> oneShots = makeOneShotExtensions();
    for (vector<OneShotExtension>::const_iterator oneShot = oneShots.begin(); oneShot != oneShots.end(); ++oneShot) {
        bool checkDom = false;
        for (int state = 0; state < playerStates; ++state) {
            vector<Belief>::size_type i = 0;
            for (; i < polytope.size(); ++i) {
                const PreciseNumber onPath = getRewardFromAlphaVectorAndBelief(alphaVectors[state], polytope[i]);
                const PreciseNumber offPath = getRewardFromOneShotAndBelief(*oneShot, polytope[i]);
                if (onPath < offPath) {
                    break;
                }
            }
            if (i == polytope.size()) {
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
    while (!stack_bMatrix.empty()) {
        // step 1: pop
        if (timeover_flag && time(NULL) - start > 1000)
            return TIMEISOVER;
        ++iteration;
        BeliefMatrix bMatrix = stack_bMatrix.back();
        stack_bMatrix.pop_back();
        vector<pair<int, int> > history;
        if (history_flag) {
            history = stack_history.back();
            stack_history.pop_back();
        }
        // step 2: 初期信念からの事後信念が目標信念分割から出ていないか調べる．
        const Belief nextCondition = beliefTransition.nextBeliefFromBeliefMatrix(condition, bMatrix);
        if (!checkSeminalBelief(nextCondition)) {
            maxIteration = max(maxIteration, iteration);
            failure.push_back(nextCondition);
            if (history_flag) {
                Belief b = condition;
                Message::display("Init : " + condition.toString());
                for (int i = history.size()-1; i >= 0; --i) {
                    b = beliefTransition.nextBelief(b, history[i].first, history[i].second);
                    Message::display("t=" + MyUtil::toString(history.size()-1-i) + "(" + automatons[PLAYER].getNameOfActions(history[i].first) + ", " + automatons[PLAYER].getNameOfSignals(history[i].second) + " Belief : " + b.toString());
                }
            }
            return FAILURE;
        }
        // step3: 信念全体の事後信念が目標信念分割に入っているか調べる．
        vector<Belief> nextPolytope = beliefSpace;
        for (int i = 0; i < opponentStates; ++i) {
            nextPolytope[i] = beliefTransition.nextBeliefFromBeliefMatrix(nextPolytope[i], bMatrix);
        }
        sort(nextPolytope.begin(), nextPolytope.end());
        bool checkWin;
        if (!nobd_flag)
            checkWin = isPolytopeInTargetBeliefDivisionWithBeliefDivision(nextPolytope);
        else
            checkWin = isPolytopeInTargetBeliefDivision(nextPolytope);
        if (beliefTransition_view_flag && (iteration % 100 == 0)) {
            Message::display("t=" + MyUtil::toString(iteration) + " Belief: " + nextCondition.toString());
            for (vector<Belief>::const_iterator b = nextPolytope.begin(); b != nextPolytope.end(); ++b) {
                pair<int, PreciseNumber> maxInfo = getMaxIndexAndReward(*b, alphaVectors);
                cout << "isIn : " << maxInfo.first << " " << *b << endl;
            }
            cout << "checkWin : " << checkWin << endl;
            cout << "size : " << stack_bMatrix.size() << endl;
            cout << "length : " << stack_history.back().size() << endl;
        }
        if (!checkWin) {
            // step4: アップデート
            for (int a = 0; a < playerActions; a++) {
                for (int o = 0; o < playerSignals; o++) {
                    const BeliefMatrix next = beliefTransition.nextBeliefMatrix(bMatrix, make_pair(a, o));
                    stack_bMatrix.push_back(next);
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
    init.set(0, ONE);
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
                } else {
                    if (preprocess_view_flag)
                        Message::display("Belief:" + b.toString());
                    BM = MyUtil::matrixMul(BM, BM);
                    const Belief next = beliefTransition.nextBeliefFromBeliefMatrix(b, BM);
                    if (b == next) { // fixed point が見つかった
                        fixedPoint = b;
                        result = TRUE;
                        break; 
                    }
                    b = next;
                }
            }
            if (preprocess_view_flag) {
                Message::display("Fixed (" + automatons[PLAYER].getNameOfActions(a) + ", " + automatons[PLAYER].getNameOfSignals(o) + ") : " + b.toString());
                string line = "isIn :";
                for (int i = 0; i < playerStates; ++i) {
                    if (isInnerPointOfDivision(b, i))
                        line += " " + automatons[PLAYER].getNameOfStates(i);
                }
                Message::display(line);
            }
            if (check_fix == false)
                continue;
            if (!checkSeminalBelief(b)) {
                failure.push_back(b);
                return FALSE;
            }
            if (!preprocess_flag)
                return TRUE;
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
 *       Belief Division の境目に信念があるときに，こちらは入っていることになる．あっちはどうなるかわからない．
 */
bool RepeatedGame::isInnerPointOfInitialDivision(const Belief &b, const int division) const  {
    const PreciseNumber target = getRewardFromAlphaVectorAndBelief(this->alphaVectors[division], b);
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
    res += "history : " + MyUtil::toString(history_flag) + "\n";
    res += "preprocess : " + MyUtil::toString(preprocess_flag) + "\n";
    res += "preprocess_view : " + MyUtil::toString(preprocess_view_flag) + "\n";
    res += "target : " + MyUtil::toString(target_flag) + "\n";   
    res += "pure : " + MyUtil::toString(pure_flag) + "\n";
    res += "timeover : " + MyUtil::toString(timeover_flag) + "\n";
    res += "belief : " + MyUtil::toString(beliefTransition_view_flag) + "\n";
    res += "no_fixed : " + MyUtil::toString(no_fixed_flag) + "\n";
    res += "global : " + MyUtil::toString(global_flag) + "\n";
    res += "nobd : " + MyUtil::toString(nobd_flag) + "\n";
    return res;
}

/*
 * 機能: OneShotExtension を文字に変換
 * 引数: oneShotExtension
 * 返り値: 文字
 */
string oneShotToString(const vector<Automaton>& ms,const OneShotExtension& oneShot) {
    string res;
    res += "OneShot Extension\n";
    res += "Action: " + ms[PLAYER].getNameOfActions(oneShot.action) + "\n";
    res += "Transition";
    for (int signal = 0; signal < ms[PLAYER].getNumberOfSignals(); ++signal) {
        res += "\n";
        const string signalStr = ms[PLAYER].getNameOfSignals(signal);
        const int nextState = oneShot.transition[signal];
        const string nextStateStr = ms[PLAYER].getNameOfStates(nextState);
        res += signalStr + " -> " + nextStateStr;
    }
    return res;
}  

/*
 * 機能: OneShotExtension の alphaVector を作成
 * 引数: oneShotExtension
 */
vector<PreciseNumber> RepeatedGame::getOneShotAlphaVector(const OneShotExtension& oneShot) {
    vector<PreciseNumber> alphaVector(this->opponentStates);
    OpponentStateProfile opState;
    do {
        PreciseNumber reward("0");
        const OpponentActionProfile opAction = JointFSA::actionProfileOfStateProfile(opState, automatons);
        const ActionProfile ap(oneShot.action, opAction);
        OpponentSignalProfile opSignal;
        do {
            const OpponentStateProfile nextOpState = JointFSA::transitionOfStateProfile(opState, opSignal, automatons);
            for (int mySignal = 0; mySignal < playerSignals; ++mySignal) {
                const StateProfile nextState(oneShot.transition[mySignal], nextOpState);
                const SignalProfile sgp(mySignal, opSignal);
                reward += environment.getSignalDistribution(ap.getIndex(), sgp.getIndex()) * getReward(nextState.getIndex());
            }
        } while (opSignal.next());
        reward *= environment.getDiscountRate();
        reward += payoff.getPayoff(PLAYER, ap.getIndex());
        alphaVector[opState.getIndex()] = reward;
    } while (opState.next());
    return alphaVector;
}
  
/*
 * 機能: OneShotExtension の Reward を得る
 * 引数: oneShotExtension と信念 b
 */
PreciseNumber RepeatedGame::getRewardFromOneShotAndBelief(const OneShotExtension& oneShot,const Belief &b) {
    return getRewardFromAlphaVectorAndBelief(getOneShotAlphaVector(oneShot), b);
}
  
PreciseNumber RepeatedGame::getReward (const int sp) const {
    return this->rewardEquation.getReward(sp);
}

vector<vector<PreciseNumber> > RepeatedGame::getAlphaVectors( void ) const {
    return alphaVectors;
}
