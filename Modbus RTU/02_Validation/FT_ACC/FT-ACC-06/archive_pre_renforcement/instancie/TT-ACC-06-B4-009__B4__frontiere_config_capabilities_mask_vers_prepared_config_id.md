# TT-ACC-06-B4-009 — Bloc 4 — frontiere_config_capabilities_mask_vers_prepared_config_id

## Objectif
Vérifier qu’une requête franchissant une frontière logique non autorisée est refusée avec exception explicite et sans effet mémoire.

## Référence mapping
- Champ amont : config_capabilities_mask (4001..4001, accès RO)
- Champ aval : prepared_config_id (4002..4003, accès RW)

## Construction du scénario
- Adresse de départ : 4001
- Longueur : 2 registre(s)
- La requête chevauche la frontière logique entre les deux champs.

## Étapes
1. Lire l’état mémoire de référence si l’opération testée est une écriture.
2. Exécuter une lecture invalide chevauchant la frontière logique.
3. Contrôler l’exception explicite.
4. Exécuter une écriture invalide chevauchant la même frontière.
5. Contrôler l’exception explicite.
6. Vérifier l’absence de modification mémoire.

## Résultat attendu
- exception explicite sur lecture invalide ;
- exception explicite sur écriture invalide ;
- aucune exécution partielle ;
- aucune modification mémoire.

## Critères d’acceptation
- cohérence stricte avec la doctrine Choix A ;
- refus déterministe ;
- absence d’effet mémoire.

## Criticité
P0
