# TT-ACC-01-B5-005 — Bloc 5 — cmd_request_param3

## Objectif
Vérifier que le champ logique multi-registres `cmd_request_param3` est accessible en lecture complète sur 2 registres.

## Exigence(s) couverte(s)
- FT-ACC-01
- Mapping unifié logique TR2
- Attribut d’accès : `RW`

## Référence mapping
- Adresse début : 5004
- Adresse fin : 5005
- Type déclaré : uint32
- Nombre de registres : 2
- Description : Paramètre 3, mot fort

## Préconditions
- FT-STR validée
- Accès Modbus opérationnel
- Simulateur en état nominal stable
- Aucun changement d’état métier pendant le test

## Données d’entrée
- Adresse de départ : 5004
- Longueur : 2

## Scénario / étapes
1. Lire les registres 5004 à 5005.
2. Contrôler l’absence d’exception Modbus.
3. Vérifier que la réponse contient exactement 2 registres.

## Résultat attendu
- la lecture est acceptée ;
- aucune exception Modbus n’est renvoyée ;
- la réponse contient exactement 2 registre(s) ;
- le champ `cmd_request_param3` est lisible conformément au mapping.

## Critères d’acceptation
- lecture réussie ;
- longueur conforme ;
- comportement cohérent avec l’attribut d’accès `RW` ;
- aucune divergence mapping ↔ comportement réel.

## Mode d’exécution
- simulateur déterministe
- automatisable

## Automatisation possible
Oui

## Traces à conserver
- trame requête ;
- trame réponse ;
- valeur(s) brute(s) lue(s) ;
- verdict ;
- anomalie associée le cas échéant.

## Niveau de criticité
P0

## Remarques / limites
- cette fiche valide l’accessibilité en lecture ;
- la sémantique de la valeur et le décodage détaillé sont hors périmètre FT-ACC-01.
