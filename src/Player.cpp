/*
 * Player.cpp
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

/*
 * This class holds a player's situation or information.
 * It has the number of player.
 * It also has his automaton and belief.
 * It has methods to get names of states, actions, or signals by inputting index. 
 * Conversly, it also has methods to get index of states, actions, or signals by inputting string.
 */

#include "Player.h"

Player::Player() {
}

Player::Player(const string &name, const Automaton &st) {
	configure(name, st);
}

Player::~Player() {

}

void Player::configure(const string &name, const Automaton &st) {
	setNameOfPlayer(name);
	setAutomaton(st);
}

void Player::setAutomaton(const Automaton &st) {
	strategy = st;
}

Automaton Player::getAutomaton() const{
	return strategy;
}

string Player::getNameOfPlayer() const{
	return namePlayer;
}

void Player::setNameOfPlayer(const string &name) {
	namePlayer = name;
}

string Player::getNameOfStates(int index) const{
	return strategy.getNameOfStates(index);
}

string Player::getNameOfActions(int index) const{
	return strategy.getNameOfActions(index);
}

string Player::getNameOfSignals(int index) const{
	return strategy.getNameOfSignals(index);
}

int Player::getNumberOfStates() const{
	return strategy.getNumberOfStates();
}

int Player::getNumberOfActions() const{
	return strategy.getNumberOfActions();
}

int Player::getNumberOfSignals() const{
	return strategy.getNumberOfSignals();
}

int Player::getActionChoice(int state) const{
	return strategy.getActionChoice(state);
}

int Player::getStateTransition(int state, int signal) const{
	return strategy.getStateTransition(state, signal);
}

int Player::getIndexOfStates(string state) const{
	return strategy.getIndexOfStates(state);
}

int Player::getIndexOfActions(string action) const{
	return strategy.getIndexOfActions(action);
}

int Player::getIndexOfSignals(string signal) const{
	return strategy.getIndexOfSignals(signal);
}


void Player::view() const{
	displayTitle(namePlayer);
	strategy.view();
}
