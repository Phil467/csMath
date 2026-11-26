/**
 * @brief Utilitaires internes du calcul. Ils ne font pas partie de l'interface.
 */
namespace CSMUTILS {

/**
 * @brief Restaure la précision globale des réels à la destruction.
 */
struct PrecRestore {
  int saved;
  PrecRestore() : saved(CSARITHMETIC::realPrecision()) {}
  ~PrecRestore() { CSARITHMETIC::setRealPrecision(saved); }
};

/**
 * @brief Conserve une constante et le nombre de décimales déjà calculées.
 */
struct ConstCache {
  csReal value;
  int places;
  ConstCache() : value(0L), places(-1) {}
};

/**
 * @brief Retourne le nombre de décimales demandé pour un réel.
 * @param a Réel.
 * @return Nombre de décimales, au moins 1.
 */
CS_FORCE_INLINE int placesOf(const csReal& a)
{
  int base = CSARITHMETIC::realPrecision();
  if (base < 1)
    base = 1;
  if (a.precision > 0) {
    long long sum = (long long)base + a.precision;
    if (sum > 1000000000LL)
      sum = 1000000000LL;
    return (int)sum;
  }
  return base;
}

/**
 * @brief Retourne le nombre de décimales demandé pour deux réels.
 * @param a Premier réel.
 * @param b Second réel.
 * @return Nombre de décimales, au moins 1.
 */
CS_FORCE_INLINE int placesOf2(const csReal& a, const csReal& b)
{
  long extra = a.precision > b.precision ? a.precision : b.precision;
  csReal probe(0L);
  probe.precision = extra > 0 ? extra : 0;
  return placesOf(probe);
}

/**
 * @brief Retourne le plus grand supplément de précision des deux réels.
 * @param a Premier réel.
 * @param b Second réel.
 * @return Supplément, ou zéro.
 */
CS_FORCE_INLINE long precField2(const csReal& a, const csReal& b)
{
  long extra = a.precision > b.precision ? a.precision : b.precision;
  return extra > 0 ? extra : 0;
}

/**
 * @brief Arrondit au nombre de décimales demandé et rétablit le champ de précision.
 * @param x Réel à arrondir.
 * @param places Nombre de décimales.
 * @param precField Supplément de précision à recopier.
 * @return Réel arrondi.
 */
CS_FORCE_INLINE csReal finish(csReal x, int places, long precField)
{
  if (places < 1)
    places = 1;
  x.precision = 0;
  x.setDecimalPlaces(places);
  x.precision = precField > 0 ? precField : 0;
  return x;
}

/**
 * @brief Construit un réel depuis un entier 64 bits.
 * @param v Entier.
 * @return Réel égal à @p v.
 */
CS_FORCE_INLINE csReal realFromInt(long long v)
{
  if (v >= (long long)std::numeric_limits<long>::min() && v <= (long long)std::numeric_limits<long>::max())
    return csReal((long)v);
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%lld", v);
  return csReal(true, buf);
}

/**
 * @brief Calcule l'exposant scientifique, partie entière de log10 de la valeur absolue.
 * @param a Réel.
 * @param exp10 Exposant écrit en sortie.
 * @return Faux si @p a est nul ou sans mantisse.
 */
CS_FORCE_INLINE bool sciExpOf(const csReal& a, long long& exp10)
{
  const uint32_t* p = (const uint32_t*)a.mantissa;
  size_t n = a.mantSize;
  if (!p || n == 0)
    return false;
  while (n > 1 && p[n - 1] == 0)
    --n;
  if (n == 1 && p[0] == 0)
    return false;
  double hi;
  size_t below;
  if (n >= 2) {
    hi = (double)p[n - 1] * 4294967296.0 + (double)p[n - 2];
    below = n - 2;
  } else {
    hi = (double)p[0];
    below = 0;
  }
  double lg = std::log10(hi) + (double)below * (32.0 * std::log10(2.0));
  exp10 = (long long)std::floor(lg + 1e-12) + (long long)a.exponent;
  return true;
}

/**
 * @brief Indique si la valeur est nulle ou plus petite que 10^(-places).
 * @param t Réel.
 * @param places Nombre de décimales.
 * @return Vrai si le terme peut être négligé.
 */
CS_FORCE_INLINE bool negligible(const csReal& t, int places)
{
  csReal u = t;
  if (u.isZero())
    return true;
  long long E = 0;
  if (!sciExpOf(u, E))
    return true;
  return E < -(long long)places - 1;
}

/**
 * @brief Retire les chiffres au-delà des décimales demandées.
 * @param v Réel modifié.
 * @param places Nombre de décimales conservées.
 */
CS_FORCE_INLINE void trimAbsolute(csReal& v, int places)
{
  if (places < 0)
    places = 0;
  if ((long long)v.exponent >= -(long long)places)
    return;
  long long drop = -(long long)v.exponent - (long long)places;
  if (drop > 0)
    v.shortenMantissa((size_t)drop);
  v.precision = 0;
}

/**
 * @brief Retourne une approximation double des premiers chiffres.
 * @param v Réel.
 * @return Approximation, ou zéro si @p v est nul.
 */
CS_FORCE_INLINE double leadingDouble(const csReal& v)
{
  csReal t = v;
  t.precision = 0;
  long long E = 0;
  if (!sciExpOf(t, E))
    return 0.0;
  long long digits = E - (long long)t.exponent + 1;
  if (digits > 24) {
    long long drop = digits - 20;
    if (drop > 0)
      t.shortenMantissa((size_t)drop);
  }
  if (t.isZero())
    return 0.0;
  return t.toDouble();
}

/**
 * @brief Retourne le quotient entier arrondi vers moins l'infini.
 * @param a Dividende.
 * @param b Diviseur.
 * @return Quotient.
 */
CS_FORCE_INLINE long long floorDiv(long long a, long long b)
{
  long long q = a / b;
  if (a % b != 0 && ((a < 0) != (b < 0)))
    --q;
  return q;
}

/**
 * @brief Élève un réel à une puissance entière positive ou nulle.
 * @param base Base.
 * @param exp Exposant positif ou nul.
 * @return Puissance.
 */
CS_FORCE_INLINE csReal ipowPos(csReal base, long long exp)
{
  csReal result(1L);
  base.precision = 0;
  while (exp > 0) {
    if (exp & 1)
      result = result * base;
    exp >>= 1;
    if (exp > 0)
      base = base * base;
    result.precision = 0;
    base.precision = 0;
  }
  return result;
}

/**
 * @brief Élève un réel à une puissance entière portée par un réel.
 * @param base Base.
 * @param exp Exposant entier.
 * @return Puissance.
 */
CS_FORCE_INLINE csReal ipowReal(csReal base, csReal exp)
{
  base.precision = 0;
  exp.precision = 0;
  long long E = 0;
  if (!sciExpOf(exp, E) || E < 18)
    return ipowPos(base, exp.isZero() ? 0 : exp.toLongLong());
  csReal result(1L);
  while (!exp.isZero()) {
    csReal q = exp.integerQuotient(2);
    q.precision = 0;
    if (exp != q * 2)
      result = result * base;
    exp = q;
    if (!exp.isZero())
      base = base * base;
    result.precision = 0;
    base.precision = 0;
    exp.precision = 0;
  }
  return result;
}

/**
 * @brief Calcule la racine n-ième positive par Newton.
 * @param mag Valeur positive.
 * @param n Ordre, au moins 2.
 * @param places Nombre de décimales visé.
 * @return Racine positive, ou zéro si elle est négligeable.
 */
CS_FORCE_INLINE csReal newtonRoot(csReal mag, long n, int places)
{
  PrecRestore restore;
  mag.precision = 0;
  if (n < 2 || mag.isZero())
    return csReal(0L);
  long long E = 0;
  if (!sciExpOf(mag, E))
    return csReal(0L);
  long long power = floorDiv(E, (long long)n);
  long long shifted = (long long)mag.exponent - power * (long long)n;
  if (shifted > (long long)std::numeric_limits<int>::max())
    shifted = std::numeric_limits<int>::max();
  if (shifted < (long long)std::numeric_limits<int>::min())
    shifted = std::numeric_limits<int>::min();
  csReal r = mag;
  r.exponent = (int)shifted;
  r.precision = 0;
  long long er = 0;
  if (sciExpOf(r, er)) {
    long long k = floorDiv(er, (long long)n);
    if (k != 0) {
      long long adj = (long long)r.exponent - k * (long long)n;
      if (adj > (long long)std::numeric_limits<int>::max())
        adj = std::numeric_limits<int>::max();
      if (adj < (long long)std::numeric_limits<int>::min())
        adj = std::numeric_limits<int>::min();
      r.exponent = (int)adj;
      power += k;
    }
  }
  if (power < -(long long)places - 2)
    return csReal(0L);

  long long sig64 = (long long)places + power + 8;
  if (sig64 < 16)
    sig64 = 16;
  if (sig64 > 1000000000LL)
    sig64 = 1000000000LL;
  int sig = (int)sig64;

  double lead = leadingDouble(r);
  double sd = 0.0;
  if (lead > 0.0 && std::isfinite(lead)) {
    if (n == 2)
      sd = std::sqrt(lead);
    else if (n == 3)
      sd = std::cbrt(lead);
    else
      sd = std::pow(lead, 1.0 / (double)n);
  }
  if (!(sd > 0.0) || !std::isfinite(sd)) {
    long long er2 = 0;
    sciExpOf(r, er2);
    sd = std::pow(10.0, ((double)er2 + 0.5) / (double)n);
  }
  if (!(sd > 0.0) || !std::isfinite(sd))
    sd = 1.0;
  csReal x(sd);
  x.precision = 0;

  int step = 16;
  if (step > sig)
    step = sig;
  while (true) {
    CSARITHMETIC::setRealPrecision(step + 4);
    csReal rr = r;
    trimAbsolute(rr, step + 6);
    if (x.isZero() || x.negativeSign())
      x = csReal(1L);
    csReal p = (n == 2) ? x : ipowPos(x, (long long)n - 1);
    csReal next = (realFromInt((long long)n - 1) * x + rr / p) / realFromInt((long long)n);
    next.precision = 0;
    x = next;
    if (step >= sig)
      break;
    int nxt = step * 2;
    if (nxt > sig || nxt < step)
      nxt = sig;
    step = nxt;
  }
  long long ne = (long long)x.exponent + power;
  if (ne > (long long)std::numeric_limits<int>::max())
    ne = std::numeric_limits<int>::max();
  if (ne < (long long)std::numeric_limits<int>::min())
    ne = std::numeric_limits<int>::min();
  x.exponent = (int)ne;
  x.precision = 0;
  return x;
}

/**
 * @brief Retourne le cache de ln(2).
 * @return Cache unique du programme.
 */
CS_FORCE_INLINE ConstCache& ln2Cache()
{
  static ConstCache c;
  return c;
}

/**
 * @brief Retourne le cache de pi.
 * @return Cache unique du programme.
 */
CS_FORCE_INLINE ConstCache& piCache()
{
  static ConstCache c;
  return c;
}

/**
 * @brief Calcule ln(2) au nombre de décimales demandé.
 * @param places Nombre de décimales.
 * @return ln(2).
 */
CS_FORCE_INLINE csReal ln2At(int places)
{
  if (places < 1)
    places = 1;
  ConstCache& c = ln2Cache();
  if (c.places >= places) {
    csReal v = c.value;
    v.setDecimalPlaces(places);
    v.precision = 0;
    return v;
  }
  int guard = places + 6;
  PrecRestore restore;
  CSARITHMETIC::setRealPrecision(guard);
  csReal z = csReal(1L) / 3;
  csReal z2 = z * z;
  csReal p = z;
  csReal sum = z;
  long long den = 1;
  long long cap = (long long)guard * 3 + 64;
  while (den < cap) {
    p = p * z2;
    den += 2;
    p.precision = 0;
    csReal term = p / realFromInt(den);
    term.precision = 0;
    if (negligible(term, guard))
      break;
    sum = sum + term;
    sum.precision = 0;
  }
  sum = sum * 2;
  sum.precision = 0;
  sum.setDecimalPlaces(guard);
  sum.precision = 0;
  c.value = sum;
  c.places = guard;
  csReal v = sum;
  v.setDecimalPlaces(places);
  v.precision = 0;
  return v;
}

/**
 * @brief Calcule arctan par la série, pour un argument de valeur absolue au plus 1.
 * @param x Argument.
 * @param places Nombre de décimales.
 * @return Arctangente.
 */
CS_FORCE_INLINE csReal atanSeries(csReal x, int places)
{
  PrecRestore restore;
  x.precision = 0;
  int guard = places < 1 ? 5 : places;
  CSARITHMETIC::setRealPrecision(guard);
  csReal y = x * x;
  csReal p = x;
  csReal sum = x;
  long long den = 1;
  long long cap = (long long)guard * 4 + 64;
  while (den < cap) {
    p = -(p * y);
    den += 2;
    p.precision = 0;
    csReal term = p / realFromInt(den);
    term.precision = 0;
    if (negligible(term, guard))
      break;
    sum = sum + term;
    sum.precision = 0;
  }
  sum.precision = 0;
  return sum;
}

/**
 * @brief Calcule pi au nombre de décimales demandé.
 * @param places Nombre de décimales.
 * @return pi.
 */
CS_FORCE_INLINE csReal piAt(int places)
{
  if (places < 1)
    places = 1;
  ConstCache& c = piCache();
  if (c.places >= places) {
    csReal v = c.value;
    v.setDecimalPlaces(places);
    v.precision = 0;
    return v;
  }
  int guard = places + 6;
  PrecRestore restore;
  CSARITHMETIC::setRealPrecision(guard);
  csReal a5 = atanSeries(csReal(1L) / 5, guard);
  csReal a239 = atanSeries(csReal(1L) / 239, guard);
  CSARITHMETIC::setRealPrecision(guard);
  csReal pi = a5 * 16 - a239 * 4;
  pi.precision = 0;
  pi.setDecimalPlaces(guard);
  pi.precision = 0;
  c.value = pi;
  c.places = guard;
  csReal v = pi;
  v.setDecimalPlaces(places);
  v.precision = 0;
  return v;
}

/**
 * @brief Calcule l'exponentielle au nombre de décimales demandé.
 * @param x Argument.
 * @param places Nombre de décimales.
 * @return Exponentielle.
 */
CS_FORCE_INLINE csReal expAt(csReal x, int places)
{
  PrecRestore restore;
  x.precision = 0;
  if (x.isZero())
    return csReal(1L);
  long long E = 0;
  long long extra = 0;
  if (sciExpOf(x, E) && E > 0)
    extra = E;
  long long guard64 = (long long)places + extra + 6;
  if (guard64 < 8)
    guard64 = 8;
  if (guard64 > 1000000000LL)
    guard64 = 1000000000LL;
  int guard = (int)guard64;
  CSARITHMETIC::setRealPrecision(guard);
  csReal l2 = ln2At(guard);
  CSARITHMETIC::setRealPrecision(guard);
  csReal q = x / l2;
  csReal half = csReal(1L) / 2;
  csReal nreal = q.negativeSign() ? (q - half).integer() : (q + half).integer();
  nreal.precision = 0;
  csReal r = x - nreal * l2;
  r.precision = 0;
  csReal sum(1L);
  csReal term(1L);
  long long k = 1;
  long long cap = (long long)guard * 3 + 64;
  while (k < cap) {
    term = term * r;
    term = term / realFromInt(k);
    term.precision = 0;
    if (negligible(term, guard))
      break;
    sum = sum + term;
    sum.precision = 0;
    ++k;
  }
  bool negN = nreal.negativeSign();
  csReal nabs = negN ? -nreal : nreal;
  nabs.precision = 0;
  csReal scale(1L);
  long long nE = 0;
  if (!nabs.isZero() && sciExpOf(nabs, nE) && nE >= 18)
    scale = ipowReal(csReal(2L), nabs);
  else if (!nabs.isZero())
    scale = ipowPos(csReal(2L), nabs.toLongLong());
  scale.precision = 0;
  if (negN)
    sum = sum / scale;
  else
    sum = sum * scale;
  sum.precision = 0;
  return sum;
}

/**
 * @brief Calcule le logarithme naturel d'un réel strictement positif.
 * @param x Réel strictement positif.
 * @param places Nombre de décimales.
 * @return Logarithme, ou zéro si @p x n'est pas strictement positif.
 */
CS_FORCE_INLINE csReal lnAt(csReal x, int places)
{
  PrecRestore restore;
  x.precision = 0;
  if (x.isZero() || x.negativeSign())
    return csReal(0L);
  if (x == csReal(1L))
    return csReal(0L);
  double lead = leadingDouble(x);
  csReal y;
  if (lead > 0.0 && std::isfinite(lead))
    y = csReal(std::log(lead));
  else {
    long long E = 0;
    sciExpOf(x, E);
    y = realFromInt(E) * csReal(std::log(10.0));
  }
  y.precision = 0;
  long long E = 0;
  sciExpOf(x, E);
  long long mag = E >= 0 ? E : -E;
  int mdigits = 1;
  while (mag > 9 && mdigits < 12) {
    mag /= 10;
    ++mdigits;
  }
  int sig = places + mdigits + 8;
  if (sig < 16)
    sig = 16;
  int step = 16;
  if (step > sig)
    step = sig;
  while (true) {
    CSARITHMETIC::setRealPrecision(step + 2);
    csReal e = expAt(-y, step + 2);
    CSARITHMETIC::setRealPrecision(step + 2);
    csReal next = y + x * e - csReal(1L);
    next.precision = 0;
    y = next;
    if (step >= sig)
      break;
    int nxt = step * 2;
    if (nxt > sig || nxt < step)
      nxt = sig;
    step = nxt;
  }
  y.precision = 0;
  return y;
}

/**
 * @brief Calcule l'arctangente au nombre de décimales demandé.
 * @param x Argument.
 * @param places Nombre de décimales.
 * @return Arctangente.
 */
CS_FORCE_INLINE csReal atanRaw(csReal x, int places)
{
  PrecRestore restore;
  x.precision = 0;
  bool neg = x.negativeSign() && !x.isZero();
  if (neg)
    x = -x;
  int guard = places + 4;
  if (guard < 8)
    guard = 8;
  CSARITHMETIC::setRealPrecision(guard);
  bool sub = false;
  if (x > csReal(1L)) {
    x = csReal(1L) / x;
    x.precision = 0;
    sub = true;
  }
  csReal rad = csReal(1L) + x * x;
  rad.precision = 0;
  csReal s = newtonRoot(rad, 2, guard);
  CSARITHMETIC::setRealPrecision(guard);
  csReal y = x / (csReal(1L) + s);
  y.precision = 0;
  csReal a = atanSeries(y, guard);
  CSARITHMETIC::setRealPrecision(guard);
  a = a + a;
  if (sub)
    a = piAt(guard) / 2 - a;
  if (neg)
    a = -a;
  a.precision = 0;
  return a;
}

/**
 * @brief Couple sinus et cosinus d'un même argument.
 */
struct SinCos {
  csReal s;
  csReal c;
};

/**
 * @brief Calcule sinus et cosinus d'un argument déjà réduit.
 * @param r Argument réduit.
 * @param places Nombre de décimales.
 * @return Sinus et cosinus.
 */
CS_FORCE_INLINE SinCos sinCosSmall(csReal r, int places)
{
  PrecRestore restore;
  r.precision = 0;
  int guard = places + 4;
  if (guard < 8)
    guard = 8;
  CSARITHMETIC::setRealPrecision(guard);
  csReal r2 = r * r;
  csReal sTerm = r;
  csReal sSum = r;
  csReal cTerm(1L);
  csReal cSum(1L);
  long long k = 1;
  long long cap = (long long)guard * 3 + 64;
  while (k < cap) {
    sTerm = sTerm * r2;
    sTerm = -(sTerm / realFromInt(k + 1) / realFromInt(k + 2));
    cTerm = cTerm * r2;
    cTerm = -(cTerm / realFromInt(k) / realFromInt(k + 1));
    sTerm.precision = 0;
    cTerm.precision = 0;
    bool ss = negligible(sTerm, guard);
    bool cc = negligible(cTerm, guard);
    if (!ss)
      sSum = sSum + sTerm;
    if (!cc)
      cSum = cSum + cTerm;
    sSum.precision = 0;
    cSum.precision = 0;
    if (ss && cc)
      break;
    k += 2;
  }
  SinCos out;
  out.s = sSum;
  out.c = cSum;
  return out;
}

/**
 * @brief Calcule sinus et cosinus après réduction modulo pi/2.
 * @param a Argument.
 * @param places Nombre de décimales.
 * @param s Sinus écrit en sortie.
 * @param c Cosinus écrit en sortie.
 */
CS_FORCE_INLINE void sinCosAt(const csReal& a, int places, csReal& s, csReal& c)
{
  PrecRestore restore;
  csReal x = a;
  x.precision = 0;
  if (x.isZero()) {
    s = csReal(0L);
    c = csReal(1L);
    return;
  }
  long long E = 0;
  long long extra = 0;
  if (sciExpOf(x, E) && E > 0)
    extra = E;
  long long guard64 = (long long)places + extra + 8;
  if (guard64 < 8)
    guard64 = 8;
  if (guard64 > 1000000000LL)
    guard64 = 1000000000LL;
  int guard = (int)guard64;
  CSARITHMETIC::setRealPrecision(guard);
  csReal halfPi = piAt(guard) / 2;
  CSARITHMETIC::setRealPrecision(guard);
  csReal q = x / halfPi;
  csReal half = csReal(1L) / 2;
  csReal nreal = q.negativeSign() ? (q - half).integer() : (q + half).integer();
  nreal.precision = 0;
  csReal mod = nreal - nreal.integerQuotient(4) * 4;
  if (mod.negativeSign())
    mod = mod + 4;
  long quad = mod.toLong() & 3;
  if (quad < 0)
    quad += 4;
  csReal r = x - nreal * halfPi;
  r.precision = 0;
  SinCos sc = sinCosSmall(r, places);
  if (quad == 0) {
    s = sc.s;
    c = sc.c;
  } else if (quad == 1) {
    s = sc.c;
    c = -sc.s;
  } else if (quad == 2) {
    s = -sc.s;
    c = -sc.c;
  } else {
    s = -sc.c;
    c = sc.s;
  }
  s.precision = 0;
  c.precision = 0;
}


/**
 * @brief Ajoute un décalage à l'exposant décimal.
 * @param v Réel modifié.
 * @param delta Décalage, en puissances de dix.
 */
CS_FORCE_INLINE void addExponent(csReal& v, long long delta)
{
  long long ne = (long long)v.exponent + delta;
  if (ne > (long long)std::numeric_limits<int>::max())
    ne = std::numeric_limits<int>::max();
  if (ne < (long long)std::numeric_limits<int>::min())
    ne = std::numeric_limits<int>::min();
  v.exponent = (int)ne;
}

/**
 * @brief Arrondit les deux parties d'un complexe.
 * @param re Partie réelle.
 * @param im Partie imaginaire.
 * @param places Nombre de décimales.
 * @param prec Supplément de précision recopié.
 * @return Complexe arrondi.
 */
CS_FORCE_INLINE csComplex finished(csReal re, csReal im, int places, long prec)
{
  return csComplex(finish(re, places, prec), finish(im, places, prec));
}

/**
 * @brief Choisit une graine sur la branche principale pour la racine de Newton.
 * @param value Complexe déjà ramené à une grandeur modérée.
 * @param n Ordre de la racine.
 * @param outRe Partie réelle de la graine.
 * @param outIm Partie imaginaire de la graine.
 */
CS_FORCE_INLINE void principalSeed(const csComplex& value, long n, double& outRe, double& outIm)
{
  long long er = 0;
  long long ei = 0;
  bool hasRe = sciExpOf(value.real, er);
  bool hasIm = sciExpOf(value.imag, ei);
  long long scale = 0;
  if (hasRe)
    scale = er;
  if (hasIm && (!hasRe || ei > scale))
    scale = ei;
  csReal re = value.real;
  csReal im = value.imag;
  addExponent(re, -scale);
  addExponent(im, -scale);
  double rd = leadingDouble(re);
  double id = leadingDouble(im);
  double mag = std::hypot(rd, id);
  if (!(mag > 0.0) || !std::isfinite(mag)) {
    outRe = 1.0;
    outIm = 0.0;
    return;
  }
  double arg = std::atan2(id, rd);
  double log10mag = std::log10(mag) + (double)scale;
  double rootMag = std::pow(10.0, log10mag / (double)n);
  double rootArg = arg / (double)n;
  outRe = rootMag * std::cos(rootArg);
  outIm = rootMag * std::sin(rootArg);
  if (!std::isfinite(outRe) || !std::isfinite(outIm) || (outRe == 0.0 && outIm == 0.0)) {
    outRe = 1.0;
    outIm = 0.0;
  }
}

/**
 * @brief Calcule la racine n-ième complexe par Newton, sur la branche principale.
 * @param value Complexe.
 * @param n Ordre, au moins 2.
 * @param places Nombre de décimales visé.
 * @return Racine, ou zéro si elle est négligeable.
 */
CS_FORCE_INLINE csComplex newtonRoot(csComplex value, long n, int places)
{
  PrecRestore restore;
  if (n < 2 || value.isZero())
    return csComplex(0L);
  long long er = 0;
  long long ei = 0;
  bool hasRe = sciExpOf(value.real, er);
  bool hasIm = sciExpOf(value.imag, ei);
  if (!hasRe && !hasIm)
    return csComplex(0L);
  long long E = hasRe ? er : ei;
  if (hasIm && (!hasRe || ei > E))
    E = ei;
  long long power = floorDiv(E, (long long)n);
  long long rem = E - power * (long long)n;
  if (rem < 0) {
    rem += n;
    power -= 1;
  }
  if (power < -(long long)places - 2)
    return csComplex(0L);
  csComplex r = value;
  addExponent(r.real, -power * (long long)n);
  addExponent(r.imag, -power * (long long)n);
  double seedRe = 1.0;
  double seedIm = 0.0;
  principalSeed(r, n, seedRe, seedIm);
  csComplex x = csComplex(csReal(seedRe), csReal(seedIm));
  long long sig64 = (long long)places + (power > 0 ? power : 0) + 8;
  if (sig64 < 16)
    sig64 = 16;
  if (sig64 > 1000000000LL)
    sig64 = 1000000000LL;
  int sig = (int)sig64;
  int step = 16;
  if (step > sig)
    step = sig;
  while (true) {
    CSARITHMETIC::setRealPrecision(step + 4);
    csComplex rr = r;
    trimAbsolute(rr.real, step + 6);
    trimAbsolute(rr.imag, step + 6);
    if (x.isZero())
      x = csComplex(1L);
    csComplex p = (n == 2) ? x : x.power(n - 1);
    if (p.isZero())
      x = csComplex(1L);
    else {
      csComplex next = (x * realFromInt((long long)n - 1) + (rr / p)) / realFromInt((long long)n);
      next.real.precision = 0;
      next.imag.precision = 0;
      x = next;
    }
    if (step >= sig)
      break;
    int nxt = step * 2;
    if (nxt > sig || nxt < step)
      nxt = sig;
    step = nxt;
  }
  addExponent(x.real, power);
  addExponent(x.imag, power);
  return x;
}

} // CSMUTILS

