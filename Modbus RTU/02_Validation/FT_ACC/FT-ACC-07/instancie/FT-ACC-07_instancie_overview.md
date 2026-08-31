# FT-ACC-07 — Vue de consolidation

## Objet
Contrôle final de couverture croisée de la famille FT-ACC.

## Couverture primaire
- 35 champs RW → FT-ACC-02 ;
- 129 champs RO non réservés → FT-ACC-03 ;
- 18 zones réservées → FT-ACC-04 ;
- total : **182 cibles logiques uniques**.

## Couvertures complémentaires
- FT-ACC-05 : effets de bord non spécifiés sur les écritures autorisées ;
- FT-ACC-06 : rejet atomique des écritures composites invalides ;
- FT-STR-06 : accessibilité en lecture et découpage FC03, couverture gelée non rejouée.

## Règles de consolidation
Un verdict conforme exige :
- 182 lignes détaillées dans `FT-ACC-07_matrice_couverture.csv` ;
- aucune cible primaire orpheline ;
- aucune cible classée dans deux classes primaires ;
- une seule occurrence canonique de l’adresse 4017 ;
- `B3_RESERVED_0` classé réservé ;
- aucune tolérance « écriture interdite acceptée mais ignorée » dans le chemin actif ;
- cohérence documentaire avec GEL-GOV-02 et `FT_ACC/Specifications.md`.
