# TT-STR-01-GLOBAL-001 — Cohérence globale inter-blocs

## Traçabilité

- Test générique : `TT-STR-01-GEN-003`
- Mapping : GEL-MAP-V1
- Gel mapping : `ff948e5917becceed7637d9c7864ec9b279be0ca`

## Objectif

Valider la cohérence structurelle globale des huit blocs Modbus définis par V1 et représentés dans GEL-MAP-V1.

## Référence de couverture

| Bloc | Début | Fin |
|---|---:|---:|
| B0 | 0 | 20 |
| B1 | 1000 | 1019 |
| B2 | 2000 | 2015 |
| B3 | 3000 | 3047 |
| B4 | 4000 | 4175 |
| B5 | 5000 | 5019 |
| B6 | 6000 | 6063 |
| B7 | 7000 | 7015 |

## Contrôles

1. Vérifier l'ordre croissant des blocs.
2. Vérifier l'absence de chevauchement inter-bloc.
3. Identifier les intervalles non exposés entre blocs.
4. Vérifier qu'aucun intervalle non exposé n'est interprété comme une plage implicite.

## Intervalles inter-blocs non exposés

- B0 → B1 : `21..999`
- B1 → B2 : `1020..1999`
- B2 → B3 : `2016..2999`
- B3 → B4 : `3048..3999`
- B4 → B5 : `4176..4999`
- B5 → B6 : `5020..5999`
- B6 → B7 : `6064..6999`

Ces intervalles découlent des bornes V1. Ils ne constituent pas des trous internes aux blocs et leur seule existence n'est pas une non-conformité FT-STR-01.

FT-STR-01 ne définit pas le comportement d'une requête Modbus visant ces adresses.

## Résultat attendu

- huit blocs présents dans la représentation dérivée ;
- aucun chevauchement inter-bloc ;
- sept intervalles non exposés explicitement identifiés ;
- aucune plage implicite entre blocs.

## Critères d'acceptation

- bornes conformes à V1/GEL-MAP-V1 ;
- aucune ambiguïté de portée ;
- aucun chevauchement ;
- intervalles inter-blocs correctement distingués des trous internes.

## Classification

- Famille : `FT-STR-01`
- Niveau : `instancié`
- Générique associé : `TT-STR-01-GEN-003`
