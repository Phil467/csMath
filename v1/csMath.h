#pragma once

/**
 * @brief Fonctions élémentaires sur csReal et csComplex.
 * Les corps sont dans cet en-tête et forcés en inline.
 * Les fonctions de calcul sont dans CSMATH. Les utilitaires sont dans CSMATH::CSMUTILS.
 * Les racines réelles passent par Newton. La racine carrée complexe s'appuie sur ces racines.
 * La racine n-ième complexe passe aussi par Newton, sur la branche principale.
 * La précision est celle des réels : realPrecision(), plus le supplément porté par l'argument.
 * Un réel non défini (racine paire d'un négatif, logarithme d'un nombre non positif) vaut zéro.
 * Le même nombre, vu comme un complexe, suit la branche principale.
 */

#if defined(CSMATH_STATIC)
  #define CSMATH_API
#elif defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_CSMATH_DLL
    #define CSMATH_API __declspec(dllexport)
  #else
    #define CSMATH_API __declspec(dllimport)
  #endif
#else
  #ifdef BUILDING_CSMATH_DLL
    #define CSMATH_API __attribute__ ((visibility ("default")))
  #else
    #define CSMATH_API
  #endif
#endif

#include "csArithmetic.h"

#include <cmath>
#include <complex>
#include <cstdint>
#include <cstdio>
#include <limits>

