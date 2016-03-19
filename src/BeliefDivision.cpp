/*
 * BeliefDivision.cpp
 *
 *  Created on: 2012. 11. 21.
 *      Author: chaerim
 */

/*
 * This class is used as initialBeliefDivision or targetBeliefDivision. 
 * It holds extreme points. It's first index means belonging to division, second index means just index, and third index means nothing(I think).
 * It holds inequalities. It's first index means belonging to division, second index means just index
 */

#include "BeliefDivision.h"

BeliefDivision::BeliefDivision() {
  dimension = 0;
}

BeliefDivision::BeliefDivision(int dimension) {
  configure(dimension);
}

void BeliefDivision::configure(int dimension) {
  this->dimension = dimension;
  extremePoints.clear();
  extremePoints.resize(dimension);
  inequalities.clear();
  inequalities.resize(dimension);
}

/*
 * 機能: division の端点をセットする
 * 引数: division と端点の集合 po
 */
void BeliefDivision::setPolytopeToDivision(int division, vector<Belief> po) {
  extremePoints[division] = po;
}

/*
 * 機能: division の端点を取得
 * 引数: division
 */
vector<Belief> BeliefDivision::getDivision(int division) const {
  return extremePoints[division];
}

/*
 * 機能: division の端点から不等式を生成
 * 引数: division
 */
void BeliefDivision::makeInequalities(int division) {
  if (extremePoints[division].size() > 0) {
	inequalities[division] = convertBeliefToInequality(extremePoints[division]);
  }
}

/*
 * 機能: division の不等式を取得
 * 引数: division
 */
Inequality BeliefDivision::getDivisionInequality(int division) const{
  return inequalities[division];
}

/*
 * 機能: 次元を取得
 */
int BeliefDivision::getNumberOfDivisions() const{
  return dimension;
}

/*
 * 機能: 空になっている division があると true を返す
 */
bool BeliefDivision::existEmptyDivision() const{
  for (int i = 0; i < dimension; i++) {
	if (extremePoints[i].size() == 0) return true;
  }
  return false;
}

/*
 * 機能: 空になっている division の集合を返す
 */
vector <int> BeliefDivision::indexOfEmptyDivisions() const{
  vector <int> res;
  for (int i = 0; i < dimension; ++i) {
	if (extremePoints[i].size() == 0)
	  res.push_back(i);
  }
  return res;
}

/*
 * 機能: 文字列化
 */
string BeliefDivision::toString() const {
  string res;
  res += "** BeliefDivision **\n";
  for (int div = 0; div < getNumberOfDivisions(); ++div) {
	res += "* division " + ::toString(div) + "\n";
	vector<Belief> ex =  getDivision(div);
	for (vector<Belief>::size_type i = 0; i < ex.size(); ++i)
	  res += ex[i].toString() + "\n";
  }
  return res;
}
      

/*
 * 機能: 同じか調べる
 * メモ: 何に使うかわからん
 */
bool operator == (const BeliefDivision& lhs, const BeliefDivision& rhs) {
  if (lhs.getNumberOfDivisions() != rhs.getNumberOfDivisions()) return false;
  for (int i = 0; i < rhs.getNumberOfDivisions(); i++) {
    vector<Belief> lhsv = lhs.getDivision(i);
    vector<Belief> rhsv = rhs.getDivision(i);
    if (lhsv.size() != rhsv.size()) return false;
    for (vector<Belief>::size_type k = 0; k < lhsv.size(); k++) {
      if (lhsv[k] != rhsv[k]) {
		return false;
      }
    }

  }
  return true;
}

/*
 * 機能: 出力
 */
ostream& operator<<(ostream &os, const BeliefDivision& b) {
  return os << b.toString();
}
