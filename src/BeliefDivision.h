/*
 * BeliefDivision.h
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 */

#include "Common.h"

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#ifndef CDDHANDLER_H_
#include "CddHandler.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

#ifndef BELIEFDIVISION_H_
#define BELIEFDIVISION_H_

class BeliefDivision {
	int dimension;
	vector< vector<Belief> > extremePoints;
	// a0 + a1 x1 + a2 x2 + .. + an xn >= 0
	vector<Inequality> inequalities;
public:
	BeliefDivision();
	BeliefDivision(int dimension);
	void configure(int dimension);
	void setPolytopeToDivision(int division, vector<Belief> po);
    //    void addPolytopeToDivision(int division, vector<Belief> po) 
	vector<Belief>  getDivision(int division) const;
	Inequality getDivisionInequality(int division) const;
	void makeInequalities(int division);
	int getNumberOfDivisions() const;
	int getSizeOfDivisions(int division) const;
	bool existEmptyDivision() const;
    vector <int> indexOfEmptyDivisions() const;
    string toString() const;
};

bool operator == (const BeliefDivision& lhs, const BeliefDivision& rhs);
ostream& operator<<(ostream &os, const BeliefDivision& b);
#endif /* BELIEFDIVISION_H_ */
