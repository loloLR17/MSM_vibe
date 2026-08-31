# TT-STR-04-GEN-001 — Structure et capacité ASCII fixe

## Objectif

Vérifier qu’un champ `ASCII fixe` dispose d’une capacité déterministe conforme à V1.

## Préconditions

- champ déclaré `ASCII fixe` dans V1 et GEL-MAP-V1 ;
- nombre de registres connu.

## Contrôles

1. Vérifier que le nombre de registres est strictement positif.
2. Vérifier que la plage du champ contient exactement ce nombre de registres.
3. Calculer la capacité : `2 × nombre_de_registres` caractères ASCII.
4. Vérifier qu’aucune longueur variable ou terminaison implicite ne modifie cette capacité structurelle.

## Résultat attendu

La capacité du champ est fixe et vaut exactement deux caractères par registre.

## Hors périmètre

La géométrie globale du bloc reste couverte par FT-STR-01.
