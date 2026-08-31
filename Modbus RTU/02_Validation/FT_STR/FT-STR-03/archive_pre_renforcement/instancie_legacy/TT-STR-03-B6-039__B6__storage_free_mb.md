# TT-STR-03-B6-039 — Bloc 6 — storage_free_mb

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `storage_free_mb`.

## Référence mapping
- Adresse début : 6007
- Adresse fin : 6008
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 6007 et 6008.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
