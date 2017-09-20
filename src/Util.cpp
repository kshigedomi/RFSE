/*
 * Util.cpp
 *
 *  Created on: 2012. 11. 27.
 *      Author: chaerim
 */

/*
 * This class has some useful functions.
 * In particular, the methods named convert-- is useful.
 * It is used by some core functions to require extreme points from inequalities, or the opposite way can do.
 */

#include "Util.h"

namespace MyUtil {

    /*
     * 出力系
     */
    ostream& operator << (ostream& os, const vector<vector<PreciseNumber> > &matrix) {
        for (vector<vector<PreciseNumber> >::size_type i = 0; i < matrix.size(); ++i) {
            os << matrix[i];
        }
        os << "\n";
        return os;
    }

    ostream& operator << (ostream& os, const vector<PreciseNumber> &vec) {
        for (vector<PreciseNumber>::size_type i = 0; i < vec.size(); ++i) {
            if (i == 0)
                os << rationalToString(vec[i]);
            else
                os << " " << rationalToString(vec[i]);
        }
        os << "\n";
        return os;
    }

    /*
     * 機能: 転置をした行列を生成
     * 引数: 転置前の行列
     * 返り値: 転置後の行列
     */
    vector<vector<PreciseNumber> > getTransposedMatrix(const vector<vector<PreciseNumber> > &matrix) {
        vector<vector<PreciseNumber> >::size_type row_size = matrix.size();
        vector<vector<PreciseNumber> >::size_type col_size = matrix[0].size();
        vector<vector<PreciseNumber> > res(row_size, vector<PreciseNumber> (col_size, ZERO));
        for (vector<vector<PreciseNumber> >::size_type i = 0; i < row_size; ++i) {
            for (vector<PreciseNumber>::size_type j = 0; j < col_size; ++j) {
                res[i][j] = matrix[j][i];
            }
        }
        return res;
    }

    /*
     * 機能: 単位行列を生成
     * 引数: 行列のサイズ
     * 返り値: 単位行列
     */
    vector<vector<PreciseNumber> > getIdentityMatrix(const vector<vector<PreciseNumber> >::size_type size) {
        vector<vector<PreciseNumber> > res(size, vector<PreciseNumber> (size, ZERO));
        for (vector<vector<PreciseNumber> >::size_type i = 0; i < size; ++i) {
            res[i][i] = ONE;
        }
        return res;
    }

    /*
     * 機能: 行列の乗算を行う
     * 引数: 対象となる行列
     * 返り値: 演算結果
     */
    vector<vector<PreciseNumber> > matrixMul(const vector<vector<PreciseNumber> > &m1, const vector<vector<PreciseNumber> > &m2) {
        const vector<vector<PreciseNumber> >::size_type row_size = m1.size();
        const vector<PreciseNumber>::size_type col_size = m2[0].size();
        vector<vector<PreciseNumber> > res(row_size, vector<PreciseNumber> (col_size, ZERO));
        for (vector<vector<PreciseNumber> >::size_type i = 0; i < row_size; ++i) {
            for (vector<PreciseNumber>::size_type j = 0; j < col_size; ++j) {
                for (vector<PreciseNumber>::size_type k = 0; k < m1[0].size(); ++k) {
                    res[i][j] += m1[i][k]*m2[k][j];
                }
            }
        }
        return res;
    }

    /*
     * 機能: ベクトルの内積を求める
     * 引数: 対象となるベクトル
     * 返り値: 内積結果
     */
    PreciseNumber innerProduct(const vector<PreciseNumber> &vec1, const vector<PreciseNumber> &vec2) {
        return inner_product(vec1.begin(), vec1.end(), vec2.begin(), ZERO);
    }

    /*
     * 機能: ベクトルを正規化（要素の和が1にする）
     * 引数: 対象となるベクトル
     * 返り値: 正規化したベクトル
     */
    vector<PreciseNumber> normalize(const vector<PreciseNumber> &vec) {
        PreciseNumber denom = accumulate(vec.begin(), vec.end(), ZERO);
        vector<PreciseNumber> res;
        transform(vec.begin(), vec.end(), back_inserter(res), bind2nd(divides<PreciseNumber>(), denom));
        return res;
    }

