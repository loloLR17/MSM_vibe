# TT-STR-06-GEN-002 — Lecture contiguë multi-champs

## Objectif

Vérifier qu'une lecture FC03 peut couvrir plusieurs champs logiques voisins lorsque toutes les adresses de la plage demandée sont exposées.

## Procédure générique

1. Sélectionner deux ou plusieurs champs logiques contigus dans un même bloc.
2. Construire une requête dont l'adresse de départ se trouve dans le premier champ et dont la fin se trouve dans un champ suivant.
3. Vérifier que toutes les adresses de la plage sont exposées.
4. Exécuter la lecture.

## Résultat attendu

- réponse FC03 normale ;
- aucune dépendance au découpage logique des champs ;
- quantité retournée égale à la quantité demandée.

## Critère d'échec

Rejet d'une plage entièrement exposée uniquement parce qu'elle traverse une frontière de champ logique.
