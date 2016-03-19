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
	int dimension;
	vector<int> data;
public:
	Profile();
	Profile(int index, const vector<int> &bounds);
    Profile(const int myData, int index, const vector<int> &bounds);
    Profile(const int myData, const Profile &p);
    Profile(const vector<int> &vec);
	Profile(int a);
	Profile(int a, int b);
	Profile(int a, int b, int c);
	Profile(int a, int b, int c, int d);
	Profile(int a, int b, int c, int d, int e);
	Profile(int a, int b, int c, int d, int e, int f);
	virtual ~Profile();
	int getSize() const;
	int getIndex(const vector<int> &bounds) const;
	int operator[] (int index) const;
    void swap(const int i, const int j);
	void view() const;
	Profile getOpponentProfile() const;
    
    //added by Dengji
    void setValue(int index, int value);
    int getValue(int index);
    //end

    string toString() const;
	string toString(const vector<string> &name) const;
};

ostream& operator<<(ostream &os, const Profile &p);
bool operator==(const Profile &lp, const Profile &rp);

typedef Profile StateProfile;
typedef Profile ActionProfile;
typedef Profile SignalProfile;

#endif /* PROFILE_H_ */
