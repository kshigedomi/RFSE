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
		if ((belief[i] < ZERO && !equal(belief[i], ZERO)) || (belief[i] > ONE && !equal(belief[i], ONE))) {
			return false;
		}
		sum += belief[i];
	}
	if (!equal(sum, ONE)) return false;
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
   res += "(" + rationalToString(belief[0]);
   for (int i = 1; i < dimension; ++i)
      res += ", " + rationalToString(belief[i]);
   res += ")";
   return res;
}

/*
 * 機能: 信念をセット
 * 引数: どの index を value にセットするか
 */
void Belief::set(int index, PreciseNumber value) {
	if (abs(value) == 0) value = 0;
	belief[index] = value;
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
		if (!equal(lhs.get(i), rhs.get(i))) return false;
	}
	return true;
}


bool operator != (const Belief& lhs, const Belief& rhs) {
	if (lhs.getDimension() != rhs.getDimension()) return true;
	for (int i = 0; i < rhs.getDimension(); i++) {
		if (!equal(lhs.get(i), rhs.get(i))) return true;
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
   const vector<PreciseNumber>::size_type size = ex.size();
   for (vector<PreciseNumber>::size_type i = 0; i < size; ++i) {
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

/*
 * 機能: 信念を2次元の gnuplot 用のポイントに変える
 * 引数: poly（凸集合の端点）
 * 返り値: gnuplot 用のポイントの集合 
 * メモ: 信念で囲まれる領域を filledcurves close で書けるように
 *       "x y1 y2" という形式に変えている
 * 注意: もちろん3次元の信念にしか使えない
 */
vector<string> gnuplotString(const vector<Belief> &poly) {
  /*
   * 信念空間そのものは高さ1,1辺 2/sqrt(3) の正三角形で描く
   * 原点は底辺の真ん中で,その対角をなす頂点が, b=(1,0,0) の点
   * すると,
   * Belief = (a, b, c) とすると point = ((1-a)/sqrt(3) - 2*b/sqrt(3), a)
   * 例えば, Belief = (0, 1, 0) なら point = (-1/sqrt(3), 0)
   */
   vector<pair<PreciseNumber, PreciseNumber> > points;
   for (vector<Belief>::size_type i = 0; i < poly.size(); ++i) {
      PreciseNumber y = poly[i].get(0);
      PreciseNumber x = (1-poly[i].get(0))/sqrt(3)-2*poly[i].get(1)/sqrt(3);
      points.push_back(make_pair(x, y));
   }
   sort(points.begin(), points.end());

  /*
   * gnuplot の filledcurves closed が使えるように,
   * "x, minY, maxY" という形に変える
   * そのために,真ん中の頂点に対応する minY or maxY を見つける必要がある
   */

   vector<string> res;
   // 端は閉じている
   res.push_back(rationalToString(points[0].first) + "\t" + rationalToString(points[0].second) + "\t" + rationalToString(points[0].second));

   for (vector<pair<PreciseNumber, PreciseNumber> >::size_type i = 1; i < points.size() - 1; ++i) {
	 const pair<PreciseNumber, PreciseNumber> target = points[i]; // target の minY or maxY を見つけ出す
	 PreciseNumber maxY = target.second, minY = target.second; // point は凸集合の端点なので,maxY か minY は元の Y と同じ
      for (vector<pair<PreciseNumber, PreciseNumber> >::size_type l = 0; l < i; ++l) {
         for (vector<pair<PreciseNumber, PreciseNumber> >::size_type r = i+1; r < points.size(); ++r) {
		   // points[l] と points[r] で引かれる直線の式を見つける
            const PreciseNumber xGap = points[r].first-points[l].first;
            if (xGap != ZERO) {
               const PreciseNumber slope = (points[r].second - points[l].second)/xGap;
               const PreciseNumber y = slope*target.first + points[l].second - slope*points[l].first;
               maxY = max(maxY, y);
               minY = min(minY, y);
            }
         }
      }
      res.push_back(rationalToString(target.first) + "\t" + rationalToString(minY) + "\t" + rationalToString(maxY));
   }

   // 端は閉じている
   res.push_back(rationalToString(points[points.size()-1].first) + "\t" + rationalToString(points[points.size() - 1].second) + "\t" + rationalToString(points[points.size()-1].second));
   return res;
}
vector<string> gnuplotString2(const vector<Belief> &poly) {
  /*
   * 信念空間そのものは高さ1 の直角三角形で描く
   * 原点は直角で，(1,0) が b=(1,0,0), (0,1) が b=(0,1,0)
   */
   vector<pair<PreciseNumber, PreciseNumber> > points;
   for (vector<Belief>::size_type i = 0; i < poly.size(); ++i) {
	 const PreciseNumber x = poly[i].get(0);
	 const PreciseNumber y = poly[i].get(1);
	 points.push_back(make_pair(x, y));
   }
   sort(points.begin(), points.end());

  /*
   * gnuplot の filledcurves closed が使えるように,
   * "x, minY, maxY" という形に変える
   * そのために,真ん中の頂点に対応する minY or maxY を見つける必要がある
   */

   vector<string> res;
   // 端は閉じている
   res.push_back(rationalToString(points[0].first) + "\t" + rationalToString(points[0].second) + "\t" + rationalToString(points[0].second));

   for (vector<pair<PreciseNumber, PreciseNumber> >::size_type i = 1; i < points.size() - 1; ++i) {
	 const pair<PreciseNumber, PreciseNumber> target = points[i]; // target の minY or maxY を見つけ出す
	 PreciseNumber maxY = target.second, minY = target.second; // point は凸集合の端点なので,maxY か minY は元の Y と同じ
      for (vector<pair<PreciseNumber, PreciseNumber> >::size_type l = 0; l < i; ++l) {
         for (vector<pair<PreciseNumber, PreciseNumber> >::size_type r = i+1; r < points.size(); ++r) {
		   // points[l] と points[r] で引かれる直線の式を見つける
            const PreciseNumber xGap = points[r].first-points[l].first;
            if (xGap != ZERO) {
               const PreciseNumber slope = (points[r].second - points[l].second)/xGap;
               const PreciseNumber y = slope*target.first + points[l].second - slope*points[l].first;
               maxY = max(maxY, y);
               minY = min(minY, y);
            }
         }
      }
      res.push_back(rationalToString(target.first) + "\t" + rationalToString(minY) + "\t" + rationalToString(maxY));
   }

   // 端は閉じている
   res.push_back(rationalToString(points[points.size()-1].first) + "\t" + rationalToString(points[points.size() - 1].second) + "\t" + rationalToString(points[points.size()-1].second));
   return res;
}
         
