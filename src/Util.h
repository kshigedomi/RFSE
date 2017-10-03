#ifndef UTIL_H_
#define UTIL_H_

/*
 * Util.h
 *
 *  Created on: 2012. 11. 27.
 *      Author: chaerim
 */


#ifndef COMMON_H_
#include "Common.h"
#endif

#include <setjmp.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace MyUtil {

    // For IO
    string numberToRational(const string &num);
    string toString(const int val);
    string rationalToString(const PreciseNumber &val);
    string resultToString(const Result &result);
    int toInteger(const string &val);
    string spaceToString(const string &s, const int size);
    vector<string> splitAtSpace(istream &fin);
    vector<string> splitAtSpace(const string &s);
    vector<string> split(const string &s, const char &c);

    ostream& operator << (ostream& os, const vector<vector<PreciseNumber> > &matrix);
    ostream& operator << (ostream& os, const vector<PreciseNumber> &vec);


    // For next
    bool next(vector<PreciseNumber> &v, const vector<PreciseNumber> &start, const vector<PreciseNumber> &interval, const vector<PreciseNumber> &end);
    bool normalNextMatrix(vector<vector<PreciseNumber> >&matrix, const vector<vector<PreciseNumber> > &start, const PreciseNumber &interval, const vector<vector<PreciseNumber> > &end);
    bool normalNext(vector<PreciseNumber> &v, const vector<PreciseNumber> &start, const PreciseNumber &interval, const vector<PreciseNumber> &end);
    bool increment(vector<int> &vec, const int limit);
    bool increment(vector<PreciseNumber> &vec, const PreciseNumber &from, const PreciseNumber &interval, const PreciseNumber &to);
    bool increment(vector<vector<PreciseNumber> > &matrix, const PreciseNumber &from, const PreciseNumber &interval, const PreciseNumber &to);
    bool slide(vector<int> &vec);

    // For matrix
    vector<vector<PreciseNumber> > getTransposedMatrix(const vector<vector<PreciseNumber> > &matrix);
    vector<vector<PreciseNumber> > getIdentityMatrix(const vector<vector<PreciseNumber> >::size_type size);
    vector<vector<PreciseNumber> > matrixMul(const vector<vector<PreciseNumber> > &m1, const vector<vector<PreciseNumber> > &m2);
    vector<PreciseNumber> matrixVectorProduct(const vector<vector<PreciseNumber> > &matrix, const vector<PreciseNumber> &vec);
    vector<PreciseNumber> vectorMatrixProduct(const vector<PreciseNumber> &vec, const vector<vector<PreciseNumber> > &matrix);
    PreciseNumber innerProduct(const vector<PreciseNumber> &vec1, const vector<PreciseNumber> &vec2);
    bool checkRegularity(const vector<vector<PreciseNumber> > &matrix, const int endTurn);
    bool checkRegularity(const vector<vector<int> > &matrix, const int endTurn);
    vector<PreciseNumber> normalize(const vector<PreciseNumber> &vec);

    // For rational number
    bool equal(const PreciseNumber &, const PreciseNumber &);

    int getIndex(const vector<string> &list, const string &value);

    PreciseNumber pow(const PreciseNumber &x, const int y);
    PreciseNumber sqrt(const PreciseNumber &x);

    bool nonumber(const char &c);
    bool nextWord(istream &is, string &buf);


};

/*
 * 機能: メッセージを出力
 * メモ: 特別な意味はないが，プリントデバックをcout,cerr に，
 *       プログラムとしての出力をこの関数に任せると後が楽
 */
namespace Message {
    template<typename T>
        void display(const T &msg) {
        cout << msg << endl;
    }
    template<typename T>
        void alert(const T &msg) {
        cerr << "* Error : " << msg << endl;
    }
};


#endif /* UTIL_H_ */
