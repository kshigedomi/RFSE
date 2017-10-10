#include "rfpe.h"

/*
 * メインクラス．
 * 入力ファイルを読み，RFSE or global RFSE かを判定して，出力ファイルに書き出す．
 */


// 2016-12-06追加
// AlphaVectorsだけを計算する
vector<vector<PreciseNumber> > RFSE::setAndGetAlphaVectors() {
	RewardEquation rewardEquation(environment, payoff, automatons);
	rewardEquation.solve();
	return rewardEquation.getAlphaVectors();
}


void RFSE::setRepeatedGame() {
	RewardEquation rewardEquation(environment, payoff, automatons);
	rewardEquation.solve();
	rg.configure(environment, payoff, automatons, rewardEquation);
	rg.solve();
}

void RFSE::setAllAutomatons(const Automaton& m) {
	for (vector<Automaton>::size_type pl = 0; pl < automatons.size(); ++pl)
		automatons[pl] = m;
}

void RFSE::setVariablesToEnvironmentAndPayoff() {
	environment.setSignalDistributionFromRawSignalDistribution(variables);
	payoff.setPayoffFromRawPayoff(variables);
}

void RFSE::checkReward(const string &out) {
	Message::display("checkReward");
	// START> ここから
	// 変数の始まり，精度，終わり
	const PreciseNumber start = PreciseNumber("1/100");
	const PreciseNumber interval = PreciseNumber("1/100");
	const PreciseNumber end = PreciseNumber("101/100");

	const bool discount = false;  // true なら割引因子ごとの平均利得．false なら変数ごとの平均利得
	const string changeString = string("x");
	// END> ここまでを変更することで必要な動作を行わせることができる．

	Writer writer(out);
	writer.writeEnvironment(rg, environment, payoff, variables);
	if (discount) {
		writer.put("ChangeVariable : discount factor");
	} else {
		writer.put("ChangeVariable : " + changeString);
	}

	for (PreciseNumber value = start; value < end; value += interval) {
		Message::display(MyUtil::rationalToString(value));
		// 変数をセット
		if (discount) {
			environment.setDiscountRate(value);
		} else {
			variables[changeString] = value;
		}
		if (!environment.isValidEnvironment()) {
			break;
		}
		// 平均利得を書き出す
		setVariablesToEnvironmentAndPayoff();
		setRepeatedGame();
		const Result result = rg.checkNoEquilibrium();
		PreciseNumber reward = PreciseNumber("-1");
		if ((result == SUCCESS) || (result == GLOBAL)) {
			vector<vector<PreciseNumber> > ex = rg.getAlphaVectors();
			reward = ex[0][0];
		}
		writer.put(MyUtil::rationalToString(value) + "\t" +  MyUtil::rationalToString((1-environment.getDiscountRate())*reward));
	}
	Message::display("End");
}

void RFSE::checkParameter(const string &out) {
	Message::display("checkParmeter");
	// START>ここから
	const PreciseNumber xStart = PreciseNumber("60/100");
	const PreciseNumber xInterval = PreciseNumber("10/100");
	const PreciseNumber xEnd = PreciseNumber("100/100");
	const PreciseNumber yStart = PreciseNumber("1/100");
	const PreciseNumber yInterval = PreciseNumber("1/100");
	const PreciseNumber yEnd = PreciseNumber("40/100");
	const string xChangeString = string("p");
	const string yChangeString = string("q");
	// END> ここまでを変更して必要な動作をさせる．

	Writer writer(out);
	setVariablesToEnvironmentAndPayoff();
	writer.writeEnvironment(rg, environment, payoff, variables);

	writer.put("PARAMETER CHECK");
	writer.put(xChangeString + "\t" + yChangeString + "Under\t" + yChangeString + "Over");
	for (PreciseNumber x = xStart; x <= xEnd; x += xInterval) {
		variables[xChangeString] = x;
		Message::display(MyUtil::rationalToString(x));
		bool isEq = false;
		PreciseNumber yOver = ZERO;
		PreciseNumber yUnder = ZERO;
		for (PreciseNumber y = yStart; y <= yEnd; y += yInterval) {
			variables[yChangeString] = y;
			setVariablesToEnvironmentAndPayoff();
			if (!environment.isValidEnvironment()) {
				if (isEq) {
					yOver = y - yInterval;
				}
				isEq = false;
				break;
			}
			setRepeatedGame();
			const Result result = rg.checkNoEquilibrium();
			if (((result == SUCCESS) || (result == GLOBAL)) && !isEq) {//first success
				Message::display("Accomotability : " + MyUtil::resultToString(result));
				yUnder = y;
				isEq = true;
			} else if ((result != SUCCESS) && (result != GLOBAL) && isEq) {//last success
				yOver = y - yInterval;
				break;
			}
		}
		if (yOver == ZERO && isEq) {
			yOver = yEnd;
		}
		writer.writeEqRange(x, yUnder, yOver);
	}
}

