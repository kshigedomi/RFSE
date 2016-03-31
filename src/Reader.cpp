/*
 * Reader.cpp
 *
 *  Created on: 2012. 12. 4.
 *       Author: chaerim
 */

/*
 * This class reads input file and sets environment, payoff, and so on.
 * Even if correation device is not found in input file, it does not return error code.
 */
#include "Reader.h"


Reader::Reader() {
    numPlayers = 0;
}

Reader::Reader(const string &nameDataFile) {
    numPlayers = 0;
    fin.open(nameDataFile.c_str(), ifstream::in);
}

Reader::~Reader() {
    if (fin) {
        fin.close();
    }
}

/*
 * 機能: 一行読み込み改行コードを削除したものを返す
 * メモ: ウインドウズとユニックスで改行コードが違うため使用
 */
ifstream &Reader::readLine(ifstream &fin, string &s) {
    getline(fin,s);
    if (s[s.size()-1] == '\n') {
        s.erase(s.size()-1);
    } else if (s[s.size()-1] == '\r') {
        s.erase(s.size()-1);
    }
    return fin;
}


// /*
//  * 機能: player が何番目のプレイヤか調べる
//  * 引数: player
//  * 返り値: 何番目か
//  */
// int Reader::getIndexOfPlayers(const string &player) {
//    return getIndex(players, player);
// }

void Reader::readTitle(string &title) {
    string buf;
    boost::regex expr("Title *: *(.+)");
    boost::smatch what;
    fin.seekg(0, ios_base::beg);
    while (!fin.eof()) {
        readLine(fin, buf);
        if (boost::regex_match(buf, what, expr)) {
            title = what[1];
            return;
        }
    }
    throw ReaderException("Read Title Error");
}

void Reader::readDiscountRate(Environment &env) {
    string buf;
    boost::regex expr("Discount Rate *: *(.+)");
    boost::smatch what;
    fin.seekg(0, ios_base::beg);
    while (!fin.eof()) {
        readLine(fin, buf);
        if (boost::regex_match(buf, what, expr)) {
            env.setDiscountRate(PreciseNumber(MyUtil::numberToRational(what[1])));
            return;
        }
    }
    throw ReaderException("Read Discount Rate Error");
}

/*
 * 機能: tag で指定されるデータを読み込む
 * 引数: tag，読み込んだデータ data, 初めから読むかどうか startFirst
 */
void Reader::readItem(const string &tag, vector<string> &data, bool startFirst) {
    string buf;
    boost::regex expr(tag + " *: *(.+)");
    boost::smatch what;

    if (startFirst) {
        fin.seekg(0, ios_base::beg);
        while (!fin.eof()) {
            this->readLine(fin, buf);
            if (boost::regex_match(buf, what, expr)) { // tag が見つかった
                data = MyUtil::splitAtSpace(what[1]); // スペースごとに分割して，dataに入れる
                return;
            }
        }
    } else {
        this->readLine(fin, buf);
        if (boost::regex_match(buf, what, expr)) {
            data = MyUtil::splitAtSpace(what[1]); // スペースごとに分割して，dataに入れる
            return;
        }
    }
    throw ReaderException("Reader " + tag + " Error");
}

void Reader::readPlayers(vector<string> &playerNames) {
    this->readItem("Players", playerNames, true);
    numPlayers = playerNames.size();
}

void Reader::readVariables(map<string, PreciseNumber> &variables) {
    vector<string> vars;
    readItem("Variables", vars, true);
    boost::regex expr("(.+)=(.+)");
    boost::smatch what;
    for (vector<string>::size_type i = 0; i < vars.size(); i++) {
        if (boost::regex_match(vars[i], what, expr)) {
            variables[what[1]] = PreciseNumber(MyUtil::numberToRational(what[2]));
        } else {
            throw ReaderException("Read Variables Error");
        }
    }
}

