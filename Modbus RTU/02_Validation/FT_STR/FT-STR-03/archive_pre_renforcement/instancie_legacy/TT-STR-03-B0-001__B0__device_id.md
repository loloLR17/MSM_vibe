# TT-STR-03-B0-001 — Bloc 0 — device_id

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `device_id`.

## Référence mapping
- Adresse début : 0
- Adresse fin : 1
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 0 et 1.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
