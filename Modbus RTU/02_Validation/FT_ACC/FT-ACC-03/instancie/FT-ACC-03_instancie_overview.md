# FT-ACC-03 — Vue d’ensemble des tests instanciés

Couverture active : **129 champs logiques RO non réservés**.

| Bloc | Nombre de champs RO testés |
|---|---:|
| B0 | 9 |
| B1 | 15 |
| B2 | 9 |
| B3 | 25 |
| B4 | 31 |
| B5 | 11 |
| B6 | 17 |
| B7 | 12 |
| **Total** | **129** |

Le champ réservé `B3_RESERVED_0` est volontairement exclu de FT-ACC-03 et relève de FT-ACC-04.

Chaque fichier `TT-ACC-03-Bx-xxx__<champ>.md` instancie la règle générique correspondant à la cardinalité du champ :
- 1 registre → `TT-ACC-03-GEN-001` ;
- plusieurs registres → `TT-ACC-03-GEN-002`.