void Reader::readAutomaton(const string &playerName, Automaton &am) {
    string buf;
    boost::regex expr("Automaton +" + playerName + " *");
    boost::smatch what;
    this->fin.seekg(0, ios_base::beg);
    while (!this->fin.eof()) {
        readLine(fin, buf);
        if (boost::regex_match(buf, what, expr)) {
            // States
            vector<string> nameStates;
            readItem("States", nameStates);
            am.setNumberOfStates(nameStates.size());
            am.setNameOfStates(nameStates);
            // Actions
            vector<string> nameActions;
            readItem("Actions", nameActions);
            am.setNumberOfActions(nameActions.size());
            am.setNameOfActions(nameActions);
            // Signals
            vector<string> nameSignals;
            readItem("Signals", nameSignals);
            am.setNumberOfSignals(nameSignals.size());
            am.setNameOfSignals(nameSignals);
            // Action choice
            string buf1, buf2, buf3;
            for (int i = 0; i < am.getNumberOfStates(); i++) {
                fin >> buf1 >> buf2;
                am.setActionChoice(am.getIndexOfStates(buf1), am.getIndexOfActions(buf2));
            }
            // State transition
            for (int i = 0; i < am.getNumberOfStates() * am.getNumberOfSignals(); i++) {
                fin >> buf1 >> buf2 >> buf3;
                am.setStateTransition(am.getIndexOfStates(buf1), am.getIndexOfSignals(buf2), am.getIndexOfStates(buf3));
            }
            break; 
        }
    }
}

/*
 * 機能: シグナル分布を読み込む
 * 引数: 読み込む環境クラス env, pls
 */
void Reader::readSignalDistribution(Environment &env, const vector<Automaton>& ms) {
    string buf;
    this->fin.seekg(0, ios_base::beg);
    while (!this->fin.eof()) {
        this->readLine(fin, buf);
        if (buf == "Signal Distribution") {
            for (int i = 0; i < env.getNumberOfActionProfiles(); i++) {
                this->readLine(fin, buf);
                boost::regex expr("(\\S+) *: *(.*)");
                boost::smatch what;
                if (boost::regex_match(buf, what, expr)) {
                    // どの行動に対するものか
                    const vector<string> actionStr = MyUtil::split(what[1], ',');
                    ActionProfile ap;
                    for (int pl = 0; pl < numPlayers; ++pl)
                        ap[pl] = ms[pl].getIndexOfActions(actionStr[pl]);
                    // 確率をセット
                    vector<string>::size_type sg = 0;
                    // 最初の行
                    const vector<string> distributions = MyUtil::splitAtSpace(what[2]);
                    for (; sg < distributions.size(); ++sg) {
                        env.setRawSignalDistribution(ap.getIndex(), sg, MyUtil::numberToRational(distributions[sg]));
                    }
                    // 次の行
                    for (; sg < env.getNumberOfSignalProfiles(); ++sg) { // 型違い warning は仕方ない
                        string distribution;
                        if (!MyUtil::nextWord(fin, distribution)) {// 次の確率読み込み
                            throw ReaderException("Read Signal distribution Error: " + sg);
                        }
                        env.setRawSignalDistribution(ap.getIndex(), sg, MyUtil::numberToRational(distribution));
                    }
                }
            }
            return;
        }
    }
    throw ReaderException("Read Signal distribution Error");
}

/*
 * 機能: エクセルが出力したシグナル分布を読み込む
 * 引数: 読み込む環境クラス env, pls
 */
void Reader::readExcelSignalDistribution(Environment &env, const vector<Automaton>& ms) {
    string buf;
    fin.seekg(0, ios_base::beg);
    while (!fin.eof()) {
        readLine(fin, buf);
        if (buf == "Signal Distribution") {
            for (int i = 0; i < env.getNumberOfActionProfiles()*env.getNumberOfSignalProfiles(); i++) {
                readLine(fin, buf);
                boost::regex expr("(\\S+) *(\\S+) *(\\S+)");
                boost::smatch what;
                if (boost::regex_match(buf, what, expr)) {
                    // どの行動とシグナルに対するものか
                    const vector<string> actionStr = MyUtil::split(what[1], ',');
                    const vector<string> signalStr = MyUtil::split(what[2], ',');
                    string stbuf;
                    ActionProfile ap;
                    for (int pl = 0; pl < numPlayers; ++pl)
                        ap[pl] = ms[pl].getIndexOfActions(actionStr[pl]);
                    SignalProfile sgp;
                    for (int pl = 0; pl < numPlayers; ++pl)
                        sgp[pl] = ms[pl].getIndexOfSignals(signalStr[pl]);
                    env.setRawSignalDistribution(ap.getIndex(), sgp.getIndex(), MyUtil::numberToRational(what[3]));
                }
            }
            return;
        }
    }
    throw ReaderException("Read Excel Signal Distribution Error");
}


