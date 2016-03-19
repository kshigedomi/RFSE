/*
 * Writer.cpp
 *
 *  Created on: 2012. 12. 6.
 *      Author: chaerim
 */

#include "Writer.h"

Writer::Writer() {
}

Writer::~Writer() {
	fout.close();
}

Writer::Writer(const string &filename) {
	fout.open(filename.c_str());
}

void Writer::openAddFileWithFilename(const string &filename){
  fout.open(filename.c_str(),std::ios::out | std::ios::app);
}

void Writer::openFileWithFilename(const string &filename){
  fout.open(filename.c_str());
}

void Writer::writeVariables(map<string,PreciseNumber> &values) {
   fout << "variables :";
   for (map<string, PreciseNumber>::iterator value = values.begin(); value != values.end(); ++value)
      fout << " " << value->first << "=" << rationalToString(value->second);
   fout << endl;
}

void Writer::writeOptions(const RepeatedGame &rg) {
   put("options");
   put(rg.optionsToString());
}

void Writer::writeEnvironment(const RepeatedGame &rg, const Environment &env,const Payoff &pd,const Players &pls, map<string,PreciseNumber> &values) {
   writeOptions(rg);
   writeVariables(values);
   put(env.toString(pls));
   put(pd.toString(pls));
}

void Writer::writeResult(const Result &result) {
   put("Result : " + resultToString(result));
}

void Writer::writePlayers(const Players &pls) {
   put(pls);
}

void Writer::writeInvariantDistribution(const vector<Belief> &id, Players &pls) {
   put("INVARIANT DISTRIBUTIONS");
   for (int myState = 0; myState < pls.getNumberOfStates(PLAYER); ++myState) {
      put("myState: " + pls.getNameOfStates(PLAYER, myState));
      put(id[myState]);
   }
}

/*
 * 機能: 信念空間を書き出す
 * 引数: 情報 rg, pls
 */
void Writer::writeBeliefSpace(const RepeatedGame &rg, Players &pls) {
  if (pls.getNumberOfOpponentStates() != 2)
   	return;
  put("Belief Space");
  for (PreciseNumber i = ZERO; i <= ONE; i += PreciseNumber("1/10")) {
	Belief b(pls.getNumberOfOpponentStates());
	b.set(0, i);
	b.set(1, ONE-i);
	writeProfitableOneShot(b, rg, pls);
    writeOffPathMaxReward(b,rg,pls);
  }
}

/*
 * 機能: オンパスよりも与えられた信念で利得の高い one shot extension を書き出す
 * 引数: Belief の集合 set, その他
 */
void Writer::writeProfitableOneShot(const vector<Belief> &set, const RepeatedGame &rg, Players &pls) {
   for (vector<Belief>::const_iterator b = set.begin(); b != set.end(); ++b)
      writeProfitableOneShot(*b, rg, pls);
}

void Writer::writeProfitableOneShot(const Belief &b,const RepeatedGame &rg, Players &pls) {
   put("Belief : " + b.toString());
   // find max on path plan
   const PreciseNumber onpathMaxReward = getMaxReward(b, rg.getAlphaVectors());
   for (int ac = 0; ac < pls.getNumberOfActions(PLAYER); ++ac) {
	 const pair<OneShotExtension, PreciseNumber> oneShot = rg.getMostProfitableOneShotWithAction(ac, b);
	 if (oneShot.second > onpathMaxReward) {
	   writeOffPath(b, oneShot.first, pls, oneShot.second);
	   put("");
	 }
   }
}

/*
 * 機能: one shot extension の中で最高の利得を与えるものを書き出す
 * 引数: belief b, その他
 */
void Writer::writeOffPathMaxReward(const Belief &b,const RepeatedGame &rg, Players &pls) {
   put("Belief : " + b.toString());
   put("maxOffPath");
   OneShotExtension maxOneShot;
   PreciseNumber maxReward = -INF;
   for (int ac = 0; ac < pls.getNumberOfActions(PLAYER); ++ac) {
	 const pair<OneShotExtension, PreciseNumber> oneShot = rg.getMostProfitableOneShotWithAction(ac, b);
	 if (oneShot.second > maxReward) {
	   maxOneShot = oneShot.first;
	   maxReward = oneShot.second;
	 }
   }
   writeOffPath(b, maxOneShot, pls, maxReward);
   put("");
}

/*
 * 機能: one shot extension を書き出す
 * 引数: one shot extension の行動 action, 遷移 transition 利得 rewards, その他 pls
 */
void Writer::writeOffPath(const Belief &b, const OneShotExtension &oneShot, const Players &pls, const PreciseNumber &oneShotReward) {
  put(oneShotToString(pls, oneShot));
  put("oneShotReward: " + rationalToString(oneShotReward));
}

void Writer::writeChangeVariables(const char &start, const char &end) {
   char c = start;
   fout << "Change :";
   while (c != end)
      fout << " " << c++;
   fout << endl;
}

void Writer::writeAveragePayoff(const Environment &env, const RepeatedGame &rg, const Players &pls) {
   const vector<vector<PreciseNumber> > alphaVectors = rg.getAlphaVectors();
   for (int myState = 0; myState < pls.getNumberOfStates(PLAYER); ++myState) {
      for (int opState = 0; opState < pls.getNumberOfOpponentStates(); ++opState) {
         const StateProfile sp = pls.getStateProfile(myState, opState);
         fout << "State:";
         for (int pl = 0; pl < pls.getNumberOfPlayers(); ++pl)
            fout << " " + pls.getNameOfStates(pl, sp[pl]);
         fout << endl;
         put("Reward : " + rationalToString((1-env.getDiscountRate()) * alphaVectors[myState][opState]));
      }
   }
   put("");
}

void Writer::writeMaxIteration(const RepeatedGame &rg) {
   put("MaxIteration: " + ::toString(rg.getMaxIteration()));
}

void Writer::writeEqRange(const PreciseNumber &x, const PreciseNumber &under, const PreciseNumber &over) {
   put(rationalToString(x) + "\t" + rationalToString(under) + "\t" + rationalToString(over));
}
   
void Writer::writeAverageSignal(const Environment &env, const Players &pls, const PreciseNumber &value) {
  for (int ac = 0; ac < env.getNumberOfActionProfiles(); ++ac) {
	const ActionProfile ap = pls.getActionProfile(ac);
	string s = "Action : (" + pls.getNameOfActions(PLAYER, ap[PLAYER]);
	for (int pl = OPPONENT; pl < pls.getNumberOfPlayers(); ++pl) {
	  s += "," + pls.getNameOfActions(pl, ap[pl]);
	}
	s += ")";
	put(s);
	PreciseNumber ave = ZERO;
	for (int sg = 0; sg < pls.getNumberOfSignals(PLAYER); ++sg) {
	  PreciseNumber prob = ZERO; // probablity my signal is sg
	  for (int opSg = 0; opSg < pls.getNumberOfOpponentSignals(); ++opSg) {
		prob += env.getSignalDistribution(pls.getActionProfile(ac), pls.getSignalProfile(sg, opSg));
	  }
	  put(pls.getNameOfSignals(PLAYER, sg) + " : " + rationalToString(prob));
	  ave += sg * value * prob;
	}
	put("Expected Signal : " + rationalToString(ave));
  }
}
		
