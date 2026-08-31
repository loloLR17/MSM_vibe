# TT-STR-03-B4-028 — Bloc 4 — campaign_duration_s

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `campaign_duration_s`.

## Référence mapping
- Adresse début : 4023
- Adresse fin : 4024
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 4023 et 4024.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
