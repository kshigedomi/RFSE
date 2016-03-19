#ifndef PLAYER_H_
#define PLAYER_H_

/*
 * Player.h
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

class Player {
private:
	string namePlayer;
	Automaton strategy;
	Belief belief;
public:
	Player();
	Player(const string &name, const Automaton &st);
	virtual ~Player();
	void configure(const string &name, const Automaton &st);
	void setAutomaton(const Automaton &st);
	int getActionChoice(int state) const;
	int getNumberOfStates() const;
	int getNumberOfActions() const;
	int getNumberOfSignals() const;
	void setNameOfPlayer(const string &name);
	string getNameOfPlayer() const;
	string getNameOfStates(int index) const;
	string getNameOfActions(int index) const;
	string getNameOfSignals(int index) const;
	int getIndexOfStates(string state) const;
	int getIndexOfActions(string action) const;
	int getIndexOfSignals(string signal) const;

	int getStateTransition(int state, int signal) const;
	Automaton getAutomaton() const;

	void view() const;


};

#endif /* PLAYER_H_ */
