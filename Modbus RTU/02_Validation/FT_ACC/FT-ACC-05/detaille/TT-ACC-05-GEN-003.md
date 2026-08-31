# TT-ACC-05-GEN-003 — Préparation de commande sans déclenchement

## Objectif
Vérifier que la préparation des registres de requête du Bloc 5 ne déclenche aucune commande en l’absence de front `submit`.

## Préconditions
- aucune commande active ;
- `cmd_request_control.submit = 0` ;
- cible RW de la zone de requête.

## Étapes
1. Capturer le Bloc 5 avant écriture.
2. Écrire une valeur nominale sur la cible en maintenant `submit = 0`.
3. Capturer le Bloc 5 après écriture.
4. Vérifier qu’aucune commande n’a été prise en compte.

## Résultat attendu
La zone de requête reflète la préparation autorisée ; aucun changement de statut/historique imputable à une exécution de commande n’est observé. Pour `cmd_request_control`, utiliser `0x0000` afin de ne générer aucun front `submit`, `cancel` ou `clear`.

## Criticité
P0
