# TT-STR-07-B1-001 — Bloc 1 — Snapshot complet

## Objectif
Valider la stabilité d’image du bloc `1` par comparaison de 20 snapshots complets.

## Référence mapping
- Bloc : `1`
- Adresse début : `1000`
- Adresse fin : `1019`

## Préconditions
- capteur en état stable
- aucune acquisition active
- accès Modbus opérationnel

## Étapes
1. Lire le bloc complet de `1000` à `1019` 20 fois consécutivement.
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
