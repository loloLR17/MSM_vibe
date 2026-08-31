# TT-ACC-06-B1-007 — Bloc 1 — lecture_multi_bloc

## Objectif
Vérifier qu’une requête lecture invalide est refusée avec exception explicite et sans effet mémoire.

## Référence mapping
- Bloc de référence : 1
- Plage valide du bloc : 1000..1019
- Adresse requête : 1019
- Longueur requête : 982

## Justification du scénario
Lecture franchissant la frontière bloc 1 -> bloc 2

## Préconditions
- FT-STR validée
- FT-ACC-01 à FT-ACC-05 validées
- accès Modbus opérationnel
- état mémoire stable avant tentative

## Étapes
1. Lire la zone de référence utile avant tentative si l’opération est une écriture.
2. Exécuter la requête lecture invalide à l’adresse `1019` sur `982` registre(s).
3. Contrôler la présence d’une exception Modbus explicite.
4. Si l’opération est une écriture, relire la zone de référence.
5. Vérifier l’absence de modification mémoire.

## Résultat attendu
- exception Modbus explicite obligatoire ;
- aucune exécution partielle ;
- aucune modification mémoire.

## Critères d’acceptation
- refus clair ;
- comportement déterministe ;
- absence d’effet mémoire.

## Traces à conserver
- trame de requête ;
- exception retournée ;
- snapshot mémoire avant/après si écriture ;
- verdict.

## Criticité
P0
