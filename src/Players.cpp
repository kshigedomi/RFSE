/*
 * This class holds a player's situation or information.
 * It has the number of player.
 * It also has his automaton.
 * It has methods to get names of states, actions, or signals by inputting index. 
 * Conversly, it also has methods to get index of states, actions, or signals by inputting string.
 * CAUSION: This class work well only if the number of all automaton's states is same as when the program constructed this class
 */

#include "Players.h"

Players::Players(){}

void Players::configure(const int num) {
   numPlayers = num;
   names.resize(numPlayers);
   automatons.resize(numPlayers);
}
/*
 * 行動，状態，シグナルの数をセットする
 */
void Players::setJointFSA() {
   numStatesOfPlayer.clear();
   numSignalsOfPlayer.clear();
   numActionsOfPlayer.clear();

   numStatesOfPlayer.resize(numPlayers);
   numSignalsOfPlayer.resize(numPlayers);
   numActionsOfPlayer.resize(numPlayers);
   
   for (int i = 0; i < numPlayers; i++) {
      numStatesOfPlayer[i] = getNumberOfStates(i);
      numSignalsOfPlayer[i] = getNumberOfSignals(i);
      numActionsOfPlayer[i] = getNumberOfActions(i);
   }

   numStatesOfOpponentPlayer.clear();
   copy(numStatesOfPlayer.begin() + 1, numStatesOfPlayer.end(), back_inserter(numStatesOfOpponentPlayer));
   numSignalsOfOpponentPlayer.clear();
   copy(numSignalsOfPlayer.begin() + 1, numSignalsOfPlayer.end(), back_inserter(numSignalsOfOpponentPlayer));
   numActionsOfOpponentPlayer.clear();
   copy(numActionsOfPlayer.begin() + 1, numActionsOfPlayer.end(), back_inserter(numActionsOfOpponentPlayer));

   opponentStates = accumulate(numStatesOfPlayer.begin() + 1, numStatesOfPlayer.end(), 1, multiplies<int>());
   opponentActions = accumulate(numActionsOfPlayer.begin() + 1, numActionsOfPlayer.end(), 1, multiplies<int>());
   opponentSignals = accumulate(numSignalsOfPlayer.begin() + 1, numSignalsOfPlayer.end(), 1, multiplies<int>());
}

int Players::getNumberOfPlayers() const {
   return numPlayers;
}

void Players::setAllAutomaton(const Automaton &m) {
   for (vector<Automaton>::size_type pl = 0; pl < automatons.size(); ++pl)
      setAutomaton(pl, m);
}
      

void Players::setAutomaton(const int pl,const Automaton &st) {
   automatons[pl] = st;
}

Automaton Players::getAutomaton(const int pl) const{
   return automatons[pl];
}

string Players::getNameOfPlayer(const int pl) const{
   return names[pl];
}

void Players::setNameOfPlayer(const int pl,const string &name) {
   names[pl] = name;
}

string Players::getNameOfStates(const int pl, const int index) const{
   return automatons[pl].getNameOfStates(index);
}

string Players::getNameOfActions(const int pl,const int index) const{
   return automatons[pl].getNameOfActions(index);
}

string Players::getNameOfSignals(const int pl, const int index) const{
   return automatons[pl].getNameOfSignals(index);
}

int Players::getNumberOfStates(const int pl) const{
   return automatons[pl].getNumberOfStates();
}

int Players::getNumberOfActions(const int pl) const{
   return automatons[pl].getNumberOfActions();
}

int Players::getNumberOfSignals(const int pl) const{
   return automatons[pl].getNumberOfSignals();

}

int Players::getActionChoice(const int pl, const int state) const{
	return automatons[pl].getActionChoice(state);
}

int Players::getStateTransition(const int pl, const int state, const int signal) const{
	return automatons[pl].getStateTransition(state, signal);
}

int Players::getIndexOfStates(const int pl,const string &state) const{
	return automatons[pl].getIndexOfStates(state);
}

int Players::getIndexOfActions(const int pl, const string &action) const{
	return automatons[pl].getIndexOfActions(action);
}

int Players::getIndexOfSignals(const int pl, const string &signal) const{
	return automatons[pl].getIndexOfSignals(signal);
}