CS_FORCE_INLINE csReal sqrt(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal m = a;
  m.precision = 0;
  if (m.isZero() || m.negativeSign())
    return csReal(0L);
  return CSMUTILS::finish(CSMUTILS::newtonRoot(m, 2, places), places, a.precision);
}

CS_FORCE_INLINE csReal cbrt(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal m = a;
  m.precision = 0;
  if (m.isZero())
    return CSMUTILS::finish(csReal(0L), places, a.precision);
  bool neg = m.negativeSign();
  if (neg)
    m = -m;
  csReal r = CSMUTILS::newtonRoot(m, 3, places);
  if (neg)
    r = -r;
  return CSMUTILS::finish(r, places, a.precision);
}

CS_FORCE_INLINE csReal root(const csReal& a, long n)
{
  int places = CSMUTILS::placesOf(a);
  if (n == 0)
    return csReal(0L);
  bool invert = false;
  if (n < 0) {
    invert = true;
    n = -n;
  }
  if (n == 1) {
    csReal r = CSMUTILS::finish(csReal(a), places, a.precision);
    if (!invert)
      return r;
    if (r.isZero())
      return r;
    CSMUTILS::PrecRestore restore;
    CSARITHMETIC::setRealPrecision(places + 4);
    return CSMUTILS::finish(csReal(1L) / r, places, a.precision);
  }
  csReal m = a;
  m.precision = 0;
  if (m.isZero())
    return CSMUTILS::finish(csReal(0L), places, a.precision);
  bool neg = m.negativeSign();
  if (neg) {
    if ((n & 1) == 0)
      return csReal(0L);
    m = -m;
  }
  csReal r = CSMUTILS::newtonRoot(m, n, places);
  if (neg)
    r = -r;
  r = CSMUTILS::finish(r, places, a.precision);
  if (!invert || r.isZero())
    return r;
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  return CSMUTILS::finish(csReal(1L) / r, places, a.precision);
}

