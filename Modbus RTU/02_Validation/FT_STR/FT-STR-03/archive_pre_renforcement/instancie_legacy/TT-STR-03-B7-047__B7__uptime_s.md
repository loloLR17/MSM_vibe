# TT-STR-03-B7-047 — Bloc 7 — uptime_s

## Objectif
Valider l'encodage uint32 (MSW puis LSW) du champ `uptime_s`.

## Référence mapping
- Adresse début : 7009
- Adresse fin : 7010
- Type : uint32

## Préconditions
- FT-STR-02 validée
- Accès Modbus opérationnel

## Étapes
1. Lire les registres 7009 et 7010.
2. Reconstituer la valeur en MSW/LSW.
3. Reconstituer la valeur en LSW/MSW (inverse).
4. Comparer les deux interprétations.

## Résultat attendu
- seule l'interprétation MSW/LSW est cohérente.

## Critères d’acceptation
- cohérence MSW/LSW validée
- incohérence LSW/MSW
