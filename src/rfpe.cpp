#include "rfpe.h"

/*
 * メインクラス．
 * 入力ファイルを読み，RFSE or global RFSE かを判定して，出力ファイルに書き出す．
 */

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
    const PreciseNumber start = PreciseNumber("1/1000");
    const PreciseNumber interval = PreciseNumber("1/1000");
    const PreciseNumber end = PreciseNumber("1/2");

    const bool discount = false;  // true なら割引因子ごとの平均利得．false なら変数ごとの平均利得
    const string changeString = string("p");
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
    Message::display("");
}  

void RFSE::checkParameter(const string &out) {
    Message::display("checkParmeter");
    // START>ここから
    const PreciseNumber xStart = PreciseNumber("5000/10000");
    const PreciseNumber xInterval = PreciseNumber("1/100");
    const PreciseNumber xEnd = ONE;
    const PreciseNumber yStart = PreciseNumber("1/10000");
    const PreciseNumber yInterval = PreciseNumber("1/100");
    const PreciseNumber yEnd = PreciseNumber("1");
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
            writer.writeResult(result);
            writer.put(m);
            vector<vector<PreciseNumber> > ex = rg.getAlphaVectors();
            writer.put("Average Payoff: " + MyUtil::rationalToString((1-environment.getDiscountRate())*(ex[0][0]/(ONE))));
        }
    }
    writer.put("End gracufully");
    Message::display("End gracufully");
}

void RFSE::checkSpecial(const string &out) {
    Message::display("checkSpecial");
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
        } else if (tmp.find("special",0) != string::npos) {
            variableStart = 4;
            option = SPECIAL;
        } else if (tmp.find("reward",0) != string::npos) {
            variableStart = 4;
            option = REWARD;
        } else if (argc >= 5) {
            filename = argv[4];
            if (tmp.find("automaton",0) != string::npos){
                variableStart = 5;
                option = AUTOMATONS;
            } else if(tmp.find("regular",0) != string::npos) {
                variableStart = 5;
                option = REGULAR;
            }
        }
    }

    // オプションの設定
    boost::regex expr("([a-zA-Z]+)=(.+)");
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
    case REWARD:
        out = prefix + "-reward";
        this->checkReward(out);
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
        Message::display("	[name]=[value]    e.g) p=0.5 q=1/6");
    }
    return 0;
}
