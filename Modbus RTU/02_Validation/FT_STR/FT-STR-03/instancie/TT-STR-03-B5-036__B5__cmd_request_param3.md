# TT-STR-03-B5-036 — Bloc 5 — cmd_request_param3

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `cmd_request_param3`.

## Référence mapping
- Adresse début : 5004
- Adresse fin : 5005
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 5004 et 5005.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
