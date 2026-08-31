# FT-ACC-05 — Fiche de spécification

## Absence d’effets de bord

- **ID** : FT-ACC-05
- **Criticité** : P0

## Objectif
Valider qu'une écriture sur une zone `RW` n'impacte que la cible explicitement adressée.

## Définition
Un effet de bord est toute modification observable d’un registre ou champ non ciblé explicitement par l’opération Modbus.

## Stratégie retenue
Snapshot bloc complet avant / après écriture, avec diff adressé.

## Critères d’acceptation
- aucune modification hors cible ;
- comportement déterministe ;
- aucune propagation non documentée.
