# TT-STR-03-B5-037 — Bloc 5 — cmd_last_timestamp

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `cmd_last_timestamp`.

## Référence mapping
- Adresse début : 5018
- Adresse fin : 5019
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 5018 et 5019.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
