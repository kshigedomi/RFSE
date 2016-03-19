#ifndef PLAYERS_H_
#define PLAYERS_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef PROFILE_H_
#include "Profile.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif


class Players {
    int numPlayers;
    vector<string> names;
    vector<Automaton> automatons;

    vector<int> numActionsOfPlayer;
    vector<int> numSignalsOfPlayer;
    vector<int> numStatesOfPlayer;

    vector<int> numActionsOfOpponentPlayer;
    vector<int> numSignalsOfOpponentPlayer;
    vector<int> numStatesOfOpponentPlayer;

    int opponentStates;
    int opponentSignals;
    int opponentActions;
    
 public:
    Players();
    
    void configure(const int num);
    void setJointFSA();
    
    void setAutomaton(const int pl, const Automaton &st);
    void setAllAutomaton(const Automaton &m);
    Automaton getAutomaton(const int pl) const;

    int getNumberOfPlayers() const;
    string getNameOfPlayer(const int pl) const;
    void setNameOfPlayer(const int pl, const string &name);

    string getNameOfStates(const int pl, const int index) const;
    string getNameOfActions(const int pl, const int index) const;
    string getNameOfSignals(const int pl, const int index) const;

    int getNumberOfStates(const int pl) const;
    int getNumberOfActions(const int pl) const;
    int getNumberOfSignals(const int pl) const;

    int getActionChoice(const int pl, const int state) const;
    int getStateTransition(const int pl, const int state, const int signal) const;

    int getIndexOfStates(const int pl, const string &state) const;
    int getIndexOfActions(const int pl, const string &state) const;
    int getIndexOfSignals(const int pl, const string &state) const;

    ActionProfile actionsOfJointStates(const StateProfile &sp) const;
	int actionsOfJointStates(const int spIndex) const;
    ActionProfile opponentActionsOfJointStates(const StateProfile &sp) const;
    int opponentActionsOfJointStates(const int spIndex) const;
    
    StateProfile transitionsOfJointStates(const StateProfile &sp, const SignalProfile &op) const; 
    int transitionsOfJointStates(const int spIndex, const int sgIndex) const;
    StateProfile opponentTransitionsOfJointStates(const StateProfile &sp, const SignalProfile &op) const;
    int opponentTransitionsOfJointStates(const int sp, const int op) const;


    StateProfile getStateProfile(const int spIndex) const;
    StateProfile getOpponentStateProfile(const int opSpIndex) const;
    StateProfile getStateProfile(const int myData, const int opData) const;
    StateProfile getStateProfile(const int myData, const StateProfile &opState_p) const;
	string stateProfileToString(const StateProfile &sp) const;
    
    ActionProfile getActionProfile(const int spIndex) const;
    ActionProfile getOpponentActionProfile(const int opSpIndex) const;
    ActionProfile getActionProfile(const int myData, const int opData) const;
    ActionProfile getActionProfile(const int myData, const ActionProfile &opAction_p) const;
	string actionProfileToString(const StateProfile &sp) const;
    
    SignalProfile getSignalProfile(const int spIndex) const;
    SignalProfile getOpponentSignalProfile(const int opSpIndex) const;
    SignalProfile getSignalProfile(const int myData, const int opData) const;
    SignalProfile getSignalProfile(const int myData, const SignalProfile &opSignal_p) const;
	string signalProfileToString(const StateProfile &sp) const;

    int getState(const StateProfile &sp) const;
    int getAction(const ActionProfile &sp) const;
    int getSignal(const SignalProfile &sp) const;
    int getOpponentState(const StateProfile &sp) const;
    int getOpponentAction(const ActionProfile &sp) const;
    int getOpponentSignal(const SignalProfile &sp) const;

    int getNumberOfOpponentStates() const;
    int getNumberOfOpponentActions() const;
    int getNumberOfOpponentSignals() const;

    int getNumberOfJointStates() const;
    int getNumberOfJointActions() const;
    int getNumberOfJointSignals() const;

    string toString() const;
	
};
ostream& operator<<(ostream &os, const Players &players);

#endif