void RFSE::checkRegularAutomatons(const string &in, const string &out) {
	Message::display("check regular start");
	Writer writer(out);
	writer.writeEnvironment(rg, environment, payoff, variables);
	Loader loader(automatons[PLAYER], in); // 元のオートマトンをセット
	long count = 0;
	setVariablesToEnvironmentAndPayoff();
	while (loader.setNextAutomatonFromRawString()) {
		// オートマトンをセット
		const Automaton m = loader.getAutomaton();
		this->setAllAutomatons(m);
		++count;
		Message::display(MyUtil::toString(count) + "番目のオートマトン");
		Message::display(m);

		// 均衡判定
		setRepeatedGame();
		const Result result = rg.checkNoEquilibrium();
		Message::display("Accomotability : " + MyUtil::resultToString(result));
		if (result == SUCCESS || result == TIMEISOVER || result == GLOBAL || result == NOTFIXED) {
			writer.put("No." + MyUtil::toString(count));
			writer.writeResult(result);
			vector<vector<PreciseNumber> > ex = rg.getAlphaVectors();
			writer.put("Average Payoff: " + MyUtil::rationalToString((1-environment.getDiscountRate())*(ex[0][0]/(ONE))));
			writer.put(m);
		}

		if (count % 100 == 0){ //100ごとに記録しておく
			writer.put(MyUtil::toString(count) + "番目終了");
		}

	}
	writer.put("End gracufully");
	Message::display("End gracufully");
}





/*
 * 2016-12-07 作成
 *option1=======
 * ALL-C, ALL-D に勝つものの書き出し
 * 元のゲームの入力データとして、プレイヤ１が All-C プレイヤ2が All-D を渡すと、
 * オートマトンの入力データから読み込んだオートマトンと、それらの戦略の組の利得を比較する．
 * 比較の結果，ALL-C，ALL-Dよりも大きい利得を与える戦略のみを出力として書き出す．
 *option2=======
 * 初期状態の利得で足切りする
 *
 * 2017-09-21 変更
 * 上記のoption1のみを用いる
 * 利得の比較対象を all_C, all-D, GT, TFT, 1-MP, 2-MP に増やした
 * 比較方法
 * m: 調査対象のFSA m': 比較対象のFSA (all-C, all-D など)
 * (1) 自分mで相手m,どちらも状態w のときと (2) 自分m'で相手m, 状態は自分がw'で相手がw を比較して
 * すべてのw'で (1) が (2) の割引期待利得が下回らない
 * これがすべての w で成り立つ → m'に勝った
 */
