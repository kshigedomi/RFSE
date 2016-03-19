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

Profile::Profile() {
	dimension = 0;
}

Profile::~Profile() {
}

/*
* Name: Profile(constructor)
* Function: convert index of jointFSA to respective index of state, action, or signal
* Input: index: index of jointFSA. bounds: radix like numStatesOfPlayer
*/
Profile::Profile(int index, const vector<int> &bounds) {
	dimension = bounds.size();
	data.resize(dimension);
	for (int i = dimension - 1; i >= 0; i--) {
		data[i] = index % bounds[i];
		index /= bounds[i];
	}
}

Profile::Profile(const int myData, int index, const vector<int> &bounds) {
   dimension = bounds.size();
   data.resize(dimension);
   for (int i = dimension - 1; i > 0; --i) {
      data[i] = index % bounds[i];
      index /= bounds[i];
   }
   data[PLAYER] = myData;
}

Profile::Profile(const int myData, const Profile &p) {
   dimension = p.getSize() + 1;
   data.resize(dimension);
   for (int i = dimension - 1; i > 0; --i)
      data[i] = p[i-1];
   data[0] = myData;
}

Profile::Profile(const vector<int> &vec) {
   dimension = vec.size();
   data = vec;
}

Profile::Profile(int a) {
	dimension = 1;
	data.resize(dimension);
	data[0] = a;
}

Profile::Profile(int a, int b) {
	dimension = 2;
	data.resize(dimension);
	data[0] = a; data[1] = b;
}

Profile::Profile(int a, int b, int c) {
	dimension = 3;
	data.resize(dimension);
	data[0] = a; data[1] = b; data[2] = c;
}

Profile::Profile(int a, int b, int c, int d) {
	dimension = 4;
	data.resize(dimension);
	data[0] = a; data[1] = b; data[2] = c; data[3] = d;
}

Profile::Profile(int a, int b, int c, int d, int e) {
	dimension = 5;
	data.resize(dimension);
	data[0] = a; data[1] = b; data[2] = c; data[3] = d; data[4] = e;
}

Profile::Profile(int a, int b, int c, int d, int e, int f) {
	dimension = 6;
	data.resize(dimension);
	data[0] = a; data[1] = b; data[2] = c; data[3] = d; data[4] = e; data[5] = f;
}

int Profile::getSize() const{
	return dimension;
}

/*
* Name: getIndex
* Function: get index as joint FSA.
* Input: bounds: radix like numActionsOfPlayers, numStatesOfPlayers, or numSignalsOfPlayer.
*/
int Profile::getIndex(const vector<int> &bounds) const{
	int res = 0;
	for (int i = 0; i < dimension; i++) {
		res *= bounds[i];
		res += data[i];
	}
	return res;
}

int Profile::operator[] (int index) const{
	if (index >= dimension) return -1;
	return data[index];
}

void Profile::swap(const int i, const int j) {
   swap(data[i], data[j]);
}

void Profile::view() const{
	cout << "(";
	for (int i = 0; i < dimension; i++) {
		if (i > 0) cout << ",";
		cout << data[i];
	}
	cout << ")" << endl;
}

void Profile::setValue(int index, int value){
    if (index < dimension) {
        data[index] = value;
    } else {
        cout<<"Wrong index in setting value for a profile!";
    }
}

int Profile::getValue(int index){
    if (index < dimension) {
        return data[index];
    } else {
        return -1;
    }
}

Profile Profile::getOpponentProfile() const {
  vector<int> opData;
  copy(data.begin() + 1, data.end(), back_inserter(opData));
  return Profile(opData);
}

string Profile::toString() const{
   string res = "(";
   res += ::toString(data[0]);
   for (int i = 1; i < dimension; ++i)
      res += ", " + ::toString(data[i]);
   res += ")";
   return res;
}

ostream& operator<<(ostream &os, const Profile &p) {
   os << p.toString();
   return os;
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
	
