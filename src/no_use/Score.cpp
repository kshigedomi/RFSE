#include "Score.h"

PreciseNumber calcAveManhattanDistance(const vector<Belief> &points, const BeliefDivision &target) {
   if (points.size() == 0)
      return ZERO;
   PreciseNumber sumDistance = ZERO;
   for (vector<Belief>::const_iterator b = points.begin(); b != points.end(); ++b)
      sumDistance += calcManhattanDistance(*b, target);
   return sumDistance / points.size();
}

PreciseNumber calcManhattanDistance(const Belief &point, const BeliefDivision &target) {
   PreciseNumber minDistance = INF;
   for (int div = 0; div < target.getNumberOfDivisions(); ++div) {
      const PreciseNumber tmp = calcManhattanDistanceWithOneDivision(point, target.getDivisionInequality(div));
      minDistance = min(minDistance, tmp);
   }
   return minDistance;
}

PreciseNumber calcManhattanDistanceWithOneDivision(const Belief &point,const Inequality &polytope) {
   for (vector<bool>::size_type i=0; i<polytope.isLinear.size(); i++){
    if (polytope.isLinear[i]){
      printf("error. is not linear\n");
      return INF;
    }
  }
  vector < vector <PreciseNumber> > coefficient = polytope.coefficient;

  /*               
   * matrix
   *   cnst x y
   * = | b -A 0 | sizeOfA
   *    | p -E E |  sizeOfX
   *    | -p E E |  sizeOfX
   *    | 0 0 E|    sizeOfX
   *    | 0 E 0|   sizeOfX
   *    | 1 -1 0| 1
   * -> Ax <= b
   *      y >= -(p-x)
   *      y >= (p-x)
   *      y >= 0
   *      x >= 0
   *      1 >= sum(x)
   */
  dd_set_global_constants();

  int sizeOfX = coefficient[0].size()-1;
  int sizeOfA = coefficient.size();
  dd_MatrixPtr M = NULL;
  dd_rowrange i;
  dd_colrange j;
  dd_rowrange m = sizeOfA+4*sizeOfX+1; //(Ax <= b) +  calculate absolute value
  dd_colrange d = coefficient[0].size()+sizeOfX;
  dd_NumberType NT = dd_Rational;
  
  M = dd_CreateMatrix(m, d);
  M->objective = dd_LPmin;
  M->numbtype = NT;
  
  // Ax <= b
  for (i = 0; i < coefficient.size(); i++) {
     for (j = 0; j < coefficient[0].size(); j++){
      dd_set(M->matrix[i][j], coefficient[i][j].get_mpq_t());
    }
    for (; j < d; j++){
      dd_set(M->matrix[i][j], ZERO.get_mpq_t());
    }
  }

  // y >= -(p-x)
  for (dd_rowrange s=0; s < sizeOfX ; s++) {
    dd_set(M->matrix[sizeOfA+s][0],point.get(s).get_mpq_t());
    for (j = 1; j < d; j++){
      if (s+1 == j){
	dd_set(M->matrix[sizeOfA+s][j],PreciseNumber(-1*ONE).get_mpq_t());
      } else if (coefficient[0].size()+s == j){
	dd_set(M->matrix[sizeOfA+s][j],ONE.get_mpq_t());
      } else {
	dd_set(M->matrix[sizeOfA+s][j],ZERO.get_mpq_t());
      }
    }
  }
  
  // y >= (p-x)
  for (dd_rowrange s=0; s < sizeOfX ; s++) {
    dd_set(M->matrix[sizeOfA+sizeOfX+s][0],PreciseNumber(-1*point.get(s)).get_mpq_t());
    for (j = 1; j < d; j++){
      if (s+1 == j){
	dd_set(M->matrix[sizeOfA+sizeOfX+s][j],ONE.get_mpq_t());
      } else if (coefficient[0].size()+s == j){
	dd_set(M->matrix[sizeOfA+sizeOfX+s][j],ONE.get_mpq_t());
      } else {
	dd_set(M->matrix[sizeOfA+sizeOfX+s][j],ZERO.get_mpq_t());
      }
    }
  }

  // y >= 0
  for (dd_rowrange s=0; s < sizeOfX ; s++){
    for (j = 0; j < d; j++) {
      if (j==coefficient[0].size()+s){
	dd_set(M->matrix[sizeOfA+2*sizeOfX+s][j],ONE.get_mpq_t());
      } else {
	dd_set(M->matrix[sizeOfA+2*sizeOfX+s][j],ZERO.get_mpq_t());
      }
    }
  }

  // x >= 0
  for (dd_rowrange s=0; s < sizeOfX; s++) {
     for (j = 0; j < d; j++) {
        if (j==s+1) {
           dd_set(M->matrix[sizeOfA+3*sizeOfX+s][j],ONE.get_mpq_t());
        } else {
           dd_set(M->matrix[sizeOfA+3*sizeOfX+s][j],ZERO.get_mpq_t());
        }
     }
  }

  // 1 >= sum(x)
  for (j = 0; j < d; j++) {
     if (j==0) {
        dd_set(M->matrix[sizeOfA+4*sizeOfX][j],ONE.get_mpq_t());
     } else if (j < coefficient[0].size()) {
        dd_set(M->matrix[sizeOfA+4*sizeOfX][j],PreciseNumber(-1*ONE).get_mpq_t());
     } else {
        dd_set(M->matrix[sizeOfA+4*sizeOfX][j],ZERO.get_mpq_t());
     }
  }

  // min(sum(y))
  for (j = 0; j < coefficient[0].size(); j++){
    dd_set(M->rowvec[j],ZERO.get_mpq_t());
  }

  for (j = coefficient[0].size(); j < d; j++){
    dd_set(M->rowvec[j],ONE.get_mpq_t());
  }

/*
  vector < vector <PreciseNumber> > tmp = dd_MatrixToVector(M);
  for (int i=0; i<tmp.size(); i++){
    for (int j=0; j<tmp[j].size(); j++){
      printf("%lf ",tmp[i][j].get_d());
    }
    printf("\n");
    }
  printf("\n");
*/

  dd_LPSolverType solver = dd_DualSimplex;
  dd_LPPtr lp;
  dd_ErrorType err;
  PreciseNumber result;
  lp = dd_Matrix2LP(M, &err);
  if (err == dd_NoError) {
    dd_LPSolve(lp, solver, &err);
    if (err == dd_NoError) {
      result = PreciseNumber(lp->optvalue);
    }
  }

  //intf("result: %lf\n",result.get_d());
  dd_FreeLPData(lp);
  dd_FreeMatrix(M);
  return result;
}

