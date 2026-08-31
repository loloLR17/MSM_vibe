# TT-STR-03-B7-046 — Bloc 7 — last_fault_timestamp

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `last_fault_timestamp`.

## Référence mapping
- Adresse début : 7004
- Adresse fin : 7005
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 7004 et 7005.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