CS_FORCE_INLINE csReal sin(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal s, c;
  CSMUTILS::sinCosAt(a, places, s, c);
  return CSMUTILS::finish(s, places, a.precision);
}

CS_FORCE_INLINE csReal cos(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal s, c;
  CSMUTILS::sinCosAt(a, places, s, c);
  return CSMUTILS::finish(c, places, a.precision);
}

CS_FORCE_INLINE csReal tan(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal s, c;
  CSMUTILS::sinCosAt(a, places, s, c);
  if (c.isZero())
    return csReal(0L);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  return CSMUTILS::finish(s / c, places, a.precision);
}

CS_FORCE_INLINE csReal asin(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal x = a;
  x.precision = 0;
  bool neg = x.negativeSign() && !x.isZero();
  if (neg)
    x = -x;
  if (x.isZero())
    return CSMUTILS::finish(csReal(0L), places, a.precision);
  if (x > csReal(1L))
    return csReal(0L);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal under = csReal(1L) - x * x;
  under.precision = 0;
  if (under.isZero() || under.negativeSign()) {
    csReal h = CSMUTILS::piAt(places) / 2;
    if (neg)
      h = -h;
    return CSMUTILS::finish(h, places, a.precision);
  }
  csReal den = CSMUTILS::newtonRoot(under, 2, places + 4);
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal v = CSMUTILS::atanRaw(x / den, places);
  if (neg)
    v = -v;
  return CSMUTILS::finish(v, places, a.precision);
}

