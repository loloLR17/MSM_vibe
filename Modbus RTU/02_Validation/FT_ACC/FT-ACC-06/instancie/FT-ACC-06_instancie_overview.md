# FT-ACC-06 — Vue d'ensemble instanciée

## Couverture active

- Bloc 2 : 2 cas
- Bloc 4 : 12 cas
- Bloc 5 : 1 cas
- Bloc 6 : 2 cas
- **Total : 17 cas**

## Principe
Chaque cas couvre une frontière adjacente où une FC16 peut mêler au moins un registre RW et au moins un registre non inscriptible (RO ou réservé).

Les lectures partielles, franchissements de champs logiques en lecture, quantités FC03 invalides et adresses inexistantes en lecture sont exclus de FT-ACC-06 actif car déjà couverts par FT-STR-06 gelée.

## Critère central
Le rejet doit être atomique : aucun registre RW pourtant inclus dans la requête ne peut être modifié, et aucun effet interne ne peut être produit.

## Cas de commande B5
Le cas 5007-5008 impose `cmd_request_control = 0x0000` pour ne créer aucun front de commande pendant l'essai de rejet.
