#ifndef MAKEAUTOMATON_H_
#define MAKEAUTOMATON_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

class MissLoadGraph{};

class MakeAutomaton {
 private:
    Automaton automaton;
    vector<Automaton> automatons;
    int numStates;
    int numActions;
    int numSignals;
    vector<vector<int> > matrix;
    vector<int> actionChoices;
    vector<vector<int> > signalToTransitions;
    int head;
    ifstream fin;

    void setAutomaton();
    void setActionChoices();
    void setTransitions();

    bool nextMatrix();
    bool nextActionChoices();
    bool nextTransitions();

    int getNumberOfBranches(const int state) const;

    bool checkActionChoices() const;
    bool checkSignalToTransitions() const;

    vector<int> getNextStates(int state) const;

 public:
    MakeAutomaton(const char filename[], const int noSt, const int noAc, const int noSg);
    ~MakeAutomaton();

    void make();
    void put(ofstream &);

    Automaton getAutomaton();

    bool nextAutomaton();

    void viewAll() const;

    string toString() const;
};

#endif
