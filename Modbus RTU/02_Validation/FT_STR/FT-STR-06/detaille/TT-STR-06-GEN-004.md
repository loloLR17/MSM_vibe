# TT-STR-06-GEN-004 — Lectures invalides

## Objectif

Vérifier le rejet déterministe des requêtes FC03 invalides conformément à GEL-GOV-02 et au protocole Modbus.

## Cas à couvrir

- adresse de départ inexistante ;
- plage débutant dans une zone exposée mais traversant une lacune non exposée ;
- quantité nulle ;
- quantité supérieure à 125 registres.

## Procédure générique

Pour chaque cas, construire la requête invalide et l'exécuter sans autre modification d'état.

## Résultat attendu

- exception Modbus standard appropriée ;
- aucune réponse partielle contenant les registres valides précédant l'erreur ;
- aucun effet de bord sur les registres ou l'état interne ;
- comportement déterministe à requête identique.

## Important

Une lecture partielle d'un champ logique n'est pas invalide si toutes les adresses demandées sont exposées. Le test ne doit jamais confondre frontière de champ logique et frontière d'adressage valide.
