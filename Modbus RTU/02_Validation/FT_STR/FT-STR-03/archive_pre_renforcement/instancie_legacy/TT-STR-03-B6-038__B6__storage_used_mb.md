# TT-STR-03-B6-038 — Bloc 6 — storage_used_mb

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `storage_used_mb`.

## Référence mapping
- Adresse début : 6005
- Adresse fin : 6006
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 6005 et 6006.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
