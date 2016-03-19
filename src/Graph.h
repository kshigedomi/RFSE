#ifndef GRAPH_H_
#define GRAPH_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef MAKESCG_H_
#include "MakeSCG.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

class Graph {
 private:
    int numOfStates;
    vector<vector<int> > matrix;
    vector<vector<vector<int> > > degreeStates;
    vector<int> inDegrees;
    vector<int> outDegrees;
    vector<int> switchStates;
    vector<int> numOfBranches;
    int numOfAllBranches;

    void setSwitchStates();
    void setDegrees();
    bool changeMatrix();
    void setMatrixInfo();
    bool notLimitBranch() const;
    bool notLimitBranch(const int state) const;
    vector<int> getDegreeStates(const int inDegree, const int outDegree) const;
 public:
    Graph(const vector<vector<int> > &m);
    
    int getNumberOfStates() const;
    bool hasBranch(const int state, const int nextState) const;
    pair<int, int> degree(const int state) const;

    vector<int> getSwitchStates() const;
    int getNumberOfAllBranches() const;

    bool setNextMatrix();

    bool checkRegularity() const;

    string toString() const;
    
    const static bool modeDebug = true;
	template<typename T> static void debugTitle(T msg) { if (modeDebug) { cout << "************ " << msg << " **************" << endl; } }
	template<typename T> static void debug(T msg) { if (modeDebug) { cout << msg << endl; } }
	template<typename T> static void displayTitle(T msg) { cout << "************ " << msg << " **************" << "\n"; }
	template<typename T> static void display(T msg) { cout << msg << endl; }
	template<typename T> static void error(T msg) { cerr << msg << endl; }
};

bool equalGraph(const Graph &left, const Graph &right);
bool permTest(int k, const Graph &left, const Graph &right, vector<int> &leftSwitchStates, const vector<int> &rightSwitchStates);

#endif
