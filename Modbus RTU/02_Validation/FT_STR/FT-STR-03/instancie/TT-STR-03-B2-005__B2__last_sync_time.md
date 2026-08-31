# TT-STR-03-B2-005 — Bloc 2 — last_sync_time

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `last_sync_time`.

## Référence mapping
- Adresse début : 2004
- Adresse fin : 2005
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 2004 et 2005.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
