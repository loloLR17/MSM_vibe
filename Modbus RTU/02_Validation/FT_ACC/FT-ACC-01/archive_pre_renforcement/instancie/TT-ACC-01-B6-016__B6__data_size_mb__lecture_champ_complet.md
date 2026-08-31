# TT-ACC-01-B6-016 — Bloc 6 — data_size_mb

## Objectif
Vérifier que le champ logique multi-registres `data_size_mb` est accessible en lecture complète sur 2 registres.

## Exigence(s) couverte(s)
- FT-ACC-01
- Mapping unifié logique TR2
- Attribut d’accès : `RO`

## Référence mapping
- Adresse début : 6023
- Adresse fin : 6024
- Type déclaré : uint32
- Nombre de registres : 2
- Description : Taille données

## Préconditions
- FT-STR validée
- Accès Modbus opérationnel
- Simulateur en état nominal stable
- Aucun changement d’état métier pendant le test

## Données d’entrée
- Adresse de départ : 6023
- Longueur : 2

## Scénario / étapes
1. Lire les registres 6023 à 6024.
2. Contrôler l’absence d’exception Modbus.
3. Vérifier que la réponse contient exactement 2 registres.

## Résultat attendu
- la lecture est acceptée ;
- aucune exception Modbus n’est renvoyée ;
- la réponse contient exactement 2 registre(s) ;
- le champ `data_size_mb` est lisible conformément au mapping.

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
