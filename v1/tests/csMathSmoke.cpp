#define CSARITHMETIC_STATIC
#define CSMATH_STATIC
#include "csMath.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

using namespace CSARITHMETIC;
using namespace CSMATH;

static int fails = 0;

static void check(bool ok, const char* name)
{
  if (ok)
    return;
  std::cout << "echec " << name << "\n";
  ++fails;
}

static bool nearDouble(const csReal& a, double expected, double tol)
{
  double g = a.toDouble();
  return std::fabs(g - expected) <= tol;
}

int main()
{
  setRealPrecision(30);

  csReal a(true, "1.5");
  check(std::fabs(a.toDouble() - 1.5) < 1e-15, "toDouble 1.5");
  check(std::fabs((double)a.toLongDouble() - 1.5) < 1e-15, "toLongDouble 1.5");

  csReal neg(true, "-2.25");
  check(std::fabs(neg.toDouble() + 2.25) < 1e-15, "toDouble -2.25");

  csReal big(true, "1e20");
  check(std::fabs(big.toDouble() / 1e20 - 1.0) < 1e-12, "toDouble 1e20");
  check(big.toLong() == std::numeric_limits<long>::max(), "toLong debordement");
  check(big.toLongLong() == std::numeric_limits<long long>::max(), "toLongLong debordement");

  csReal tiny(true, "1e-20");
  check(tiny.toLong() == 0 && tiny.toLongLong() == 0, "troncature fractionnaire nulle");

  csReal cut(true, "-3.9");
  check(cut.toLong() == -3 && cut.toLongLong() == -3, "troncature vers zero");

  csReal exact(true, "9007199254740993");
  check(exact.toLongLong() == 9007199254740993LL, "toLongLong exact");

  csReal lim(true, "-2147483648");
  check(lim.toLong() == (long)-2147483648L, "toLong borne negative");
  check(lim.toLongLong() == -2147483648LL, "toLongLong borne");

  csReal threeB(true, "3000000000");
  check(threeB.toLongLong() == 3000000000LL, "toLongLong 3e9");
  if (3000000000LL > (long long)std::numeric_limits<long>::max())
    check(threeB.toLong() == std::numeric_limits<long>::max(), "toLong sature 3e9");
  else
    check(threeB.toLong() == 3000000000L, "toLong 3e9");

  check(sqrt(csReal(4L)).toLong() == 2, "sqrt 4");
  check(sqrt(csReal(-4L)).isZero(), "sqrt negatif");
  check(nearDouble(sqrt(csReal(2L)), std::sqrt(2.0), 1e-12), "sqrt 2");
  check(cbrt(csReal(27L)).toLong() == 3, "cbrt 27");
  check(cbrt(csReal(-8L)).toLong() == -2, "cbrt -8");
  check(root(csReal(32L), 5).toLong() == 2, "racine 5 de 32");
  check(root(csReal(81L), 4).toLong() == 3, "racine 4 de 81");
  check(nearDouble(root(csReal(16L), -2), 0.25, 1e-12), "racine -2");
  check(root(csReal(-8L), 2).isZero(), "racine paire negative");

  check(sin(csReal(0L)).isZero(), "sin 0");
  check(nearDouble(cos(csReal(0L)), 1.0, 1e-12), "cos 0");
  check(nearDouble(sin(pi() / 2), 1.0, 1e-9), "sin pi/2");
  check(nearDouble(cos(pi()), -1.0, 1e-9), "cos pi");
  check(nearDouble(exp(csReal(0L)), 1.0, 1e-12), "exp 0");
  check(nearDouble(exp(csReal(1L)), std::exp(1.0), 1e-12), "exp 1");
  check(ln(csReal(1L)).isZero(), "ln 1");
  check(nearDouble(ln(e()), 1.0, 1e-9), "ln e");
  check(nearDouble(log10(csReal(1000L)), 3.0, 1e-9), "log10 1000");
  check(pow(csReal(2L), csReal(10L)).toLong() == 1024, "pow 2^10");
  check(pow(csReal(-2L), csReal(3L)).toLong() == -8, "pow (-2)^3");
  check(nearDouble(pow(csReal(2L), csReal(true, "0.5")), std::sqrt(2.0), 1e-9), "pow 2^0.5");
  check(floor(csReal(true, "-1.5")).toLong() == -2, "floor -1.5");
  check(ceil(csReal(true, "-1.5")).toLong() == -1, "ceil -1.5");
  check(floor(csReal(true, "1.5")).toLong() == 1, "floor 1.5");
  check(ceil(csReal(true, "1.5")).toLong() == 2, "ceil 1.5");
  check(nearDouble(hypot(csReal(3L), csReal(4L)), 5.0, 1e-9), "hypot 3 4");
  check(nearDouble(atan(csReal(1L)) * 4, pi().toDouble(), 1e-9), "4 atan 1");
  check(nearDouble(asin(csReal(1L)), pi().toDouble() / 2.0, 1e-9), "asin 1");
  check(nearDouble(tanh(csReal(0L)), 0.0, 1e-12), "tanh 0");

  csComplex negFour(csReal(-4L));
  csComplex sneg = sqrt(negFour);
  check(nearDouble(sneg.real, 0.0, 1e-9), "sqrt complexe -4 reel");
  check(nearDouble(sneg.imag, 2.0, 1e-9), "sqrt complexe -4 imag");
  csComplex cube = cbrt(csComplex(csReal(27L)));
  check(nearDouble(cube.real, 3.0, 1e-9) && nearDouble(cube.imag, 0.0, 1e-9), "cbrt complexe 27");
  csComplex ei = exp(csComplex(csReal(0L), pi()));
  check(nearDouble(ei.real, -1.0, 1e-8) && nearDouble(ei.imag, 0.0, 1e-8), "exp i pi");
  csComplex pureI = sin(csComplex(csReal(0L), csReal(1L)));
  check(nearDouble(pureI.real, 0.0, 1e-8), "sin i reel");
  check(nearDouble(pureI.imag, std::sinh(1.0), 1e-8), "sin i imag");
  csComplex fl(-1.5, 2.2);
  check(floor(fl).real.toLong() == -2 && floor(fl).imag.toLong() == 2, "floor complexe");
  check(ceil(fl).real.toLong() == -1 && ceil(fl).imag.toLong() == 3, "ceil complexe");

  if (fails) {
    std::cout << fails << " echec(s)\n";
    return 1;
  }
  std::cout << "ok\n";
  return 0;
}