void RFSE::payoff_AllCD_filter(const string &in, const string &out) {

	//all-C,D での判定を行うか
	bool beatCD = true;

	// 利得比較対象の pre-FSA
	// 現状では調べるFSAと比較対象のFSAが同じ状態数でないと動かないため，差を埋めるために冗長な状態を付け加えている．
	vector<Automaton> benchmarkFSA;
	int consideringStateOfPlayer[] = {1, 1, 2, 2, 2, 3}; // いくつめの状態までが必要な状態であるか？ それぞれbenchmarkFSANameの各要素と対応
	int numOfBenchmarkFSA[] = {0, 5, 6, 0}; // 最初からいくつのFSAを利得比較対象に用いるか？ (2状態FSAとの比較対象に3状態FSAを使わないようにしている)
	string benchmarkFSAName[] = {"all-C", "all-D", "GT", "TFT", "1-MP", "2-MP"};

	Message::display("check regular start");
	Writer writer(out);
	string buf = "beat ";
	for(int i=0; i<numOfBenchmarkFSA[automatons[0].getNumberOfStates()-1]; i++){
		if(i == numOfBenchmarkFSA[automatons[0].getNumberOfStates()-1]-1){
			buf += " and ";
		}else if(i>0){
			buf += ", ";
		}
		buf += benchmarkFSAName[i];
	}
	writer.put(buf + " strategies");
	Loader loader(automatons[PLAYER], in); // 調べるオートマトンのリストをセット

	long count_clearAut = 0;
	long count_Aut = 0;
	setVariablesToEnvironmentAndPayoff();
	//問題設定の書き出し
	// writer.put("R1-R1 payoff is greater than: " + MyUtil::rationalToString(targetPayoff));
	writer.put(environment.toString());
	writer.put(payoff.toString());

	//元データのオートマトン プレイヤ１が ALL-C，プレイヤ2が ALL-D を仮定
	// Automaton all_C = automatons[0];
	// Automaton all_D = automatons[1];
	string FSApath = "../benchmarkFSA/St" + MyUtil::toString(automatons[0].getNumberOfStates()) + "Ac" + MyUtil::toString(automatons[0].getNumberOfActions()) + "/";

	// 利得比較対象のpre-FSAの読み込み (ファイルパス: RFSE/benchmarkFSA/St?Ac? )
	for(int i=0; i<numOfBenchmarkFSA[automatons[0].getNumberOfStates()-1]; i++){
		Automaton am(automatons[0].getNumberOfStates(), automatons[0].getNumberOfActions(), automatons[0].getNumberOfSignals());
		Reader automatonFile(FSApath + benchmarkFSAName[i] + ".dat");
		automatonFile.readAutomaton("Player1", am);
		benchmarkFSA.push_back(am);
		// Message::display(am);
	}

	// Automaton all_C(automatons[0].getNumberOfStates(), automatons[0].getNumberOfActions(), automatons[0].getNumberOfSignals());
	// Reader allC_file(FSApath + "all-C.dat");
	// allC_file.readAutomaton("Player1", all_C);
	// Message::display(all_C);

	while (loader.setNextAutomatonFromRawString()) {
		Message::display("Cleared count: " + MyUtil::toString(count_clearAut));
		Message::display("Now checking: No." + MyUtil::toString(count_Aut));
		// オートマトンをセット
		const Automaton m = loader.getAutomaton();
		automatons[1] = m;

		//challenge で対challenge のときの利得
		automatons[0] = m;
		vector<vector<PreciseNumber> > ex_challenge = setAndGetAlphaVectors();
		// count_Aut=238: State3Action2Signal2 における 2-MP
		// if(count_Aut == 238){
		// 	Message::display("2-MP vs 2-MP");
		// 	Message::display(automatons[0]);
		// 	Message::display(automatons[1]);
		// }

		// ここから利得の比較
		bool challenge_defeated = false;
		if(beatCD){
			for(int FSAid=0; FSAid<numOfBenchmarkFSA[automatons[0].getNumberOfStates()-1]; FSAid++){
				automatons[0] = benchmarkFSA[FSAid];
				// Message::display(automatons[0]);
				vector<vector<PreciseNumber> > ex_benchmark = setAndGetAlphaVectors();

				// todo: 3プレイヤ以上の場合をどうするか？
				// クラス OpponentStateProfile を用いていたが全員 (？) 同じ初期ステートの場合だけ調べるということを表現できるか？
				for(int opState = 0; opState < automatons[PLAYER].getNumberOfStates(); ++opState){
					for(int myState=0; myState<consideringStateOfPlayer[FSAid]; myState++){
						// if(count_Aut == 238){
						// 	Message::display("My: " + MyUtil::toString(opState) + " (in FSA #" + MyUtil::toString(count_Aut) + ") vs " + MyUtil::toString(opState) + " (in " + benchmarkFSAName[FSAid] + ") / OpponentState: " + MyUtil::toString(opState));
						// 	cout << "m: " << ex_challenge[opState][opState] << " " << benchmarkFSAName[FSAid] << ": " << ex_benchmark[myState][opState] << endl << endl;
						// }
						if(ex_challenge[opState][opState] < ex_benchmark[myState][opState]){
							challenge_defeated = true;
						}
					}
				}
				// if(challenge_defeated) break;
			}

			// while(true){}
			//all_C で対challenge のときの利得
			// automatons[0] = all_C;
			// vector<vector<PreciseNumber> > ex_all_C = setAndGetAlphaVectors();
			// if(count_Aut == 238){
			// 	Message::display("all-C vs 2-MP");
			// 	Message::display(automatons[0]);
			// 	Message::display(automatons[1]);
			// }

			//all_D で対challenge のときの利得
			// automatons[0] = all_D;
			// vector<vector<PreciseNumber> > ex_all_D = setAndGetAlphaVectors();
			// if(count_Aut == 238){
			// 	Message::display("all-D vs 2-MP");
			// 	Message::display(automatons[0]);
			// 	Message::display(automatons[1]);
			// }

			//All-C,All-Dに勝つかを判定
			// for (int myState = 0; myState < automatons[PLAYER].getNumberOfStates(); ++myState)
			// {
			// 	OpponentStateProfile opState;
			// 	do {
			// 		if(count_Aut == 238){
			// 			cout << "State of the opponent:" << myState<< endl;
			// 			cout << "m: " << ex_challenge[myState][myState] << " all-C: " << ex_all_C[myState][myState] << " all-D: " << ex_all_D[myState][myState] << endl;
			// 		}
			// 		//all-C に負けたら flag を立てる
			// 		if(ex_challenge[myState][myState] < ex_all_C[myState][myState])
			// 		{
			// 			if(count_Aut == 238)
			// 				cout << "vs all-C: " << ex_challenge[myState][myState] << " " << ex_all_C[myState][myState] << endl;
			// 			challenge_defeated = true;
			// 		}
			// 		//all-D に負けても flag を立てる
			// 		if(ex_challenge[myState][myState] < ex_all_D[myState][myState])
			// 		{
			// 			if(count_Aut == 238)
			// 				// cout << "vs all-D: " << ex_challenge[myState][opState.getIndex()] << " " << ex_all_D[myState][opState.getIndex()] << endl;
			// 				cout << "vs all-D: " << ex_challenge[myState][myState] << " " << ex_all_D[myState][myState] << endl;
			// 			challenge_defeated = true;
			// 		}
			// 	} while (opState.next());
			// }
		}

		//書き出す
		if(!challenge_defeated){
 			//R1-R1 の利得で振り分ける，指定値以上のものだけを書き出す
 			// 	if(((1-environment.getDiscountRate())*ex_challenge[0][0]) > targetPayoff){
			Message::display(m);
 			writer.put("REGULAR AUTOMATON");
 			writer.put(m.toRawString());
 			count_clearAut++;
 			// 	}
		}
		count_Aut++;
	}
	writer.put("num of cleared Automaton: " + MyUtil::toString(count_clearAut));
	writer.put("num of checked Automaton: " + MyUtil::toString(count_Aut));
	writer.put("End gracufully");
	Message::display("num of cleared Automaton: " + MyUtil::toString(count_clearAut));
	Message::display("num of checked Automaton: " + MyUtil::toString(count_Aut));
	Message::display("End gracufully");
}



