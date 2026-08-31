# TT-ACC-06-B7-007 — Bloc 7 — frontiere_supply_voltage_mV_vers_reserved_7A

## Objectif
Vérifier qu’une requête franchissant une frontière logique non autorisée est refusée avec exception explicite et sans effet mémoire.

## Référence mapping
- Champ amont : supply_voltage_mV (7013..7013, accès RO)
- Champ aval : reserved_7A (7014..7015, accès RO)

## Construction du scénario
- Adresse de départ : 7013
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
