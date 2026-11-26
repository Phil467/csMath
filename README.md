# csMath

Fonctions élémentaires sur les `csReal` et les `csComplex` de [csArithmetic](https://github.com/Phil467/csArithmetic), édition Opt-13. Les racines passent par Newton. Les arguments sont en radians.

La version publiée ici est [v1](v1).

Les fonctions de calcul sont dans le namespace `CSMATH`. Les utilitaires sont dans `CSMATH::CSMUTILS`. Les corps sont dans l'en-tête et forcés en inline : inclure `csMath.h` suffit pour les appeler. `csMath.inl` doit rester à côté de `csMath.h`.

La précision est celle des réels, `realPrecision()`, plus le supplément porté par l'argument. Un réel non défini (racine paire d'un négatif, logarithme d'un nombre non positif) vaut zéro. Le même nombre, vu comme un complexe, suit la branche principale.

## Fonctions

Chaque fonction existe pour `csReal` et pour `csComplex`, sauf `pi`, `e` et `hypot` de deux complexes, qui renvoient un réel.

`sqrt`, `cbrt`, `root`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`, `exp`, `ln`, `log`, `log10`, `log2`, `pow`, `sinh`, `cosh`, `tanh`, `hypot`, `floor`, `ceil`, `pi`, `e`.

`log` est le logarithme naturel, comme `ln`.

## Contenu de v1

| Chemin | Rôle |
|---|---|
| [v1/csMath.h](v1/csMath.h) | Déclarations |
| [v1/csMath.inl](v1/csMath.inl) | Corps inline |
| [v1/csMath.cpp](v1/csMath.cpp) | Ancre quelques copies hors ligne dans la bibliothèque |
| [v1/tests/csMathSmoke.cpp](v1/tests/csMathSmoke.cpp) | Essai de fumée |
| [v1/lib/windows/libcsMath.a](v1/lib/windows/libcsMath.a) | Archive statique MinGW UCRT64 |
| [v1/lib/windows/csMath.dll](v1/lib/windows/csMath.dll) | DLL Windows |
| [v1/lib/windows/libcsMath.dll.a](v1/lib/windows/libcsMath.dll.a) | Bibliothèque d'import de la DLL |
| [v1/lib/android/arm64-v8a/libcsMath.a](v1/lib/android/arm64-v8a/libcsMath.a) | Archive statique Android arm64, API 21, pour Cxxdroid |

Les archives sont petites. Le calcul est dans l'en-tête. L'archive garde une copie hors ligne de `sqrt`, `cbrt`, `root`, `sin`, `cos`, `exp`, `ln` et `pow`.

## Compilation

Il faut les en-têtes et la bibliothèque de csArithmetic Opt-13 (`csArithmetic.h`, `csArithmeticOpt13.h`). Les deux en-têtes de csMath se placent dans le même dossier.

Pour l'archive statique, définir `CSARITHMETIC_STATIC` et `CSMATH_STATIC` avant les en-têtes.

```cpp
#define CSARITHMETIC_STATIC
#define CSMATH_STATIC
#include "csMath.h"

using namespace CSARITHMETIC;
using namespace CSMATH;

int main()
{
    setRealPrecision(30);
    csReal r = sqrt(csReal(2L));
    r.print("sqrt(2) = ");
}
```

Un entier nu, comme dans `sqrt(2)`, est pris par `sqrt` de la bibliothèque C. Écrire `CSMATH::sqrt(csReal(2L))`. `csReal(0)` est ambigu : écrire `csReal(0L)`.

Windows, MinGW UCRT64, en statique :

```text
g++ -std=c++17 -O2 -DNDEBUG -DCSARITHMETIC_STATIC -DCSMATH_STATIC -I. -I<csArithmeticOpt-13> essai.cpp -L. -lcsMath -lcsArithmeticOpt13
```

Windows, avec la DLL : ne pas définir `CSARITHMETIC_STATIC`, lier `libcsMath.dll.a` et `libcsArithmetic.a`, et laisser `csMath.dll` et `csArithmetic.dll` à côté de l'exécutable.

Cxxdroid, les en-têtes et `libcsMath.a` à côté du source, avec l'archive d'arithmétique :

```text
-std=c++17 -DCSARITHMETIC_STATIC -DCSMATH_STATIC -I. -L. -lcsMath -lcsArithmetic
```

## Licence

Licence MIT, dans [LICENSE](LICENSE). Usage, copie, modification et redistribution sont permis, y compris dans un programme commercial. La notice de copyright et le texte de la licence restent joints aux copies. Les versions modifiées n'ont pas à être publiées.