void RFSE::checkSpecial(const string &out) {
}

void RFSE::checkParticular(const string &in, const string &out) {
}


//2017-03-16追加の関数
//pomdpに変換して判定
//2playerだけ考える（automatons[1]からpomdpを作成してるから，これをjointFSAでうまいことすれば3player以上でもできるはず）
void RFSE::checkPomdp(const string &pomdpFile) {
	//直接入力の変数
	//initialState: 自分と相手の初期状態（厳密には，pomdpの範囲では相手のみ，rgのときは自分と相手の初期状態）
	//基本的に0でいいと思う（入力データのはじめの状態がinitialになるから）
	//horizon: pomdpSolver の horizon を指定する
	const int horizon_begin = 0;
	const int horizon_step = 1;
	const int horizon_end = 0;
	// int initialState = 0;

	//出力用ファイルの準備
	Writer writer(pomdpFile);
	writer.writeEnvironment(rg, environment, payoff, variables);

	//入力戦略組の利得行列を rgMatrixAlpha とする
	this->setRepeatedGame();
	vector< vector<PreciseNumber> > rgMatrixAlpha = rg.getAlphaVectors();

	//170329作成
	//↑をpomdp-solveが読めるalphaベクトルの形式に書き出す
	PomUtil::writeTerminalAlpha(automatons, rgMatrixAlpha, pomdpFile);

	//入力戦略をpomdp-solveが読めるpolicyの形式に書き出す
	//初期policyの入力ってどうやるのか？ 初期policy入力のためのコマンドラインオプションが見つからない
	//いらないとのこと (170331)


	// bool improved = false;


	for(int initialState = 0; initialState < automatons[1].getNumberOfStates(); initialState++){
		Message::display("------------");
		Message::display("initialState: " + MyUtil::toString(initialState));
		Message::display("------------");

		writer.put("------------");
		writer.put("initialState: " + MyUtil::toString(initialState));

		int horizon = horizon_begin;
		int horizon_count = 0;
		bool improved = false;

		// string pomdpFileName = pomdpFile + "_no" + MyUtil::toString(automaton_count) + "_state" + MyUtil::toString(initialState);
		string pomdpFileName = pomdpFile;

		//入力戦略組の利得行列を rgMatrixAlpha とする
		this->setRepeatedGame();
		vector< vector<PreciseNumber> > rgMatrixAlpha = rg.getAlphaVectors();

		// 初期alphaベクトルを作成
		// if(horizon == horizon_begin){
		PomUtil::writeTerminalAlpha(automatons, rgMatrixAlpha, pomdpFileName);
		// }

		PreciseNumber pomdpAlpha, rgAlpha;


		while(horizon <= horizon_end){

			vector< vector<PreciseNumber> > pomdpMatrixAlpha;
			//pomdpMatrixAlpha を作成
			try{
				pomdpMatrixAlpha = PomUtil::getPomdpAlpha(automatons[1], payoff, environment, initialState, (horizon == horizon_begin ? horizon : horizon_step), pomdpFileName);
			}

			catch(int s){
				if(s == 124){
					Message::display("pomdp-solve timeout");
					writer.put("pomdp-solve timeout");
				}else{
					Message::display("pomdp-solve terminated abnormally");
					writer.put("pomdp-solve terminated abnormally");
				}
				improved = true;
				break;
			}

			catch(...){
				Message::display("pomdp-solve error?");
				improved = true;
				break;
			}

			//initialState に対して，最大となる要素を maxState とする
			//maxPomdpState = argmax_{i \in State} pomdpMatrixAlpha[i][initialState]
			int maxPomdpState = PomUtil::maxStateAlphaForInitial(pomdpMatrixAlpha, initialState);
			int maxRgState = PomUtil::maxStateAlphaForInitial(rgMatrixAlpha, initialState);

			//比較対象の確定
			pomdpAlpha = pomdpMatrixAlpha[maxPomdpState][initialState];
			rgAlpha = rgMatrixAlpha[maxRgState][initialState];

			Message::display("pomdpAlpha: " + MyUtil::rationalToString(pomdpAlpha));
			Message::display("rpgamAlpha: " + MyUtil::rationalToString(rgAlpha));


			if(MyUtil::equal(pomdpAlpha, rgAlpha)){
				//correct (not improved)

			}else{
				//incorrect (improved)
				improved = true;
				writer.put("pomdpAlpha: " + MyUtil::rationalToString(pomdpAlpha));
				writer.put("rpgamAlpha: " + MyUtil::rationalToString(rgAlpha));
				break;
			}

			horizon += horizon_step;
			horizon_count++;

			// not improved
			// if(!improved){
			//
			// }else{
			// 	break;
			// }
		}



		//170329
		//pomdpAlpha と rgAlpha を比較して，誤差を許して同じならばcorrectとする
		if(!improved){
			writer.put("pomdpAlpha: " + MyUtil::rationalToString(pomdpAlpha));
			writer.put("rpgamAlpha: " + MyUtil::rationalToString(rgAlpha));
			writer.put("correct");
			Message::display("correct");
		}else{
			Message::display("fail");
			writer.put("fail");
		}


	}



	//initialState に対して，最大となる要素を maxState とする
	// int maxState = PomUtil::maxStateAlphaForInitial(pomdpMatrixAlpha, initialState);

	//比較対象の確定
	// PreciseNumber pomdpAlpha = pomdpMatrixAlpha[maxState][initialState];
	// PreciseNumber rgAlpha = rgMatrixAlpha[initialState][initialState];
	// Message::display("pomdpAlpha: " + MyUtil::rationalToString(pomdpAlpha));
	// Message::display("rpgamAlpha: " + MyUtil::rationalToString(rgAlpha));

	//pomdpAlpha と rgAlpha を比較して， rgのほうが大きければcorrectとする．
	//丸め誤差とかがあると思うので，厳密な比較はできていない
	// if(pomdpAlpha > rgAlpha){
	// 	Message::display("fail");
	// }else{
	// 	Message::display("correct");
	// }


	writer.put("------------");

	Message::display("End");
}