CS_FORCE_INLINE csReal acos(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal x = a;
  x.precision = 0;
  if (x.negativeSign() ? (csReal(0L) - x) > csReal(1L) : x > csReal(1L))
    return csReal(0L);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal v = CSMUTILS::piAt(places) / 2 - asin(a);
  return CSMUTILS::finish(v, places, a.precision);
}

CS_FORCE_INLINE csReal atan(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  return CSMUTILS::finish(CSMUTILS::atanRaw(a, places), places, a.precision);
}

CS_FORCE_INLINE csReal atan2(const csReal& y, const csReal& x)
{
  int places = CSMUTILS::placesOf2(y, x);
  long prec = CSMUTILS::precField2(y, x);
  csReal yy = y;
  csReal xx = x;
  yy.precision = 0;
  xx.precision = 0;
  if (yy.isZero() && xx.isZero())
    return CSMUTILS::finish(csReal(0L), places, prec);
  if (xx.isZero()) {
    csReal h = CSMUTILS::piAt(places) / 2;
    if (yy.negativeSign())
      h = -h;
    return CSMUTILS::finish(h, places, prec);
  }
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal a = CSMUTILS::atanRaw(yy / xx, places);
  if (xx.negativeSign()) {
    csReal p = CSMUTILS::piAt(places + 2);
    if (yy.negativeSign())
      a = a - p;
    else
      a = a + p;
  }
  return CSMUTILS::finish(a, places, prec);
}