ActionProfile Players::actionsOfJointStates(const StateProfile &sp) const{
   int actions = 0;
   for (int p = 0; p < sp.getSize(); p++) {
	 actions *= getNumberOfActions(p); // basis
	 actions += getActionChoice(p, sp[p]); // action
   }
   return getActionProfile(actions);
}

int Players::actionsOfJointStates(const int spIndex) const {
  return getAction(actionsOfJointStates(getStateProfile(spIndex)));
}

  
ActionProfile Players::opponentActionsOfJointStates(const StateProfile &sp) const{
   int actions = 0;
   const int size = sp.getSize();
   if (numPlayers != size + 1) {
      Message::alert("size error in RepeatedGame::opponentActionOfJointStates");
      assert(true);
   }
   for (int p = OPPONENT; p < size + 1; p++) {
      actions *= getNumberOfActions(p);
      actions += getActionChoice(p, sp[p-1]);
   }
   return getOpponentActionProfile(actions);
}

int Players::opponentActionsOfJointStates(const int spIndex) const {
   const StateProfile sp = getOpponentStateProfile(spIndex);
   int actions = 0;
   for (int p = OPPONENT; p < getNumberOfPlayers(); p++) {
      actions *= getNumberOfActions(p);
      actions += getActionChoice(p, sp[p-1]);
   }
   return actions;
}
   

/*
 * Name: transitionsOfJointStates
 * Function: get next state profile. input sp transitions to it with input op.
 * Input: sp: current state profile. op: current signal profile.
 * Output: next state profile
 */
StateProfile Players::transitionsOfJointStates(const StateProfile &sp, const SignalProfile &op) const{
   int transitions = 0;
   for (int p = 0; p < numPlayers; p++) {
      transitions *= getNumberOfStates(p);
      transitions += getStateTransition(p, sp[p], op[p]);
   }
   return getStateProfile(transitions);
}

int Players::transitionsOfJointStates(const int spIndex, const int sgIndex) const {
   const StateProfile sp = getStateProfile(spIndex);
   const SignalProfile op = getSignalProfile(sgIndex);
   int transitions = 0;
   for (int p = 0; p < numPlayers; p++) {
      transitions *= getNumberOfStates(p);
      transitions += getStateTransition(p, sp[p], op[p]);
   }
   return transitions;
}

StateProfile Players::opponentTransitionsOfJointStates(const StateProfile &sp, const SignalProfile &op) const{
   int transitions = 0;
   const int size = sp.getSize();
   const int tmp = op.getSize();
   if (numPlayers != size + 1 || tmp != size) {
      cerr << "ERROR : size error" << endl;
      cerr << "RepeatedGame::opponentActionOfJointStates" << endl;
      assert(true);
   }
   for (int p = OPPONENT; p < size + 1; p++) {
      transitions *= getNumberOfStates(p);
      transitions += getStateTransition(p, sp[p-1], op[p-1]);
   }
   return getOpponentStateProfile(transitions);
}

int Players::opponentTransitionsOfJointStates(const int spIndex, const int sgIndex) const {
   const StateProfile sp = getOpponentStateProfile(spIndex);
   const SignalProfile op = getOpponentSignalProfile(sgIndex);
   int transitions = 0;
   for (int p = OPPONENT; p < getNumberOfPlayers(); p++) {
      transitions *= getNumberOfStates(p);
      transitions += getStateTransition(p, sp[p-1], op[p-1]);
   }
   return transitions;
}

StateProfile Players::getStateProfile(const int spIndex) const{
   return StateProfile(spIndex, numStatesOfPlayer);
}

StateProfile Players::getOpponentStateProfile(const int opSpIndex) const{
   return StateProfile(opSpIndex, numStatesOfOpponentPlayer);
}

ActionProfile Players::getActionProfile(const int apIndex) const{
   return ActionProfile(apIndex, numActionsOfPlayer);
}

ActionProfile Players::getOpponentActionProfile(const int opApIndex) const{
   return ActionProfile(opApIndex, numActionsOfOpponentPlayer);
}

SignalProfile Players::getSignalProfile(const int opIndex) const{
   return SignalProfile(opIndex, numSignalsOfPlayer);
}