    /*
     * 機能: 行列とベクトルの乗算
     * 引数: 対象となる行列とベクトル
     * 返り値: 演算結果のベクトル
     */
    vector<PreciseNumber> matrixVectorProduct(const vector<vector<PreciseNumber> > &matrix, const vector<PreciseNumber> &vec) {
        const vector<vector<PreciseNumber> >::size_type row_size = matrix.size();
        vector<PreciseNumber> res(row_size, ZERO);
        for (vector<vector<PreciseNumber> >::size_type i = 0; i < row_size; ++i) {
            for (vector<PreciseNumber>::size_type j = 0; j < vec.size(); ++j)
                res[i] += matrix[i][j] * vec[j];
        }
        return res;
    }

    /*
     * 機能: 行列とベクトルの乗算
     * 引数: 対象となる行列とベクトル
     * 返り値: 演算結果のベクトル
     */
    vector<PreciseNumber> vectorMatrixProduct(const vector<PreciseNumber> &vec, const vector<vector<PreciseNumber> > &matrix) {
        const vector<vector<PreciseNumber> >::size_type row_size = matrix.size();
        const vector<PreciseNumber>::size_type col_size = matrix[0].size();
        vector<PreciseNumber> res(row_size, ZERO);
        for (vector<vector<PreciseNumber> >::size_type i = 0; i < row_size; ++i) {
            for (vector<PreciseNumber>::size_type k = 0; k < col_size; ++k) {
                res[i] += vec[k] * matrix[k][i];
            }
        }
        return res;
    }

    /*
     * 機能: 数字の文字列を有理数化
     * 例1: 0.1 -> 1/10
     * 例2: -0.1 -> -1/10
     * 引数: 数字の文字列
     * 返り値: 有理数化した文字列
     */
    string numberToRational(const string &number) {
        string::size_type pos = number.find(".");
        string res = "";

        if (pos == string::npos) {
            res = number; // 数字が少数でない
        }
        else {
            bool negative = false;
            if (number[0] == '-') {
                negative = true;
            }
            string part1 = number.substr((negative ? 1 : 0), pos - (negative ? 1 : 0));
            string part2 = number.substr(pos + 1);
            if (part1.length() == 1 && part1[0] == '0') part1 = "";
            // 分母
            string d = "1";
            for (string::size_type i = 0; i < part2.length(); i++) d += "0";
            // 分子
            string n = "", nn = part1 + part2;
            bool ck = false;
            for (string::size_type i = 0; i < nn.length(); i++) {
                if (nn[i] != '0') ck = true;
                if (ck) n += nn[i]; //0以降をnに保存
            }
            // part1 + part2 = "0...0" であった場合
            if (!ck) n += "0";
            res = (negative ? "-" : "") + n + "/" + d;
        }
        return res;
    }

    /*
     * 機能: v をインクリメントして次の v を生成，ただし v[0] はデクリメント
     * 引数: 対象となるベクトル v, 始まり，途中の差，終わり
     * 返り値: インクリメントできたら true
     */
    bool next(vector<PreciseNumber> &v, const vector<PreciseNumber> &start, const vector<PreciseNumber> &interval, const vector<PreciseNumber> &end) {
        const vector<PreciseNumber>::size_type size = v.size();
        for (int i = size - 1; i >= 0; --i) {
            if (i != 0) {
                v[i] += interval[i];
                if (v[i] <= end[i])
                    return true;
            } else {
                v[i] -= interval[i];
                if (v[i] >= end[i])
                    return true;
            }
            v[i] = start[i];
        }
        return false;
    }

    /*
     * 機能: matrix をインクリメントして次の matrix を生成
     * 引数: 対象となる行列 matrix, 始まり，途中の差，終わり
     * 返り値: インクリメントできたら true
     */
    bool normalNextMatrix(vector<vector<PreciseNumber> >&matrix, const vector<vector<PreciseNumber> > &start, const PreciseNumber &interval, const vector<vector<PreciseNumber> > &end) {
        vector<vector<PreciseNumber> >::size_type size = matrix.size();
        for (vector<vector<PreciseNumber> >::size_type i = 0; i < size; ++i) {
            if (normalNext(matrix[i], start[i], interval, end[i]))
                return true;
        }
        return false;
    }