void RFSE::checkRegularByPomdp(const string &in, const string &out) {
	//直接入力の変数
	//horizon: pomdpSolver の horizon を指定する
	const int horizon_begin = 0;
	const int horizon_step = 1;
	const int horizon_end = 0;

	// string pomdpFile = out + "-converted";
	string pomdpFile = out;

	Message::display("check POMDP regular start");
	Writer writer(out);
	writer.put("AutomatonList: " + in);
	writer.writeEnvironment(rg, environment, payoff, variables);

	Loader loader(automatons[PLAYER], in); // 元のオートマトンをセット
	setVariablesToEnvironmentAndPayoff();
	long automaton_count = 0;
	while (loader.setNextAutomatonFromRawString()) {
		// オートマトンをセット
		const Automaton m = loader.getAutomaton();
		this->setAllAutomatons(m);
		++automaton_count;
		Message::display(MyUtil::toString(automaton_count) + "番目のオートマトン");
		Message::display(m);

		//2017.05.02 表を横に埋めていく形式に

		// 均衡判定
		//initialState で for まわして，全部のinitialState についてしらべる→pureが調べられる（と思う）
		//initialState: 相手 (環境) の初期信念
		for(int initialState = 0; initialState < m.getNumberOfStates(); initialState++){
			Message::display("------------");
			Message::display("initialState: " + MyUtil::toString(initialState));
			Message::display("------------");

			int horizon = horizon_begin;
			int horizon_count = 0;
			bool improved = false;

			while(horizon <= horizon_end){
				//前のステップで改善されてたら飛ばす
				// if(improved[automaton_count-1][initialState]){
				// 	Message::display("Denied");
				// 	continue;
				// }

				// string pomdpFileName = pomdpFile + "_no" + MyUtil::toString(automaton_count) + "_state" + MyUtil::toString(initialState);
				string pomdpFileName = pomdpFile;

				//horizon = horizon_begin なら初期 alphaベクトルを作る
				//入力戦略組の利得行列を rgMatrixAlpha とする
				this->setRepeatedGame();
				vector< vector<PreciseNumber> > rgMatrixAlpha = rg.getAlphaVectors();

				// 初期alphaベクトルを作成
				if(horizon == horizon_begin){
					PomUtil::writeTerminalAlpha(automatons, rgMatrixAlpha, pomdpFileName);
				}

				//pomdpMatrixAlpha を作成
				vector< vector<PreciseNumber> > pomdpMatrixAlpha;
				//pomdpMatrixAlpha を作成
				try{
					pomdpMatrixAlpha = PomUtil::getPomdpAlpha(automatons[1], payoff, environment, initialState, (horizon == horizon_begin ? horizon : horizon_step), pomdpFileName);
				}

				catch(int s){
					if(s == 124){
						Message::display("pomdp-solve timeout");
					}else{
						Message::display("pomdp-solve terminated abnormally");
					}
					improved = true;
					break;
				}

				catch(...){
					Message::display("pomdp-solve error?");
					improved = true;
					break;
				}

				//initialState に対して，最大となる要素を maxState とする
				//maxPomdpState = argmax_{i \in State} pomdpMatrixAlpha[i][initialState]
				int maxPomdpState = PomUtil::maxStateAlphaForInitial(pomdpMatrixAlpha, initialState);
				int maxRgState = PomUtil::maxStateAlphaForInitial(rgMatrixAlpha, initialState);

				//比較対象の確定
				PreciseNumber pomdpAlpha = pomdpMatrixAlpha[maxPomdpState][initialState];
				PreciseNumber rgAlpha = rgMatrixAlpha[maxRgState][initialState];

				//170329
				//pomdpAlpha と rgAlpha を比較して，誤差を許して同じならばcorrectとする
				// if(MyUtil::equal(pomdpAlpha, rgAlpha)){
				// 	Message::display("correct");
				// 	writer.put("================");
				// 	writer.put("No. " + MyUtil::toString(automaton_count));
				// 	writer.put("InitialState: " + MyUtil::toString(initialState));
				// 	writer.put(automatons[PLAYER].toString());
				// }else{
				// 	Message::display("fail");
				// }

				Message::display("pomdpAlpha: " + MyUtil::rationalToString(pomdpAlpha));
				Message::display("rpgamAlpha: " + MyUtil::rationalToString(rgAlpha));


				if(MyUtil::equal(pomdpAlpha, rgAlpha)){
					//correct (not improved)

				}else{
					//incorrect (improved)
					improved = true;
					break;
				}

				horizon += horizon_step;
				horizon_count++;
			}

			// not improved
			if(!improved){
				Message::display("correct");
				writer.put("================");
				writer.put("No. " + MyUtil::toString(automaton_count));
				writer.put("InitialState: " + MyUtil::toString(initialState));
				writer.put(automatons[PLAYER].toString());
			}else{
				Message::display("incorrect");
			}
		}
	}



	Message::display("End");
}




