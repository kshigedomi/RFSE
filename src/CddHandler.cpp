#include "CddHandler.h"

/*
 * This file handles the cddlib
 */

dd_MatrixPtr dd_VectorToMatrix(const vector< vector<PreciseNumber> > &input, const dd_RepresentationType rep) {
   dd_MatrixPtr M = NULL;
   dd_rowrange i;
   dd_colrange j;
   dd_rowrange m = input.size();
   dd_colrange d = input[0].size();
   dd_NumberType NT = dd_Rational;

   M = dd_CreateMatrix(m, d);
   M->representation = rep;
   M->numbtype = NT;

   for (i = 0; i < m; i++) {
      for (j = 0; j < d; j++) {
         dd_set(M->matrix[i][j], input[i][j].get_mpq_t());
      }
   }
   return M;
}

dd_MatrixPtr dd_VectorToMatrix(const vector< vector<PreciseNumber> > &input, const dd_RepresentationType rep, const vector<bool> &isLinear) {
   dd_MatrixPtr M = NULL;
   dd_rowrange i;
   dd_colrange j;
   dd_rowrange m = input.size();
   dd_colrange d = input[0].size();
   dd_NumberType NT = dd_Rational;

   M = dd_CreateMatrix(m, d);
   M->representation = rep;
   M->numbtype = NT;
   if (isLinear.size() > 0) {
      for (int i = 1; i <= m; i++) {
         if (isLinear[i - 1]) {
            set_addelem(M->linset, i);
         }
      }
   }
   for (i = 0; i < m; i++) {
      for (j = 0; j < d; j++) {
         dd_set(M->matrix[i][j], input[i][j].get_mpq_t());
      }
   }
   return M;
}

vector< vector<PreciseNumber> > dd_MatrixToVector(const dd_MatrixPtr &M) {
   vector< vector<PreciseNumber> > res;
   res.resize(M->rowsize);
   for (int i = 0; i < M->rowsize; i++) {
      res[i].resize(M->colsize);
      for (int j = 0; j < M->colsize; j++) {
         res[i][j] = PreciseNumber(M->matrix[i][j]);
      }
   }

   return res;
}

vector< vector<int> > dd_SetFamilyToVector(dd_SetFamilyPtr &GI) {
   vector< vector<int> > res(GI->famsize);

   for (long i = 0; i < GI->famsize; i++) {
      for (long j = 1; j <= GI->set[i][0]; j++) { // 型違い warning は仕方ない? よく型がわからない
         if (set_member(j, GI->set[i])) {
            res[i].push_back(j - 1);
         }
      }
   }

   return res;
}

Inequality convertPointToInequality(const vector< vector<PreciseNumber> > &input)
{
   vector< vector<PreciseNumber> > res;
   vector<bool> il;

   dd_set_global_constants();
   dd_MatrixPtr M = dd_VectorToMatrix(input, dd_Generator);
   dd_PolyhedraPtr poly;
   dd_ErrorType err;
   // TODO this error is caused by number inconsistency
   poly = dd_DDMatrix2Poly(M, &err);
   if (err == dd_NoError) {
      dd_MatrixPtr A;
      A = dd_CopyInequalities(poly);
      res = dd_MatrixToVector(A);

      il.resize(res.size());

      for (vector<vector<PreciseNumber> >::size_type i = 1; i <= res.size(); i++) {
         if (set_member(i, A->linset)) il[i - 1] = true;
      }

      dd_FreeMatrix(A);
   }
   else {
      cout << "Error" << endl;
   }

   dd_FreeMatrix(M);
   dd_FreePolyhedra(poly);

   dd_free_global_constants();

   return Inequality(res, il);
}

vector<Belief> convertPointToBelief(const vector< vector<PreciseNumber> > &input) {
   vector<Belief> res;
   for (vector<vector<PreciseNumber> >::size_type i = 0; i < input.size(); i++) {
      if (input[i][0] == 0) continue; // ignore if result is extreme ray
      PreciseNumber lastBelief = 1;
      Belief b(input[i].size());
      for (vector<PreciseNumber>::size_type j = 1; j < input[i].size(); j++) {
         b.set(j - 1, input[i][j]);
         lastBelief -= input[i][j];
      }
      b.set(input[i].size() - 1, lastBelief);
      res.push_back(b);
   }

   return res;
}


