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
	vector<string> nameStates;
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

    void setNumberOfStates(const int nOSt);
    void setNumberOfActions(const int nOAc);
    void setNumberOfSignals(const int nOSg);

    void setActionChoice(const int state, const int action);
	void setStateTransition(const int state, const int signal, const int nextState);
	int getActionChoice(const int state) const;
	int getStateTransition(const int state, const int signal) const;
	vector<int> getStateTransition(const int state) const;

	int getNumberOfStates() const;
	int getNumberOfActions() const;
	int getNumberOfSignals() const;

	int getIndexOfStates(const string& state) const;
	int getIndexOfActions(const string& action) const;
	int getIndexOfSignals(const string& signal) const;

    vector<string> getNameOfStates() const;
	string getNameOfStates(const int index) const;
	void setNameOfStates(const vector<string> &names);
    void setNameOfState(const int state, const string &name);
    vector<string> getNameOfActions() const;
	string getNameOfActions(const int index) const;
	void setNameOfActions(const vector<string> &names);
    void setNameOfAction(const int state, const string &name);
    vector<string> getNameOfSignals() const;
	string getNameOfSignals(const int index) const;
	void setNameOfSignals(const vector<string> &names);
    void setNameOfSignal(const int state, const string &name);

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
