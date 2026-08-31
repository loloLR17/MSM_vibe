# TT-ACC-01-B4-049 — Bloc 4 — reserved_4E_A

## Objectif
Vérifier que le champ logique multi-registres `reserved_4E_A` est accessible en lecture complète sur 5 registres.

## Exigence(s) couverte(s)
- FT-ACC-01
- Mapping unifié logique TR2
- Attribut d’accès : `RO`

## Référence mapping
- Adresse début : 4111
- Adresse fin : 4115
- Type déclaré : uint16[5]
- Nombre de registres : 5
- Description : Réservé, doit lire 0

## Préconditions
- FT-STR validée
- Accès Modbus opérationnel
- Simulateur en état nominal stable
- Aucun changement d’état métier pendant le test

## Données d’entrée
- Adresse de départ : 4111
- Longueur : 5

## Scénario / étapes
1. Lire les registres 4111 à 4115.
2. Contrôler l’absence d’exception Modbus.
3. Vérifier que la réponse contient exactement 5 registres.

## Résultat attendu
- la lecture est acceptée ;
- aucune exception Modbus n’est renvoyée ;
- la réponse contient exactement 5 registre(s) ;
- le champ `reserved_4E_A` est lisible conformément au mapping.

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
