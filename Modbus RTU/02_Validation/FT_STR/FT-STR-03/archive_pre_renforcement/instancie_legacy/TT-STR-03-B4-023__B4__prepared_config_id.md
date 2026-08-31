# TT-STR-03-B4-023 — Bloc 4 — prepared_config_id

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `prepared_config_id`.

## Référence mapping
- Adresse début : 4002
- Adresse fin : 4003
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 4002 et 4003.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
