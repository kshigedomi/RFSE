#ifndef LOADER_H_
#define LOADER_H_

#ifndef COMMOM_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

class Loader {
 private:
    Automaton automaton;
    ifstream fin;
    int numOfStates;
    int numOfActions;
    int numOfSignals;
    string searchStr;

    vector<int> actionChoices;

 public:
    Loader(const Automaton &m,const string &filename, const string search="REGULAR AUTOMATON");
    ~Loader();
    
    bool loadGraph();

    vector<Automaton> loadAllAutomatons();
    
    vector<Automaton>::size_type countAllAutomatons();

    void reset();

    bool setNextAutomaton();
    bool setNextAutomatonAndVariables(map<string, PreciseNumber> &values);
    bool setNextAutomatonFromRawString();
    vector<string> getExactNames() const;
    Automaton getAutomaton() const;
    bool skipAutomaton(int num);
};
#endif
