#ifndef POMRG_H_
#define POMRG_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

#ifndef WRITER_H_
#include "Writer.h"
#endif

//.pomdp を作成するためのクラス
class PomRG {
private:
	//rgは元のRepeatedGame関係，pomは変換後のpomdp関係を指す
	Automaton rgAut;
	Payoff rgPayoff;
	Environment rgEnv;
	BeliefTransition rgBt;
	//普通の1mpだと，pomStNameは "RR","RP","PR","PP" 的な要素が入る
	vector< vector<string> > pomStName;
	void makeStName();
	vector< vector< vector<PreciseNumber> > > rgTransition;
	void makeT();
	void makeBT();

public:
	PomRG(const Automaton, const Payoff, const Environment);
	void makePomdpFile(const string&, const int);
};

//.pomdp を実行したり， .alpha を読んだりするための関数
namespace PomUtil{
	vector< vector<PreciseNumber> > getPomdpAlpha(const Automaton aut, const Payoff po, const Environment env, const int initialState, const int horizon, string pomdpFile);
	void writeTerminalAlpha(vector<Automaton> ams, vector< vector<PreciseNumber> > matrixAlpha, string pomdpFile);
	int solvePomdp(const string &pomdpFile, const int horizon);
	vector< vector<PreciseNumber> > makePomdpAlpha(const string &pomdpFile);
	int maxStateAlphaForInitial(const vector<vector<PreciseNumber> > &alphaMatrix, const int initialState);
};


#endif
