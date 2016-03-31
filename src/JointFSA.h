#ifndef JOINTFSA_H_
#define JOINTFSA_H_

#include "Automaton.h"
#include "Profile.h"

namespace JointFSA {
    int getMultipleAccumulate(vector<Automaton>::const_iterator beg, vector<Automaton>::const_iterator end, int (Automaton::*member)() const);
    ActionProfile actionProfileOfStateProfile(const StateProfile sp, const vector<Automaton>& ms);
    OpponentActionProfile actionProfileOfStateProfile(const OpponentStateProfile sp, const vector<Automaton>& ms);
    StateProfile transitionOfStateProfile(const StateProfile &sp, const SignalProfile &sgp, const vector<Automaton>& ms);
    OpponentStateProfile transitionOfStateProfile(const OpponentStateProfile &sp, const OpponentSignalProfile &sgp, const vector<Automaton>& ms);
};
        
#endif    
