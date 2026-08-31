# FT-ACC-03 — Refus d’écriture sur les champs RO

## Objectif
Démontrer que chaque champ logique déclaré **RO non réservé** dans le mapping V1 est effectivement non inscriptible par Modbus.

## Préconditions
- la cible existe dans le mapping V1 ;
- son accès est déclaré `RO` ;
- la cible n’est pas une zone réservée ;
- le comportement autonome normal de la cible est connu ou observable lorsque la valeur est dynamique.

## Règle normative
Toute tentative d’écriture sur une cible RO est un accès Modbus invalide au sens de GEL-GOV-02.

Le résultat attendu est obligatoirement :
1. une exception Modbus standard appropriée ;
2. aucune modification de la cible **imputable à la requête rejetée** ;
3. aucune modification d’un autre registre ou état interne imputable à la requête rejetée ;
4. aucune exécution partielle ;
5. un comportement déterministe pour une même requête dans un même état pertinent.

Une acceptation silencieuse, même sans modification observable, est un échec.

## Cibles stables et cibles dynamiques
Deux modes d’observation sont distingués :

### Cible stable
Lorsque la V1 et le contexte d’essai permettent de considérer la valeur stable pendant la fenêtre de test, la relecture après rejet doit être identique à la référence avant tentative.

### Cible dynamique
Lorsque la cible peut évoluer de manière autonome (`uptime`, temps courant, télémétrie, états calculés, compteurs, etc.), une égalité bit à bit avant/après n’est **pas** exigée.

Il faut démontrer que :
- la tentative rejetée n’a pas forcé la valeur écrite ;
- l’évolution éventuelle reste compatible avec le comportement autonome normal ;
- aucun changement causal spécifique à la requête rejetée n’est observé.

Ainsi, GEL-GOV-02 impose l’absence d’effet de l’écriture rejetée, pas l’immobilité temporelle d’une donnée naturellement dynamique.

## Stratégie de test
- champ RO mono-registre : tentative d’écriture unitaire, notamment FC06 lorsqu’elle est supportée ;
- champ RO multi-registres : tentative d’écriture du champ logique complet avec FC16 lorsqu’elle est supportée ;
- répétition possible pour confirmer le caractère déterministe.

L’index instancié exhaustif identifie les 129 cibles RO non réservées. Les critères d’acceptation sont portés par les génériques afin d’éviter des divergences entre fiches répétitives.

## Hors périmètre
- zones réservées : FT-ACC-04 ;
- écritures composites mêlant plusieurs classes d’accès : FT-ACC-06 ;
- adresses inexistantes en lecture et structure d’exposition : FT-STR-06 ;
- validité métier des valeurs : FT-LIM ;
- cohérence structurelle : FT-STR.
