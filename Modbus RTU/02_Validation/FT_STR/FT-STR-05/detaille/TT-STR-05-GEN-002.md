# TT-STR-05-GEN-002 — Valeur structurelle nulle des zones réservées

## Objectif

Vérifier sur l'implémentation que chaque registre d'une zone réservée pour laquelle V1 impose la neutralité retourne `0x0000`.

## Procédure

1. Sélectionner une zone réservée issue de V1/GEL-MAP-V1.
2. Lire l'intégralité de la zone avec une opération Modbus valide.
3. Vérifier chaque registre retourné.
4. Répéter pour toutes les zones réservées couvertes.

## Critère

Chaque registre réservé soumis à la règle normative vaut exactement `0x0000`.

Une valeur non nulle est une non-conformité FT-STR-05.

## Séparation des responsabilités

- la stabilité temporelle de l'image n'est pas évaluée ici : FT-STR-07 ;
- les écritures sur réservés ne sont pas tentées ici : FT-ACC ;
- la validité d'une lecture partielle est traitée par FT-STR-06.
