#include "MakeGraph.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "USAGE : MakeGraph prefix numStates numSignals" << endl;
        exit(1);
    }
    const int numStates = MyUtil::toInteger(argv[2]);
    const int numSignals = MyUtil::toInteger(argv[3]);
    MakeGraph maker(numStates, numSignals, 10);
    maker.make();
    ofstream fout(argv[1]);
    while (maker.setNextGraph()) {
        Graph graph = maker.getGraph();
        fout << "REGULAR GRAPH" << endl;
        fout << graph.toString() << endl;
    }
}

MakeGraph::MakeGraph(const int nOSt, const int nOSg, const int end){
    numStates = nOSt;
    numSignals = nOSg;
    endTurn = end;
    head = HEAD_FIRST;
    index = 0;
    const int limitOfBranch = min(numStates, numSignals);
    const int limitOfAllBranch = numStates * limitOfBranch;
    graphs.resize(limitOfAllBranch + 1);
}

Graph MakeGraph::getGraph() const{
    return graphs[index][head];
}

bool MakeGraph::setNextGraph() {
    ++head;
    if (graphs[index].size() == head) { // 型違い warning は仕方ない
        head = 0;
        do {
            ++index;
        } while (index < graphs.size() && graphs[index].size() == 0); // 型違い warning は仕方ない
    }
    return index != graphs.size(); // 型違い warning は仕方ない
}

void MakeGraph::make() {
    MakeSCG maker(numStates, numSignals);
    while (maker.nextSCG()) {
        vector<vector<int> > scg = maker.getSCG();
        Graph graph(scg);
        if (isNewGraph(graph) && MyUtil::checkRegularity(scg, endTurn))
            setGraph(graph);
    }
}

void MakeGraph::setGraph(const Graph &graph) {
    const int numAllBranches = graph.getNumberOfAllBranches();
    graphs[numAllBranches].push_back(graph);
}

bool MakeGraph::isNewGraph(const Graph &graph) const {
    const int numAllBranches = graph.getNumberOfAllBranches();
    const bool result = find_if(graphs[numAllBranches].begin(), graphs[numAllBranches].end(), boost::bind1st(equalGraph, graph)) == graphs[numAllBranches].end();
    return result;
}

