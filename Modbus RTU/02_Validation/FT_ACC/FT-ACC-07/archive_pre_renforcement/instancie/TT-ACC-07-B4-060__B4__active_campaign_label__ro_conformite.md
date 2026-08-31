# TT-ACC-07-B4-060 — Bloc 4 — active_campaign_label

## Objectif
Vérifier que le comportement observé du champ `active_campaign_label` est strictement conforme au mapping.

## Référence mapping
- Bloc : 4
- Adresse début : 4132
- Adresse fin : 4147
- Nombre de registres : 16
- Type déclaré : ASCII fixe
- Accès documentaire : RO
- Description : Miroir actif, ASCII fixe 32 caractères, padding 0x00

## Règle attendue
lecture OK ; écriture refusée avec exception explicite ; aucune modification mémoire

## Préconditions
- FT-STR validée
- FT-ACC-01 à FT-ACC-06 validées
- accès Modbus opérationnel
- système stable
- valeur initiale lisible si nécessaire

## Scénario / étapes
1. Lire exactement `16` registre(s) à partir de l’adresse `4132`.
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
