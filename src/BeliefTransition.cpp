#include "BeliefTransition.h"

/*
 * このクラスは信念の遷移を受け持つ.
 * 時間節約のため,長いヒストリの遷移は行列 (BeliefMatrix, BM) として,保存する     
 * つまり, nextBelief = BM * beliefTransition * Belief
 * かつ, nextBM = BM * beliefTransition
 */

/*
 * 機能: BeliefTransition を作る
 * 引数: 環境変数 env, pd , pls
 * メモ: beliefTransition[myAction][mySignal][nextOpState][opState] は
 *       (myAction, mySignal) を見た後 opState にいた相手が nextOpState に移る確率が入っている
 */
void BeliefTransition::make(const Environment& env, const Payoff &pd, const vector<Automaton>& ms) {
    this->playerActions = ms[PLAYER].getNumberOfActions();
    this->playerSignals = ms[PLAYER].getNumberOfSignals();
    this->opponentStates = JointFSA::getMultipleAccumulate(ms.begin()+1, ms.end(), &Automaton::getNumberOfStates);
    this->opponentSignals = JointFSA::getMultipleAccumulate(ms.begin()+1, ms.end(), &Automaton::getNumberOfSignals);
    this->beliefTransition.clear();
    this->beliefTransition.resize(playerActions, vector<vector<vector<PreciseNumber> > > (playerSignals, vector<vector<PreciseNumber> >(opponentStates, vector<PreciseNumber>(opponentStates, ZERO))));
    OpponentStateProfile opState;
    do {
        const OpponentActionProfile opAction(JointFSA::actionProfileOfStateProfile(opState, ms));
        OpponentSignalProfile opSignal;
        do {
            const OpponentStateProfile nextOpState(JointFSA::transitionOfStateProfile(opState, opSignal, ms));
            for (int myAction = 0; myAction < playerActions; ++myAction) {
                const ActionProfile ap(myAction, opAction);
                for (int mySignal = 0; mySignal < playerSignals; ++mySignal) {
                    const SignalProfile sgp(mySignal, opSignal);
                    this->beliefTransition[myAction][mySignal][nextOpState.getIndex()][opState.getIndex()] += env.getSignalDistribution(ap.getIndex(), sgp.getIndex());
                }
            }
        } while (opSignal.next());
    } while (opState.next());
}
               
/*
 * 機能: 最初の BeliefMatrix（単位行列）を生成
 * 返り値: 単位行列
 */
BeliefMatrix  BeliefTransition::createBeliefMatrix(const int opponentStates) const { 
    return MyUtil::getIdentityMatrix(opponentStates);
}

/*
 * 機能: 現在の観測から,次の BM を作り出す
 * 返り値: 次の BM
 * 引数: 現在の BM, 観測 firstOb
 * 注意: nextBM = BM * beliefTransition． つまり，与えられた観測は最初の観測
 */
BeliefMatrix BeliefTransition::nextBeliefMatrix(const BeliefMatrix &bMatrix, const pair<int, int> &firstOb) const {
    return MyUtil::matrixMul(bMatrix, beliefTransition[firstOb.first][firstOb.second]);
}

/*
 * 機能: BM と初期信念から,事後信念を作り出す
 * 引数: 初期信念 bd, BM
 * 返り値: 事後信念
 */
Belief BeliefTransition::nextBeliefFromBeliefMatrix(const Belief &bd, const BeliefMatrix &bMatrix) const{
    vector<PreciseNumber> num = MyUtil::matrixVectorProduct(bMatrix, bd.getVector());
    num = MyUtil::normalize(num);
    return Belief(num);
}

/*
 * 機能: BM を使わずに,行動とシグナルから,事後信念を作り出す.
 * 引数: 事前信念 b, 行動 myAction, シグナル mySignal
 * 返り値: 事後信念
 */
Belief BeliefTransition::nextBelief(const Belief &b, const int myAction, const int mySignal) const {
    vector<PreciseNumber> belief = MyUtil::matrixVectorProduct(beliefTransition[myAction][mySignal], b.getVector());
    belief = MyUtil::normalize(belief);
    return Belief(belief);
}