PreciseNumber calcDistance(const Belief &point,const Inequality &polytope) {
  for (int i=0; i<polytope.isLinear.size(); i++){
    if (polytope.isLinear[i]){
      printf("error. is not linear\n");
      return INF;
    }
  }
  vector < vector <PreciseNumber> > coefficient = polytope.coefficient;
  
  /*               
   * matrix
   *   cnst x y z
   * = | b -A 0 0| sizeOfA
   *    | p -E E 0|  sizeOfX
   *    | -p E E 0|  sizeOfX
   *    | 0 0 E 0|    sizeOfX
   *    | 0 E 0 0|   sizeOfX
   *    | 1 -1 0 0| 1
   *    | 0 0 -E 1| sizeOfX
   * -> Ax <= b
   *      y >= -(p-x)
   *      y >= (p-x)
   *      y >= 0
   *      x >= 0
   *      1 >= sum(x)
   *       z >= y
   */
  dd_set_global_constants();

  int sizeOfX = coefficient[0].size()-1;
  int sizeOfA = coefficient.size();
  dd_MatrixPtr M = NULL;
  dd_rowrange i;
  dd_colrange j;
  dd_rowrange m = sizeOfA+5*sizeOfX+1; //(Ax <= b) +  calculate absolute value
  dd_colrange d = coefficient[0].size()+sizeOfX+1;
  dd_NumberType NT = dd_Rational;
  
  M = dd_CreateMatrix(m, d);
  M->objective = dd_LPmin;
  M->numbtype = NT;
  
  // Ax <= b
  for (i = 0; i < coefficient.size(); i++) {
    for (j = 0; j < coefficient[0].size(); j++){
      dd_set(M->matrix[i][j], coefficient[i][j].get_mpq_t());
    }
    for (; j < d; j++){
      dd_set(M->matrix[i][j], ZERO.get_mpq_t());
    }
  }

  // y >= -(p-x)
  for (dd_rowrange s=0; s < sizeOfX ; s++) {
    dd_set(M->matrix[sizeOfA+s][0],point.get(s).get_mpq_t());
    for (j = 1; j < d; j++){
      if (s+1 == j){
	dd_set(M->matrix[sizeOfA+s][j],PreciseNumber(-1*ONE).get_mpq_t());
      } else if (coefficient[0].size()+s == j){
	dd_set(M->matrix[sizeOfA+s][j],ONE.get_mpq_t());
      } else {
	dd_set(M->matrix[sizeOfA+s][j],ZERO.get_mpq_t());
      }
    }
  }
  
  // y >= (p-x)
  for (dd_rowrange s=0; s < sizeOfX ; s++) {
    dd_set(M->matrix[sizeOfA+sizeOfX+s][0],PreciseNumber(-1*point.get(s)).get_mpq_t());
    for (j = 1; j < d; j++){
      if (s+1 == j){
	dd_set(M->matrix[sizeOfA+sizeOfX+s][j],ONE.get_mpq_t());
      } else if (coefficient[0].size()+s == j){
	dd_set(M->matrix[sizeOfA+sizeOfX+s][j],ONE.get_mpq_t());
      } else {
	dd_set(M->matrix[sizeOfA+sizeOfX+s][j],ZERO.get_mpq_t());
      }
    }
  }

  // y >= 0
  for (dd_rowrange s=0; s < sizeOfX ; s++){
    for (j = 0; j < d; j++) {
      if (j==coefficient[0].size()+s){
	dd_set(M->matrix[sizeOfA+2*sizeOfX+s][j],ONE.get_mpq_t());
      } else {
	dd_set(M->matrix[sizeOfA+2*sizeOfX+s][j],ZERO.get_mpq_t());
      }
    }
  }

  // x >= 0
  for (dd_rowrange s=0; s < sizeOfX; s++) {
     for (j = 0; j < d; j++) {
        if (j==s+1) {
           dd_set(M->matrix[sizeOfA+3*sizeOfX+s][j],ONE.get_mpq_t());
        } else {
           dd_set(M->matrix[sizeOfA+3*sizeOfX+s][j],ZERO.get_mpq_t());
        }
     }
  }
  // 1 >= sum(x)
  for (j = 0; j < d; j++) {
     if (j==0) {
        dd_set(M->matrix[sizeOfA+4*sizeOfX][j],ONE.get_mpq_t());
     } else if (j < coefficient[0].size()) {
        dd_set(M->matrix[sizeOfA+4*sizeOfX][j],PreciseNumber(-1*ONE).get_mpq_t());
     } else {
        dd_set(M->matrix[sizeOfA+4*sizeOfX][j],ZERO.get_mpq_t());
     }
  }

  // z >= y
  for (dd_rowrange s=0; s < sizeOfX; s++) {
     for (j = 0; j < d; j++) {
        if (j==coefficient[0].size()+s) {
           dd_set(M->matrix[sizeOfA+4*sizeOfX+2][j],PreciseNumber(-1*ONE).get_mpq_t());
        }  else if (j==coefficient[0].size()+sizeOfX) {
           dd_set(M->matrix[sizeOfA+4*sizeOfX+2][j],ONE.get_mpq_t());
        } else {
           dd_set(M->matrix[sizeOfA+4*sizeOfX+2][j],ZERO.get_mpq_t());
        }
     }
  }
  // min(z)
  for (j=0; j<d; j++) {
     if (j == coefficient[0].size()+sizeOfX) {
        dd_set(M->rowvec[j],ONE.get_mpq_t());
     } else {
        dd_set(M->rowvec[j],ZERO.get_mpq_t());
     }
  }

  /*  vector < vector <PreciseNumber> > tmp = dd_MatrixToVector(M);
  for (int i=0; i<tmp.size(); i++){
    for (int j=0; j<tmp[j].size(); j++){
      printf("%lf ",tmp[i][j].get_d());
    }
    printf("\n");
    }
    printf("\n");*/
  dd_LPSolverType solver = dd_DualSimplex;
  dd_LPPtr lp;
  dd_ErrorType err;
  PreciseNumber result;
  lp = dd_Matrix2LP(M, &err);
  if (err == dd_NoError) {
    dd_LPSolve(lp, solver, &err);
    if (err == dd_NoError) {
      result = PreciseNumber(lp->optvalue);
    }
  }

  //intf("result: %lf\n",result.get_d());
  dd_FreeLPData(lp);
  dd_FreeMatrix(M);
  return result;
}

