#include "Gene.h"

/*
 * first generation
 */
Gene::Gene(const Automaton &m) {
   automaton = m;
   setScore(INF);
   const int numStates = automaton.getNumberOfStates();
   const int numActions = automaton.getNumberOfActions();
   const int numSignals = automaton.getNumberOfSignals();
   do {
      for (int state=0; state<numStates; state++) {
         automaton.setActionChoice(state,rand() % numActions);
      }
      for (int state=0; state <numStates; state++) {
         for (int signal=0; signal<numSignals; signal++) {
            automaton.setStateTransition(state,signal,rand() % numStates);
         }
      }
   } while (!doesActionsMeaningful() || !automaton.checkRegularity() || automaton.hasUselessState());
   setExactName();
   doesSet = false;
}

/*
 * after second generation 
 */
Gene::Gene(const Gene& mother,const Gene& father,const int rateOfMutation) {
   setScore(INF);
   do {
      cross(mother.getAutomaton(),father.getAutomaton());
      mutation(rateOfMutation);
   } while (!doesActionsMeaningful() || !automaton.checkRegularity() || automaton.hasUselessState());
   doesSet = false;
   setExactName();
}

void Gene::cross(const Automaton& mother,const Automaton& father) {
   const int numStates = automaton.getNumberOfStates();
   const int numSignals = automaton.getNumberOfSignals();
   automaton = mother;
   int cut = rand() % (numStates-1) + 1;
   for (int state=cut; state < numStates; state++) {
      automaton.setActionChoice(state,father.getActionChoice(state));
   }
   for (int state=cut; state < numStates; state++) {
      for (int signal=0; signal < numSignals; signal++) {
         automaton.setStateTransition(state,signal,father.getStateTransition(state,signal));
      }
   }
}
void Gene::mutation(const int rateOfMutation) {
   const int numStates = automaton.getNumberOfStates();
   const int numActions = automaton.getNumberOfActions();
   const int numSignals = automaton.getNumberOfSignals();
   for (int state=0; state<numStates; state++) {
      if (rand() % rateOfMutation==0) {
         automaton.setActionChoice(state,rand() % numActions);
      }
   }
   for (int state=0; state < numStates; state++) {
      for (int signal=0; signal < numSignals; signal++) {
         if (rand() % rateOfMutation == 0) {
            automaton.setStateTransition(state,signal,rand() % numStates);
         }
      }
   }
}

void Gene::setExactName() {
   const int numStates = automaton.getNumberOfStates();
   vector<int> nameCount(automaton.getNumberOfActions(),1);
   vector<string> names(numStates);
   for (int state = 0; state < numStates; ++state) {
      int action = automaton.getActionChoice(state);
      names[state] = automaton.getNameOfActions(action) + ::toString(nameCount[action]);
      ++nameCount[action];
   }
   automaton.setNameOfStates(names);
}

Automaton Gene::getAutomaton() const {
   return automaton;
}

bool Gene::doesSetScore() const{
   return doesSet;
}

void Gene::setScore(const PreciseNumber &value) {
   doesSet = true;
   score = value;
}

PreciseNumber Gene::getScore() const{
   return score;
}

//すべてのアクションが同じならfalse
bool Gene::doesActionsMeaningful () const {
   const int numStates = automaton.getNumberOfStates();
   for (int i=0; i<numStates; i++) {
      const int action1 = automaton.getActionChoice(i);
      for (int j=0; j<i; j++) {
         const int action2 = automaton.getActionChoice(j);
         if (action1 != action2) {
            return true;
         }
      }
   }
   return false;
}

string Gene::toString() const{
   string s = automaton.toString();
   s += "Score : " + rationalToString(score);
   return s;
}

bool operator < (const Gene & left, const Gene &right) {
   return left.getScore() < right.getScore();
}

bool operator==(const Gene& left, const Gene &right) {
   return left.getAutomaton() == right.getAutomaton();
}
   
ostream& operator<<(ostream &os, const Gene &gene) {
   return os << gene.toString();
}
