# TT-STR-03-B3-008 — Bloc 3 — B3_LAST_UPDATE_TR2

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `B3_LAST_UPDATE_TR2`.

## Référence mapping
- Adresse début : 3004
- Adresse fin : 3005
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 3004 et 3005.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
