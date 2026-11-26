#include "csMath.h"

/* Garde une copie hors ligne dans la bibliothèque. L'appelant, lui, reçoit le corps inline. */
__attribute__((used))
static void csMathKeepSymbols()
{
  csReal (*realSqrt)(const csReal&) = CSMATH::sqrt;
  csComplex (*complexSqrt)(const csComplex&) = CSMATH::sqrt;
  csReal (*realCbrt)(const csReal&) = CSMATH::cbrt;
  csComplex (*complexCbrt)(const csComplex&) = CSMATH::cbrt;
  csReal (*realRoot)(const csReal&, long) = CSMATH::root;
  csComplex (*complexRoot)(const csComplex&, long) = CSMATH::root;
  csReal (*realSin)(const csReal&) = CSMATH::sin;
  csComplex (*complexSin)(const csComplex&) = CSMATH::sin;
  csReal (*realCos)(const csReal&) = CSMATH::cos;
  csComplex (*complexCos)(const csComplex&) = CSMATH::cos;
  csReal (*realExp)(const csReal&) = CSMATH::exp;
  csComplex (*complexExp)(const csComplex&) = CSMATH::exp;
  csReal (*realLn)(const csReal&) = CSMATH::ln;
  csComplex (*complexLn)(const csComplex&) = CSMATH::ln;
  csReal (*realPow)(const csReal&, const csReal&) = CSMATH::pow;
  csComplex (*complexPow)(const csComplex&, const csComplex&) = CSMATH::pow;
  (void)realSqrt; (void)complexSqrt;
  (void)realCbrt; (void)complexCbrt;
  (void)realRoot; (void)complexRoot;
  (void)realSin; (void)complexSin;
  (void)realCos; (void)complexCos;
  (void)realExp; (void)complexExp;
  (void)realLn; (void)complexLn;
  (void)realPow; (void)complexPow;
}
