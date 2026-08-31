# TT-STR-03-B4-024 — Bloc 4 — active_config_id

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `active_config_id`.

## Référence mapping
- Adresse début : 4004
- Adresse fin : 4005
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 4004 et 4005.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
