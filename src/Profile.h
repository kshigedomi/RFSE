/*
 * Profile.h
 *
 *  Created on: 2013. 1. 8.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef PROFILE_H_
#define PROFILE_H_

#ifndef UTIL_H_
#include "Util.h"
#endif

class Profile {
 private:
	int dimension;
	vector<int> data;
    vector<int> bounds;
    vector<vector<string> > nameData;
 public:
    /*
     * Function: コンストラクタ
     * Input: index: index of jointFSA. bounds: numStatesOfPlayer のような既定
     */
	Profile(int index, const vector<int> &argBounds, const vector<vector<string> >& name);
    Profile(const int playerData, const Profile &opData, const vector<int> &argBounds, const vector<vector<string> > &name);
	virtual ~Profile();
	int getSize() const;
    /*
     * Name: getIndex
     * Function: get index as joint FSA.
     * Input: bounds: radix like numActionsOfPlayers, numStatesOfPlayers, or numSignalsOfPlayer.
     */
	int getIndex() const;
	int& operator[] (const int index);
	const int& operator[] (const int index) const ;
    int& at(const int index);
    const int& at(const int index) const ;
    vector<int>::iterator begin(void) ;
    vector<int>::const_iterator begin(void) const ;
    vector<int>::iterator end(void) ;
    vector<int>::const_iterator end(void) const ;
    string toString() const;
    bool next() ;
};

ostream& operator<<(ostream &os, const Profile &p);
bool operator==(const Profile &lp, const Profile &rp);

class OpponentActionProfile : public Profile {
 public:
    // 自分以外のプレイヤの行動数
    static vector<int> numActionsOfOpponentPlayer;
    // 自分プレイヤの行動の名前
    static vector<vector<string> > nameActionsOfOpponentPlayer;
    // コンストラクタ
    OpponentActionProfile(int index = 0);
};

class ActionProfile : public Profile {
 public:
    // 各プレイヤの行動数
    static vector<int> numActionsOfPlayer;
    // 各プレイヤの行動の名前
    static vector<vector<string> > nameActionsOfPlayer;
    // コンストラクタ
    ActionProfile(int index = 0);
    ActionProfile(const int myAction, const OpponentActionProfile& opAction);
};

class OpponentStateProfile : public Profile {
 public:
    // 自分以外のプレイヤの状態数
    static vector<int> numStatesOfOpponentPlayer;
    // 自分プレイヤの状態の名前
    static vector<vector<string> > nameStatesOfOpponentPlayer;
    // コンストラクタ
    OpponentStateProfile(int index = 0);
};

class StateProfile : public Profile {
 public:
    // 各プレイヤの状態数
    static vector<int> numStatesOfPlayer;
    // 各プレイヤの状態の名前
    static vector<vector<string> > nameStatesOfPlayer;
    // コンストラクタ
    StateProfile(int index = 0);
    StateProfile(const int myState, const OpponentStateProfile &opState);
};

class OpponentSignalProfile : public Profile {
 public:
    // 自分以外のプレイヤのシグナル数
    static vector<int> numSignalsOfOpponentPlayer;
    // 自分以外のプレイヤのシグナルの名前
    static vector<vector<string> > nameSignalsOfOpponentPlayer;
    // コンストラクタ
    OpponentSignalProfile(int index = 0);
};

class SignalProfile : public Profile {
 public:
    // 各プレイヤのシグナル数
    static vector<int> numSignalsOfPlayer;
    // 各プレイヤのシグナルの名前
    static vector<vector<string> > nameSignalsOfPlayer;
    // コンストラクタ
    SignalProfile(int index = 0);
    SignalProfile(const int mySignal, const OpponentSignalProfile& opSignal);
};

#endif /* PROFILE_H_ */