    /*
     * 機能: v をインクリメントして次の v を生成
     * 引数: 対象となるベクトル v, 始まり，途中の差，終わり
     * 返り値: インクリメントできたら true
     */
    bool normalNext(vector<PreciseNumber> &v, const vector<PreciseNumber> &start, const PreciseNumber &interval, const vector<PreciseNumber> &end) {
        vector<PreciseNumber>::size_type size = v.size();
        for (vector<PreciseNumber>::size_type i = 0; i < size; ++i) {
            v[i] += interval;
            if (v[i] <= end[i])
                return true;
            v[i] = start[i];
        }
        return false;
    }

    /*
     * 機能: matrix をインクリメントして次の matrix を生成
     * 引数: 対象となる行列 matrix, 始まり，途中の差，終わり
     * 返り値: インクリメントできたら true
     */
    bool increment(vector<vector<PreciseNumber> > &matrix, const PreciseNumber &from, const PreciseNumber &interval, const PreciseNumber &to) {
        vector<vector<PreciseNumber> >::size_type row_size = matrix.size();
        for (vector<vector<PreciseNumber> >::size_type i = 0; i < row_size; ++i) {
            if (increment(matrix[i], from, interval, to))
                return true;
        }
        return false;
    }

    /*
     * 機能: v をインクリメントして次の v を生成
     * 引数: 対象となるベクトル v, 始まり，途中の差，終わり
     * 返り値: インクリメントできたら true
     */
    bool increment(vector<PreciseNumber> &vec, const PreciseNumber &from, const PreciseNumber &interval, const PreciseNumber &to) {
        const vector<PreciseNumber>::size_type size = vec.size();
        for (vector<PreciseNumber>::size_type i = 0; i < size; ++i) {
            vec[i] += interval;
            if (vec[i] <= to)
                return true;
            else
                vec[i] = from;
        }
        return false;
    }

    /*
     * 機能: v をインクリメントして次の v を生成
     * 引数: 対象となるベクトル v
     * 返り値: インクリメントできたら true
     */
    bool increment(vector<int> &vec, const int limit) {
        for (vector<int>::size_type i = 0; i < vec.size(); ++i) {
            ++vec[i];
            if (vec[i] < limit)
                return true;
            else
                vec[i] = 0;
        }
        return false;
    }

    /*
     * 機能: バイナリのベクトル vec の1をスライドさせる
     * 例1: [1,0,1,1,0] -> [1,0,1,0,1]
     * 例2: [1,0,1,0,1] -> [1,0,0,1,1]
     * 例3: [0,0,1,1,1] -> false
     * 引数: 対象となるベクトル vec
     * 返り値: スライドできたら true
     */
    bool slide(vector<int> &vec) {
        int clearBranches = 0, i = vec.size() - 1;
        while (i >= 0 && vec[i]) {
            vec[i] = 0;
            ++clearBranches;
            --i;
        }
        if (i < 0) {// ベクトルの中身がすべて1
            return false;
        }
        while (i >= 0 && !vec[i])
            --i;
        if (i < 0) { // すべての1が後ろに寄っている（例3）
            return false;
        }
        //slide
        vec[i] = 0;
        ++clearBranches;
        ++i;
        while (clearBranches > 0) {
            vec[i] = 1;
            --clearBranches;
            ++i;
        }
        return true;
    }

    /*
     * 機能: 有理数を文字列に変換
     * 引数: 対象となる有理数
     * 返り値: 文字列
     */
    string rationalToString(const PreciseNumber &val) {
        int len = 10;
        string fm = "%." + toString(len) + "Ff";
        char buf[255];

        gmp_sprintf(buf, fm.c_str(), mpf_class(val).get_mpf_t());
        stringstream s;
        s << buf;
        return s.str();
    }

