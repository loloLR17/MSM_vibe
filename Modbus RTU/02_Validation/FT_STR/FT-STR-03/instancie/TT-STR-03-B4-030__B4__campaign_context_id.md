# TT-STR-03-B4-030 — Bloc 4 — campaign_context_id

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `campaign_context_id`.

## Référence mapping
- Adresse début : 4056
- Adresse fin : 4057
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 4056 et 4057.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
