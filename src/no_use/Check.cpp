#include "Check.h"


/*
 * 多市場用
 * a_i = (a_i^1,a_i^2,...,a_i^m) を生成
 */
vector<int> toProfile(int index, const int radix, const int size) {
   vector<int> res(size, 0);
   for (int i = size - 1; i >= 0; --i) {
      res[i] = index % radix;
      index /= radix;
   }
   return res;
}

string profileToString(const vector<int> &profile, const vector<string> names) {
  string res;
  for (int i = 0; i < profile.size(); ++i)
	res += names[profile[i]];
  return res;
}

int countDifferences(const vector<int> &data1, const vector<int> &data2) {
  int count = 0;
  for (int i = 0; i < data1.size(); ++i) {
	if (data1[i] != data2[i])
	  ++count;
  }
  return count;
}

vector<int> estimateOpponentSignal(const vector<int> &myAc_p, const vector<int> &opAc_p, const vector<int> &mySg_p) {
  vector<int> res(myAc_p.size());
  for (int i = 0; i < myAc_p.size(); ++i) {
	if (opAc_p[i] == mySg_p[i]) // 正しいシグナルが生じる
	  res[i] = myAc_p[i];
	else {
	  if (myAc_p[i] == 0)
		res[i] = 1;
	  else
		res[i] = 0;
	}
  }
  return res;
}
		  
/*
 * 行動と自分に生じたシグナルから，生じるであろう相手のシグナルを表示
 * 正しいシグナル action[k]がsignal[k]とする
 */
int main(int argc, char *argv[]) {
   if (argc < 5) {
      cout << "USAGE : ./Check numMarkets numActions numSignals [actionNames] [signalNames] [candidateActions]" << endl;
      exit(1);
   }
   // 入力受け取り
   const int numMarkets = toInteger(string(argv[1]));
   const int numActions = toInteger(string(argv[2]));
   const int numSignals = toInteger(string(argv[3]));
   const int numAllActions = pow(numActions, numMarkets);
   const int numAllSignals = pow(numSignals, numMarkets);
   vector<string> actionNames(numActions);
   vector<string> signalNames(numSignals);
   for (int i = 0; i < numActions; ++i)
	 actionNames[i] = argv[4+i];
   for (int i = 0; i < numSignals; ++i)
	 signalNames[i] = argv[4+i+numActions];
   vector<int> candidates;
   for (int i = 0; i < argc-4-numActions-numSignals; ++i) {
	 candidates.push_back(toInteger(argv[i+4+numSignals+numActions]));
   }
   
   // メイン処理
   for (int mySg = 0; mySg < numAllSignals; ++mySg) {
	 vector<int> mySg_p = toProfile(mySg, numSignals, numMarkets);
	 cout << "MySignal : " << profileToString(mySg_p, signalNames) << endl;
	 // 相手の行動を推測
	 pair<int, vector<int> > minError(make_pair<int, vector<int> >(numMarkets, vector<int>()));
	 for (int i = 0; i < candidates.size(); ++i) {
	   vector<int> opAc_p = toProfile(candidates[i], numActions, numMarkets);
	   int error = countDifferences(opAc_p, mySg_p);
	   if (error < minError.first)
		 minError = make_pair(error, vector<int> (1 , candidates[i]));
	   else if (error == minError.first)
		 minError.second.push_back(candidates[i]);
	 }
	 cout << "OpAction might be ";
	 for (int i = 0; i < minError.second.size(); ++i) {
	   if (i != 0)
		 cout << ", ";
	   const int opAc = minError.second[i];
	   cout << profileToString(toProfile(opAc, numActions, numMarkets), actionNames);
	 }
	 cout << endl;
	 // 自分の行動とシグナルと相手の行動から相手のシグナルを推測
	 // 1行目
	 cout << setw(5) << " " << "|";
	 for (int i = 0; i < candidates.size(); ++i)
	   cout << setw(5) << profileToString(toProfile(candidates[i], numActions, numMarkets), actionNames);
	 cout << endl;
	 // 2行目
	 for (int i = 0; i < candidates.size() * 8; ++i)
	   cout << "_";
	 cout << endl;
	 // 3行目以降
	 for (int i = 0; i < candidates.size(); ++i) {
	   const int myAc = candidates[i];
	   const vector<int> myAc_p = toProfile(myAc, numActions, numMarkets);
	   cout << setw(5) << profileToString(myAc_p, actionNames);
	   cout << "|";
	   for (int j = 0; j < candidates.size(); ++j) {
		 const int opAc = candidates[j];
		 const vector<int> opAc_p = toProfile(opAc, numActions, numMarkets);
		 cout << setw(5) << profileToString(estimateOpponentSignal(myAc_p, opAc_p, mySg_p), signalNames);
	   }
	   cout << endl;
	 }
   }
}

