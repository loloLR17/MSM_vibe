# TT-STR-03-B1-003 — Bloc 1 — active_campaign_id

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `active_campaign_id`.

## Référence mapping
- Adresse début : 1013
- Adresse fin : 1014
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 1013 et 1014.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