CS_FORCE_INLINE csReal exp(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  return CSMUTILS::finish(CSMUTILS::expAt(a, places), places, a.precision);
}

CS_FORCE_INLINE csReal ln(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal x = a;
  x.precision = 0;
  if (x.isZero() || x.negativeSign())
    return csReal(0L);
  return CSMUTILS::finish(CSMUTILS::lnAt(x, places), places, a.precision);
}

CS_FORCE_INLINE csReal log(const csReal& a)
{
  return ln(a);
}

CS_FORCE_INLINE csReal log10(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal x = a;
  x.precision = 0;
  if (x.isZero() || x.negativeSign())
    return csReal(0L);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal v = CSMUTILS::lnAt(x, places + 2) / CSMUTILS::lnAt(csReal(10L), places + 2);
  return CSMUTILS::finish(v, places, a.precision);
}

CS_FORCE_INLINE csReal log2(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal x = a;
  x.precision = 0;
  if (x.isZero() || x.negativeSign())
    return csReal(0L);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal v = CSMUTILS::lnAt(x, places + 2) / CSMUTILS::ln2At(places + 2);
  return CSMUTILS::finish(v, places, a.precision);
}

CS_FORCE_INLINE csReal pow(const csReal& a, const csReal& b)
{
  int places = CSMUTILS::placesOf2(a, b);
  long prec = CSMUTILS::precField2(a, b);
  csReal base = a;
  csReal expv = b;
  base.precision = 0;
  expv.precision = 0;
  csReal ip = expv.integer();
  bool whole = (expv == ip);
  if (base.isZero()) {
    if (whole && ip.isZero())
      return CSMUTILS::finish(csReal(1L), places, prec);
    return csReal(0L);
  }
  if (whole) {
    CSMUTILS::PrecRestore restore;
    CSARITHMETIC::setRealPrecision(places + 4);
    bool expNeg = ip.negativeSign();
    csReal expAbs = expNeg ? -ip : ip;
    expAbs.precision = 0;
    csReal q = expAbs.integerQuotient(2);
    bool odd = expAbs != q * 2;
    bool neg = base.negativeSign() && odd;
    csReal mag = base.negativeSign() ? -base : base;
    mag.precision = 0;
    csReal r = CSMUTILS::ipowReal(mag, expAbs);
    if (neg)
      r = -r;
    if (expNeg) {
      if (r.isZero())
        return csReal(0L);
      r = csReal(1L) / r;
    }
    return CSMUTILS::finish(r, places, prec);
  }
  if (base.negativeSign())
    return csReal(0L);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal w = expv * CSMUTILS::lnAt(base, places + 2);
  w.precision = 0;
  return CSMUTILS::finish(CSMUTILS::expAt(w, places), places, prec);
}

