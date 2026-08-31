# TT-STR-03-B4-029 — Bloc 4 — storage_limit_mb

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `storage_limit_mb`.

## Référence mapping
- Adresse début : 4026
- Adresse fin : 4027
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 4026 et 4027.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
