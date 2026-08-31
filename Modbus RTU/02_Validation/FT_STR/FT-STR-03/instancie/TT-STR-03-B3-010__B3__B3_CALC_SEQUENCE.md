# TT-STR-03-B3-010 — Bloc 3 — B3_CALC_SEQUENCE

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `B3_CALC_SEQUENCE`.

## Référence mapping
- Adresse début : 3008
- Adresse fin : 3009
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 3008 et 3009.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