vector<Belief> convertPointToBeliefWithReward(const vector< vector<PreciseNumber> > &input, vector<bool> &isPoint) {
   vector<Belief> res(input.size());
   for (vector<vector<PreciseNumber> >::size_type i = 0; i < input.size(); i++) {
      if (input[i][0] == 0) continue; // ignore if result is extreme ray
      isPoint[i] = true;
      PreciseNumber lastBelief = 1;
      Belief b(input[i].size() - 1);
      for (vector<PreciseNumber>::size_type j = 1; j < input[i].size() - 1; j++) {
         b.set(j - 1, input[i][j]);
         lastBelief -= input[i][j];
      }
      b.set(input[i].size() - 2, lastBelief);
      res[i] = b;
   }

   return res;
}

vector< vector<PreciseNumber> > convertPointToPoint(const vector< vector<PreciseNumber> > &input)
{
   vector< vector<PreciseNumber> > res;
   dd_set_global_constants();
   dd_MatrixPtr M = dd_VectorToMatrix(input, dd_Generator);
   dd_ErrorType err;
   dd_rowset redrows = dd_RedundantRows(M, &err);
   dd_MatrixPtr M2 = dd_MatrixSubmatrix(M, redrows);
   res = dd_MatrixToVector(M2);
   dd_FreeMatrix(M2);
   dd_FreeMatrix(M);
   dd_free_global_constants();
   return res;
}

vector<Belief> getConvex(vector<Belief> &ps) {
   dd_set_global_constants();

   dd_rowrange i;
   dd_colrange j;
   dd_rowrange m = ps.size();
   dd_colrange d = ps[0].getDimension();

   dd_MatrixPtr M = dd_CreateMatrix(m, d);
   M->representation = dd_Generator;
   M->numbtype = dd_Rational;
   for (i = 0; i < m; i++) {
      dd_set(M->matrix[i][0], ONE.get_mpq_t());
      for (j = 1; j < d; j++) {
         dd_set(M->matrix[i][j], ps[i].get(j - 1).get_mpq_t());
      }
   }
   dd_ErrorType err;
   dd_rowset redrows = dd_RedundantRows(M, &err);
   dd_MatrixPtr M2 = dd_MatrixSubmatrix(M, redrows);
   vector<Belief> res;
   for (int i = 0; i < M2->rowsize; i++) {
      Belief b(d);
      PreciseNumber last = ONE;
      for (int j = 1; j < M2->colsize; j++) {
         b.set(j - 1, PreciseNumber(M2->matrix[i][j]));
         last -= PreciseNumber(M2->matrix[i][j]);
      }
      b.set(d - 1, last);
      res.push_back(b);
   }

   dd_FreeMatrix(M2);
   dd_FreeMatrix(M);
   dd_free_global_constants();

   return res;
}

PolytopeInfo convertInequalityToPolytopeInformation(const Inequality &input) {
   PolytopeInfo polyInfo;

   polyInfo.inequalities = input;

   dd_set_global_constants();
   dd_MatrixPtr M = dd_VectorToMatrix(input.coefficient, dd_Inequality, const_cast<Inequality &>(input).isLinear);

   dd_PolyhedraPtr poly;
   dd_ErrorType err;
   poly = dd_DDMatrix2Poly(M, &err);

   dd_MatrixPtr G;
   G = dd_CopyGenerators(poly);
   polyInfo.extremePoints = dd_MatrixToVector(G);
   dd_FreeMatrix(G);

   dd_SetFamilyPtr GI;
   GI = dd_CopyIncidence(poly);
   polyInfo.pointDivision = dd_SetFamilyToVector(GI);
   dd_FreeSetFamily(GI);

   dd_SetFamilyPtr GI2;
   GI2 = dd_CopyInputIncidence(poly);
   polyInfo.inequalityDivision = dd_SetFamilyToVector(GI2);
   dd_FreeSetFamily(GI2);

   dd_FreeMatrix(M);
   dd_FreePolyhedra(poly);

   dd_free_global_constants();

   return polyInfo;
}

