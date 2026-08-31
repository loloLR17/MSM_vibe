# TT-ACC-03-GEN-001 — Refus d’écriture d’un champ RO mono-registre

## Objectif
Vérifier qu’un champ logique RO non réservé occupant un seul registre refuse toute écriture.

## Préconditions
- cible déclarée RO et non réservée ;
- fonction d’écriture unitaire supportée ;
- comportement autonome de la cible connu lorsque la donnée est dynamique.

## Étapes
1. Lire la cible et enregistrer la référence initiale.
2. Émettre une tentative d’écriture unitaire avec une valeur de test distincte, notamment via FC06 lorsque supportée.
3. Vérifier la réponse Modbus.
4. Relire la cible et les éventuels états associés utiles au diagnostic.
5. Répéter si nécessaire dans des préconditions équivalentes.

## Résultat attendu
- exception Modbus standard appropriée ;
- aucune modification imputable à la requête rejetée ;
- aucun effet de bord ;
- aucune exécution partielle ;
- comportement déterministe.

### Interprétation de la relecture
- **cible stable** : valeur après rejet identique à la référence initiale ;
- **cible dynamique** : l’évolution autonome est admise si elle reste cohérente avec le fonctionnement normal et si la valeur écrite n’est pas imposée par la tentative rejetée.

L’égalité temporelle stricte n’est donc pas un critère universel pour les RO dynamiques.
