# TT-ACC-05-GEN-001 — Écriture RW sans effet fonctionnel associé

## Objectif
Vérifier qu’une écriture RW nominale ne modifie que sa cible et les éventuelles évolutions autonomes démontrées comme indépendantes.

## Préconditions
- FT-STR gelée ; FT-ACC-02 validée ;
- cible RW identifiée par le mapping ;
- valeur nominale maîtrisée ;
- état de référence capturable.

## Étapes
1. Capturer l’état pertinent avant écriture.
2. Écrire la valeur nominale sur la cible.
3. Capturer l’état après écriture.
4. Classer toutes les différences observées.

## Résultat attendu
Aucun changement hors cible n’est imputable à l’écriture, sauf effet V1 explicitement autorisé ou évolution autonome démontrée.

## Criticité
P0
