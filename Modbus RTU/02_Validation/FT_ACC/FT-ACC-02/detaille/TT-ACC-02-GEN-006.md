# TT-ACC-02-GEN-006 — Cohérence permission V1 ↔ mapping ↔ comportement

## Objectif
Vérifier qu'une cible déclarée RW par V1 est correctement instanciée comme RW dans GEL-MAP-V1 et qu'une écriture nominale autorisée n'est pas refusée par l'implémentation.

## Étapes
1. Identifier la cible dans V1.
2. Vérifier l'attribut d'accès correspondant dans GEL-MAP-V1.
3. Exécuter le scénario nominal adapté au type et à la sémantique du champ.
4. Comparer le comportement observé au droit d'accès attendu.

## Résultat attendu
Aucune divergence V1 ↔ mapping ↔ comportement n'est observée.

## Classification
- divergence V1 ↔ mapping : anomalie documentaire du mapping dérivé ;
- divergence mapping/V1 ↔ implémentation : anomalie d'implémentation ;
- validité métier de la valeur : hors FT-ACC-02.
