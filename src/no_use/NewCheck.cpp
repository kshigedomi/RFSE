#include "NewCheck.h"

/*
 * 理論的な計算のとき用
 */
int main(int argc, char **argv) {
  const int M = toInteger(string(argv[1]));
  
  const PreciseNumber x = PreciseNumber("1/10");
  const PreciseNumber y = PreciseNumber("1/10");

  const PreciseNumber pStart = PreciseNumber("1/2") + EPS;
  const PreciseNumber pInterval = PreciseNumber("1/1000");
  const PreciseNumber pEnd = ONE;

  int misse = 0;
  int missska = 0;
  int suc = 0;

  for (PreciseNumber p = pStart; p < pEnd; p += pInterval) {
	const PreciseNumber s = ONE - p;
	PreciseNumber pm = ONE;
	PreciseNumber sm = ONE;
	for (int i = 0; i < M; ++i) {
	  pm *= p;
	  sm *= s;
	}


	PreciseNumber d = ONE - sm*sm;

    PreciseNumber Kr = ((1-d)*x)/(d*(p-s));
    PreciseNumber Kp = ((1-d)*y)/(d*(p-s));


    PreciseNumber vrvp = (M*(1-d)+d*((-M*s*Kr-M*p*Kp)+sm*(M*Kr+M*Kp)))/(1-d*(1-2*sm));

    PreciseNumber vr = 2*M - d*(M*s*Kr+sm*(vrvp-M*Kr))/(1-d);

    PreciseNumber vp = M+d*(M*p*Kp+sm*(vrvp-M*Kp))/(1-d);

    PreciseNumber cond1 = (1-d)*x-d*sm/s*(p-s)*(vrvp-M*Kr);
    if (cond1 > 0) {
       cout << "cond1" << endl;
       continue;
    }
    PreciseNumber cond2 = (1-d)*x-d*pm/p*(p-s)*(vrvp-M*Kp);
    if (cond2 > 0){
       cout << "cond2" << endl;
       continue;
    }
    PreciseNumber cond3 = vrvp - M*Kr;
    if (cond3 < 0){
       cout << "cond3" << endl;
       continue;
    }
    PreciseNumber cond4 = vrvp - M*Kp;
    if (cond4 < 0){
       cout << "cond4" << endl;
       continue;
    }
    cout << "success " << rationalToString(vr/2/M) << endl;
    cout << rationalToString(vrvp) << endl;
    suc++;
  }
  cout << suc << endl;
}
	


	  
