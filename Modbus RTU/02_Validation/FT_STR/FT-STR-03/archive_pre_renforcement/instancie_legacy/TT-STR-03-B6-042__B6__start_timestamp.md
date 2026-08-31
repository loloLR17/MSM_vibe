# TT-STR-03-B6-042 — Bloc 6 — start_timestamp

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `start_timestamp`.

## Référence mapping
- Adresse début : 6016
- Adresse fin : 6017
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 6016 et 6017.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
