# TT-STR-03-B3-014 — Bloc 3 — B3_PEAK_GLOBAL_MG

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `B3_PEAK_GLOBAL_MG`.

## Référence mapping
- Adresse début : 3016
- Adresse fin : 3017
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 3016 et 3017.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
