# TT-STR-03-B4-026 — Bloc 4 — active_config_crc

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `active_config_crc`.

## Référence mapping
- Adresse début : 4010
- Adresse fin : 4011
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 4010 et 4011.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
