#ifndef AUTOMATON_H_
#define AUTOMATON_H_

/*
 * Automaton.h
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

class Automaton {
	int numStates;
	int numActions;
	int numSignals;
	mutable vector<string> nameStates;
	vector<string> nameActions;
	vector<string> nameSignals;

	vector<int> actionChoice;
	vector< vector<int> > stateTransition;
    vector<int> numOfActionStates;
    void visit(int v, vector< vector<int> >& scc, stack<int> &S, vector<bool> &inS, vector<int> &low, vector<int> &num, int& time);

public:
	Automaton();
    Automaton(const int nOSt, const int nOAc, const int nOSg);
	virtual ~Automaton();

    void addOneState();
    bool checkRegularity();

    void setWorstStrategy();

    void setNumberOfStates(const int nOSt);
    void setNumberOfActions(const int nOAc);
    void setNumberOfSignals(const int nOSg);

    bool setActionChoice(const int state, const int action);
	bool setStateTransition(const int state, const int signal, const int nextState);
	int getActionChoice(int state) const;
	int getStateTransition(int state, int signal) const;
	vector<int> getStateTransition(int state) const;

	int getNumberOfStates() const;
	int getNumberOfActions() const;
	int getNumberOfSignals() const;

	int getIndexOfStates(string state) const;
	int getIndexOfActions(string action) const;
	int getIndexOfSignals(string signal) const;

	string getNameOfStates(int index) const;
	bool setNameOfStates(const vector<string> &names);
    bool setNameOfState(const int state, const string &name) const;
	string getNameOfActions(int index) const;
	bool setNameOfActions(const vector<string> &names);
    bool setNameOfAction(const int state, const string &name);
	string getNameOfSignals(int index) const;
	bool setNameOfSignals(vector<string> &names);
    bool setNameOfSignal(const int state, const string &name);

	vector<string> getNamesOfStates() const;
	vector<string> getNamesOfActions() const;
	vector<string> getNamesOfSignals() const;

	vector<int> getActionChoices() const;
	vector< vector<int> > getAllStateTransition() const;

    Automaton deleteTransientFSA();

    void setExactNames() const;
    string toString() const;
    string toRawString() const;
    void setNumberOfActionStates();
    vector<int> getNumberOfActionStates() const;
    bool hasUselessState() const;
};

bool operator == (const Automaton& lhs, const Automaton& rhs);
bool equalAutomatons(const Automaton &left, const Automaton& right);
ostream& operator<<(ostream &os, const Automaton &m);

#endif /* AUTOMATON_H_ */