    /*
     * 機能: 整数文字列を整数に変換
     * 引数: 対象となる文字列
     * 返り値: 整数
     */
    int toInteger(const string &val) {
        int res;
        istringstream s(val);
        s >> res;
        return res;
    }

    /*
     * 機能: 文字列にスペースを出力
     * 引数: 対象となる文字列とスペースのサイズ
     * 返り値: 文字列
     */
    string spaceToString(const string &s, const int size) {
        ostringstream ss;
        ss << setw(size) << s;
        return ss.str();;
    }

    /*
     * 機能: 1行読み込んで，1文字のスペースで分ける
     * 引数: 対象となる入力ストリーム
     * 返り値: 分けた文字列
     */
    vector<string> splitAtSpace(istream &fin) {
        vector<string> res;
        string buf;
        getline(fin, buf);
        boost::split(res, buf, boost::is_space());
        return res;
    }

    /*
     * 機能: 文字列をスペースで分ける．空文字列は無視．
     * 引数: 対象となる文字列
     * 返り値: 分けた文字列
     */
    vector<string> splitAtSpace(const string &s) {
        vector<string> res;
        istringstream iss(s);
        string tmp;
        while (!iss.eof()) {
            iss >> tmp;
            if (tmp.size() != 0)
                res.push_back(tmp);
        }
        return res;
    }

    /*
     * 機能: 文字列をデリミタで分ける
     * 引数: 文字列 s, デリミタ c
     * 返り値: 分けた文字列
     */
    vector<string> split(const string &s, const char &c) {
        vector<string> res;
        boost::algorithm::split(res, s, bind2nd(equal_to<char>(), c));
        return res;
    }

    /*
     * 機能: 結果を文字列に変換
     * 引数: 結果
     * 返り値: 文字列
     */
    string resultToString(const Result &result) {
        switch (result) {
        case SUCCESS:
            return "SUCCESS";
        case NOTEXIST:
            return "NOTEXIST";
        case FAILURE:
            return "FAILURE";
        case NOTSURE:
            return "NOTSURE";
        case TIMEISOVER:
            return "TIMEISOVER";
        case PREPROFAILURE:
            return "PREPROFAILURE";
        case GLOBAL:
            return "GLOBAL";
        case NOTCONSISTENT:
            return "NOTCONSISTENT";
        case NOTFIXED:
            return "NOTFIXED";
        default:
            cerr << "Error : resultToString in Util.cpp" << endl;
            assert(true);
        }
        return "";
    }

    /*
     * 機能: 行列が regular か確かめる
     * 引数: 行列 matrix, 何回繰り返したもので確かめるか endTurn
     * 返り値: 結果
     */
    bool checkRegularity(const vector<vector<PreciseNumber> > &matrix, const int endTurn) {
        const int numStates = matrix.size();
        vector<vector<int> > tmp(numStates, vector<int>(numStates, false));
        // PreciseNumber をかけ続けると時間がかかるので int型に変える
        for (int row = 0; row < numStates; ++row) {
            for (int col = 0; col < numStates; ++col) {
                if (matrix[row][col] > 0)
                    tmp[row][col] = true;
            }
        }
        return MyUtil::checkRegularity(tmp, endTurn);
    }

    /*
     * 機能: 行列が regular か確かめる
     * 引数: 行列 matrix, 何回繰り返したもので確かめるか endTurn
     * 返り値: 結果
     */
    bool checkRegularity(const vector<vector<int> > &matrix, const int endTurn) {
        vector<vector<int> > tmp = matrix;
        const int numOfStates = matrix.size();
        // tmp = matrix^endTurn になるまでかける
        for (int turn = 0; turn < endTurn; ++turn) {
            vector<vector<int> > next(numOfStates, vector<int>(numOfStates, 0));
            for (int i = 0; i < numOfStates; ++i) {
                for (int j = 0; j < numOfStates; ++j) {
                    for (int k = 0; k < numOfStates; ++k)
                        next[i][j] += (tmp[i][k] && tmp[k][j]); // int型をかけ続けるとオーバーフローするので，and演算で代用する
                }
            }
            tmp = next;
        }
        for (int i = 0; i < numOfStates; ++i) {
            for (int j = 0; j < numOfStates; ++j) {
                if (tmp[i][j] == 0)
                    return false;
            }
        }
        return true;
    }