pair< vector< vector<PreciseNumber> >, vector< vector<int> > > convertInequalityToPointWithIncidence(const Inequality &input)
{
   vector< vector<PreciseNumber> > res1;
   vector< vector<int> > res2;

   dd_set_global_constants();
   dd_MatrixPtr M = dd_VectorToMatrix(input.coefficient, dd_Inequality, const_cast<Inequality &>(input).isLinear);

   dd_PolyhedraPtr poly;
   dd_ErrorType err;
   poly = dd_DDMatrix2Poly(M, &err);

   dd_MatrixPtr G;
   G = dd_CopyGenerators(poly);
   res1 = dd_MatrixToVector(G);
   dd_FreeMatrix(G);

   dd_SetFamilyPtr GI;
   GI = dd_CopyIncidence(poly);
   res2 = dd_SetFamilyToVector(GI);
   dd_FreeSetFamily(GI);

   dd_FreeMatrix(M);
   dd_FreePolyhedra(poly);

   dd_free_global_constants();

   return make_pair(res1, res2);
}

vector< vector<PreciseNumber> > convertInequalityToPoint(const Inequality &input)
{
   vector< vector<PreciseNumber> > res1;
   dd_set_global_constants();
   dd_MatrixPtr M = dd_VectorToMatrix(const_cast<Inequality &>(input).coefficient, dd_Inequality, const_cast<Inequality &>(input).isLinear);
   dd_PolyhedraPtr poly;
   dd_ErrorType err;
   poly = dd_DDMatrix2Poly(M, &err);
   if (err == dd_NoError) {
      dd_MatrixPtr G;
      G = dd_CopyGenerators(poly);
      res1 = dd_MatrixToVector(G);
      dd_FreeMatrix(G);
   }
   dd_FreeMatrix(M);
   dd_FreePolyhedra(poly);
   dd_free_global_constants();
   return res1;
}


// dd_PolyhedraPtr convertBeliefToHPolytope(const vector<Belief> &set) {
//    vector<vector<PreciseNumber> > b2p = convertBeliefToPoint(set);

//    dd_set_global_constants();
//    dd_MatrixPtr M = dd_VectorToMatrix(b2p, dd_Generator);
//    dd_PolyhedraPtr poly;
//    dd_ErrorType err;
//    poly = dd_DDMatrix2Poly(M, &err);
//    if (err != dd_NoError)
//       throw ErrorConverBeliefToPolytope();
//    dd_MatrixPtr ineq = dd_CopyInequalities(poly);
//    dd_MatrixPtr G = dd_CopyGenerators(poly);
//    dd_PolyhedraPtr res = dd_CreatePolyhedraData(ineq->rowsize, ineq->colsize);
//    res->representation = dd_Inequality;
//    res->homogeneous = dd_TRUE;

//    for (dd_rowrage i = 1; i <=  ineq->rowsize; ++i) {
//       if (set_member(i, M->linset)) {
//          poly->EqualityIndex[i] = 1;
//       }
//       for (j = 1; j <= ineq->colsize; ++j) {
//          dd_set(poly->A[i-1][j-1], ineq->matrix[i-1][j-1]);
//          if (j==1 && dd_Nonzero(ineq->matrix[i-1][j-1])) poly->homogeneous == dd_FALSE;
//       }
//    }
//    dd_FreeMatrix(M);
//    dd_free_global_constants();
//    return poly;
// }


vector< vector<PreciseNumber> > intersection(const Inequality &inequalities1, const Inequality &inequalities2) {
   Inequality inequalities = inequalities1;
   inequalities.coefficient.insert(inequalities.coefficient.end(), inequalities2.coefficient.begin(), inequalities2.coefficient.end());
   inequalities.isLinear.insert(inequalities.isLinear.end(), inequalities2.isLinear.begin(), inequalities2.isLinear.end());
   return convertInequalityToPoint(inequalities);
}

