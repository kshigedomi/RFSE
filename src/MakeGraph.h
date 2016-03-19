#ifndef MAKEGRAPH_H_
#define MAKEGRAPH_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef GRAPH_H_
#include "Graph.h"
#endif

class MakeGraph {
 private:
    static const int HEAD_FIRST = -1;
    int numStates;
    int numSignals;
    int endTurn; // for check Regular
    vector<vector<Graph> > graphs;
    int head;
    int index;
    
    void setGraph(const Graph &graph);
    bool isNewGraph(const Graph &graph) const;
 public:
    MakeGraph(const int nOSt, const int limitOfBranch, const int end);
    Graph getGraph() const;
    bool setNextGraph();
    void make();
};
#endif