CS_FORCE_INLINE csReal sinh(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal ep = CSMUTILS::expAt(a, places + 2);
  csReal en = CSMUTILS::expAt(-a, places + 2);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal r = (ep - en) / 2;
  return CSMUTILS::finish(r, places, a.precision);
}

CS_FORCE_INLINE csReal cosh(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal ep = CSMUTILS::expAt(a, places + 2);
  csReal en = CSMUTILS::expAt(-a, places + 2);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal r = (ep + en) / 2;
  return CSMUTILS::finish(r, places, a.precision);
}

CS_FORCE_INLINE csReal tanh(const csReal& a)
{
  int places = CSMUTILS::placesOf(a);
  csReal x = a;
  csReal e = CSMUTILS::expAt(x + x, places + 2);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal r = (e - csReal(1L)) / (e + csReal(1L));
  return CSMUTILS::finish(r, places, a.precision);
}

CS_FORCE_INLINE csReal hypot(const csReal& x, const csReal& y)
{
  int places = CSMUTILS::placesOf2(x, y);
  long prec = CSMUTILS::precField2(x, y);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal xx = x;
  csReal yy = y;
  xx.precision = 0;
  yy.precision = 0;
  csReal s = xx * xx + yy * yy;
  s.precision = 0;
  return CSMUTILS::finish(CSMUTILS::newtonRoot(s, 2, places), places, prec);
}

CS_FORCE_INLINE csReal floor(const csReal& a)
{
  csReal t = a;
  csReal ip = t.integer();
  if (t.negativeSign() && !(t == ip))
    ip = ip - 1;
  ip.precision = a.precision > 0 ? a.precision : 0;
  return ip;
}

