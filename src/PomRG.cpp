#include "PomRG.h"

//オートマトンのひな形を受け取ったオートマトンから作る
PomRG::PomRG(const Automaton aut, const Payoff po, const Environment env){
	rgAut = aut;
	rgPayoff = po;
	rgEnv = env;
	//前のピリオドの状態＋今のピリオドの状態での2次元ベクトル
	makeStName();
	makeT();
	makeBT();
}


//pomStに名前をつける
void PomRG::makeStName(){
	pomStName.resize(rgAut.getNumberOfStates());
	for(int st1=0; st1 < rgAut.getNumberOfStates(); st1++){
		pomStName[st1].resize(rgAut.getNumberOfStates());
		for(int st2=0; st2 < rgAut.getNumberOfStates(); st2++){
			pomStName[st1][st2] = rgAut.getNameOfStates(st1)+rgAut.getNameOfStates(st2);
		}
	}
}


//T:の元を作る
void PomRG::makeT(){
	//初期化
	rgTransition.resize(rgAut.getNumberOfStates());
	 for(int st=0; st < rgAut.getNumberOfStates(); st++){
	 	rgTransition[st].resize(rgAut.getNumberOfActions());
	 	for(int ac=0; ac < rgAut.getNumberOfActions(); ac++){
	 		rgTransition[st][ac].assign(rgAut.getNumberOfStates(), ZERO);
	 	}
	 }
	 //値の代入
	 for(int myac=0; myac < rgAut.getNumberOfActions(); myac++){
	 	for(int st1=0; st1 < rgAut.getNumberOfStates(); st1++){
			//ActoionProfileが1次元で格納
	 		int ap = myac*rgAut.getNumberOfActions()
	 			+ rgAut.getActionChoice(st1);
	 		//SignalProfileが1次元で格納されてる
	 		for (int opsg=0; opsg<rgAut.getNumberOfSignals(); opsg++){
	 			int st2 = rgAut.getStateTransition(st1,opsg);
	 			for (int mysg=0; mysg<rgAut.getNumberOfSignals(); mysg++){
	 				int sp = mysg*rgAut.getNumberOfSignals() + opsg;
	 				rgTransition[st1][myac][st2] += rgEnv.getSignalDistribution(ap,sp);
	 			}
	 		}
	 	}
	}
}



//O: を作成するための BeliefTransition （rgBt） を作成する
void PomRG::makeBT(){
	vector<Automaton> va;
	//2はプレイヤ数に合わせて変える？？
	va.assign(2,rgAut);
	rgBt.make(rgEnv, rgPayoff, va);
}