/*
 * 機能: BM を使わずに,行動とシグナルから,事後信念を作り出す.
 * 引数: 事前信念の集合 b, 行動 myAction, シグナル mySignal
 * 返り値: 事後信念の集合
 */
vector<Belief> BeliefTransition::nextBeliefSet(const vector<Belief> &bd, const int action, const int signal) const {
    vector<Belief> res;
    for (vector<Belief>::size_type i = 0; i < bd.size(); i++) {
        res.push_back(nextBelief(bd[i], action, signal));
    }
    sort(res.begin(), res.end());
    return res;
}

/*
 * 機能: 使ってない, 何に使うかもよくわからない
 */
vector<Belief> BeliefTransition::nextBeliefDivision(const vector<Belief> &bd, const int action, const int signal) const {
    vector<Belief> res;

    for (vector<Belief>::size_type i = 0; i < bd.size(); i++) {
        res.push_back(nextBelief(bd[i], action, signal));
    }

    vector< vector<PreciseNumber> > b2p = convertBeliefToPoint(res);
    vector< vector<PreciseNumber> > p2p = convertPointToPoint(b2p);

    res = convertPointToBelief(b2p);
    sort(res.begin(), res.end());

    return res;
}

/*
 * 機能: 昔使っていた,nextBelief.
 * メモ: こちらはほぼ必ず正しい
 */
Belief BeliefTransition::oldNextBelief(const Environment &env, const vector<Automaton> &ms, const Belief &bp, const int myAction, const int mySignal) const {
    Belief res(bp.getDimension());
    OpponentStateProfile nextState;
    do {
        PreciseNumber num = 0, denom = 0;
        OpponentStateProfile opState;
        do {
            PreciseNumber num2 = 0, denom2 = 0;
            const OpponentActionProfile opAction = JointFSA::actionProfileOfStateProfile(opState, ms);
            const ActionProfile ap = ActionProfile(myAction, opAction);
            OpponentSignalProfile opSignal;
            do {
                const SignalProfile sgp(mySignal, opSignal);
                const OpponentStateProfile nextOpState = JointFSA::transitionOfStateProfile(opState, opSignal, ms);
                if (nextState == nextOpState) {
                    num2 += env.getSignalDistribution(ap.getIndex(), sgp.getIndex());
                }
                denom2 += env.getSignalDistribution(ap.getIndex(), sgp.getIndex());
            } while (opSignal.next());
            num += bp.get(opState.getIndex()) * num2;
            denom += bp.get(opState.getIndex()) * denom2;
        } while (opState.next());
        if (denom == 0) {
            denom = 1;
        }
        num /= denom;
        if (num < 0) {
            num = 0;
        }
        if (num > 1) {
            num = 1;
        }
        res.set(nextState.getIndex(), num);
    } while (nextState.next());
    return res;
}   



/*
 * 機能: 信念の遷移を文字列化
 */
string BeliefTransition::toString(const Environment &environment, const vector<Automaton> &ms) const{
    string res;
    int opponentStates = JointFSA::getMultipleAccumulate(ms.begin()+1, ms.end(), &Automaton::getNumberOfStates);
    vector<Belief> beliefSpace = makeBeliefSpace(opponentStates);

    for (int myAction = 0; myAction < ms[PLAYER].getNumberOfActions(); ++myAction) {
        for (int mySignal = 0; mySignal < ms[PLAYER].getNumberOfSignals(); ++mySignal) {
            res +=  "(" + ms[PLAYER].getNameOfActions(myAction) + ", " + ms[PLAYER].getNameOfSignals(mySignal) + ")\n";
            for (int myState = 0; myState < ms[PLAYER].getNumberOfStates(); ++myState) {
                res += oldNextBelief(environment, ms, beliefSpace[myState], myAction, mySignal).toString();
                res += "\n";
            }
        }
    }
    return res;
}
