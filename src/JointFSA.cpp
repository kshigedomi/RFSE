#include "JointFSA.h"

namespace JointFSA {
    int getMultipleAccumulate(vector<Automaton>::const_iterator beg, vector<Automaton>::const_iterator end, int (Automaton::*member)() const ) {
        int res = 1;
        for (vector<Automaton>::const_iterator m = beg; m != end; ++m) {
            res *= ((*m).*member)(); // const_iterator には ->* 演算子がない？
        }
        return res;
    }
    
    ActionProfile actionProfileOfStateProfile(const StateProfile sp, const vector<Automaton>& ms) {
        ActionProfile ap;
        for (int i = 0; i < sp.getSize(); ++i) {
            ap[i] = ms[i].getActionChoice(sp[i]);
        }
        return ap;
    }
    OpponentActionProfile actionProfileOfStateProfile(const OpponentStateProfile sp, const vector<Automaton>& ms) {
        OpponentActionProfile ap;
        for (int i = 0; i < sp.getSize(); ++i) {
            ap[i] = ms[i+1].getActionChoice(sp[i]);
        }
        return ap;
    }

    StateProfile transitionOfStateProfile(const StateProfile &sp, const SignalProfile &sgp, const vector<Automaton>& ms) {
        StateProfile nextState;
        for (int i = 0; i < sp.getSize(); ++i) {
            nextState[i] = ms[i].getStateTransition(sp[i], sgp[i]);
        }
        return nextState;
    }

    OpponentStateProfile transitionOfStateProfile(const OpponentStateProfile &sp, const OpponentSignalProfile &sgp, const vector<Automaton>& ms) {
        OpponentStateProfile nextState;
        for (int i = 0; i < sp.getSize(); ++i) {
            nextState[i] = ms[i+1].getStateTransition(sp[i], sgp[i]);
        }
        return nextState;
    }
};