//.pomdpファイルを作る
//関数長いけど，単純な動きなので勘弁
//2プレイヤ限定だと思うけど，jointとしてrgAut入力すればいいと思うし，そのつもりで作ってる
//入力： string out: pomdpの出力ファイル名（.pomdp は自動でつく）
//     int initialSt: pomdpの初期状態（RGでいう相手の初期状態にあたる）
void PomRG::makePomdpFile(const string &out, const int initialSt){
	Writer writer(out+".pomdp");
	string str;

	str = "discount: " + MyUtil::rationalToString(rgEnv.getDiscountRate());
	writer.put(str);

	writer.put("values: reward");

	str = "states:";
	for(int st1=0; st1 < rgAut.getNumberOfStates(); st1++){
		for(int st2=0; st2 < rgAut.getNumberOfStates(); st2++){
			str = str + " " + pomStName[st1][st2];
		}
	}
	writer.put(str);

	str = "actions:";
	for(int ac=0; ac < rgAut.getNumberOfActions(); ac++){
		str = str + " " + rgAut.getNameOfActions(ac);
	}
	writer.put(str);

	str = "observations:";
	for(int sg=0; sg < rgAut.getNumberOfSignals(); sg++){
		str = str + " " + rgAut.getNameOfSignals(sg);
	}
	writer.put(str);

	//初期状態の設定，多分合ってるはず
	str = "start: " + pomStName[0][initialSt];
	writer.put(str);

	//T:ac:nowSt:nextSt ProbValue
	for(int ac=0; ac < rgAut.getNumberOfActions(); ac++){
		for(int nowSt2=0; nowSt2 < rgAut.getNumberOfStates(); nowSt2++){
			int nextSt1 = nowSt2;
			for(int nextSt2=0; nextSt2 < rgAut.getNumberOfStates(); nextSt2++){
				for(int nowSt1=0; nowSt1 < rgAut.getNumberOfStates(); nowSt1++){
					str = "T: " + rgAut.getNameOfActions(ac)
						+ " : " + pomStName[nowSt1][nowSt2]
						+ " : " + pomStName[nextSt1][nextSt2]
						+ " " +  MyUtil::rationalToString(rgTransition[nowSt2][ac][nextSt2]);
					writer.put(str);
				}
			}
		}
	}


	//O:ac:nextSt:sg ProbValue
	for(int myac=0; myac < rgAut.getNumberOfActions(); myac++){
		for(int nextSt1=0; nextSt1 < rgAut.getNumberOfStates(); nextSt1++){
			for(int nextSt2=0; nextSt2 < rgAut.getNumberOfStates(); nextSt2++){
				//tmpProb[mysg]には， O(mysg,opsg | (myac, ac(opst))) が入る
				vector<PreciseNumber> tmpProb;
				tmpProb.assign(rgAut.getNumberOfSignals(), ZERO);
				PreciseNumber tmpProbSum = ZERO;
				for(int opsg=0; opsg<rgAut.getNumberOfSignals(); opsg++){
					if(rgAut.getStateTransition(nextSt1, opsg) == nextSt2){
						for(int mysg=0; mysg<rgAut.getNumberOfSignals(); mysg++){
							ActionProfile ap(myac, rgAut.getActionChoice(nextSt1));
							SignalProfile sp(mysg, opsg);
							tmpProb[mysg] += rgEnv.getSignalDistribution(ap.getIndex(),sp.getIndex());
							tmpProbSum += rgEnv.getSignalDistribution(ap.getIndex(),sp.getIndex());
						}
					}
				}
				//tmpProbを使って書き出し
				if(tmpProbSum > PreciseNumber(0)){
					for(int mysg=0; mysg<rgAut.getNumberOfSignals(); mysg++){
						str = "O: " + rgAut.getNameOfActions(myac)
							+ " : " + pomStName[nextSt1][nextSt2]
							+ " : " + rgAut.getNameOfSignals(mysg)
							+ " " + MyUtil::rationalToString(tmpProb[mysg]/tmpProbSum);
						writer.put(str);
					}
				}else{
					//初回ピリオドより後のピリオドで遷移することのない状態（初回ピリオドの状態をこの状態であると指定しない限り到達しない状態）についても O: を記述する必要があるため，適当に値を入れる．つまり，値には特に意味はないので，適当に当確率にする
					for(int mysg=0; mysg<rgAut.getNumberOfSignals(); mysg++){
						str = "O: " + rgAut.getNameOfActions(myac)
							+ " : " + pomStName[nextSt1][nextSt2]
							+ " : " + rgAut.getNameOfSignals(mysg)
							+ " " + MyUtil::rationalToString(1.0/rgAut.getNumberOfSignals());
						writer.put(str);
					}
				}
			}
		}
	}

	//R:ac:nowSt:*:* RewardValue
	for(int nowSt2=0; nowSt2 < rgAut.getNumberOfStates(); nowSt2++){
		for(int myac=0; myac < rgAut.getNumberOfActions(); myac++){
			for(int nowSt1=0; nowSt1 < rgAut.getNumberOfStates(); nowSt1++){
				//ActionProfileが1次元で格納されてる
				int ap = myac*rgAut.getNumberOfActions()
					+ rgAut.getActionChoice(nowSt2);
				str = "R: " + rgAut.getNameOfActions(myac)
					+ " : " + pomStName[nowSt1][nowSt2] + " : * : * "
				 	+ MyUtil::rationalToString(rgPayoff.getPayoff(0,ap));
				writer.put(str);
			}
		}
	}
}




namespace PomUtil{
	//pomdp solver を利用して， AlphaVector を得る
	vector< vector<PreciseNumber> > getPomdpAlpha(const Automaton aut, const Payoff po, const Environment env, const int initialState, const int horizon, string pomdpFile){
		//.pomdp作成
		PomRG pomrg(aut, po, env);
		//initialStateは相手の初期状態
		pomrg.makePomdpFile(pomdpFile, initialState);

		//.pomdp実行	horizon は pomdp の動作回数を指定するオプション
		PomUtil::solvePomdp(pomdpFile, horizon);

		//pomdp の出力 .alpha を読みとる
		//PreciseNumber pomdpMatrixAlpha = PomUtil::makeAlphaVector(pomdpFile);
		return PomUtil::makePomdpAlpha(pomdpFile);
	}