void RFSE::checkAutomatons(const string &in, const string &out) {
	Message::display("check autoamtons");
	Message::display("Input: " + in);
	Message::display("Output: " + out);
	Writer writer(out);
	writer.writeEnvironment(rg, environment, payoff, variables);
	// 次の行の文字列を変えることで対象となる結果を変えることができる．
	// 現状ではNOTFIXEDだったものが判定されている．
	Loader loader(automatons[PLAYER], in, "NOTFIXED");
	while (loader.setNextAutomatonAndVariables(variables)) {
		const Automaton m = loader.getAutomaton();
		Message::display(m);
		this->setAllAutomatons(m);
		// 均衡判定
		setRepeatedGame();
		const Result result = rg.checkNoEquilibrium();
		Message::display("Accomotability : " + MyUtil::resultToString(result));
		if (result == SUCCESS || result == TIMEISOVER || result == GLOBAL || result == NOTFIXED) {
			writer.writeResult(result);
			writer.put(m);
			vector<vector<PreciseNumber> > ex = rg.getAlphaVectors();
			writer.put("Average Payoff: " + MyUtil::rationalToString((1-environment.getDiscountRate())*(ex[0][0]/(ONE))));
		}
	}
	Message::display("End gracufully");
	writer.put("End gracufully");
}

