# TT-STR-06-GEN-003 — Segmentation des grandes plages exposées

## Objectif

Vérifier qu'une plage exposée plus longue que la capacité maximale d'une requête FC03 reste intégralement lisible par segmentation.

## Règle

Une requête FC03 porte sur 1 à 125 registres. Une plage exposée plus longue ne doit pas imposer une lecture monolithique impossible.

## Procédure générique

1. Identifier une plage exposée de plus de 125 registres.
2. Construire une première lecture valide de 125 registres maximum.
3. Construire une ou plusieurs lectures suivantes couvrant exactement la suite de la plage, sans lacune ni chevauchement requis.
4. Vérifier que chaque segment demandé est accepté.

## Cas GEL-MAP-V1

Le Bloc 4 couvre 4000..4175, soit 176 registres. Une segmentation conforme peut par exemple être :
- 4000..4124 : 125 registres ;
- 4125..4175 : 51 registres.

D'autres découpages sont conformes tant que chaque requête respecte les limites FC03 et ne vise que des adresses exposées.

## Résultat attendu

Toute la plage est accessible par une suite de requêtes valides sans contrainte de frontière de champ logique.
