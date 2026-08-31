# TT-ACC-01-B0-008 — Bloc 0 — serial_number

## Objectif
Vérifier que le champ ASCII fixe `serial_number` est accessible en lecture complète sur 8 registres.

## Exigence(s) couverte(s)
- FT-ACC-01
- Mapping unifié logique TR2
- Attribut d’accès : `RO`

## Référence mapping
- Adresse début : 8
- Adresse fin : 15
- Type déclaré : ASCII fixe
- Nombre de registres : 8
- Description : Numéro de série

## Préconditions
- FT-STR validée
- Accès Modbus opérationnel
- Simulateur en état nominal stable
- Aucun changement d’état métier pendant le test

## Données d’entrée
- Adresse de départ : 8
- Longueur : 8

## Scénario / étapes
1. Lire les registres 8 à 15.
2. Contrôler l’absence d’exception Modbus.
3. Vérifier que la réponse contient exactement 8 registres.

## Résultat attendu
- la lecture est acceptée ;
- aucune exception Modbus n’est renvoyée ;
- la réponse contient exactement 8 registre(s) ;
- le champ `serial_number` est lisible conformément au mapping.

## Critères d’acceptation
- lecture réussie ;
- longueur conforme ;
- comportement cohérent avec l’attribut d’accès `RO` ;
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