    /*
     * 機能: PreciseNumber が誤差を許容して同じか判定する
     * 引数: 対象となる PreciseNumber
     * 返り値: 結果
     */
    bool equal(const PreciseNumber &a, const PreciseNumber &b) {
        return abs(a - b) < EPS2;
    }

    /*
     * 機能: 整数を文字列にする
     * 引数: 整数
     * 返り値: 文字列
     */
    string toString(const int val) {
        stringstream s;
        s << val;
        return s.str();
    }

    /*
     * 機能: list の何番目に value があるか調べる
     * 引数: list と value
     * 返り値: 何番目か
     * メモ: 存在しなかったら，list.size() を返す
     */
    int getIndex(const vector<string> &list, const string &value) {
        return int(find(list.begin(), list.end(), value) - list.begin());
    }

    /*
     * 機能: x の y 乗を調べる
     * 引数: x,y
     * 返り値: x^y
     */
    PreciseNumber pow(const PreciseNumber &x, const int y) {
        PreciseNumber res = ONE;
        for (int i = 0; i < y; ++i) {
            res *= x;
        }
        return res;
    }

    /*
     * 機能: 数字や変数（文字）でないか判定
     * 引数: 判定する文字
     * 返り値: 判定結果
     */
    bool nonumber(const char &c) {
        return !(isdigit(c) || isalpha(c));
    }

    /*
     * 機能: 空白を読み飛ばして次の言葉を返す
     * 返り値: 判定結果
     */
    bool nextWord(istream &is, string &buf) {
        buf = ""; // buf clear
        if (is.eof())
            return false;
        char c;
        // 空白文字の読み飛ばし
        do {
            is.get(c);
        } while (isspace(c));
        // 読み込み
        do {
            buf += c;
            is.get(c);
        } while (!isspace(c) && !is.eof());
        return true;
    }

    /*
     * 機能: タイムアウトを設定してコマンドを実行
     * 引数: コマンド, 制限時間 (sec)
     * 返り値  -1: タイムアウト -2: タイムアウトにならなかったがコマンドが異常終了
     *        0以上: 正常終了
     */
    int systemWithTimeout(const char *cmd, int timeout){
      int rtn, status, exit_code;
      static int child_pid;
      char *cmd_for_execv[] = {"/bin/bash", "-c", NULL, NULL};
      cmd_for_execv[2] = (char *) cmd;

      if(sigsetjmp(MyUtil::env, 1)) {
        /* タイムアウト */
        alarm(0);
        signal(SIGALRM, SIG_DFL);
        if(child_pid > 0) {
          rtn = kill(child_pid, SIGTERM);
          fprintf(stderr, "KILLED by TIMEOUT\n");
        }
        return -1;
      }

      child_pid = fork();
      switch(child_pid){
        case -1:
          fprintf(stderr, "ERROR! fork() failed\n");
          return -2;
        case 0:
          /* 子プロセス */
          rtn = execv(cmd_for_execv[0], cmd_for_execv);
          exit(99);
          break;
        default:
          /* 親プロセス */
          /* アラームとシグナルハンドラをセット */
          alarm(timeout);
          signal(SIGALRM, system_timeout);

          /* 子プロセスの終了を待つ */
          child_pid = waitpid(child_pid, &status, WUNTRACED);
          if(WIFEXITED(status)){
            // 正常に終了
            exit_code = WEXITSTATUS(status);
          }else{
            // 異常終了
            exit_code = -2;
          }
      }

      /* セットしたアラームとシグナルハンドラをクリア */
      alarm(0);
      signal(SIGALRM, SIG_DFL);
      return exit_code;

    }

    /*
     * 機能: タイムアウト時に呼ばれるシグナルハンドラ
     *       systemWithTimeout() のみから呼ばれる
     */
    void system_timeout(int sig){
      siglongjmp(MyUtil::env, 1);
    }

} // end namespace MyUtil