	//入力するAlphaVector (rgMatrixAlpha) をファイルに書き出す
	void writeTerminalAlpha(vector<Automaton> ams, vector< vector<PreciseNumber> > matrixAlpha, string pomdpFile){
		string filename = pomdpFile + ".alpha";
		// string filename = pomdpFile + "_terminal.alpha";
		Writer alphaWriter(filename.c_str());
		for(unsigned int j=0; j < matrixAlpha.size(); j++){
			alphaWriter.put(ams[0].getActionChoice(j));
			string buff;
			for(unsigned int k=0; k < matrixAlpha[j].size(); k++){
				for(unsigned int l=0; l < matrixAlpha[j].size(); l++){
					if(k>0 || l>0) buff += " ";
					buff += MyUtil::rationalToString(matrixAlpha[j][l]);
				}
			}
			alphaWriter.put(buff + "\n");
		}
	}

	//.pomdp の実行
	// void solvePomdp(const string &pomdpFile, const int horizon){
	// 	string pomdpCmd = "pomdp-solve -pomdp " + pomdpFile + ".pomdp -o " + pomdpFile + " -horizon " + MyUtil::toString(horizon);
	// 	system(pomdpCmd.c_str());
	// }

	//.pomdp の実行 (初期alpha渡す)
	const int TIME_LIMIT = 600;  //sec
	void solvePomdp(const string &pomdpFile, const int horizon){
		// string pomdpCmd = "pomdp-solve -pomdp " + pomdpFile + ".pomdp -o " + pomdpFile + " -terminal_values " + pomdpFile + "_terminal.alpha -horizon "
		// string pomdpCmd = "pomdp-solve -pomdp " + pomdpFile + ".pomdp -o " + pomdpFile + " -terminal_values " + pomdpFile + ".alpha -horizon "
		// 	+ MyUtil::toString(horizon) + " -stop_criteria bellman -time_limit " + MyUtil::toString(TIME_LIMIT);
		string pomdpCmd = "pomdp-solve -pomdp " + pomdpFile + ".pomdp -o " + pomdpFile + " -terminal_values " + pomdpFile + ".alpha -horizon "
			+ MyUtil::toString(horizon) + " -stop_criteria bellman";
		system(pomdpCmd.c_str());
	}

	//solverから返ってきたファイルを読んで，状態0の alphaVector の [0][initialState] の要素を取り出して，pomdpAlpha に入れる
	vector< vector<PreciseNumber> > makePomdpAlpha(const string &pomdpFile){
		string alphaFile = pomdpFile + ".alpha";
		std::ifstream ifs(alphaFile.c_str());
		std::string str;
		vector< vector<PreciseNumber> > pomdpAlpha;
		int p;
		int strCount=0;

		if(ifs.fail()){
			Message::display("pomdp read error");
			exit(0);
		}
		while(getline(ifs,str)){
			strCount++;
			//行動のラベルの行（最初の行） と 区切りの空白行はスキップする
			if(strCount%3 != 2) continue;

			//スペースごとに区切りながら，値を代入していく
			vector<PreciseNumber> inner;
			while( (p = str.find(" ")) != str.npos ){
				inner.push_back (PreciseNumber(MyUtil::numberToRational(str.substr(0,p))));
				str = str.substr(p+1);//スペースも含めて削除
			}
			pomdpAlpha.push_back(inner);
		}

		return pomdpAlpha;
	}

	//alphaMatrix[x][initialState] が 最大となる要素の x を出力する
	int maxStateAlphaForInitial(const vector<vector<PreciseNumber> > &alphaMatrix, const int initialState){
		int maxst=0;
		for(int st=0; st<alphaMatrix.size(); st++){
			// if(alphaMatrix[st]>alphaMatrix[maxst]){
			if(alphaMatrix[st][initialState]>alphaMatrix[maxst][initialState]){
				maxst = st;
			}
		}
		return maxst;
	}



}
