/*
 * Belief.cpp
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

/*
 * This class has belief. It represent possibility about what opponent player is in opponent states.
 * It also has expected payoff when the player has this belief.
 * It also has a method check whether belief is validity.
 */

#include "Belief.h"

Belief::Belief() {
	dimension = 0;
}

Belief::Belief(const int dimension) {
	configure(dimension);
}

Belief::Belief(const int dimension, const PreciseNumber &value) {
   configure(dimension);
   fill(belief.begin(), belief.end(), value);
}

Belief::Belief(const vector<PreciseNumber> &vec) {
   configure(vec.size());
   copy(vec.begin(), vec.end(), belief.begin());
   
}
      

Belief::~Belief() {

}

void Belief::configure(const int dimension) {
	this->dimension = dimension;
	belief.resize(dimension);
}

/*
 * 機能: その信念の次元を取得
 * 返り値: 次元
 */
int Belief::getDimension() const{
	return dimension;
}

/*
 * 機能: 正しい信念かを誤差を許して判定
 * 返り値: true or false
 */
bool Belief::checkValidity() const
{
	PreciseNumber sum = ZERO;
	for (int i = 0; i < dimension; i++) {
		if ((belief[i] < ZERO && !MyUtil::equal(belief[i], ZERO)) || (belief[i] > ONE && !MyUtil::equal(belief[i], ONE))) {
			return false;
		}
		sum += belief[i];
	}
	if (!MyUtil::equal(sum, ONE)) return false;
	return true;
}

/*
 * 機能: 信念を取得
 * 引数: 受け取りたいのはどの index の信念か
 * 返り値: 信念
 */
PreciseNumber Belief::get(int index) const{
	return belief[index];
}

/*
 * 機能: 信念を取得
 * 返り値: 信念
 */
vector<PreciseNumber> Belief::getVector() const {
   return belief;
}

/*
 * 機能: 文字列化
 * 返り値: 文字列
 */
string Belief::toString() const{
   string res;
   res += "(" + MyUtil::rationalToString(belief[0]);
   for (int i = 1; i < dimension; ++i)
      res += ", " + MyUtil::rationalToString(belief[i]);
   res += ")";
   return res;
}

/*
 * 機能: 信念をセット
 * 引数: どの index を value にセットするか
 */
void Belief::set(const int index, const PreciseNumber& value) {
	if (abs(value) == 0) {
        belief[index] = ZERO;
    } else {
        belief[index] = value;
    }
}

/*
 * 機能: 大きい小さい系
 * メモ: ソート用
 */
bool operator < (const Belief& lhs, const Belief& rhs) {
	for (int i = 0; i < lhs.getDimension(); i++) {
		if (lhs.get(i) == rhs.get(i)) {
			continue;
		}
		else if (lhs.get(i) < rhs.get(i)) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

bool operator > (const Belief& lhs, const Belief& rhs) {
	for (int i = 0; i < lhs.getDimension(); i++) {
		if (lhs.get(i) == rhs.get(i)) {
			continue;
		}
		else if (lhs.get(i) > rhs.get(i)) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

/*
 * 機能: 誤差許容で同じ信念か調べる
 * 返り値: ture or false
 */
bool operator == (const Belief& lhs, const Belief& rhs) {
	if (lhs.getDimension() != rhs.getDimension()) return false;
	for (int i = 0; i < rhs.getDimension(); i++) {
		if (!MyUtil::equal(lhs.get(i), rhs.get(i))) return false;
	}
	return true;
}


bool operator != (const Belief& lhs, const Belief& rhs) {
	if (lhs.getDimension() != rhs.getDimension()) return true;
	for (int i = 0; i < rhs.getDimension(); i++) {
		if (!MyUtil::equal(lhs.get(i), rhs.get(i))) return true;
	}
	return false;
}

/*
 * 機能: 信念の引き算
 * メモ: 何に使うかわからん
 */
PreciseNumber operator - (const Belief& lhs, const Belief& rhs) {
	PreciseNumber res;
	for (int i = 0; i < rhs.getDimension(); i++) {
		res += abs(lhs.get(i) - rhs.get(i));
	}
	return res;
}

/*
 * 機能: 信念と alphaVector から,その信念における利得を算出
 * 引数: alphaVector と 信念
 * 返り値: 利得
 */
PreciseNumber getRewardFromAlphaVectorAndBelief(const vector<PreciseNumber> &alphaVector, const Belief &b){
   PreciseNumber res = 0;
   for (vector<PreciseNumber>::size_type i = 0; i < alphaVector.size(); ++i)
      res += b.get(i) * alphaVector[i];
   return res;
}

/*
 * 機能: 出力
 */
ostream& operator<<(ostream& os, const Belief& b) {
   return os << b.toString();
}

/*
 * 機能: その信念において,期待値最大の ex を探す
 * 引数: 信念 b と alphaVector の集合 ex
 * 返り値: 期待値最大の ex のインデックスとその利得
 */
pair<int, PreciseNumber> getMaxIndexAndReward(const Belief &b, const vector<vector<PreciseNumber> > &ex) {
    PreciseNumber maxReward = -INF;
    int maxIndex = 0;
    typedef vector<PreciseNumber>::size_type size_type;
    const size_type size = ex.size();
    for (size_type i = 0; i < size; ++i) {
        const PreciseNumber reward = getRewardFromAlphaVectorAndBelief(ex[i], b); // この信念における利得を計算
        if (reward > maxReward) {
            maxReward = reward;
            maxIndex = i;
        }
   }
    return make_pair(maxIndex, maxReward);
}

/*
 * 機能: その信念において,期待値最大の ex を探す
 * 引数: 信念 b と alphaVector の集合 ex
 * 返り値: 期待値最大の利得
 */
PreciseNumber getMaxReward(const Belief &b, const vector<vector<PreciseNumber> > &ex) {
    const pair<int, PreciseNumber> maxInfo = getMaxIndexAndReward(b, ex);
    return maxInfo.second;
}

/*
 * 機能: 信念空間を作る
 * 引数: 相手の状態数
 * 返り値: 信念空間=単位ベクトルの集合
 */
vector<Belief> makeBeliefSpace(const int opponentStates) {
   vector<Belief> beliefSpace;
   for (int i = 0; i < opponentStates; i++) {
      Belief e(opponentStates);
      e.set(i, ONE);
      beliefSpace.push_back(e);
   }
   sort(beliefSpace.begin(), beliefSpace.end());
   return beliefSpace;
}

