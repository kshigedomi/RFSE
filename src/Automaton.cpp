/*
 * Automaton.cpp
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

/* 
 * This class represents automaton and class Player have it's own automaton.
 * It has the number of states, actions, and signals.
 * It also has string of states, actions, and signals.
 * It also has actionchoice. It represent action the player does when he is a certain state.
 * It also has state transitions. It represent state the player goes when he does a certain actions and he recieves a certain signals.
 */

#include "Automaton.h"

Automaton::Automaton() {
	numSignals = 0;
	numActions = 0;
	numStates = 0;
}

Automaton::Automaton(const int nOSt, const int nOAc, const int nOSg) {
   numStates = nOSt;
   numActions = nOAc;
   numSignals = nOSg;
   actionChoice.resize(numStates);
   stateTransition.resize(numStates, vector<int>(numSignals));
}

Automaton::~Automaton() {

}

/*
 * 機能: 状態数，行動数，シグナル数のセット，および resize
 */
void Automaton::setNumberOfStates(const int nOSt) {
    this->numStates = nOSt;
    actionChoice.resize(numStates);
    stateTransition.resize(numStates);
    nameStates.resize(numStates);
}

void Automaton::setNumberOfActions(const int nOAc) {
   numActions = nOAc;
   nameActions.resize(numActions);
}

void Automaton::setNumberOfSignals(const int nOSg) {
   numSignals = nOSg;
   for (int st = 0; st < numStates; ++st)
      stateTransition[st].resize(numSignals);
   nameSignals.resize(numSignals);
}

/*
 * 機能: regular かどうかを判定する
 */
bool Automaton::checkRegularity() {
   vector<vector<int> > tmp(numStates, vector<int>(numStates, false));
   for (int st = 0; st < numStates; ++st) {
      for (int sg = 0; sg < numSignals; ++sg) {
         const int nextState = getStateTransition(st, sg);
         tmp[st][nextState] = true;
      }
   }
   return MyUtil::checkRegularity(tmp, 10);
}
/*
 * 機能: 状態を足す．
 * 注意: 状態数が増えると Players の状態数をセットし直し， setJointFSA を呼ぶ必要がある．
 */
void Automaton::addOneState() {
   setNumberOfStates(numStates + 1);
}

/*
 * 機能: 名前を返す
 */
string Automaton::getNameOfStates(int index) const{
	return nameStates[index];
}
string Automaton::getNameOfActions(int index) const {
	return nameActions[index];
}
string Automaton::getNameOfSignals(int index) const {
	return nameSignals[index];
}
vector<string> Automaton::getNameOfStates() const {
    return nameStates;
}
vector<string> Automaton::getNameOfActions() const {
    return nameActions;
}
vector<string> Automaton::getNameOfSignals() const {
    return nameSignals;
}

/*
 * 機能: 名前をセット
 * メモ: 状態数と異なる場合は例外を送出
 */
void Automaton::setNameOfStates(const vector<string> &names) {
   if (names.size() != this->getNumberOfStates()) { //型違い warning は仕方ない
       cout << "length_error" << endl;
       throw length_error("Automaton::setNameOfStates");
   } else {
       this->nameStates = names;
   }
}

void Automaton::setNameOfActions(const vector<string> &names) {
   if (names.size() != this->getNumberOfActions()) //型違い warning は仕方ない
       throw length_error("Automaton::setNameOfActions");
   else {
      nameActions = names;
   }
}

void Automaton::setNameOfSignals(const vector<string> &names) {
   if (names.size() != this->getNumberOfSignals()) //型違い warning は仕方ない
       throw length_error("Automaton::setNameOfSignals");
   else {
      nameSignals = names;
   }
}

void Automaton::setNameOfState(const int state, const string &name) {
  if (state < getNumberOfStates()) {
	nameStates[state] = name;
  } else {
      throw range_error("Automaton::setNameOfState(" + MyUtil::toString(state) + ", " + name);
  }
}

void Automaton::setNameOfAction(const int action, const string &name) {
  if (action < getNumberOfActions()) {
	nameActions[action] = name;
  } else {
      throw range_error("Automaton::setNameOfAction(" + MyUtil::toString(action) + ", " + name);
  }
}

