# FT-ACC-04 — Interdiction d’écriture des zones réservées

## Identification
- ID : FT-ACC-04
- Famille : FT-ACC
- Criticité : P0

## Objectif
Démontrer que toute tentative d’écriture sur une zone réservée exposée par le mapping est rejetée conformément à GEL-GOV-02.

## Règle normative
Pour toute requête d’écriture visant un registre réservé :
1. la requête est rejetée par une exception Modbus standard appropriée ;
2. aucun registre n’est modifié ;
3. aucun état interne n’est modifié du fait de la requête ;
4. aucune partie de la requête n’est exécutée ;
5. le comportement est déterministe et répétable.

Une écriture acceptée silencieusement, même sans effet observable, est non conforme.

## Périmètre inclus
- réservés unitaires ;
- zones réservées multi-registres ;
- écriture unitaire lorsque la cible logique ne contient qu’un registre ;
- écriture de la zone logique complète lorsqu’elle contient plusieurs registres.

## Hors périmètre
- valeur nominale de lecture des réservés : FT-STR ;
- champs RO non réservés : FT-ACC-03 ;
- adresse inexistante, débordement ou requête composite mêlant zones autorisées et interdites : FT-ACC-06 ;
- validité métier : FT-LIM.

## Règle d’instanciation
Le mapping gelé est la source opérationnelle des zones réservées. Une seule fiche active est créée par zone logique réservée unique ; aucun doublon d’adresse ou d’alias documentaire ne doit générer un second test logique.

## Critère de réussite
FT-ACC-04 est satisfaite si toutes les zones réservées instanciées refusent l’écriture selon la règle ci-dessus, sans mutation ni exécution partielle.
