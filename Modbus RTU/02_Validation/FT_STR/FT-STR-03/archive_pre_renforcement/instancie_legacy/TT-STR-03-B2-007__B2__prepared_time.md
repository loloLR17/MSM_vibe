# TT-STR-03-B2-007 — Bloc 2 — prepared_time

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `prepared_time`.

## Référence mapping
- Adresse début : 2008
- Adresse fin : 2009
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 2008 et 2009.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
