/*
 * Parser.cpp
 *
 *  Created on: 2012. 12. 25.
 *      Author: chaerim
 */

/*
 * This class completes the job after class Read read input file. The job is what interprets input file .
 */

#include "Parser.h"

/*
* Name: Parser (I comment all function in this file)
* Function: convert string to number  
* Input:	s: string in input file like "1-p-2*r"  
*			var: mappings string to number like p=0.95
*			p: point is reading now
* Output:	A number. It obtains by applying mapping to string.
*/

PreciseNumber parseEquation(const string &s, map<string, PreciseNumber> &var) {
	result res = equation(s, var);
	return res.first;
}

result equation(const string &s, map<string, PreciseNumber> &var, int p) {
	result r = factor(s, var, p);
	while (s[r.second] == '+' || s[r.second] == '-') {
		result r_ = factor(s, var, r.second + 1);
		if (s[r.second] == '+') {
			r.first += r_.first;
		} else {
			r.first -= r_.first;
		}
		r.second = r_.second;
	}
	return r;
}

result factor(const string &s, map<string, PreciseNumber> &var, int p) {
	result r = term(s, var, p);
	while (s[r.second] == '*' || s[r.second] == '/') {
		result r_ = term(s, var, r.second + 1);
		if (s[r.second] == '*') {
			r.first *= r_.first;
		} else {
			r.first /= r_.first;
		}
		r.second      = r_.second;
	}
	return r;
}

result term(const string &s, map<string, PreciseNumber> &var, int p) {
	if (s[p] == '-') {
		result r = factor(s, var, p + 1);
		r.first *= -1;
		return r;
	} else if (s[p] == '(') {
		result r = equation(s, var, p + 1);
		r.second += 1; // skip ')'
		return r;
	} else if (isalpha(s[p])) {
		string varname = "";
		while (isalnum(s[p])) {
			varname += s[p++];
		}
		return result(var[varname], p);
	} else {
		PreciseNumber value = 0;
		while (isdigit(s[p])) {
			value = value * 10 + (s[p++] - '0');
		}
		return result(value, p);
	}
}
