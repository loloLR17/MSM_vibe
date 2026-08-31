# TT-ACC-07-B3-014 — Bloc 3 — B3_RMS_Z_MG

## Objectif
Vérifier que le comportement observé du champ `B3_RMS_Z_MG` est strictement conforme au mapping.

## Référence mapping
- Bloc : 3
- Adresse début : 3022
- Adresse fin : 3023
- Nombre de registres : 2
- Type déclaré : uint32
- Accès documentaire : RO
- Description : RMS axe Z

## Règle attendue
lecture OK ; écriture refusée avec exception explicite ; aucune modification mémoire

## Préconditions
- FT-STR validée
- FT-ACC-01 à FT-ACC-06 validées
- accès Modbus opérationnel
- système stable
- valeur initiale lisible si nécessaire

## Scénario / étapes
1. Lire exactement `2` registre(s) à partir de l’adresse `3022`.
2. Tenter une écriture bornée à la même plage avec une valeur de test adaptée.
3. Observer le comportement de l’écriture ou de la tentative d’écriture.
4. Relire la même plage.
5. Comparer le comportement observé à la règle attendue issue du mapping.

## Résultat attendu
- comportement strictement conforme au mapping ;
- verdict non ambigu ;
- stabilité sur répétition.

## Critères d’acceptation
- lecture conforme ;
- écriture ou refus conforme ;
- cohérence write → read ou non-effet conforme au type d’accès ;
- aucune divergence documentaire non justifiée.

## Traces à conserver
- trame de lecture initiale ;
- trame d’écriture ou tentative d’écriture ;
- exception éventuelle ;
- trame de relecture ;
- verdict.

## Criticité
P0
