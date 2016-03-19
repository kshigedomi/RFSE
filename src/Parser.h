/*
 * Parser.h
 *
 *  Created on: 2012. 12. 25.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

#ifndef PARSER_H_
#define PARSER_H_

typedef pair<PreciseNumber, int> result;

PreciseNumber parseEquation(const string &s, map<string, PreciseNumber> &var);
result equation(const string &s, map<string, PreciseNumber> &var, int p = 0);
result factor(const string &s, map<string, PreciseNumber> &var, int p = 0);
result term(const string &s, map<string, PreciseNumber> &var, int p = 0);

#endif /* PARSER_H_ */
