#ifndef MAKESCC_H_
#define MAKESCC_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

class MakeSCG {
 private:
    static const int SPECIAL = 2;
    static const int BRANCH = 1;
    
    int numOfStates;
    int limitOfBranches;
    vector<vector<int> > matrix;
    vector<vector<vector<int> > > scgs;
    int head;

    bool hasBranch(const int state, const int nextState) const;

    int getNumberOfBranches(const int state) const;

    void setSpecialBranch(const int state, const int nextState);
    void clearBranch(const int state, const int nextState);
    bool checkSpace(const int state) const;
    bool notLimitBranch(const int state) const;
    bool hasGoneAllStates(const vector<int> &count) const;
    vector<int> pathThisCycle(const vector<int> &countThisCycle) const;

    void goToNextState(const int state, const int nextState, vector<int> &count, vector<int> &countThisCycle);
    void returnNextState(const int state, const int nextState, vector<int> &count, vector<int> &countThisCycle);


    bool nextMatrixFromMatrix();
    bool specialIncrement(const int state);

    void make();
    void makeSCGFromMatrix();
    void makeEulerStart(vector<int> &S, vector<int> &count);
    void makeEuler(vector<int> &S, const int state, vector<int> &count, vector<int> &countThisCycle);

 public:
    MakeSCG(const int nOSt, const int numOfSignals);
    
    bool nextSCG();
    vector<vector<int> > getSCG() const;
};

#endif
