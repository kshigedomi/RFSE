#include "Graph.h"

/*
 * This class has matrix, and do isomorphism algorithm
 */
Graph::Graph(const vector<vector<int> > &m) {
   matrix = m;
   numOfStates = matrix.size();
   degreeStates.resize(numOfStates+1, vector<vector<int> >(numOfStates+1));
   numOfAllBranches = 0;
   inDegrees.resize(numOfStates, 0);
   outDegrees.resize(numOfStates, 0);
   setDegrees();
   setSwitchStates();
}

/*
 * set degrees of all states, set number of all branches,
 * and, classify all states based on these indegree and outdegree
 */
void Graph::setDegrees() {
   for (int state = 0; state < numOfStates; ++state) {
      // in
      int inDegree = 0;
      for (int preState = 0; preState < numOfStates; ++preState) {
         if (hasBranch(preState, state)) {// branch exist
            ++inDegree;
         }
      }
      // out
      int outDegree = 0;
      for (int nextState = 0; nextState < numOfStates; ++nextState) {
         if (hasBranch(state, nextState)) {//branch exist
            ++outDegree;
         }
      }
      degreeStates[inDegree][outDegree].push_back(state);
      inDegrees[state] = inDegree;
      outDegrees[state] = outDegree;
      numOfAllBranches += outDegree;
   }
}

vector<int> Graph::getDegreeStates(const int inDegree, const int outDegree) const {
   return degreeStates[inDegree][outDegree];
}

int Graph::getNumberOfAllBranches() const {
   return numOfAllBranches;
}

int Graph::getNumberOfStates() const {
   return numOfStates;
}

bool Graph::hasBranch(const int state, const int nextState) const {
   return matrix[state][nextState];
}

pair<int, int> Graph::degree(const int state) const {
   return make_pair(inDegrees[state], outDegrees[state]);
}

vector<int> Graph::getSwitchStates() const {
   return switchStates;
}

/*
 * switch state has all states and, they are ordering based on their degree
 */
void Graph::setSwitchStates() {
   for (int inDegree = 0; inDegree < numOfStates + 1; ++inDegree) {
      for (int outDegree = 0; outDegree < numOfStates + 1; ++outDegree) {
         const vector<int> degreeStates = getDegreeStates(inDegree, outDegree);
         copy(degreeStates.begin(), degreeStates.end(), back_inserter(switchStates));
      }
   }
}

/*
 * isomorphism algorithm
 */
bool equalGraph(const Graph &left, const Graph &right) {
   vector<int> tmp = left.getSwitchStates();
   return permTest(0, left, right, tmp, right.getSwitchStates());
}

/*
 * all i < k leftSwitchStates[i] is corresponded to rightSwitchStates[i], and
 * now change so as leftSwitchState[k] is corresponded to rightSwitchStates[k]
 */
bool permTest(int k, const Graph &left, const Graph &right, vector<int> &leftSwitchStates, const vector<int> &rightSwitchStates) {
   const int numOfStates = left.getNumberOfStates();
   if (k == numOfStates)
      return true;
   for (int i = k; i < numOfStates; ++i) {
      const int leftTargetState = leftSwitchStates[i];
      const int rightTargetState = rightSwitchStates[k];
      if (left.degree(leftTargetState) != right.degree(rightTargetState))
         break; // switch state is ordering the degree. So, if leftdegree != rightdegree once, all right states do not correspond to left state
      swap(leftSwitchStates[i], leftSwitchStates[k]);
      int j = 0;
      for (; j <= k; ++j) {
         const int leftState = leftSwitchStates[j];
         const int rightState = rightSwitchStates[j];
         if (left.hasBranch(leftState, leftTargetState) != right.hasBranch(rightState, rightTargetState)) break;
         if (left.hasBranch(leftTargetState, leftState) != right.hasBranch(rightTargetState, rightState)) break;
      }
      if (j > k) {// leftSwitchStates[k] is corresponding to rightSwitchStates[k]
         if (permTest(k+1, left, right, leftSwitchStates, rightSwitchStates))
            return true;
      }
      // return
      swap(leftSwitchStates[i], leftSwitchStates[k]);
   }
   return false;
}

string Graph::toString() const {
   string res;
   for (int i = 0; i < numOfStates; ++i) {
      for (int j = 0; j < numOfStates; ++j) {
         res += ::toString(matrix[i][j]) + " ";
      }
      res += "\n";
   }
   return res;
}
