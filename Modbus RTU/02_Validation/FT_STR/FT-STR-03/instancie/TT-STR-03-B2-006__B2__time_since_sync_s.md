# TT-STR-03-B2-006 — Bloc 2 — time_since_sync_s

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `time_since_sync_s`.

## Référence mapping
- Adresse début : 2006
- Adresse fin : 2007
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 2006 et 2007.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
