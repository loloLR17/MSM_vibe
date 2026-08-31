# FT-ACC — Spécifications de famille

## 1. Objet
La famille FT-ACC valide les droits d’accès Modbus et l’absence d’effets non spécifiés pour l’interface TR2, conformément à la spécification V1 gelée, au mapping unifié dérivé et à la doctrine GEL-GOV-02.

## 2. Hiérarchie documentaire
1. Spécification Modbus RTU V1 gelée ;
2. mapping unifié dérivé et gelé ;
3. fiches `source/` FT-ACC ;
4. tests génériques `detaille/` ;
5. tests instanciés `instancie/`.

Le mapping est la source opérationnelle d’instanciation ; il n’est pas une norme indépendante.

## 3. Décomposition gelée
| ID | Statut | Objet |
|---|---|---|
| FT-ACC-01 | RETIRÉE | Ancienne validation de lecture, transférée à FT-STR-06 gelée |
| FT-ACC-02 | ACTIVE | Écriture autorisée des champs RW |
| FT-ACC-03 | ACTIVE | Refus d’écriture des champs RO non réservés |
| FT-ACC-04 | ACTIVE | Refus d’écriture des zones réservées |
| FT-ACC-05 | ACTIVE | Absence d’effets de bord non spécifiés lors des écritures autorisées |
| FT-ACC-06 | ACTIVE | Rejet atomique des écritures composites invalides |
| FT-ACC-07 | ACTIVE | Consolidation de couverture et cohérence globale FT-ACC |

## 4. Doctrine des accès invalides
Toute requête Modbus invalide doit :
- produire une exception Modbus standard appropriée ;
- ne modifier aucun registre ni état interne ;
- ne jamais être exécutée partiellement ;
- produire un comportement déterministe.

Une écriture interdite acceptée silencieusement, même sans effet observable, est non conforme.

Une lecture d’un sous-ensemble valide n’est pas invalide du seul fait qu’elle est partielle.

Une valeur métier hors domaine écrite dans un registre RW valide ne relève pas automatiquement de FT-ACC ; son traitement relève de FT-LIM et/ou des règles fonctionnelles du bloc.

## 5. Couverture primaire
La classification primaire active du mapping logique comprend :
- 35 champs RW → FT-ACC-02 ;
- 129 champs RO non réservés → FT-ACC-03 ;
- 18 zones réservées → FT-ACC-04 ;
- total : 182 cibles logiques uniques.

FT-ACC-05 et FT-ACC-06 apportent des couvertures complémentaires respectivement sur les effets non spécifiés et sur l’atomicité des écritures composites invalides.

## 6. Critères de gel
FT-ACC est gelable lorsque :
- FT-ACC-01 est explicitement retirée et non exécutable ;
- les 182 cibles primaires sont couvertes sans doublon ni orphelin ;
- les 35 RW sont cohérents avec FT-ACC-02 et FT-ACC-05 ;
- les 129 RO non réservés sont cohérents avec FT-ACC-03 ;
- les 18 réservés sont cohérents avec FT-ACC-04 ;
- les frontières composites pertinentes sont couvertes par FT-ACC-06 ;
- FT-ACC-07 ne révèle aucune divergence entre V1, mapping et tests actifs ;
- aucun artefact actif ne conserve une doctrine antérieure incompatible avec GEL-GOV-02.
