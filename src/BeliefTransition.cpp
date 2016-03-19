#include "BeliefTransition.h"

/*
 * このクラスは信念の遷移を受け持つ.
 * 時間節約のため,長いヒストリの遷移は行列 (BeliefMatrix, BM) として,保存する     
 * つまり, nextBelief = BM * beliefTransition * Belief
 * かつ, nextBM = BM * beliefTransition
 */

/*
 * 機能: BeliefTransition を作る
 * 引数: 環境変数 env, pd , pls
 * メモ: beliefTransition[myAction][mySignal][nextOpState][opState] は
 *       (myAction, mySignal) を見た後 opState にいた相手が nextOpState に移る確率が入っている
 */
void BeliefTransition::make(const Environment& env, const Payoff &pd, const Players &pls) {
  // 準備
  playerActions = pls.getNumberOfActions(PLAYER);
  playerSignals = pls.getNumberOfSignals(PLAYER);
  opponentStates = pls.getNumberOfOpponentStates();
  opponentSignals = pls.getNumberOfOpponentSignals();

  beliefTransition.clear();

  beliefTransition.resize(playerActions, vector<vector<vector<PreciseNumber> > > (playerSignals, vector<vector<PreciseNumber> >(opponentStates, vector<PreciseNumber>(opponentStates, ZERO))));

  for (int opState = 0; opState < opponentStates; ++opState) {
	const StateProfile opState_p = pls.getOpponentStateProfile(opState); // 相手の状態の組
	const ActionProfile opAction_p = pls.opponentActionsOfJointStates(opState_p); // そのときの相手の行動の組
	for (int opSignal = 0; opSignal < opponentSignals; ++opSignal) {
	  const SignalProfile opSignal_p = pls.getOpponentSignalProfile(opSignal); // 相手のシグナルの組
	  const StateProfile nextOpState_p = pls.opponentTransitionsOfJointStates(opState_p, opSignal_p); // そのとき,相手が移る次の状態の組
	  const int nextOpState = pls.getOpponentState(nextOpState_p); // 相手の状態を index 化
	  for (int myAction = 0; myAction < playerActions; ++myAction) {
		const ActionProfile ap(myAction, opAction_p); // 行動の組
		for (int mySignal = 0; mySignal < playerSignals; ++mySignal) {
		  const SignalProfile op(mySignal, opSignal_p); // シグナルの組
		  beliefTransition[myAction][mySignal][nextOpState][opState] += env.getSignalDistribution(ap, op); // 確率を入れる
		}
	  }
	}
  }
}
               
/*
 * 機能: 最初の BeliefMatrix（単位行列）を生成
 * 返り値: 単位行列
 */
BeliefMatrix  BeliefTransition::createBeliefMatrix(const int opponentStates) const { 
  return getIdentityMatrix(opponentStates);
}

/*
 * 機能: 現在の観測から,次の BM を作り出す
 * 返り値: 次の BM
 * 引数: 現在の BM, 観測 firstOb
 * 注意: nextBM = BM * beliefTransition
 */
BeliefMatrix BeliefTransition::nextBeliefMatrix(const BeliefMatrix &bMatrix, const pair<int, int> &firstOb) const {
  return matrixMul(bMatrix, beliefTransition[firstOb.first][firstOb.second]);
}

/*
 * 機能: BM と初期信念から,事後信念を作り出す
 * 引数: 初期信念 bd, BM
 * 返り値: 事後信念
 */
Belief BeliefTransition::nextBeliefFromBeliefMatrix(const Belief &bd, const BeliefMatrix &bMatrix) const{
  vector<PreciseNumber> num = matrixVectorProduct(bMatrix, bd.getVector());
  num = normalize(num);
  return Belief(num);
}

/*
 * 機能: BM を使わずに,行動とシグナルから,事後信念を作り出す.
 * 引数: 事前信念 b, 行動 myAction, シグナル mySignal
 * 返り値: 事後信念
 */
Belief BeliefTransition::nextBelief(const Belief &b, const int myAction, const int mySignal) const {
  vector<PreciseNumber> belief = matrixVectorProduct(beliefTransition[myAction][mySignal], b.getVector());
  belief = normalize(belief);
  return Belief(belief);
}

/*
 * 機能: BM を使わずに,行動とシグナルから,事後信念を作り出す.
 * 引数: 事前信念の集合 b, 行動 myAction, シグナル mySignal
 * 返り値: 事後信念の集合
 */
vector<Belief> BeliefTransition::nextBeliefSet(const vector<Belief> &bd, const int action, const int signal) const {
  vector<Belief> res;

  for (vector<Belief>::size_type i = 0; i < bd.size(); i++) {
	res.push_back(nextBelief(bd[i], action, signal));
  }

  sort(res.begin(), res.end());

  return res;
}

/*
 * 機能: 使ってない, 何に使うかもよくわからない
 */
vector<Belief> BeliefTransition::nextBeliefDivision(const vector<Belief> &bd, const int action, const int signal) const {
  vector<Belief> res;

  for (vector<Belief>::size_type i = 0; i < bd.size(); i++) {
	res.push_back(nextBelief(bd[i], action, signal));
  }

  vector< vector<PreciseNumber> > b2p = convertBeliefToPoint(res);
  vector< vector<PreciseNumber> > p2p = convertPointToPoint(b2p);

  res = convertPointToBelief(b2p);
  sort(res.begin(), res.end());

  return res;
}

/*
 * 機能: 昔使っていた,nextBelief.
 * メモ: こちらは必ず正しい
 */