void Reader::readPayoffMatrix(Payoff &po, const vector<Automaton> &ms) {
    string buf;
    this->fin.seekg(0, ios_base::beg);
    while (!this->fin.eof()) {
        this->readLine(fin, buf);
        if (buf == "Payoff Matrix") {
            boost::regex expr("(\\S+) *: *(.*)");
            boost::smatch what;
            for (int ac = 0; ac < po.getNumberOfActionProfiles(); ++ac) {
                this->readLine(fin, buf);
                if (boost::regex_match(buf, what, expr)) {
                    // どの行動か読み取る
                    const vector<string> actionStr = MyUtil::split(what[1], ',');
                    ActionProfile ap;
                    for (vector<Automaton>::size_type pl = 0; pl < ms.size(); ++pl) {
                        ap[pl] = ms[pl].getIndexOfActions(actionStr[pl]);
                    }
                    // 利得を読み取る
                    const vector<string> payoff_s = MyUtil::splitAtSpace(what[2]);
                    for (vector<Automaton>::size_type pl = 0; pl < ms.size(); ++pl) {
                        po.setRawPayoff(pl, ap.getIndex(), MyUtil::numberToRational(payoff_s[pl]));
                    }
                }
            }
            return;
        }
    }
    throw ReaderException("Read Payoff Error");
}

void Reader::read(string &title, Environment &env, Payoff &po, vector<Automaton>& ms, map<string, PreciseNumber>& var) {
    if (!fin) {
        throw ReaderException("File is not found");
    }      
    this->readTitle(title);
    this->readDiscountRate(env);
    this->readVariables(var);
    vector<string> playerNames;
    this->readPlayers(playerNames);
    vector<int> numStatesOfPlayer(this->numPlayers);
    vector<int> numActionsOfPlayer(this->numPlayers);
    vector<int> numSignalsOfPlayer(this->numPlayers);
    vector<vector<string> > nameStatesOfPlayer(this->numPlayers);
    vector<vector<string> > nameActionsOfPlayer(this->numPlayers);
    vector<vector<string> > nameSignalsOfPlayer(this->numPlayers);
    ms.resize(this->numPlayers);
   
    for (int pl = 0; pl < numPlayers; pl++) {
        readAutomaton(playerNames[pl], ms[pl]);
        numStatesOfPlayer[pl] = ms[pl].getNumberOfStates();
        numActionsOfPlayer[pl] = ms[pl].getNumberOfActions();
        numSignalsOfPlayer[pl] = ms[pl].getNumberOfSignals();
        nameStatesOfPlayer[pl] = ms[pl].getNameOfStates();
        nameActionsOfPlayer[pl] = ms[pl].getNameOfActions();
        nameSignalsOfPlayer[pl] = ms[pl].getNameOfSignals();
    }

    StateProfile::numStatesOfPlayer = numStatesOfPlayer;
    ActionProfile::numActionsOfPlayer = numActionsOfPlayer;
    SignalProfile::numSignalsOfPlayer = numSignalsOfPlayer;
    copy(numStatesOfPlayer.begin() + 1, numStatesOfPlayer.end(), back_inserter(OpponentStateProfile::numStatesOfOpponentPlayer));
    copy(numActionsOfPlayer.begin() + 1, numActionsOfPlayer.end(), back_inserter(OpponentActionProfile::numActionsOfOpponentPlayer));
    copy(numSignalsOfPlayer.begin() + 1, numSignalsOfPlayer.end(), back_inserter(OpponentSignalProfile::numSignalsOfOpponentPlayer));
    StateProfile::nameStatesOfPlayer = nameStatesOfPlayer;
    ActionProfile::nameActionsOfPlayer = nameActionsOfPlayer;
    SignalProfile::nameSignalsOfPlayer = nameSignalsOfPlayer;
    copy(nameStatesOfPlayer.begin() + 1, nameStatesOfPlayer.end(), back_inserter(OpponentStateProfile::nameStatesOfOpponentPlayer));
    copy(nameActionsOfPlayer.begin() + 1, nameActionsOfPlayer.end(), back_inserter(OpponentActionProfile::nameActionsOfOpponentPlayer));
    copy(nameSignalsOfPlayer.begin() + 1, nameSignalsOfPlayer.end(), back_inserter(OpponentSignalProfile::nameSignalsOfOpponentPlayer));

    env.configure(numActionsOfPlayer, numSignalsOfPlayer);
    if (title.find("EXCEL", 0) == string::npos)
        this->readSignalDistribution(env, ms);
    else
        this->readExcelSignalDistribution(env, ms);

    po.configure(numPlayers, numActionsOfPlayer);
    readPayoffMatrix(po, ms);
}