namespace CSMATH {

/**
 * @brief Calcule la racine carrée.
 * Une valeur négative ou nulle donne zéro.
 * @param a Réel.
 * @return Racine carrée.
 */
CS_FORCE_INLINE csReal sqrt(const csReal& a);
/**
 * @brief Calcule la racine carrée sur la branche principale.
 * Un réel positif ou nul reprend la racine réelle. Un réel négatif donne un imaginaire pur de partie imaginaire positive.
 * @param a Complexe.
 * @return Racine carrée.
 */
CS_FORCE_INLINE csComplex sqrt(const csComplex& a);
/**
 * @brief Calcule la racine cubique.
 * Le signe du réel est conservé.
 * @param a Réel.
 * @return Racine cubique.
 */
CS_FORCE_INLINE csReal cbrt(const csReal& a);
/**
 * @brief Calcule la racine cubique.
 * Un réel pur reprend la racine cubique réelle, y compris pour une valeur négative.
 * @param a Complexe.
 * @return Racine cubique.
 */
CS_FORCE_INLINE csComplex cbrt(const csComplex& a);
/**
 * @brief Calcule la racine n-ième.
 * Un ordre nul donne zéro. Un ordre négatif inverse le résultat. La racine paire d'un négatif donne zéro.
 * @param a Réel.
 * @param n Ordre de la racine.
 * @return Racine n-ième.
 */
CS_FORCE_INLINE csReal root(const csReal& a, long n);
/**
 * @brief Calcule la racine n-ième sur la branche principale.
 * Un réel pur non négatif, ou un ordre impair, reprend la racine réelle. Un ordre négatif inverse le résultat.
 * @param a Complexe.
 * @param n Ordre de la racine.
 * @return Racine n-ième.
 */
CS_FORCE_INLINE csComplex root(const csComplex& a, long n);
/**
 * @brief Calcule le sinus.
 * @param a Argument, en radians.
 * @return Sinus.
 */
CS_FORCE_INLINE csReal sin(const csReal& a);
/**
 * @brief Calcule le sinus.
 * @param a Argument, en radians.
 * @return Sinus.
 */
CS_FORCE_INLINE csComplex sin(const csComplex& a);
/**
 * @brief Calcule le cosinus.
 * @param a Argument, en radians.
 * @return Cosinus.
 */
CS_FORCE_INLINE csReal cos(const csReal& a);
/**
 * @brief Calcule le cosinus.
 * @param a Argument, en radians.
 * @return Cosinus.
 */
CS_FORCE_INLINE csComplex cos(const csComplex& a);
/**
 * @brief Calcule la tangente.
 * Un cosinus nul donne zéro.
 * @param a Argument, en radians.
 * @return Tangente.
 */
CS_FORCE_INLINE csReal tan(const csReal& a);
/**
 * @brief Calcule la tangente.
 * Un cosinus nul donne zéro.
 * @param a Argument, en radians.
 * @return Tangente.
 */
CS_FORCE_INLINE csComplex tan(const csComplex& a);
/**
 * @brief Calcule l'arcsinus.
 * Un argument hors de [-1, 1] donne zéro.
 * @param a Réel.
 * @return Arcsinus, en radians.
 */
CS_FORCE_INLINE csReal asin(const csReal& a);
/**
 * @brief Calcule l'arcsinus sur la branche principale.
 * Un réel de valeur absolue au plus 1 reprend l'arcsinus réel.
 * @param a Complexe.
 * @return Arcsinus.
 */
CS_FORCE_INLINE csComplex asin(const csComplex& a);
/**
 * @brief Calcule l'arccosinus.
 * Un argument hors de [-1, 1] donne zéro.
 * @param a Réel.
 * @return Arccosinus, en radians.
 */
CS_FORCE_INLINE csReal acos(const csReal& a);
/**
 * @brief Calcule l'arccosinus sur la branche principale.
 * @param a Complexe.
 * @return Arccosinus.
 */
CS_FORCE_INLINE csComplex acos(const csComplex& a);
/**
 * @brief Calcule l'arctangente.
 * @param a Réel.
 * @return Arctangente, en radians.
 */
CS_FORCE_INLINE csReal atan(const csReal& a);
/**
 * @brief Calcule l'arctangente sur la branche principale.
 * Un réel pur reprend l'arctangente réelle.
 * @param a Complexe.
 * @return Arctangente.
 */
CS_FORCE_INLINE csComplex atan(const csComplex& a);
/**
 * @brief Calcule l'angle du vecteur (x, y), dans (-pi, pi].
 * Les deux arguments nuls donnent zéro.
 * @param y Ordonnée.
 * @param x Abscisse.
 * @return Angle, en radians.
 */
CS_FORCE_INLINE csReal atan2(const csReal& y, const csReal& x);
/**
 * @brief Calcule l'arctangente à deux arguments sur la branche principale.
 * Les deux arguments nuls donnent zéro.
 * @param y Ordonnée.
 * @param x Abscisse.
 * @return Angle.
 */
CS_FORCE_INLINE csComplex atan2(const csComplex& y, const csComplex& x);
/**
 * @brief Calcule l'exponentielle.
 * @param a Réel.
 * @return Exponentielle.
 */
CS_FORCE_INLINE csReal exp(const csReal& a);
/**
 * @brief Calcule l'exponentielle.
 * Un imaginaire nul reprend l'exponentielle réelle.
 * @param a Complexe.
 * @return Exponentielle.
 */
CS_FORCE_INLINE csComplex exp(const csComplex& a);
/**
 * @brief Calcule le logarithme naturel.
 * Une valeur négative ou nulle donne zéro.
 * @param a Réel.
 * @return Logarithme naturel.
 */
CS_FORCE_INLINE csReal ln(const csReal& a);
/**
 * @brief Calcule le logarithme naturel sur la branche principale.
 * Un réel strictement positif reprend le logarithme réel. Zéro donne zéro.
 * @param a Complexe.
 * @return Logarithme naturel.
 */
CS_FORCE_INLINE csComplex ln(const csComplex& a);
/**
 * @brief Calcule le logarithme naturel.
 * Même résultat que ln().
 * @param a Réel.
 * @return Logarithme naturel.
 */
CS_FORCE_INLINE csReal log(const csReal& a);
/**
 * @brief Calcule le logarithme naturel.
 * Même résultat que ln().
 * @param a Complexe.
 * @return Logarithme naturel.
 */
CS_FORCE_INLINE csComplex log(const csComplex& a);
/**
 * @brief Calcule le logarithme décimal.
 * Une valeur négative ou nulle donne zéro.
 * @param a Réel.
 * @return Logarithme décimal.
 */
CS_FORCE_INLINE csReal log10(const csReal& a);
/**
 * @brief Calcule le logarithme décimal sur la branche principale.
 * Zéro donne zéro.
 * @param a Complexe.
 * @return Logarithme décimal.
 */
CS_FORCE_INLINE csComplex log10(const csComplex& a);
/**
 * @brief Calcule le logarithme en base 2.
 * Une valeur négative ou nulle donne zéro.
 * @param a Réel.
 * @return Logarithme en base 2.
 */
CS_FORCE_INLINE csReal log2(const csReal& a);
/**
 * @brief Calcule le logarithme en base 2 sur la branche principale.
 * Zéro donne zéro.
 * @param a Complexe.
 * @return Logarithme en base 2.
 */
CS_FORCE_INLINE csComplex log2(const csComplex& a);
/**
 * @brief Élève un réel à une puissance réelle.
 * Zéro puissance zéro vaut un. Une base négative avec un exposant non entier donne zéro.
 * @param a Base.
 * @param b Exposant.
 * @return Puissance.
 */
CS_FORCE_INLINE csReal pow(const csReal& a, const csReal& b);
/**
 * @brief Élève un complexe à une puissance complexe.
 * Zéro puissance zéro vaut un. Un exposant entier tient dans une puissance entière. Deux réels positifs reprennent la puissance réelle.
 * @param a Base.
 * @param b Exposant.
 * @return Puissance.
 */
CS_FORCE_INLINE csComplex pow(const csComplex& a, const csComplex& b);
/**
 * @brief Calcule le sinus hyperbolique.
 * @param a Réel.
 * @return Sinus hyperbolique.
 */
CS_FORCE_INLINE csReal sinh(const csReal& a);
/**
 * @brief Calcule le sinus hyperbolique.
 * Un imaginaire nul reprend le sinus hyperbolique réel.
 * @param a Complexe.
 * @return Sinus hyperbolique.
 */
CS_FORCE_INLINE csComplex sinh(const csComplex& a);
/**
 * @brief Calcule le cosinus hyperbolique.
 * @param a Réel.
 * @return Cosinus hyperbolique.
 */
CS_FORCE_INLINE csReal cosh(const csReal& a);
/**
 * @brief Calcule le cosinus hyperbolique.
 * Un imaginaire nul reprend le cosinus hyperbolique réel.
 * @param a Complexe.
 * @return Cosinus hyperbolique.
 */
CS_FORCE_INLINE csComplex cosh(const csComplex& a);
/**
 * @brief Calcule la tangente hyperbolique.
 * @param a Réel.
 * @return Tangente hyperbolique.
 */
CS_FORCE_INLINE csReal tanh(const csReal& a);
/**
 * @brief Calcule la tangente hyperbolique.
 * Un cosinus hyperbolique nul donne zéro.
 * @param a Complexe.
 * @return Tangente hyperbolique.
 */
CS_FORCE_INLINE csComplex tanh(const csComplex& a);
/**
 * @brief Calcule la racine carrée de x^2 + y^2.
 * @param x Premier réel.
 * @param y Second réel.
 * @return Hypoténuse.
 */
CS_FORCE_INLINE csReal hypot(const csReal& x, const csReal& y);
/**
 * @brief Calcule la racine carrée de la somme des carrés des modules.
 * @param x Premier complexe.
 * @param y Second complexe.
 * @return Hypoténuse réelle.
 */
CS_FORCE_INLINE csReal hypot(const csComplex& x, const csComplex& y);
/**
 * @brief Retourne le plus grand entier inférieur ou égal au réel.
 * @param a Réel.
 * @return Partie entière vers moins l'infini.
 */
CS_FORCE_INLINE csReal floor(const csReal& a);
/**
 * @brief Applique floor à chaque partie.
 * @param a Complexe.
 * @return Complexe des parties entières vers moins l'infini.
 */
CS_FORCE_INLINE csComplex floor(const csComplex& a);
/**
 * @brief Retourne le plus petit entier supérieur ou égal au réel.
 * @param a Réel.
 * @return Partie entière vers plus l'infini.
 */
CS_FORCE_INLINE csReal ceil(const csReal& a);
/**
 * @brief Applique ceil à chaque partie.
 * @param a Complexe.
 * @return Complexe des parties entières vers plus l'infini.
 */
CS_FORCE_INLINE csComplex ceil(const csComplex& a);
/**
 * @brief Retourne pi à la précision courante des réels.
 * @return pi.
 */
CS_FORCE_INLINE csReal pi();
/**
 * @brief Retourne e à la précision courante des réels.
 * @return e.
 */
CS_FORCE_INLINE csReal e();

#include "csMath.inl"

}
