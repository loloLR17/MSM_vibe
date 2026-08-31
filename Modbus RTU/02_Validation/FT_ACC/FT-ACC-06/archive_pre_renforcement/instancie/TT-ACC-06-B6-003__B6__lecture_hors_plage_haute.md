# TT-ACC-06-B6-003 — Bloc 6 — lecture_hors_plage_haute

## Objectif
Vérifier qu’une requête lecture invalide est refusée avec exception explicite et sans effet mémoire.

## Référence mapping
- Bloc de référence : 6
- Plage valide du bloc : 6000..6063
- Adresse requête : 6064
- Longueur requête : 1

## Justification du scénario
Adresse 6064 après fin du bloc 6 (6000..6063)

## Préconditions
- FT-STR validée
- FT-ACC-01 à FT-ACC-05 validées
- accès Modbus opérationnel
- état mémoire stable avant tentative

## Étapes
1. Lire la zone de référence utile avant tentative si l’opération est une écriture.
2. Exécuter la requête lecture invalide à l’adresse `6064` sur `1` registre(s).
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