vector<Belief> getSampleBeliefs(const int numOpStates) {
   vector<Belief> samples;
   Belief b(numOpStates);
   for (int opState=0; opState < numOpStates; opState++) {
      b.set(opState,1/(double)numOpStates);
   }
   samples.push_back(b);
   for (int i=0; i<numOpStates; i++) {
      for (int opState = 0; opState < numOpStates; opState++) {
         if (i == opState) {
            b.set(opState,(numOpStates+1)/(double)(2*numOpStates));
         } else {
            b.set(opState,1/(double)(2*numOpStates));
         }
      }
      samples.push_back(b);
   }
   return samples;
}

PreciseNumber calcAveGapBetweenRewards(const vector<Belief> &samples, const vector<vector<PreciseNumber> > &alphaVectors, const vector<OneShotExtension> &oneShotExtensions) {
   assert(samples.size() != 0);
   PreciseNumber sumGap = ZERO;
   for (vector<Belief>::size_type i=0; i<samples.size(); i++) {
	 const PreciseNumber reward = getMaxReward(samples[i], alphaVectors);
	 const PreciseNumber oneShotReward = getOffPathMaxReward(samples[i], oneShotExtensions);
      const PreciseNumber gap = oneShotReward - reward;
      sumGap += max(gap, ZERO);
   }
   return sumGap / samples.size();
}

PreciseNumber maxRewardWithBelief(const vector < vector <PreciseNumber> > &vectors,const Belief &b){
   PreciseNumber res=ZERO;
   for (int i=0; i < vectors.size(); i++) {
      const PreciseNumber tmp = getRewardFromAlphaVectorAndBelief(vectors[i],b);
      res = max(res, tmp);
   }
   return res;
}


