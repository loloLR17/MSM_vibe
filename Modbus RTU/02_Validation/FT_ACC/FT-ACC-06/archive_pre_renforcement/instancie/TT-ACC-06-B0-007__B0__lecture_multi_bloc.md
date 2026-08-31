# TT-ACC-06-B0-007 — Bloc 0 — lecture_multi_bloc

## Objectif
Vérifier qu’une requête lecture invalide est refusée avec exception explicite et sans effet mémoire.

## Référence mapping
- Bloc de référence : 0
- Plage valide du bloc : 0..20
- Adresse requête : 20
- Longueur requête : 981

## Justification du scénario
Lecture franchissant la frontière bloc 0 -> bloc 1

## Préconditions
- FT-STR validée
- FT-ACC-01 à FT-ACC-05 validées
- accès Modbus opérationnel
- état mémoire stable avant tentative

## Étapes
1. Lire la zone de référence utile avant tentative si l’opération est une écriture.
2. Exécuter la requête lecture invalide à l’adresse `20` sur `981` registre(s).
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
