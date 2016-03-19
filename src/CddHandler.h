#ifndef CDDHANDLER_H_
#define CDDHANDLER_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#include "setoper.h"
#include "cdd.h"

struct PolytopeInfo {
  Inequality inequalities;
  vector<vector<PreciseNumber> > extremePoints;
  vector<vector<int> > pointDivision;
  vector<vector<int> > inequalityDivision;
};

class ErrorConvertBeliefToPolytope{};

PolytopeInfo convertInequalityToPolytopeInformation(const Inequality &input);

dd_MatrixPtr dd_VectorToMatrix(const vector< vector<PreciseNumber> > &input, const dd_RepresentationType rep);
dd_MatrixPtr dd_VectorToMatrix(const vector< vector<PreciseNumber> > &input, const dd_RepresentationType rep, const vector<bool> &isLinear);
vector< vector<PreciseNumber> > dd_MatrixToVector(const dd_MatrixPtr &M);

vector< vector<int> > dd_SetFamilyToVector(dd_SetFamilyPtr &GI);

Inequality convertPointToInequality(const vector< vector<PreciseNumber> > &input);
Inequality convertBeliefToInequality(const vector<Belief> &nextBeliefDivision);

pair< vector< vector<PreciseNumber> >, vector< vector<int> > > convertInequalityToPointWithIncidence(const Inequality &input);

vector< vector<PreciseNumber> > intersection(const Inequality &inequalities1, const Inequality &inequalities2);
vector<Belief> merge(const vector<Belief> &polytope1, const vector<Belief> &polytope2);

vector< vector<PreciseNumber> > convertBeliefToPoint(const vector<Belief> &nextBeliefDivision);
vector< vector<PreciseNumber> > convertPointToPoint(const vector< vector<PreciseNumber> > &input);

vector< vector<PreciseNumber> > convertInequalityToPoint(const Inequality &input);

vector<Belief> convertPointToBelief(const vector< vector<PreciseNumber> > &input);
vector<Belief> convertPointToBeliefWithReward(const vector< vector<PreciseNumber> > &input, vector<bool> &isPoint);

bool innerPolytope(vector<Belief> &innerPolytope, vector<Belief> &outerPolytope);
bool isInnerPointOfPolytope(const Belief &point, const vector<Belief> &polytope);

#endif