Belief BeliefTransition::oldNextBelief(const Environment &env, const Players &pls, const Belief &bp, const int myAction, const int mySignal) const {
  Belief res(bp.getDimension());

  for (int nextState = 0; nextState < opponentStates; nextState++) { // nextState is belief index
	PreciseNumber num = 0, denom = 0;
	for (int opState = 0; opState < opponentStates; opState++) {
	  PreciseNumber num2 = 0, denom2 = 0;
	  const StateProfile opState_p = pls.getOpponentStateProfile(opState);
	  const ActionProfile opAction_p = pls.opponentActionsOfJointStates(opState_p);
	  const ActionProfile ap = pls.getActionProfile(myAction, opAction_p);
	  for (int opSignal = 0; opSignal < opponentSignals; opSignal++) {
		const SignalProfile opSignal_p = pls.getOpponentSignalProfile(opSignal);
		const StateProfile nextOpState_p = pls.opponentTransitionsOfJointStates(opState_p, opSignal_p);
		const int nextOpState = pls.getState(nextOpState_p);
		if (nextState == nextOpState) {
		  num2 += env.getSignalDistribution(ap, pls.getSignalProfile(mySignal, opSignal_p));
		}
		denom2 += env.getSignalDistribution(ap, pls.getSignalProfile(mySignal, opSignal_p));
	  }
         
	  num += bp.get(opState) * num2;
	  denom += bp.get(opState) * denom2;
	}
      
	if (denom == 0) {
	  denom = 1;
	}
      
	num /= denom;

	if (num < 0) {
	  num = 0;
	}
	if (num > 1) {
	  num = 1;
	}

	res.set(nextState, num);
  }
  return res;
}   


void BeliefTransition::test(const Environment &env, const Players &pls, const Belief &bp) const {
  srand(time(NULL));
  const int playerActions = pls.getNumberOfActions(PLAYER);
  const int playerSignals = pls.getNumberOfSignals(PLAYER);

  vector<pair<int, int> > test_history;
  const int test_length = 50;

  for (int t = 0; t < test_length; ++t) {
	test_history.push_back(make_pair(rand() % playerActions, rand() % playerSignals));
  }

  BeliefMatrix BM = createBeliefMatrix(pls.getNumberOfOpponentStates());

  for (int t = test_length - 1; t >= 0; --t) {
	BM = nextBeliefMatrix(BM, test_history[t]);
  }

  Belief BMBelief = nextBeliefFromBeliefMatrix(bp, BM);

  Belief oldBelief = bp;
  Belief newBelief = bp;
  
  for (int t = 0; t < test_length; ++t) {
	oldBelief = oldNextBelief(env, pls, oldBelief, test_history[t].first, test_history[t].second);
	newBelief = nextBelief(newBelief, test_history[t].first, test_history[t].second);
  }

  cout << "BM " << BMBelief << endl;
  cout << "old " << oldBelief << endl;
  cout << "new " << newBelief << endl;
}

/*
 * 機能: 昔使っていた,nextBelief と比べる
 * メモ: テスト用
 */
void BeliefTransition::test(const Environment &env, const Players &pls, const Belief &bp, const int myAction, const int mySignal) const {
  // 出力
  cout << "test start" << endl;
  string res;
  res += "** BeliefTransition **\n";

  for (int myac = 0; myac < playerActions; ++myac) {
	res += "* MyAction = " + ::toString(myac) + "\n";
	for (int mySignal = 0; mySignal < playerSignals; ++mySignal) {
	  res += "** mySignal = " + ::toString(mySignal) + "\n";
	  for (int nextOpState = 0; nextOpState < opponentSignals; ++nextOpState) {
		res += "*** nextOpState = " + ::toString(nextOpState) + "\n";
		for (int opState = 0; opState < opponentStates; ++opState) {
		  res += "**** opState = " + ::toString(opState) + "\n";
		  res += rationalToString(beliefTransition[myac][mySignal][nextOpState][opState]) + "\n";
		}
	  }
	}
  }

  // 昔の信念遷移と比べる
  Belief test1 = oldNextBelief(env, pls, bp, myAction, mySignal);

  Belief test2 = nextBelief(bp, myAction, mySignal);

  cout << "oldBelief" << endl;
  cout << test1.toString() << endl;
  cout << "new" << endl;
  cout << test2.toString() << endl;
  if (test1 != test2) {
	cout << "ERROR: Belief Transition" << endl;
	cout << bp.toString() << endl;
	cout << "action = " << myAction << endl;
	cout << "signal = " << mySignal << endl;
	cout << "correct = " + test1.toString() << endl;
	cout << "bad = " + test2.toString() << endl;
  }
  cout << "test end" << endl;
}

/*
 * 機能: 信念の遷移を文字列化
 */
string BeliefTransition::toString(const Environment &environment, const Players &players) const{
  string res;
  vector<Belief> beliefSpace = makeBeliefSpace(players.getNumberOfOpponentStates());

  for (int myAction = 0; myAction < players.getNumberOfActions(PLAYER); ++myAction) {
	for (int mySignal = 0; mySignal < players.getNumberOfSignals(PLAYER); ++mySignal) {
	  res +=  "(" + players.getNameOfActions(PLAYER, myAction) + ", " + players.getNameOfSignals(PLAYER, mySignal) + ")\n";
	  for (int myState = 0; myState < players.getNumberOfStates(PLAYER); ++myState) {
		res += oldNextBelief(environment, players, beliefSpace[myState], myAction, mySignal).toString();
		res += "\n";
	  }
	}
  }
  return res;
}
