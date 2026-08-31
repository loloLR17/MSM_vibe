# TT-STR-07-B7-004 — Bloc 7 — last_fault_timestamp

## Objectif
Valider la cohérence multi-registres du champ `uint32` `last_fault_timestamp` en état stable.

## Référence mapping
- Adresse début : `7004`
- Adresse fin : `7005`

## Préconditions
- capteur en état stable
- accès Modbus opérationnel

## Étapes
1. Lire le couple de registres 20 fois consécutivement.
2. Reconstruire la valeur `MSW<<16 | LSW` à chaque lecture.
3. Comparer toutes les reconstructions.
4. Alterner lecture de ce champ avec lecture d’un autre champ du bloc.
5. Vérifier que la reconstruction reste identique.

## Résultat attendu
- 0 incohérence de reconstruction ;
- 0 variation ;
- aucun mismatch observable `MSW/LSW`.

## Critères d’acceptation
- toutes les reconstructions identiques ;
- aucune valeur incohérente ;
- aucune divergence après alternance.
