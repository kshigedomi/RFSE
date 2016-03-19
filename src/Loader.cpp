#include "Loader.h"

/*
 * Load automaton. 
 * variables:
 *  automaton: original automaton
 *  filename: input file name
 *  search: string before automaton
 */

Loader::Loader(const Automaton &m, const string &filename, const string search) {
   automaton = m;
   fin.open(filename.c_str());
   numOfStates = automaton.getNumberOfStates();
   numOfSignals = automaton.getNumberOfSignals();
   numOfActions = automaton.getNumberOfActions();
   actionChoices.resize(numOfStates);
   searchStr = search;
}

Loader::~Loader() {
   fin.close();
}

vector<Automaton> Loader::loadAllAutomatons() {
   vector<Automaton> res;
   while (setNextAutomatonFromRawString())
      res.push_back(getAutomaton());
   return res;
}

void Loader::reset() {
   fin.seekg(0, ios_base::beg);
}

vector<Automaton>::size_type Loader::countAllAutomatons() {
   vector<Automaton>::size_type i = 0;
   reset();
   string buf;
   while (!fin.eof()) {
      getline(fin, buf);
      if (buf.find(searchStr, 0) != string::npos) {
         ++i;
      }
   }
   return i;
}

bool Loader::setNextAutomatonAndVariables(map<string, PreciseNumber> &values) {
   string buf;
   while (!fin.eof()) {
      getline(fin, buf);
      if (buf.find(searchStr, 0) != string::npos) {
         // variables
		/*
         vector<string> assign = splitAtSpace(fin);
         for (vector<string>::size_type i = 0; i < assign.size(); ++i) {
            boost::regex expr("(.+)=(.+)");
            boost::smatch what;
            if (boost::regex_match(assign[i], what, expr)) {
               values[what[1]] = numberToRational(what[2]);
            }
         }
		*/
         // state
         vector<string> stateNames = splitAtSpace(fin);
         for (int state = 0; state < numOfStates; ++state)
            automaton.setNameOfState(state, stateNames[state + 2]);
         // action
         vector<string> actionNames = splitAtSpace(fin);
         for (int action = 0; action < numOfActions; ++action)
            automaton.setNameOfAction(action, actionNames[action + 2]);
         // signal
         vector<string> signalNames = splitAtSpace(fin);
         for (int signal = 0; signal < numOfSignals; ++signal)
            automaton.setNameOfSignal(signal, signalNames[signal + 2]);
         // actionChoice
         for (int count = 0; count < numOfStates; ++count) {
            vector<string> actionChoice = splitAtSpace(fin);
            const int state = automaton.getIndexOfStates(actionChoice[0]);
            const int action = automaton.getIndexOfActions(actionChoice[1]);
            automaton.setActionChoice(state, action);
         }
         // transition
         for (int countState = 0; countState < numOfStates; ++countState) {
            for (int countSignal = 0; countSignal < numOfSignals; ++countSignal) {
               vector<string> transition = splitAtSpace(fin);
               const int state = automaton.getIndexOfStates(transition[0]);
               const int signal = automaton.getIndexOfSignals(transition[1]);
               const int nextState = automaton.getIndexOfStates(transition[2]);
               automaton.setStateTransition(state, signal, nextState);
            }
         }
         return true;
      }
   }
   return false;
}            

// For check regular automatons
bool Loader::setNextAutomatonFromRawString() {
   string buf;
   while (!fin.eof()) {
      getline(fin, buf);
      if (buf.find(searchStr, 0) != string::npos) {
         // actionChoice
         getline(fin, buf);
         if (buf.find("ActionChoice", 0) == string::npos) {
            return false;
         }
         for (int count = 0; count < numOfStates; ++count) {
            vector<string> actionChoice = splitAtSpace(fin);
            const int state = toInteger(actionChoice[0]);
            const int action = toInteger(actionChoice[1]);
            automaton.setActionChoice(state, action);
         }
         // transition
         getline(fin, buf);
         if (buf.find("StateTransition", 0) == string::npos){
            return false;
		 }         
         for (int countState = 0; countState < numOfStates; ++countState) {
            for (int countSignal = 0; countSignal < numOfSignals; ++countSignal) {
               vector<string> transition = splitAtSpace(fin);
               const int state = toInteger(transition[0]);
               const int signal = toInteger(transition[1]);
               const int nextState = toInteger(transition[2]);
               automaton.setStateTransition(state, signal, nextState);
            }
         }
         return true;
      }
   }
   return false;
}            

Automaton Loader::getAutomaton() const {
   return automaton;
}

/*
 * 機能: 指定された数読み込むオートマトンを読み飛ばす
 * 入力: スキップする数 num
 */
bool Loader::skipAutomaton(int num) {
   string buf;
   while (!fin.eof() && num != 0) {
      getline(fin, buf);
      if (buf.find(searchStr, 0) != string::npos)
         --num;
   }
   return num==0;
}
   
   
