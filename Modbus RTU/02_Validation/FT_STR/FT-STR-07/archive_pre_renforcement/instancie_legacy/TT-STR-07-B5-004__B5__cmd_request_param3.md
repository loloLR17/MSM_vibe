# TT-STR-07-B5-004 — Bloc 5 — cmd_request_param3

## Objectif
Valider la cohérence multi-registres du champ `uint32` `cmd_request_param3` en état stable.

## Référence mapping
- Adresse début : `5004`
- Adresse fin : `5005`

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