vector<Belief> merge(const vector<Belief> &polytope1, const vector<Belief> &polytope2) {
   vector< vector<PreciseNumber> > points1 = convertBeliefToPoint(polytope1);
   vector< vector<PreciseNumber> > points2 = convertBeliefToPoint(polytope2);
   points1.insert(points1.end(), points2.begin(), points2.end());
   return convertPointToBelief(convertPointToPoint(points1));
}

vector< vector<PreciseNumber> > convertBeliefToPoint(const vector<Belief> &nextBeliefDivision) {
   vector< vector<PreciseNumber> > res(nextBeliefDivision.size());

   for (vector<vector<PreciseNumber> >::size_type i = 0; i < nextBeliefDivision.size(); i++) {
      res[i].resize(const_cast<Belief &>(nextBeliefDivision[i]).getDimension());
      res[i][0] = 1;
      for (int j = 0; j < const_cast<Belief &>(nextBeliefDivision[i]).getDimension() - 1; j++) {
         res[i][j + 1] = const_cast<Belief &>(nextBeliefDivision[i]).get(j);
      }
   }

   return res;
}

Inequality convertBeliefToInequality(const vector<Belief> &nextBeliefDivision) {
   vector< vector<PreciseNumber> > b2p = convertBeliefToPoint(nextBeliefDivision);
   return convertPointToInequality(b2p);
}

// modified by Dengji on 8 Aug 13
bool isInnerPointOfPolytope(const Belief &point, const vector<Belief> &polytope) {
   bool result = false;
   dd_set_global_constants();

   dd_MatrixPtr M = NULL;
   dd_rowrange i;
   dd_colrange j;
   dd_rowrange m = polytope.size() + 1;
   dd_colrange d = const_cast<vector<Belief> &>(polytope)[0].getDimension() + 2;
   dd_NumberType NT = dd_Rational;

   M = dd_CreateMatrix(m, d);
   M->objective = dd_LPmax;
   M->numbtype = NT;

   for (i = 0; i < m - 1; i++) {
      dd_set(M->matrix[i][0], ZERO.get_mpq_t());
      dd_set(M->matrix[i][1], ONE.get_mpq_t());
      for (j = 0; j < d - 2; j++) {
         dd_set(M->matrix[i][j + 2], PreciseNumber(const_cast<vector<Belief> &>(polytope)[i].get(j) * -1).get_mpq_t());
      }
   }
   

   dd_set(M->matrix[m - 1][0], ONE.get_mpq_t());
   dd_set(M->matrix[m - 1][1], ONE.get_mpq_t());
   dd_set(M->rowvec[0], ZERO.get_mpq_t());
   dd_set(M->rowvec[1], PreciseNumber(ONE * -1).get_mpq_t());

   for (j = 0; j < d - 2; j++) {
      dd_set(M->matrix[i][j + 2], PreciseNumber(point.get(j) * -1).get_mpq_t());
      dd_set(M->rowvec[j + 2], PreciseNumber(point.get(j)).get_mpq_t());
   }

   dd_LPSolverType solver = dd_DualSimplex;
   dd_LPPtr lp;
   dd_ErrorType err;
   lp = dd_Matrix2LP(M, &err);
   if (err == dd_NoError) {
      dd_LPSolve(lp, solver, &err);
      if (err == dd_NoError) {
          //if (PreciseNumber(lp->optvalue) == ZERO) {
          if (PreciseNumber(lp->optvalue) <= ZERO) { /*Dengji: it should be also OK for <0, as =0 means the point is on the faces of the polytope, it will be negative if it is inside.*/
            result = true;
         }
      }
      dd_FreeLPData(lp);
   }
   else {
      cerr << "error" << endl;
   }

   dd_FreeMatrix(M);

   dd_free_global_constants();

   return result;
}

