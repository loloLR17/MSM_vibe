# TT-STR-03-B3-020 — Bloc 3 — B3_PEAK_Z_MG

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `B3_PEAK_Z_MG`.

## Référence mapping
- Adresse début : 3028
- Adresse fin : 3029
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 3028 et 3029.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