void RFSE::checkAllInformations(const string &out) {
	Writer writer(out);
	Message::display("check start");
	Message::display(automatons[PLAYER].toString());
	writer.writeEnvironment(rg, environment, payoff, variables);

	// 均衡判定
	this->setRepeatedGame();
	Message::display(rg.getBeliefDivision().toString());
	Message::display(rg.getTargetBeliefDivision().toString());
	vector<vector<PreciseNumber> > ex = rg.getAlphaVectors();;
	Message::display("Average Payoff : " + MyUtil::rationalToString((1-environment.getDiscountRate())*ex[0][0]));

	const Result result = rg.checkNoEquilibrium();
	Message::display("Accomotability : " + MyUtil::resultToString(result));

	// 書き込み
	writer.writeResult(result);
	if (result == FAILURE || result == NOTCONSISTENT) {
		writer.writeProfitableOneShot(rg.getFailure(), rg, automatons);
	} else if (result == NOTEXIST) {
		writer.writeBeliefSpace(rg, automatons);
	}
	writer.writeMaxIteration(rg);
	writer.writeAveragePayoff(environment, rg, automatons);
}





void RFSE::checkRFSE(const string &datafile, const string &prefix, int argc, char **argv) {
	Reader reader(datafile);
	string title;
	reader.read(title, this->environment, this->payoff, this->automatons, this->variables);

	// どのメソッドを呼ぶを決める
	int variableStart = 3;
	Option option = NONE;
	string filename;
	if (argc >= 4) {
		string tmp = argv[3];
		if(tmp.find("parameter",0) != string::npos) {
			variableStart = 4;
			option = PARA;
		} else if (tmp.find("reward",0) != string::npos) {
			variableStart = 4;
			option = REWARD;
		} else if (tmp.find("special",0) != string::npos) {
			variableStart = 4;
			option = SPECIAL;
		} else if (tmp.find("pomdp",0) != string::npos) {
			variableStart = 4;
			option = POMDP;
		} else if (argc >= 5) {
			filename = argv[4];
			if (tmp.find("automaton",0) != string::npos){
				variableStart = 5;
				option = AUTOMATONS;
			} else if(tmp.find("regular",0) != string::npos) {
				variableStart = 5;
				option = REGULAR;
			} else if (tmp.find("particular",0) != string::npos) {
			 	variableStart = 5;
			 	option = PARTICULAR;
			} else if(tmp.find("pomreg",0) != string::npos) {
				variableStart = 5;
				option = POMREG;
			} else if (tmp.find("filter",0) != string::npos) {
			 	variableStart = 5;
			 	option = FILTER;
			}
		}
	}

	// オプションの設定
	boost::regex expr("([a-zA-Z0-9]+)=(.+)");
	boost::smatch what;

	PreciseNumber value;
	for (int i = variableStart; i < argc; i++) {
		string exp = argv[i];
		if (exp[0] == '-') {
			if (exp == "-debug") {
				rg.set_history_flag(true);
				rg.set_preprocess_view_flag(true);
			} else if (exp == "-history") {
				rg.set_history_flag(true);
			} else if (exp == "-preprocessview") {
				rg.set_preprocess_view_flag(true);
			} else if (exp == "-target") {
				rg.set_target_flag(true);
			} else if (exp == "-preprocess") {
				rg.set_preprocess_flag(true);
			} else if (exp == "-pure") {
				rg.set_pure_flag(true);
			} else if (exp == "-timeover") {
				rg.set_timeover_flag(true);
			} else if (exp == "-belief") {
				rg.set_beliefTransition_view_flag(true);
			} else if (exp == "-nofixed") {
				rg.set_no_fixed_flag(true);
			} else if (exp == "-global") {
				rg.set_global_flag(true);
			} else if (exp == "-nobd") {
				rg.set_nobd_flag(true);
			} else {
				Message::alert("Fail to parse options in arguments");
				Message::display("Abort");
				return;
			}
		} else if (boost::regex_match(exp, what, expr)) {
			variables[what[1]] = MyUtil::numberToRational(what[2]);;
		} else {
			Message::alert("Fail to parse variables in arguments");
			Message::display("Abort");
			return;
		}
	}

	// シグナル分布と利得表の設定
	this->setVariablesToEnvironmentAndPayoff();
	environment.checkEnvironment();

	string out;
	switch (option) {
	case NONE:
		this->checkAllInformations(prefix);
		break;
	case AUTOMATONS:
		this->checkAutomatons(filename, prefix);
		break;
	case PARA:
		out = prefix + "-para";
		this->checkParameter(out);
		break;
	case REGULAR:
		out = prefix + "-regular";
		this->checkRegularAutomatons(filename, out);
		break;
	case SPECIAL:
		out = prefix + "-special";
		this->checkSpecial(out);
		break;
	case POMDP:
		out = prefix + "-pomdp";
		this->checkPomdp(out);
		break;
	case PARTICULAR:
		out = prefix + "-particular";
		this->checkParticular(filename, out);
		break;
	case POMREG:
		out = prefix + "-pomreg";
		this->checkRegularByPomdp(filename, out);
		break;
	case REWARD:
		out = prefix + "-reward";
		this->checkReward(out);
		break;
	case FILTER:
		out = prefix + "-filter";
		this->payoff_AllCD_filter(filename, out);
		break;
	}
}

int main(int argc, char **argv) {
	if (argc >= 3) {
		RFSE rfse;
		try {
			rfse.checkRFSE(string(argv[1]), string(argv[2]), argc, argv);
		} catch (exception ex) {
			cout << ex.what() << endl;
		}
	} else {
		Message::display("Usage : rfpe [datafile] [output-prefix] {[variables], ...}");
		Message::display("OR : rfpe [datafile] [output-prefix] [option] {[input-data] [varables], ...}");
		Message::display("More Help : rfpe --help");
		Message::display("variables :");
		Message::display("	[name]=[value]	e.g) p=0.5 q=1/6");
	}
	return 0;
}