void Automaton::setNameOfSignal(const int signal, const string &name) {
    if (signal < getNumberOfSignals()) {
        nameSignals[signal] = name;
    } else {
        throw range_error("Automaton::setNameOfSignal(" + MyUtil::toString(signal) + ", " + name);
    }
}

/*
 * 機能: 選択行動をセット
 * 返り値: セットできたら true
 */
void Automaton::setActionChoice(const int state, const int action) {
  if (state < getNumberOfStates() && action < getNumberOfActions()) {
      this->actionChoice[state] = action;
   } else {
      throw range_error("Automaton::setActionChoice(" + MyUtil::toString(state) + ", " + MyUtil::toString(action));
   }
}
/*
 * 機能: 選択行動を取得
 */
int Automaton::getActionChoice(const int state) const {
	return actionChoice[state];
}

/*
 * 機能: 状態遷移をセット
 * 返り値: セットできたら true
 */
void Automaton::setStateTransition(const int state, const int signal, const int nextState) {
  if (state < getNumberOfStates() && signal < getNumberOfSignals() && nextState < getNumberOfStates()) {
      this->stateTransition[state][signal] = nextState;
  } else {
      throw range_error("Automaton::setStateTransition(" + MyUtil::toString(state) + ", " +  MyUtil::toString(signal) + ", " + MyUtil::toString(nextState));
  }
}
/*
 * 機能: 状態遷移を取得
 */
int Automaton::getStateTransition(const int state, const int signal) const {
  return stateTransition[state][signal];
}

vector<int> Automaton::getStateTransition(const int state) const {
	return stateTransition[state];
}

/*
 * 機能: 状態数，行動数，シグナル数を取得
 */
int Automaton::getNumberOfStates() const {
	return numStates;
}
int Automaton::getNumberOfActions() const {
	return numActions;
}
int Automaton::getNumberOfSignals() const {
	return numSignals;
}


/*
 * 機能: 状態数，行動数，シグナル数を取得
 */
int Automaton::getIndexOfStates(const string& state) const {
	const int res = MyUtil::getIndex(nameStates, state);
    if (res < getNumberOfStates())
        return res;
    else
        throw out_of_range("Automaton::getIndexOfStates(" + state + ")");
}

int Automaton::getIndexOfActions(const string& action) const {
	const int res = MyUtil::getIndex(nameActions, action);
    if (res < getNumberOfActions())
        return res;
    else
        throw out_of_range("Automaton::getIndexOfActions(" + action + ")");
}

int Automaton::getIndexOfSignals(const string& signal) const {
	const int res = MyUtil::getIndex(nameSignals, signal);
    if (res < getNumberOfSignals())
        return res;
    else
        throw out_of_range("Automaton::getIndexOfSignals(" + signal + ")");
}

/*
 * 機能: 状態数，行動数，シグナルの名前を取得
 */
vector<string> Automaton::getNamesOfStates() const {
	return nameStates;
}

vector<string> Automaton::getNamesOfActions() const {
	return nameActions;
}

vector<string> Automaton::getNamesOfSignals() const {
	return nameSignals;
}

/*
 * 機能: 選択行動を取得
 */
vector<int> Automaton::getActionChoices() const {
	return actionChoice;
}

/*
 * 機能: 状態遷移を取得
 */
vector< vector<int> > Automaton::getAllStateTransition() const {
	return stateTransition;
}

/*
 * 機能: 行動ごとにその行動をしている状態の数を数える
 */
void Automaton::setNumberOfActionStates() {
   numOfActionStates.resize(numActions, 0);
   for (int state = 0; state < numStates; ++state) {
      const int action = getActionChoice(state);
      ++numOfActionStates[action];
   }
}
vector<int> Automaton::getNumberOfActionStates() const {
   return numOfActionStates;
}

/*
 * 機能: MakeAutomaton のために，名前なしで FSA を出力する
 */
string Automaton::toRawString() const {
   string res;
   // 選択行動
   res += "ActionChoice\n";
   for (int st = 0; st < numStates; ++st)
      res += MyUtil::toString(st) + " " + MyUtil::toString(actionChoice[st]) + "\n";
   // 状態遷移
   res += "StateTransition\n";
   for (int st = 0; st < numStates; ++st) {
      for (int sg = 0; sg < numSignals; ++sg)
         res += MyUtil::toString(st) + " " + MyUtil::toString(sg) + " " + MyUtil::toString(stateTransition[st][sg]) + "\n";
   }
   return res;
}

