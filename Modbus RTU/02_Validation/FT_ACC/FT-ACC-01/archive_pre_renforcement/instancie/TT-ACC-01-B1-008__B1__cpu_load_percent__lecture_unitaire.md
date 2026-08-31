# TT-ACC-01-B1-008 — Bloc 1 — cpu_load_percent

## Objectif
Vérifier que le champ logique `cpu_load_percent` déclaré lisible est accessible en lecture unitaire.

## Exigence(s) couverte(s)
- FT-ACC-01
- Mapping unifié logique TR2
- Attribut d’accès : `RO`

## Référence mapping
- Adresse début : 1008
- Adresse fin : 1008
- Type déclaré : uint16
- Nombre de registres : 1
- Description : Charge CPU (%)

## Préconditions
- FT-STR validée
- Accès Modbus opérationnel
- Simulateur en état nominal stable
- Aucun changement d’état métier pendant le test

## Données d’entrée
- Adresse de départ : 1008
- Longueur : 1

## Scénario / étapes
1. Lire le registre 1008.
2. Contrôler l’absence d’exception Modbus.
3. Vérifier que la réponse contient exactement 1 registre.

## Résultat attendu
- la lecture est acceptée ;
- aucune exception Modbus n’est renvoyée ;
- la réponse contient exactement 1 registre(s) ;
- le champ `cpu_load_percent` est lisible conformément au mapping.

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
