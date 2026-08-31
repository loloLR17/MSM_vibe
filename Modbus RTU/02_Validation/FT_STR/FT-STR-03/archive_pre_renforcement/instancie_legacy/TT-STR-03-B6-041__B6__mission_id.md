# TT-STR-03-B6-041 — Bloc 6 — mission_id

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `mission_id`.

## Référence mapping
- Adresse début : 6014
- Adresse fin : 6015
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 6014 et 6015.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
