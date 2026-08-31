# TT-STR-07-B6-001 — Bloc 6 — Snapshot complet

## Objectif
Valider la stabilité d’image du bloc `6` par comparaison de 20 snapshots complets.

## Référence mapping
- Bloc : `6`
- Adresse début : `6000`
- Adresse fin : `6063`

## Préconditions
- capteur en état stable
- aucune acquisition active
- accès Modbus opérationnel

## Étapes
1. Lire le bloc complet de `6000` à `6063` 20 fois consécutivement.
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
