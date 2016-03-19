#include "MakeSCG.h"

MakeSCG::MakeSCG(const int nOSt, const int numOfSignals) {
   numOfStates = nOSt;
   limitOfBranches = min(numOfStates, numOfSignals);
   matrix.resize(numOfStates, vector<int>(numOfStates, 0));
   head = -1;
   make();
}
// For output
bool MakeSCG::nextSCG() {
   ++head;
   const int size = scgs.size();
   return head != size;
}
// For output
vector<vector<int> > MakeSCG::getSCG() const {
   return scgs[head];
}

/*
 * Name: Make SCG (strongly connected graph)
 * SCG consists of some euler cycles, 
 * which the cycle pathes the branch only once.
 * 1. find euler graph (and set in S)
 * 2. Let S be the big start state, and return 1.
 *** Variables:
 * count[state] means how many times path the state
 */
void MakeSCG::make() {
   const int start = 0; //start state
   vector<int> count(numOfStates, 0); //count what go to state
   ++count[start];
   vector<int> S(1, start); // start states
   makeEulerStart(S, count);
}

/*
 * Function: make all euler graphs from the states in S
 * Hint: count[state] is the number of times which the program pathes state.
 */
void MakeSCG::makeEulerStart(vector<int> &S, vector<int> &count) {
   for (vector<int>::iterator state = S.begin(); state != S.end(); ++state) {
      if (checkSpace(*state)) {
         for (int nextState = 0; nextState < numOfStates; ++nextState) {
            if (find(S.begin(), S.end(), nextState) == S.end()) {
               vector<int> countThisCycle(numOfStates, 0);
               goToNextState(*state, nextState, count, countThisCycle);
               makeEuler(S, nextState, count, countThisCycle);
               returnNextState(*state, nextState, count, countThisCycle);
            }
         }
      }
   }
}

void MakeSCG::goToNextState(const int state, const int nextState, vector<int> &count, vector<int> &countThisCycle) {
   setSpecialBranch(state, nextState);
   ++count[nextState];
   ++countThisCycle[nextState];
}

void MakeSCG::returnNextState(const int state, const int nextState, vector<int> &count, vector<int> &countThisCycle) {
   clearBranch(state, nextState);
   --count[nextState];
   --countThisCycle[nextState];
}

// num is the number of states which it has gone to
void MakeSCG::makeEuler(vector<int> &S, const int state, vector<int> &count, vector<int> &countThisCycle) {
   for (int nextState = 0; nextState < numOfStates; ++nextState) {
      if (state == nextState || hasBranch(state, nextState))
         continue;
      if (find(S.begin(), S.end(), nextState) != S.end()) {
         if (hasGoneAllStates(count)) { // the SCG is created.
            setSpecialBranch(state, nextState);
            makeSCGFromMatrix();
            clearBranch(state, nextState);
         } else {
            vector<int> tmp = S;
            const vector<int> path = pathThisCycle(countThisCycle);
            copy(path.begin(), path.end(), back_inserter(tmp));
            goToNextState(state, nextState, count, countThisCycle);
            makeEulerStart(tmp, count);
            returnNextState(state, nextState, count, countThisCycle);
         }
      } else {
         if (checkSpace(nextState)) {
            goToNextState(state, nextState, count, countThisCycle);
            makeEuler(S, nextState, count, countThisCycle);
            returnNextState(state, nextState, count, countThisCycle);
         }
      }
   }
}
         
int MakeSCG::getNumberOfBranches(const int state) const {
   int res = 0;
   for (int nextState = 0; nextState < numOfStates; ++nextState) {
      if (hasBranch(state, nextState))
         ++res;
   }
   return res;
}

bool MakeSCG::nextMatrixFromMatrix() {
   for (int state = 0; state < numOfStates; ++state) {
      while (specialIncrement(state)) {
         if (notLimitBranch(state))
            return true;
      }
   }
   return false;
}

bool MakeSCG::notLimitBranch(const int state) const {
   return getNumberOfBranches(state) <= limitOfBranches;
}

// do increment, but special branches leave something alone
bool MakeSCG::specialIncrement(const int state) {
   for (int nextState = 0; nextState < numOfStates; ++nextState) {
      if (matrix[state][nextState] == SPECIAL)
         continue;
      else if (matrix[state][nextState])
         matrix[state][nextState] = 0;
      else {
         matrix[state][nextState] = BRANCH;
         return true;
      }
   }
   return false;
}

/*
 * The present matrix is scg, which branch is special branch
 * So, this function make scgs by attaching useless branches
 */
void MakeSCG::makeSCGFromMatrix() {
   scgs.push_back(matrix);
   while (nextMatrixFromMatrix()) {
      scgs.push_back(matrix);
   }
}

/* 
 * check the program has gone to all states
 */
bool MakeSCG::hasGoneAllStates(const vector<int> &count) const {
   int tmp = 0;
   for (int state = 0; state < numOfStates; ++state) {
      if (count[state])
         ++tmp;
   }
   return tmp == numOfStates;
}

/*
 * return the set of states, which go to at this cycle.
 */
vector<int> MakeSCG::pathThisCycle(const vector<int> &countThisCycle) const{
   vector<int> res;
   for (int state = 0; state < numOfStates; ++state) {
      if (countThisCycle[state])
         res.push_back(state);
   }
   return res;
}

bool MakeSCG::checkSpace(const int state) const {
   return getNumberOfBranches(state) < limitOfBranches;
}

void MakeSCG::setSpecialBranch(const int state, const int nextState) {
   matrix[state][nextState] = SPECIAL;
}

void MakeSCG::clearBranch(const int state, const int nextState) {
   matrix[state][nextState] = 0;
}

bool MakeSCG::hasBranch(const int state, const int nextState) const {
   return matrix[state][nextState];
}