CS_FORCE_INLINE csReal ceil(const csReal& a)
{
  csReal t = a;
  csReal ip = t.integer();
  if (!t.negativeSign() && !t.isZero() && !(t == ip))
    ip = ip + 1;
  ip.precision = a.precision > 0 ? a.precision : 0;
  return ip;
}

CS_FORCE_INLINE csReal pi()
{
  int p = CSARITHMETIC::realPrecision();
  if (p < 1)
    p = 1;
  return CSMUTILS::piAt(p);
}

CS_FORCE_INLINE csReal e()
{
  int p = CSARITHMETIC::realPrecision();
  if (p < 1)
    p = 1;
  return CSMUTILS::finish(CSMUTILS::expAt(csReal(1L), p), p, 0);
}

CS_FORCE_INLINE csComplex sqrt(const csComplex& a)
{
  csComplex z = a;
  if (z.isReal()) {
    if (!z.real.negativeSign() || z.real.isZero())
      return csComplex(sqrt(z.real));
    return csComplex(csReal(0L), sqrt(-z.real));
  }
  int places = CSMUTILS::placesOf2(z.real, z.imag);
  long prec = CSMUTILS::precField2(z.real, z.imag);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal mod = hypot(z.real, z.imag);
  csReal sum = mod + z.real;
  csReal diff = mod - z.real;
  if (sum.negativeSign())
    sum = csReal(0L);
  if (diff.negativeSign())
    diff = csReal(0L);
  csReal re = sqrt(sum / 2);
  csReal im = sqrt(diff / 2);
  if (z.imag.negativeSign())
    im = -im;
  return CSMUTILS::finished(re, im, places, prec);
}

CS_FORCE_INLINE csComplex root(const csComplex& a, long n)
{
  int places = CSMUTILS::placesOf2(a.real, a.imag);
  long prec = CSMUTILS::precField2(a.real, a.imag);
  if (n == 0)
    return csComplex(0L);
  bool invert = false;
  long order = n;
  if (order < 0) {
    invert = true;
    order = -order;
  }
  csComplex z = a;
  if (z.isReal()) {
    bool neg = z.real.negativeSign() && !z.real.isZero();
    if (!neg || (order & 1))
      return csComplex(root(z.real, invert ? -order : order));
  }
  if (z.isZero())
    return csComplex(0L);
  if (order == 1) {
    csComplex r = CSMUTILS::finished(z.real, z.imag, places, prec);
    if (!invert || r.isZero())
      return r;
    CSMUTILS::PrecRestore restore;
    CSARITHMETIC::setRealPrecision(places + 4);
    r = csComplex(1L) / r;
    return CSMUTILS::finished(r.real, r.imag, places, prec);
  }
  csComplex r = CSMUTILS::newtonRoot(z, order, places);
  r = CSMUTILS::finished(r.real, r.imag, places, prec);
  if (!invert || r.isZero())
    return r;
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  r = csComplex(1L) / r;
  return CSMUTILS::finished(r.real, r.imag, places, prec);
}

CS_FORCE_INLINE csComplex cbrt(const csComplex& a)
{
  return root(a, 3L);
}

CS_FORCE_INLINE csComplex exp(const csComplex& a)
{
  csComplex z = a;
  if (z.imag.isZero())
    return csComplex(exp(z.real));
  int places = CSMUTILS::placesOf2(z.real, z.imag);
  long prec = CSMUTILS::precField2(z.real, z.imag);
  csReal mag = exp(z.real);
  csReal s;
  csReal c;
  CSMUTILS::sinCosAt(z.imag, places, s, c);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  return CSMUTILS::finished(mag * c, mag * s, places, prec);
}

CS_FORCE_INLINE csComplex ln(const csComplex& a)
{
  csComplex z = a;
  if (z.isZero())
    return csComplex(0L);
  if (z.isReal() && !z.real.negativeSign())
    return csComplex(ln(z.real));
  int places = CSMUTILS::placesOf2(z.real, z.imag);
  long prec = CSMUTILS::precField2(z.real, z.imag);
  csReal mod = hypot(z.real, z.imag);
  return CSMUTILS::finished(ln(mod), atan2(z.imag, z.real), places, prec);
}

CS_FORCE_INLINE csComplex log(const csComplex& a)
{
  return ln(a);
}

CS_FORCE_INLINE csComplex log10(const csComplex& a)
{
  csComplex z = a;
  if (z.isZero())
    return csComplex(0L);
  int places = CSMUTILS::placesOf2(z.real, z.imag);
  long prec = CSMUTILS::precField2(z.real, z.imag);
  csComplex w = ln(z);
  csReal base = ln(csReal(10L));
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  return CSMUTILS::finished(w.real / base, w.imag / base, places, prec);
}

CS_FORCE_INLINE csComplex log2(const csComplex& a)
{
  csComplex z = a;
  if (z.isZero())
    return csComplex(0L);
  int places = CSMUTILS::placesOf2(z.real, z.imag);
  long prec = CSMUTILS::precField2(z.real, z.imag);
  csComplex w = ln(z);
  csReal base = CSMUTILS::ln2At(places + 2);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  return CSMUTILS::finished(w.real / base, w.imag / base, places, prec);
}

CS_FORCE_INLINE csComplex pow(const csComplex& a, const csComplex& b)
{
  csComplex base = a;
  csComplex expv = b;
  if (base.isZero()) {
    if (expv.isZero())
      return csComplex(1L);
    return csComplex(0L);
  }
  if (expv.isReal()) {
    csReal ip = expv.real.integer();
    if (expv.real == ip) {
      long long width = 0;
      if (ip.isZero() || !CSMUTILS::sciExpOf(ip, width) || width < 9)
        return base.power(ip.isZero() ? 0 : ip.toLong());
    }
    if (base.isReal() && !base.real.negativeSign())
      return csComplex(pow(base.real, expv.real));
  }
  return exp(expv * ln(base));
}

