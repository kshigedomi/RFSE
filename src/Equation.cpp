/*
 * Equation.cpp
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 *  Commented by Shun July 2013
 */

/*
 * This class solve equation to require v^\theta. It represents reward when the player is in state \theta.
 * It uses LU decompression.
 * It has coefficient of equation, and result
 * Hint: Original equation is x = dTx + R, where x is a vector, d is a discount rate, T is a transition matrix, and R is a payoff vector.
 */

#include "Equation.h"

Equation::Equation() {
	dimension = 0;
	success = false;
}

Equation::Equation(int dimension) {
	configure(dimension);
}

Equation::Equation(int dimension, const vector< vector<PreciseNumber> > &eq) {
	configure(dimension, eq);
}

Equation::~Equation() {

}

void Equation::configure(int dimension) {
	this->dimension = dimension;
	equation.resize(dimension);
	for (int i = 0; i < dimension; i++) {
		equation[i].resize(dimension + 1);
	}
	result.resize(dimension);
	success = true;
}

void Equation::configure(int dimension, const vector< vector<PreciseNumber> > &eq) {
	this->dimension = dimension;
	this->equation = eq;
	result.resize(dimension);
	success = true;
}

int Equation::getDimension() const {
	return dimension;
}

/*
 * Name: solve
 * Function: solve equation, and require v^x. It is reward associated input automaton and joint state x.
 */
bool Equation::solve() {
	vector<PreciseNumber> y(dimension);

	// LU decomposition O(n^3)
	for (int i = 0; i < dimension - 1; i++) {
		if (equation[i][i] == 0) {
			return false;
		}
		for (int j = i + 1 ; j < dimension; j++) {
			if (equation[i][i] == 0) {
				return false;
			}
			equation[j][i] /= equation[i][i];
			for (int k = i + 1; k < dimension; k++) {
				equation[j][k] -= equation[j][i] * equation[i][k];
			}
		}
	}

	for (int i = 0; i < dimension; i++) {
       y[i] = equation[i][dimension];
       for (int j = 0; j < i; j++) {
          y[i] -= equation[i][j] * y[j];
       }
	}

	for (int i = dimension - 1; i >= 0; i--) {
		result[i] = y[i];
		for (int j = dimension - 1; j >= i + 1; j--) {
			result[i] -= equation[i][j] * result[j];
		}
		if (equation[i][i] == 0) {
			return false;
		}
		result[i] /= equation[i][i];
	}

	return true;
}

vector<PreciseNumber> Equation::getResult() const{
	return result;
}

PreciseNumber Equation::getResult(int index) const{
	return result[index];
}