/*
 * 機能: 同じだったら，true
 * メモ: 同じ意味を持つ状態を探し出して，実質同じオートマトンか見つけている
 *       もしかしたら間違っているかも？
 */
bool operator == (const Automaton& lhs, const Automaton& rhs) {
	if (lhs.getNumberOfStates() != rhs.getNumberOfStates()) return false;
	if (lhs.getNumberOfActions() != rhs.getNumberOfActions()) return false;
	if (lhs.getNumberOfSignals() != rhs.getNumberOfSignals()) return false;
    
    // 行動が同じ状態を探しだし，true を付ける
    const int numStates = lhs.getNumberOfStates();
    const int numSignals = lhs.getNumberOfSignals();
    vector<vector<int> > equalStates(numStates, vector<int>(numStates));
    for (int lst = 0; lst < numStates; ++lst) {
       for (int rst = 0; rst < numStates; ++rst)
          equalStates[lst][rst] = (lhs.getActionChoice(lst) == rhs.getActionChoice(rst));
    }
    // 遷移先で違う行動をする状態を見つけ出し，false を付ける
    bool check = true;
    while (check) {
       check = false; // 変わるところがなかったら false のままになる
       for (int lst = 0; lst < numStates; ++lst) {
          for (int rst = 0; rst < numStates; ++rst) {
             // もう既に違うなら skip
             if (!equalStates[lst][rst])
                continue;
             // 遷移先が同じか調べる
             for (int sg = 0; sg < numSignals; ++sg) {
                const int lNextState = lhs.getStateTransition(lst, sg);
                const int rNextState = rhs.getStateTransition(rst, sg);
                if (!equalStates[lNextState][rNextState]) { // 遷移先が違う
                   equalStates[lst][rst] = false;
                   check = true;
                   break;
                }
             }
          }
       }
    }
    // 意味が同じ状態が同じだけ，両方の FSA にあるか調べる
    for (int lst = 0; lst < numStates; ++lst) {
       bool check = false;
       for (int rst = 0; rst < numStates; ++rst) {
          if (equalStates[lst][rst]) {
             check = true;
             for (int i = 0; i < numStates; ++i) // 対応する状態は一つだけ
                equalStates[i][rst] = false;
             break;
          }
       }
       if (!check) { // 同じ状態がなかった
          return false;
       }
    }
    return true;
}

/*
 * 機能: 無駄な状態がないか調べる
 * メモ: 無駄な状態とは，意味が同じ状態が他にあるときを指す
 *       探し方は operator== とほぼ同じ
 *       もしかしたら間違っているかも？
 */
bool Automaton::hasUselessState() const{
   // 行動が同じ状態を探しだし，true を付ける
   // ただし，行より列が大きいところしか意味がない
   vector<vector<int> > equalStates(numStates, vector<int>(numStates, true));
   for (int state = 0; state < numStates; ++state)
      for (int target = 0; target < state; ++target)
         equalStates[state][target] = (getActionChoice(state) == getActionChoice(target));

   // 遷移先で違う行動をする状態を見つけ出し，false を付ける
   bool check = true;
   while (check) {
      check = false; // 変わるところがなかったら false のままになる
      // state と target が同じか調べる
      for (int state = 0; state < numStates; ++state) {
         for (int target = 0; target < state; ++target) {
             // もう既に違うなら skip
            if (!equalStates[state][target])
               continue;
            for (int signal = 0; signal < numSignals; ++signal) {
               const int nextState = getStateTransition(state, signal);
               const int nextTargetState = getStateTransition(target, signal);
               const int row = max(nextState, nextTargetState);
               const int col = min(nextState, nextTargetState);
               if (!equalStates[row][col]) { // 遷移先が違う
                  equalStates[state][target] = false;
                  check = true;
                  break;
               }
            }
         }
      }
   }
   // 1つでも同じ（無駄な）状態があったら，true
   for (int state = 0; state < numStates; ++state) {
      for (int target = 0; target < state; ++target) {
         if (equalStates[state][target])
            return true;
      }
   }
   return false;
}
   
/*
 * メモ: 使ってない
 */
