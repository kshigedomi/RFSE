#include <vector>
#include <iostream>

using namespace std;


void viewProbWithPublic(const vector<int> &myActions, const vector<int> &opActions, const vector<int> &mySignals, const vector<int> &opSignals) {
  const int numMarket = myActions.size();
  for (int mark = 0; mark < numMarket; ++mark) {
	if (mark != 0)
	  cout << "*";
    if (mySignals[mark] != opSignals[mark]) {
       cout << "n";
    } else if (myActions[mark] == 1 || opActions[mark] == 1) {
       if (mySignals[mark] == 1)
          cout << "(1-e-n)";
       else
          cout << "(e-n)";
    } else {
       if (mySignals[mark] == 0)
          cout << "(1-e-n)";
       else
          cout << "(e-n)";
    }
  }
}
   


int main() {
  const int numActions = 2;
  const int numSignals = 2;
  const int numAllActions = 8;
  const int numAllSignals = 8;
  const int numMarket = 3;
