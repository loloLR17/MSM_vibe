# TT-STR-07-B4-001 — Bloc 4 — Snapshot complet

## Objectif
Valider la stabilité d’image du bloc `4` par comparaison de 20 snapshots complets.

## Référence mapping
- Bloc : `4`
- Adresse début : `4000`
- Adresse fin : `4175`

## Préconditions
- capteur en état stable
- aucune acquisition active
- accès Modbus opérationnel

## Étapes
1. Lire le bloc complet de `4000` à `4175` 20 fois consécutivement.
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