void Automaton::visit(int v, vector< vector<int> >& scc, stack<int> &S, vector<bool> &inS, vector<int> &low, vector<int> &num, int& time) {
	low[v] = num[v] = ++time;
	S.push(v); inS[v] = true;
	for (int i = 0; i < numSignals; i++) {
		int w = stateTransition[v][i];
		if (num[w] == 0) {
			visit(w, scc, S, inS, low, num, time);
			low[v] = min(low[v], low[w]);
		} else if (inS[w])
		low[v] = min(low[v], num[w]);
	}
	if (low[v] == num[v]) {
		scc.push_back(vector<int>());
		while (1) {
			int w = S.top(); S.pop(); inS[w] = false;
			scc.back().push_back(w);
			if (v == w) break;
		}
	}
}

/*
 * メモ: 使ってない
 */
Automaton Automaton::deleteTransientFSA() {
	vector< vector<int> > scc;
	vector<int> num(numStates), low(numStates);
	stack<int> S;
	vector<bool> inS(numStates);
	int time = 0;
	for (int u = 0; u < numStates; u++) {
		if (num[u] == 0) {
			visit(u, scc, S, inS, low, num, time);
		}
	}

	int numSCCs = scc.size();

	if (numSCCs == 1) {
		return *this;
	}

	vector<int> stateToSCC(numStates, -1);
	for (int i = 0; i < numSCCs; i++) {
       for (vector<int>::size_type j = 0; j < scc[i].size(); j++) {
			stateToSCC[scc[i][j]] = i;
		}
	}

	vector<bool> hasOutDegree(numSCCs, false);
	for (int i = 0; i < numSCCs; i++) {
       for (vector<int>::size_type j = 0; j < scc[i].size(); j++) {
			for (int o = 0; o < numSignals; o++) {
				if (stateToSCC[stateTransition[scc[i][j]][o]] != i) {
					hasOutDegree[i] = true;
					break;
				}
			}
			if (hasOutDegree[i]) break;
		}
	}

	vector<bool> isTransient(numStates, true);
	vector<int> newIndex(numStates, -1);
	vector<string> newNameStates;

	int numNewStates = 0;
	for (int i = 0; i < numSCCs; i++) {
		if (hasOutDegree[i]) continue;
		for (vector<int>::size_type j = 0; j < scc[i].size(); j++) {
			isTransient[scc[i][j]] = false;
			newIndex[scc[i][j]] = numNewStates;
			newNameStates.push_back(nameStates[scc[i][j]]);
			numNewStates++;
		}
	}

	Automaton newFSA;
	newFSA.setNameOfStates(newNameStates);
	newFSA.setNameOfActions(nameActions);
	newFSA.setNameOfSignals(nameSignals);
	for (int i = 0; i < numStates; i++) {
		if (isTransient[i]) continue;
		newFSA.setActionChoice(newIndex[i], actionChoice[i]);
		for (int o = 0; o < numSignals; o++) {
			newFSA.setStateTransition(newIndex[i], o, newIndex[stateTransition[i][o]]);
		}
	}

	return newFSA;
}

/*
 * 機能: 文字列化
 */
string Automaton::toString() const {
   string s;
   s += "States :";
   for (int state = 0; state < this->getNumberOfStates(); ++state)
      s += " " + this->getNameOfStates(state);
   s += "\n";
   s += "Actions :";
   for (int action = 0; action < this->getNumberOfActions(); ++action)
      s += " " + this->getNameOfActions(action);
   s += "\n";
   s += "Signals :";
   for (int signal = 0; signal < this->getNumberOfSignals(); ++signal)
      s += " " + this->getNameOfSignals(signal);
   s += "\n";
   for (int st=0; st < this->getNumberOfStates(); st++) {
      s += this->getNameOfStates(st);
      s += " ";
      s += this->getNameOfActions(getActionChoice(st));
      s += "\n";
   }
   for (int st=0; st < this->getNumberOfStates(); st++) {
      for (int sg=0; sg < this->getNumberOfSignals(); sg++) {
         int nextstate = this->getStateTransition(st,sg);
         s += this->getNameOfStates(st);
         s += " ";
         s += this->getNameOfSignals(sg);
         s += " ";
         s += this->getNameOfStates(nextstate);
         s += "\n";
      }
   }
   return s;
}

/*
 * 機能: 出力
 */
ostream& operator<<(ostream &os, const Automaton &m) {
   return os << m.toString();
}
