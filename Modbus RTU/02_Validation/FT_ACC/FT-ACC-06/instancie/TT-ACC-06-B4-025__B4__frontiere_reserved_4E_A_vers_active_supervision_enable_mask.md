# TT-ACC-06-B4-025 — Bloc 4 — frontiere_reserved_4E_A_vers_active_supervision_enable_mask

## Objectif
Vérifier qu’une requête franchissant une frontière logique non autorisée est refusée avec exception explicite et sans effet mémoire.

## Référence mapping
- Champ amont : reserved_4E_A (4111..4115, accès RO)
- Champ aval : active_supervision_enable_mask (4116..4116, accès RO)

## Construction du scénario
- Adresse de départ : 4115
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
