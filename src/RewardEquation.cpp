#include "RewardEquation.h"

RewardEquation::RewardEquation() {}
RewardEquation::RewardEquation(const Environment& env, const Payoff& po, const vector<Automaton>& ms) {
    configure(env, po, ms);
}

RewardEquation::~RewardEquation(){}

void RewardEquation::configure(const Environment& env, const Payoff& po, const vector<Automaton>& ms) {
    this->automatons = ms;
    this->environment = env;
    this->payoff = po;
    this->numPlayers = ms.size();
    this->playerStates = ms[PLAYER].getNumberOfStates();
    this->opponentStates = JointFSA::getMultipleAccumulate(ms.begin()+1, ms.end(), Automaton::getNumberOfStates);
    this->numJointStates = JointFSA::getMultipleAccumulate(ms.begin(), ms.end(), Automaton::getNumberOfStates);
    alphaVectors.clear();
}

void RewardEquation::solve() {
    setRewardEquation();
    equation.solve();
    makeAlphaVectors();
}
                                      

void RewardEquation::setRewardEquation() {
    const vector< vector<PreciseNumber> > rewardEquation = makeRewardEquation(PLAYER);
    this->equation.configure(numJointStates, rewardEquation);
}

/*
 * 機能: equation に割引期待利得を解かせるためにセットする
 * メモ: v=u+dAv が割引期待利得の式-> (E-dA)v=u を equation で解く
 */
vector< vector<PreciseNumber> > RewardEquation::makeRewardEquation(const int player) {
    vector< vector<PreciseNumber> > equation(numJointStates, vector<PreciseNumber>(numJointStates + 1));
    StateProfile sp;
    do {
        const ActionProfile ap(JointFSA::actionProfileOfStateProfile(sp, this->automatons));
        equation[sp.getIndex()][numJointStates] = payoff.getPayoff(player, ap.getIndex());
        SignalProfile sgp;
        do {
            const StateProfile nextState = JointFSA::transitionOfStateProfile(sp, sgp, this->automatons);
            equation[sp.getIndex()][nextState.getIndex()] += environment.getSignalDistribution(ap.getIndex(), sgp.getIndex());
        } while (sgp.next());
    } while (sp.next());
    for (int i = 0; i < numJointStates; i++) {
        for (int j = 0; j < numJointStates; j++) {
            equation[i][j] = environment.getDiscountRate() * equation[i][j] * PreciseNumber("-1");
        }
        equation[i][i] = ONE + equation[i][i];
    }

    return equation;
}

void RewardEquation::makeAlphaVectors() {
    for (int i = 0; i < playerStates; i++) {
        vector<PreciseNumber> aV(opponentStates);
        for (int j = 0; j < opponentStates; j++) {
            const StateProfile sp = StateProfile(i, OpponentStateProfile(j));
            aV[j] = getReward(sp.getIndex());
        }
        alphaVectors.push_back(aV);
    }
}

PreciseNumber RewardEquation::getReward(const int state) const {
    return this->equation.getResult(state);
}

vector< vector<PreciseNumber> > RewardEquation::getAlphaVectors() const {
    return alphaVectors;
}
