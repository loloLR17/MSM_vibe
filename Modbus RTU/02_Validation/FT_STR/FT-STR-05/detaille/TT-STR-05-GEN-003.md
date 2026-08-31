# TT-STR-05-GEN-003 — Absence de sentinelle implicite

## Objectif

Empêcher la réintroduction d'une convention globale non normative telle que `0 = absent`, `0 = invalide` ou `ID = 0 → non renseigné`.

## Procédure

1. Identifier toute règle de sentinelle utilisée par les documents ou tests actifs.
2. Pour chaque règle, retrouver sa définition explicite dans V1 pour le champ concerné.
3. Refuser toute généralisation à d'autres champs.
4. Si V1 ne définit pas la signification, classer le comportement `NON DÉFINI / À ARBITRER`.

## Critères

- aucune sentinelle globale implicite ;
- toute sentinelle active est traçable à une règle V1 champ par champ ;
- la simple valeur numérique `0` ne déclenche aucune interprétation métier supplémentaire sans règle V1.

## Exemple

Un champ pour lequel V1 définit explicitement `0 = invalide` peut utiliser cette sentinelle. Cette définition ne s'étend à aucun autre identifiant ou compteur.