SignalProfile Players::getOpponentSignalProfile(const int opOpIndex) const{
   return SignalProfile(opOpIndex, numSignalsOfOpponentPlayer);
}

StateProfile Players::getStateProfile(const int myData, const int opData) const{
   return StateProfile(myData, opData, numStatesOfPlayer);
}

string Players::stateProfileToString(const StateProfile &sp) const {
  string res = "(";
  for (int pl = 0; pl < getNumberOfPlayers(); ++pl) {
	if (pl != 0)
	  res +=",";
	res += getNameOfStates(pl, sp[pl]);
  }
  res += ")";
  return res;
}

/*
 * 機能: プロファイルを文字列にして返す
 * 引数: プロファイルとプレイヤ
 * 返り値: 文字列
 */
string Players::actionProfileToString(const ActionProfile &sp) const {
  string res = "(";
  for (int pl = 0; pl < getNumberOfPlayers(); ++pl) {
	if (pl != 0)
	  res +=",";
	res += getNameOfActions(pl, sp[pl]);
  }
  res += ")";
  return res;
}


string Players::signalProfileToString(const SignalProfile &sp) const {
  string res = "(";
  for (int pl = 0; pl < getNumberOfPlayers(); ++pl) {
	if (pl != 0)
	  res +=",";
	res += getNameOfSignals(pl, sp[pl]);
  }
  res += ")";
  return res;
}


ActionProfile Players::getActionProfile(const int myData, const int opData) const{
   return ActionProfile(myData, opData, numActionsOfPlayer);
}

SignalProfile Players::getSignalProfile(const int myData, const int opData) const{
   return SignalProfile(myData, opData, numSignalsOfPlayer);
}

StateProfile Players::getStateProfile(const int myData, const StateProfile &opState_p) const{
   return StateProfile(myData, opState_p);
}

ActionProfile Players::getActionProfile(const int myData, const ActionProfile &opAction_p) const{
   return ActionProfile(myData, opAction_p);
}

SignalProfile Players::getSignalProfile(const int myData, const SignalProfile &opSignal_p) const{
   return SignalProfile(myData, opSignal_p);
}

int Players::getState(const StateProfile& sp) const{
   return sp.getIndex(numStatesOfPlayer);
}

int Players::getOpponentState(const StateProfile &sp) const{
   return sp.getIndex(numStatesOfOpponentPlayer);
}

int Players::getAction(const ActionProfile& sp) const{
   return sp.getIndex(numActionsOfPlayer);
}

int Players::getOpponentAction(const ActionProfile &sp) const{
   return sp.getIndex(numActionsOfOpponentPlayer);
}

int Players::getSignal(const SignalProfile& sp) const{
   return sp.getIndex(numSignalsOfPlayer);
}

int Players::getOpponentSignal(const SignalProfile &sp) const{
   return sp.getIndex(numSignalsOfOpponentPlayer);
}

int Players::getNumberOfOpponentStates() const {
   return opponentStates;
}

int Players::getNumberOfOpponentSignals() const {
   return opponentSignals;
}

int Players::getNumberOfOpponentActions() const {
   return opponentActions;
}

int Players::getNumberOfJointStates() const {
   return accumulate(numStatesOfPlayer.begin(), numStatesOfPlayer.end(), 1, multiplies<int>());
}

int Players::getNumberOfJointActions() const {
   return accumulate(numActionsOfPlayer.begin(), numActionsOfPlayer.end(), 1, multiplies<int>());
}

int Players::getNumberOfJointSignals() const {
   return accumulate(numSignalsOfPlayer.begin(), numSignalsOfPlayer.end(), 1, multiplies<int>());
}

string Players::toString() const {
   string res;

   res += "** Players **\n";
   res += "* numPlayers = " + ::toString(numPlayers) + "\n";
   res += "** name **\n";
   res += names[PLAYER];
   for (int pl = OPPONENT; pl < numPlayers; ++pl)
      res += " " + names[pl];
   res += "\n";
   
   res += "** Automaton **\n";
   for (int pl = 0; pl < numPlayers; ++pl)
      res += automatons[pl].toString();
   res += "\n";
   
   return res;
}

ostream& operator<<(ostream &os, const Players &pls) {
   return os << pls.toString();
}

