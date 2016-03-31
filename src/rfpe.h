#ifndef SEMINAL_H_
#define SEMINAL_H_

/*
 * seminal.h
 *
 *  Created on: 2012. 12. 26.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef AUTOMATON_H_
#include "Automaton.h"
#endif

#ifndef BELIEF_H_
#include "Belief.h"
#endif

#ifndef ENVIRONMENT_H_
#include "Environment.h"
#endif

#ifndef PAYOFF_H_
#include "Payoff.h"
#endif

#ifndef LOADER_H_
#include "Loader.h"
#endif

#ifndef REPEATEDGAME_H_
#include "RepeatedGame.h"
#endif

#ifndef REWARDEQUATION_H_
#include "RewardEquation.h"
#endif

#ifndef UTIL_H_
#include "Util.h"
#endif

#ifndef READER_H_
#include "Reader.h"
#endif

#ifndef WRITER_H_
#include "Writer.h"
#endif

enum Option {
    NONE,AUTOMATONS,PARA,
    REGULAR,SPECIAL,REWARD,
};

class RFSE {

 private:
    RepeatedGame rg;
    Payoff payoff;
    Environment environment;
    vector<Automaton> automatons;
    map<string,PreciseNumber> variables;

    void setAllAutomatons(const Automaton &m);

    ///<summary>
    ///均衡判定前の下準備を行う．
    ///</summary>
    ///<remark><code>setSignalDistributionFromRawSignalDistribution</code>が呼ばれている場合のみ正しく動作する．</remark>
    void setRepeatedGame();

    ///<summary>
    ///均衡判定前の下準備を行う．
    ///具体的には変数をシグナル分布と利得表にセットする．
    ///</summary>
    void setVariablesToEnvironmentAndPayoff();

    ///<summary>
    ///平均利得を計算する．
    ///x軸をコード中で指定された値にして動かす．"[x軸の値]\t[平均利得]"がファイルに記入される．
    ///なお平均利得は均衡でないときは，-1とする．
    ///</summary>
    ///<param name=out>出力ファイル名</param>
    void checkReward(const string &out);
  
    ///<summary>
    ///均衡になるパラメータの範囲を計算する．
    ///x軸，y軸をコード中で指定された値にして動かす．"[x軸の値]\t[y軸の始まり]\t[y軸の終わり]"がファイルに記入される．
    ///</summary>
    ///<param name=out>出力ファイル名</param>
    ///<remark>
    ///y軸の値を上げていき均衡にならなくなったら，記入してx軸を動かす．
    ///従って，均衡になる範囲が凸でないなら，正しく動作しない．
    ///</remark>
    void checkParameter(const string &out);

    ///<summary>
    ///regularなオートマトンを<paramref name=in>から受け取り均衡判定する．
    ///</summary>
    ///<param name=in>対象となるファイル名．./MakeAutomatonで作られた．</param>
    ///<param name=out>出力ファイル名</param>
    ///<remark>すべてのプレイヤが同じオートマトンを用いる．</remark>
    void checkRegularAutomatons(const string &in, const string &out);

    ///<summary>
    ///特に使用意図を指定しない関数．
    ///</summary>
    ///<param name=out>出力ファイル名</param>
    void checkSpecial(const string &out);

    ///<summary>
    ///<paramref name=in>の中のオートマトンに対し均衡判定を行う．
    ///</summary>
    ///<param name=in>対象となるファイル名．<code>checkRegularAutomaton</code>を用いて作られたことを仮定する．</param>
    ///<param name=out>出力ファイル名</param>
    ///<remark>コードを変更することで，読み込み対象となるオートマトンに変更を加える．詳しくは<code>Loader</code>クラスを見よ．</remark>
    void checkAutomatons(const string &in, const string &out);

    ///<summary>
    ///均衡判定を行う．いろいろな情報を出力する．
    ///</summary>
    ///<param name=out>出力ファイル名</param>
    void checkAllInformations(const string &out);

 public:
    ///<summary>
    ///このメソッドは次のことを行う．
    ///1.<paramref name=datafile>から各種データを受け取る．
    ///2.<paramref name=argv>からオプションを受け取る．
    ///3.オプションに従いrfseの判定を行い，結果を<paramref name=prefix>とオプションで指定されたファイルに書き込む．
    ///もし何らかのエラーが生じた際は，エラーメッセージを出した後実行は停止する．
    ///</summary>
    ///<param name=datafile>データファイルの名前．</param>
    ///<param name=prefix>書き込みファイルの名前．この文字列にさらにオプションで指定された名前が付与されて書き込みファイルの名前となる．</param>
    ///<param name=argc><paramref name=argv>の個数</param>
    ///<param name=argv>
    ///実行した際の文字列．
    ///つまり，rfpe [datafile] [output-prefix] {[variables], ...}
    ///もしくはrfpe [datafile] [output-prefix] [option] {[input-data] [varables], ...}
    ///</param>
    ///<remark>最初に呼ばれることを想定していて，このメソッドからクラス内の各種メソッドを呼び出す．</remark>
    void checkRFSE(const string &datafile, const string &prefix, int argc, char **argv);
};

#endif /* SEMINAL_H_ */
