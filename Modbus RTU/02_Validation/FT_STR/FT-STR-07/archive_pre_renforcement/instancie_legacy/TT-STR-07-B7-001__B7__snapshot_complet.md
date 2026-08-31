# TT-STR-07-B7-001 — Bloc 7 — Snapshot complet

## Objectif
Valider la stabilité d’image du bloc `7` par comparaison de 20 snapshots complets.

## Référence mapping
- Bloc : `7`
- Adresse début : `7000`
- Adresse fin : `7015`

## Préconditions
- capteur en état stable
- aucune acquisition active
- accès Modbus opérationnel

## Étapes
1. Lire le bloc complet de `7000` à `7015` 20 fois consécutivement.
2. Comparer tous les snapshots bit à bit.
3. Comparer le snapshot 1 au snapshot 20.
4. Signaler toute variation.

## Résultat attendu
- 20 snapshots identiques ;
- 0 variation ;
- aucune instabilité localisée.

## Critères d’acceptation
- identité bit à bit de tous les snapshots ;
- aucune variation locale ;
- aucune variation globale.