CS_FORCE_INLINE csComplex sin(const csComplex& a)
{
  csComplex z = a;
  if (z.imag.isZero())
    return csComplex(sin(z.real));
  int places = CSMUTILS::placesOf2(z.real, z.imag);
  long prec = CSMUTILS::precField2(z.real, z.imag);
  csReal s;
  csReal c;
  CSMUTILS::sinCosAt(z.real, places, s, c);
  csReal sh = sinh(z.imag);
  csReal ch = cosh(z.imag);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  return CSMUTILS::finished(s * ch, c * sh, places, prec);
}

CS_FORCE_INLINE csComplex cos(const csComplex& a)
{
  csComplex z = a;
  if (z.imag.isZero())
    return csComplex(cos(z.real));
  int places = CSMUTILS::placesOf2(z.real, z.imag);
  long prec = CSMUTILS::precField2(z.real, z.imag);
  csReal s;
  csReal c;
  CSMUTILS::sinCosAt(z.real, places, s, c);
  csReal sh = sinh(z.imag);
  csReal ch = cosh(z.imag);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  return CSMUTILS::finished(c * ch, -(s * sh), places, prec);
}

CS_FORCE_INLINE csComplex tan(const csComplex& a)
{
  int places = CSMUTILS::placesOf2(a.real, a.imag);
  long prec = CSMUTILS::precField2(a.real, a.imag);
  csComplex s = sin(a);
  csComplex c = cos(a);
  if (c.isZero())
    return csComplex(0L);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csComplex q = s / c;
  return CSMUTILS::finished(q.real, q.imag, places, prec);
}

CS_FORCE_INLINE csComplex asin(const csComplex& a)
{
  int places = CSMUTILS::placesOf2(a.real, a.imag);
  long prec = CSMUTILS::precField2(a.real, a.imag);
  csComplex z = a;
  if (z.isReal()) {
    csReal x = z.real;
    csReal ax = x.negativeSign() ? -x : x;
    if (ax.isZero() || !(ax > csReal(1L)))
      return csComplex(asin(x));
  }
  csComplex under = csComplex(1L) - (z * z);
  csComplex s = sqrt(under);
  csComplex iz(-z.imag, z.real);
  csComplex w = ln(iz + s);
  return CSMUTILS::finished(w.imag, -w.real, places, prec);
}

CS_FORCE_INLINE csComplex acos(const csComplex& a)
{
  int places = CSMUTILS::placesOf2(a.real, a.imag);
  long prec = CSMUTILS::precField2(a.real, a.imag);
  csComplex v = asin(a);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csReal half = pi() / 2;
  return CSMUTILS::finished(half - v.real, -v.imag, places, prec);
}

CS_FORCE_INLINE csComplex atan(const csComplex& a)
{
  int places = CSMUTILS::placesOf2(a.real, a.imag);
  long prec = CSMUTILS::precField2(a.real, a.imag);
  csComplex z = a;
  if (z.isReal())
    return csComplex(atan(z.real));
  csComplex unit = csComplex::imaginaryUnit();
  csComplex den = unit - z;
  if (den.isZero())
    return csComplex(0L);
  csComplex w = ln((unit + z) / den);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  return CSMUTILS::finished(-w.imag / 2, w.real / 2, places, prec);
}

CS_FORCE_INLINE csComplex atan2(const csComplex& y, const csComplex& x)
{
  int places = CSMUTILS::placesOf2(y.real, x.real);
  long extra = CSMUTILS::precField2(y.real, x.real);
  long ey = CSMUTILS::precField2(y.imag, x.imag);
  if (ey > extra)
    extra = ey;
  csComplex unit = csComplex::imaginaryUnit();
  csComplex left = x;
  csComplex xy = left + (unit * y);
  if (xy.isZero())
    return csComplex(0L);
  csReal mod = hypot(xy.real, xy.imag);
  csComplex w = ln(xy / mod);
  return CSMUTILS::finished(w.imag, -w.real, places, extra);
}

CS_FORCE_INLINE csComplex sinh(const csComplex& a)
{
  csComplex z = a;
  if (z.imag.isZero())
    return csComplex(sinh(z.real));
  int places = CSMUTILS::placesOf2(z.real, z.imag);
  long prec = CSMUTILS::precField2(z.real, z.imag);
  csReal sh = sinh(z.real);
  csReal ch = cosh(z.real);
  csReal s;
  csReal c;
  CSMUTILS::sinCosAt(z.imag, places, s, c);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  return CSMUTILS::finished(sh * c, ch * s, places, prec);
}

CS_FORCE_INLINE csComplex cosh(const csComplex& a)
{
  csComplex z = a;
  if (z.imag.isZero())
    return csComplex(cosh(z.real));
  int places = CSMUTILS::placesOf2(z.real, z.imag);
  long prec = CSMUTILS::precField2(z.real, z.imag);
  csReal sh = sinh(z.real);
  csReal ch = cosh(z.real);
  csReal s;
  csReal c;
  CSMUTILS::sinCosAt(z.imag, places, s, c);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  return CSMUTILS::finished(ch * c, sh * s, places, prec);
}

CS_FORCE_INLINE csComplex tanh(const csComplex& a)
{
  int places = CSMUTILS::placesOf2(a.real, a.imag);
  long prec = CSMUTILS::precField2(a.real, a.imag);
  csComplex sh = sinh(a);
  csComplex ch = cosh(a);
  if (ch.isZero())
    return csComplex(0L);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csComplex q = sh / ch;
  return CSMUTILS::finished(q.real, q.imag, places, prec);
}

CS_FORCE_INLINE csReal hypot(const csComplex& x, const csComplex& y)
{
  int places = CSMUTILS::placesOf2(x.real, y.real);
  long prec = CSMUTILS::precField2(x.real, y.real);
  CSMUTILS::PrecRestore restore;
  CSARITHMETIC::setRealPrecision(places + 4);
  csComplex xx = x;
  csComplex yy = y;
  csReal n = xx.norm() + yy.norm();
  return CSMUTILS::finish(CSMUTILS::newtonRoot(n, 2, places), places, prec);
}

CS_FORCE_INLINE csComplex floor(const csComplex& a)
{
  return csComplex(floor(a.real), floor(a.imag));
}

CS_FORCE_INLINE csComplex ceil(const csComplex& a)
{
  return csComplex(ceil(a.real), ceil(a.imag));
}
