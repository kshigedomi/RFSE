/*
 * Profile.cpp
 *
 *  Created on: 2013. 1. 8.
 *      Author: chaerim
 */

/*
 * This class represents StateProfile, ActionProfile, and SignalProfile.
 * It holds respectively a player1's data and a player2's data. Data represent her action, her state, or her signal.
 * But, input is index of jointFSA's action, state, or signal. So, it convert data of jointFSA to data of each player.
 * It also a method to convert data of each player to data of jointFSA named getIndex.
 */
#include "Profile.h"

Profile::~Profile() {
}

Profile::Profile(int index, const vector<int> &argBounds, const vector<vector<string> >& name) : bounds(argBounds), nameData(name) {
	dimension = bounds.size();
	data.resize(dimension);
	for (int i = dimension - 1; i >= 0; i--) {
		data[i] = index % bounds[i];
		index /= bounds[i];
	}
}

Profile::Profile(const int playerData, const Profile& opData, const vector<int> &argBounds, const vector<vector<string> >& name)
    :  bounds(argBounds), nameData(name) {
	this->dimension = bounds.size();
    this->data.push_back(playerData);
    copy(opData.begin(), opData.end(), back_inserter(this->data));
}

int Profile::getSize() const{
	return dimension;
}

int Profile::getIndex() const{
	int res = 0;
	for (int i = 0; i < dimension; i++) {
		res *= bounds[i];
		res += data[i];
	}
	return res;
}

int& Profile::operator[] (const int index) {
	return data[index];
}

const int& Profile::operator[] (const int index) const{
	return data[index];
}

int& Profile::at(int index) {
    return data.at(index);
}

const int& Profile::at(int index) const {
    return data.at(index);
}

vector<int>::iterator Profile::begin( void ) {
    return data.begin();
}

vector<int>::const_iterator Profile::begin( void ) const {
    return data.begin();
}

vector<int>::iterator Profile::end( void ) {
    return data.end();
}

vector<int>::const_iterator Profile::end( void ) const {
    return data.end();
}

/*
 * 機能: 次のプロファイルにする。
 * 返り値: 次のプロファイルが存在しないなら false， そうでないなら true
 */
bool Profile::next() {
    for (int i = dimension-1; i >= 0; --i) {
        ++data[i];
        if (data[i] < bounds[i])
            return true;
        else
            data[i] = 0;
    }
    return false;
}

string Profile::toString() const {
  string res = "(";
  for (int i = 0; i < dimension; ++i) {
	if (i != 0)
	  res +=",";
	res += nameData[i][data[i]];
  }
  res += ")";
  return res;
}
    

ostream& operator<<(ostream &os, const Profile &p) {
    return os << p.toString();
}

bool operator==(const Profile &lp, const Profile &rp) {
  if (lp.getSize() != rp.getSize())
	return false;
  for (int pl = 0; pl < lp.getSize(); ++pl) {
	if (lp[pl] != rp[pl])
	  return false;
  }
  return true;
}

ActionProfile::ActionProfile (int index)
    : Profile(index, ActionProfile::numActionsOfPlayer, ActionProfile::nameActionsOfPlayer) {}
ActionProfile::ActionProfile (const int a, const OpponentActionProfile &opAction)
    : Profile(a, opAction, ActionProfile::numActionsOfPlayer, ActionProfile::nameActionsOfPlayer) {}
vector<int> ActionProfile::numActionsOfPlayer;
vector<vector<string> > ActionProfile::nameActionsOfPlayer;

OpponentActionProfile::OpponentActionProfile (int index)
    : Profile(index, OpponentActionProfile::numActionsOfOpponentPlayer, OpponentActionProfile::nameActionsOfOpponentPlayer) {}
vector<int> OpponentActionProfile::numActionsOfOpponentPlayer;
vector<vector<string> > OpponentActionProfile::nameActionsOfOpponentPlayer;


StateProfile::StateProfile (int index)
    : Profile(index, StateProfile::numStatesOfPlayer, StateProfile::nameStatesOfPlayer) {}
StateProfile::StateProfile (const int a, const OpponentStateProfile &opState)
    : Profile(a, opState, StateProfile::numStatesOfPlayer, StateProfile::nameStatesOfPlayer) {}
vector<int> StateProfile::numStatesOfPlayer;
vector<vector<string> > StateProfile::nameStatesOfPlayer;
OpponentStateProfile::OpponentStateProfile (int index)
    : Profile(index, OpponentStateProfile::numStatesOfOpponentPlayer, OpponentStateProfile::nameStatesOfOpponentPlayer) {}
vector<int> OpponentStateProfile::numStatesOfOpponentPlayer;
    // 自分プレイヤの状態の名前
vector<vector<string> > OpponentStateProfile::nameStatesOfOpponentPlayer;


SignalProfile::SignalProfile (int index)
    : Profile(index, SignalProfile::numSignalsOfPlayer, SignalProfile::nameSignalsOfPlayer) {}
SignalProfile::SignalProfile (const int a, const OpponentSignalProfile &opSignal)
    : Profile(a, opSignal, SignalProfile::numSignalsOfPlayer, SignalProfile::nameSignalsOfPlayer) {}
vector<int> SignalProfile::numSignalsOfPlayer;
vector<vector<string> > SignalProfile::nameSignalsOfPlayer;

OpponentSignalProfile::OpponentSignalProfile (int index)
    : Profile(index, OpponentSignalProfile::numSignalsOfOpponentPlayer, OpponentSignalProfile::nameSignalsOfOpponentPlayer) {}
vector<int> OpponentSignalProfile::numSignalsOfOpponentPlayer;
vector<vector<string> > OpponentSignalProfile::nameSignalsOfOpponentPlayer;




