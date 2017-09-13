/*
 * common.h
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#ifndef GMPRATIONAL
#define GMPRATIONAL
#endif

#ifndef COMMON_H_
#define COMMON_H_

#include <cstdio>
#include <cstdlib>
#include <numeric>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <string>
#include <sstream>
#include <complex>
#include <cstring>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <cmath>
#include <cstdarg>
#include <stack>
#include <queue>
#include <gmpxx.h>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/functional.hpp>
#include <boost/bind.hpp>

using namespace std;

typedef double Number;
typedef mpq_class PreciseNumber;

const PreciseNumber EPS = PreciseNumber("1/1000000000");
const PreciseNumber EPS2= PreciseNumber("1/1000000");
const PreciseNumber INF = PreciseNumber("1000000000");
const PreciseNumber ONE = PreciseNumber(1);
const PreciseNumber ZERO = PreciseNumber(0);

// for 2 person game
#define PLAYER 0
#define OPPONENT 1

enum Result {
    FAILURE = 0,
    NOTEXIST = 1,
    SUCCESS = 2,
    NOTSURE = 3,
    TIMEISOVER = 4,
    PREPROFAILURE = 5,
    GLOBAL = 6,
    NOTCONSISTENT = 7,
	NOTFIXED = 8,
};

struct Inequality {
	vector< vector<PreciseNumber> > coefficient;
	vector<bool> isLinear;
	Inequality() {}
Inequality(vector < vector<PreciseNumber> > ce, vector<bool> il) : coefficient(ce), isLinear(il) {}
};

struct OneShotExtension {
public:
    int action;
    vector<int> transition;
OneShotExtension(const int ac, const vector<int> &ts) : action(ac), transition(ts) {}
OneShotExtension(const int nOfSg) : transition(vector<int> (nOfSg)) {}
    OneShotExtension() {}
};

typedef struct OneShotExtension OneShotExtension;

static map<string, PreciseNumber> globalVar;

#endif /* COMMON_H_ */
