/*
 * Writer.cpp
 *
 *  Created on: 2012. 12. 6.
 *      Author: chaerim
 */

#include "Writer.h"

Writer::Writer() {
}

Writer::~Writer() {
	fout.close();
}

Writer::Writer(const string &filename) {
	fout.open(filename.c_str());
}

void Writer::openAddFileWithFilename(const string &filename){
    fout.open(filename.c_str(),std::ios::out | std::ios::app);
}

void Writer::openFileWithFilename(const string &filename){
    fout.open(filename.c_str());
}

void Writer::writeVariables(const map<string,PreciseNumber> &values) {
    fout << "variables :";
    for (map<string, PreciseNumber>::const_iterator value = values.begin(); value != values.end(); ++value)
        fout << " " << value->first << "=" << MyUtil::rationalToString(value->second);
    fout << endl;
}

void Writer::writeOptions(const RepeatedGame &rg) {
    fout << "options" << endl;
    fout << rg.optionsToString() << endl;
}

void Writer::writeEnvironment(const RepeatedGame &rg, const Environment &env,const Payoff &pd, const map<string,PreciseNumber> &values) {
    writeOptions(rg);
    writeVariables(values);
    fout << env.toString() << endl;
    fout << pd.toString() << endl;
}

void Writer::writeResult(const Result &result) {
    put("Result : " + MyUtil::resultToString(result));
}

void Writer::writeInvariantDistribution(const vector<Belief> &id, const vector<Automaton>& ms) {
    put("INVARIANT DISTRIBUTIONS");
    for (int myState = 0; myState < ms[PLAYER].getNumberOfStates(); ++myState) {
        put("myState: " + ms[PLAYER].getNameOfStates(myState));
        put(id[myState]);
    }
}

/*
 * 機能: 信念空間を書き出す
 * 引数: 情報 rg, pls
 */
void Writer::writeBeliefSpace(const RepeatedGame &rg, const vector<Automaton>& ms) {
    int opponentStates = JointFSA::getMultipleAccumulate(ms.begin() + 1, ms.end(), Automaton::getNumberOfStates);
    if (opponentStates != 2)
        return;
    put("Belief Space");
    for (PreciseNumber i = ZERO; i <= ONE; i += PreciseNumber("1/10")) {
        Belief b(opponentStates);
        b.set(0, i);
        b.set(1, ONE-i);
        writeProfitableOneShot(b, rg, ms);
        writeOffPathMaxReward(b, rg, ms);
    }
}

/*
 * 機能: オンパスよりも与えられた信念で利得の高い one shot extension を書き出す
 * 引数: Belief の集合 set, その他
 */
void Writer::writeProfitableOneShot(const vector<Belief> &set, const RepeatedGame &rg, const vector<Automaton> &ms) {
    for (vector<Belief>::const_iterator b = set.begin(); b != set.end(); ++b)
        writeProfitableOneShot(*b, rg, ms);
}

void Writer::writeProfitableOneShot(const Belief &b,const RepeatedGame &rg, const vector<Automaton>& ms) {
    put("Belief : " + b.toString());
    // find max on path plan
    const PreciseNumber onpathMaxReward = getMaxReward(b, rg.getAlphaVectors());
    for (int ac = 0; ac < ms[PLAYER].getNumberOfActions(); ++ac) {
        const pair<OneShotExtension, PreciseNumber> oneShot = rg.getMostProfitableOneShotWithAction(ac, b);
        if (oneShot.second > onpathMaxReward) {
            writeOffPath(b, oneShot.first, ms, oneShot.second);
            put("");
        }
    }
}

/*
 * 機能: one shot extension の中で最高の利得を与えるものを書き出す
 * 引数: belief b, その他
 */
void Writer::writeOffPathMaxReward(const Belief &b,const RepeatedGame &rg, const vector<Automaton>& ms) {
    put("Belief : " + b.toString());
    put("maxOffPath");
    OneShotExtension maxOneShot;
    PreciseNumber maxReward = -INF;
    for (int ac = 0; ac < ms[PLAYER].getNumberOfActions(); ++ac) {
        const pair<OneShotExtension, PreciseNumber> oneShot = rg.getMostProfitableOneShotWithAction(ac, b);
        if (oneShot.second > maxReward) {
            maxOneShot = oneShot.first;
            maxReward = oneShot.second;
        }
    }
    writeOffPath(b, maxOneShot, ms, maxReward);
    put("");
}

/*
 * 機能: one shot extension を書き出す
 * 引数: one shot extension の行動 action, 遷移 transition 利得 rewards, その他 pls
 */
void Writer::writeOffPath(const Belief &b, const OneShotExtension &oneShot, const vector<Automaton>& ms, const PreciseNumber &oneShotReward) {
    put(oneShotToString(ms, oneShot));
    put("oneShotReward: " + MyUtil::rationalToString(oneShotReward));
}

void Writer::writeChangeVariables(const char &start, const char &end) {
    char c = start;
    fout << "Change :";
    while (c != end)
        fout << " " << c++;
    fout << endl;
}

void Writer::writeAveragePayoff(const Environment &env, const RepeatedGame &rg, const vector<Automaton>& ms) {
    const vector<vector<PreciseNumber> > alphaVectors = rg.getAlphaVectors();
    for (int myState = 0; myState < ms[PLAYER].getNumberOfStates(); ++myState) {
        OpponentStateProfile opState;
        do {
            const StateProfile sp(myState, opState);
            fout << "State:" << sp << endl;
            put("Reward : " + MyUtil::rationalToString((1-env.getDiscountRate()) * alphaVectors[myState][opState.getIndex()]));
        } while (opState.next());
    }
    put("");
}

void Writer::writeMaxIteration(const RepeatedGame &rg) {
    put("MaxIteration: " + MyUtil::toString(rg.getMaxIteration()));
}

void Writer::writeEqRange(const PreciseNumber &x, const PreciseNumber &under, const PreciseNumber &over) {
    put(MyUtil::rationalToString(x) + "\t" + MyUtil::rationalToString(under) + "\t" + MyUtil::rationalToString(over));
}
   
void Writer::writeAverageSignal(const Environment &env, const vector<Automaton>& ms, const PreciseNumber &value) {
    ActionProfile ap;
    do {
        put("Action : " + ap.toString());
        PreciseNumber ave = ZERO;
        for (int sg = 0; sg < ms[PLAYER].getNumberOfSignals(); ++sg) {
            PreciseNumber prob = ZERO; // 自分のシグナルが sg である確率
            OpponentSignalProfile opSignal;
            do {
                prob += env.getSignalDistribution(ap.getIndex(), SignalProfile(sg, opSignal).getIndex());
            } while (opSignal.next());
            put(ms[PLAYER].getNameOfSignals(sg) + " : " + MyUtil::rationalToString(prob));
            ave += sg * value * prob;
        }
        put("Expected Signal : " + MyUtil::rationalToString(ave));
    } while (ap.next());
}
		
