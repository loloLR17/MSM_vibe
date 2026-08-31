# TT-STR-01-GEN-003 — Cohérence globale inter-blocs

## Objectif

Vérifier génériquement que l'ensemble des blocs définis par V1 forme une cartographie non ambiguë et sans chevauchement.

## Entrées

- bornes normatives de tous les blocs ;
- représentation dérivée correspondante.

## Contrôles

1. Ordonner les blocs par adresse de début.
2. Vérifier que chaque bloc se termine avant le début du suivant.
3. Identifier explicitement chaque intervalle non exposé entre deux blocs successifs.
4. Vérifier qu'aucun intervalle inter-blocs n'est interprété comme un champ, un bloc ou une plage implicite.

## Règle importante

Un intervalle non exposé entre deux blocs n'est pas un trou interne et n'est pas une anomalie structurelle lorsqu'il résulte des bornes définies par V1.

Le comportement d'une requête adressant un tel intervalle n'est pas déterminé par FT-STR-01 ; il relève de la doctrine des accès invalides et des familles de validation concernées.

## Résultat attendu

Aucun chevauchement inter-bloc et une identification non ambiguë de toutes les plages exposées et non exposées.

## Instanciation

Ce test est instancié par `TT-STR-01-GLOBAL-001`.
