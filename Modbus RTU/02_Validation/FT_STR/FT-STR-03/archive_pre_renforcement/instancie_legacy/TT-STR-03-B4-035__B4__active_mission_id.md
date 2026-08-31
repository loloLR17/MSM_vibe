# TT-STR-03-B4-035 — Bloc 4 — active_mission_id

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `active_mission_id`.

## Référence mapping
- Adresse début : 4130
- Adresse fin : 4131
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 4130 et 4131.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
