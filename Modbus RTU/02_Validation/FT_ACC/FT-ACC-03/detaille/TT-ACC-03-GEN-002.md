# TT-ACC-03-GEN-002 — Refus d’écriture d’un champ RO multi-registres

## Objectif
Vérifier qu’un champ logique RO non réservé occupant plusieurs registres refuse une écriture complète du champ.

## Préconditions
- cible déclarée RO et non réservée ;
- écriture multiple FC16 supportée ;
- comportement autonome de la cible connu lorsque la donnée est dynamique.

## Étapes
1. Lire le champ complet et enregistrer la référence initiale.
2. Préparer un motif d’écriture distinct et cohérent avec la largeur du champ.
3. Émettre une FC16 couvrant exactement le champ logique complet.
4. Vérifier la réponse Modbus.
5. Relire le champ complet et les éventuels états associés utiles au diagnostic.
6. Répéter si nécessaire dans des préconditions équivalentes.

## Résultat attendu
- exception Modbus standard appropriée ;
- aucun mot du champ n’est modifié par la requête rejetée ;
- aucun autre registre ou état interne n’est modifié par cette requête ;
- aucune exécution partielle ;
- comportement déterministe.

### Interprétation de la relecture
- **cible stable** : le champ complet reste identique à la référence initiale ;
- **cible dynamique** : une évolution autonome cohérente est admise, mais la tentative rejetée ne doit imposer aucun des mots écrits ni provoquer de transition causale spécifique.
