#include "MakeAutomaton.h"

int main(int argc, char *argv[]) {
   if (argc != 6) {
      cout << "USAGE : MakeAutomaton datafiledir prefixdir numStates numActions numSignals" << endl;
      exit(1);
   }
   const int numStates = MyUtil::toInteger(argv[3]);
   const int numActions = MyUtil::toInteger(argv[4]);
   const int numSignals = MyUtil::toInteger(argv[5]);
   MakeAutomaton maker(argv[1], numStates, numActions, numSignals);
   ofstream fout(argv[2]);
   maker.put(fout);
   // while (maker.nextAutomaton()) {
   //    Automaton m = maker.getAutomaton();
   //    fout << "REGULAR AUTOMATON" << endl;
   //    fout << m.toRawString() << endl;
   // }
   fout.close();
}


MakeAutomaton::MakeAutomaton(const char filename[], const int noSt, const int noAc, const int noSg):
   automaton(noSt, noAc, noSg)
{
   fin.open(filename);
   numStates = noSt;
   numActions = noAc;
   numSignals = noSg;
   matrix.resize(numStates, vector<int>(numStates));
   actionChoices.resize(numStates, 0);
   signalToTransitions.resize(numStates, vector<int>(numSignals, 0));
   head = -1;
}

MakeAutomaton::~MakeAutomaton() {
   fin.close();
}
// 次のグラフを読み出す
bool MakeAutomaton::nextMatrix() {
   string buf;
   while (!fin.eof()) {
      getline(fin, buf);
      if (buf.find("REGULAR GRAPH", 0) != string::npos) {
         for (int state = 0; state < numStates; ++state) {
            const vector<string> matrixStr = MyUtil::splitAtSpace(fin);
            for (int nextState = 0; nextState < numStates; ++nextState) {
               matrix[state][nextState] = MyUtil::toInteger(matrixStr[nextState]);
            }
         }
         return true;
      }
   }
   return false;
}
/*
 * テスト用
 */
void MakeAutomaton::viewAll() const{
   cout << "Matrix" << endl;
   for (vector<vector<int> >::size_type i = 0; i < matrix.size(); ++i) {
      for (vector<int>::size_type j = 0; j < matrix[i].size(); ++j)
         cout << matrix[i][j] << endl;
      cout << endl;
   }
   cout << endl;
   cout << "actionChoices" << endl;
   for (int state = 0; state < numStates; ++state)
      cout << actionChoices[state];
   cout << endl;
   cout << "signalToTransitions" << endl;
   for (vector<vector<int> >::size_type i = 0; i < matrix.size(); ++i) {
      for (vector<int>::size_type j = 0; j < matrix[i].size(); ++j)
         cout << signalToTransitions[i][j] << endl;
      cout << endl;
   }
   cout << endl;
}

void MakeAutomaton::make() {
  while (nextMatrix()) { // 次のグラフを読み出す
      do {
		if (checkActionChoices() && checkSignalToTransitions()) { // 行動とシグナルの確認
		  setAutomaton(); // セット
		  if (!automaton.hasUselessState()) // 無駄な状態がないか確認
			automatons.push_back(automaton); // 保存
         }
      } while (nextActionChoices() || nextTransitions());//インクリメント: 行動がインクリメントできなくなったら，シグナルをインクリメント
   }
   return;
}

void MakeAutomaton::put(ofstream& fout) {
   while (nextMatrix()) {
      do {
         if (checkActionChoices() && checkSignalToTransitions()) {
            setAutomaton();
            if (!automaton.hasUselessState()) {
               fout << "REGULAR AUTOMATON" << endl;
               fout << automaton.toRawString() << endl;
            }
         }
      } while (nextActionChoices() || nextTransitions());//インクリメント: 行動がインクリメントできなくなったら，シグナルをインクリメント
   }
   return;
}   

bool MakeAutomaton::nextAutomaton() {
   ++head;
   if(head == automatons.size()) // 型違い warning は仕方ない
      return false;
   return true;
}

Automaton MakeAutomaton::getAutomaton(){
   return automatons[head];
}

void MakeAutomaton::setActionChoices() {
   for (int state = 0; state < numStates; ++state)
      automaton.setActionChoice(state, actionChoices[state]);
}

vector<int> MakeAutomaton::getNextStates(const int state) const {
   vector<int> res;
   for (int nextState = 0; nextState < numStates; ++nextState) {
      if (matrix[state][nextState])
         res.push_back(nextState);
   }
   return res;
}

void MakeAutomaton::setTransitions() {
   for (int state = 0; state < numStates; ++state) {
      vector<int> nextStates = getNextStates(state);
      for (int signal = 0; signal < numSignals; ++signal) {
         const int branch = signalToTransitions[state][signal];
         const int nextState = nextStates[branch];
         automaton.setStateTransition(state, signal, nextState);
      }
   }
}


void MakeAutomaton::setAutomaton() {
   setActionChoices();
   setTransitions();
}

bool MakeAutomaton::nextActionChoices() {
   return MyUtil::increment(actionChoices, numActions);
}

// check not doing same action at all states
bool MakeAutomaton::checkActionChoices() const{
   const int tmp = count(actionChoices.begin(), actionChoices.end(), actionChoices[0]);
   return tmp != numStates;
}

bool MakeAutomaton::nextTransitions() {
   for (int state = 0; state < numStates; ++state) {
      if (MyUtil::increment(signalToTransitions[state], getNumberOfBranches(state)))
         return true;
   }
   return false;
}

int MakeAutomaton::getNumberOfBranches(const int state) const{
   int res = 0;
   for (int nextState = 0; nextState < numStates; ++nextState) {
      if (matrix[state][nextState])
         ++res;
   }
   return res;
}
   

// check able to attach at least one signal to all branches
bool MakeAutomaton::checkSignalToTransitions() const{
   for (int state = 0; state < numStates; ++state) {
      for (int branch = 0; branch < getNumberOfBranches(state); ++branch) {
         if (find(signalToTransitions[state].begin(), signalToTransitions[state].end(), branch) == signalToTransitions[state].end())
            return false;
      }
   }
   return true;
}

string MakeAutomaton::toString() const {
   string res = "** MakeAutomaton **\n";

   res += "* Matrix *\n";
   for (int i = 0; i < numStates; ++i) {
      for (int j = 0; j < numStates; ++j)
         res += MyUtil::toString(matrix[i][j]);
      res += "\n";
   }

   res += "* ActionChoice *\n";
   for (int i = 0; i < numStates; ++i)
      res += MyUtil::toString(actionChoices[i]);
   res +="\n";

   res += "* SignalToTransitions *\n";
   for (int i = 0; i < numStates; ++i) {
      for (int j = 0; j < numSignals; ++j)
         res += MyUtil::toString(signalToTransitions[i][j]);
      res += "\n";
   }

   return res;
}
      
