# TT-STR-04-GEN-003 — Padding terminal et ASCII strict

## Objectif

Valider le remplissage `0x00` des octets inutilisés et l’absence d’octets non ASCII dans les données utiles.

## Cas à couvrir

- chaîne vide ;
- chaîne de longueur impaire ;
- chaîne partielle ;
- chaîne occupant exactement la capacité.

## Vecteurs minimaux

Pour une zone d’au moins deux registres :

- chaîne vide → `0x0000 0x0000 ...` ;
- `A` → `0x4100 0x0000 ...` ;
- `ABC` → `0x4142 0x4300 ...` ;
- chaîne pleine → aucun octet de padding requis.

## Contrôles

1. Vérifier les octets utiles contre la valeur d’essai connue.
2. Vérifier que chaque octet inutile en fin de zone vaut `0x00`.
3. Vérifier que chaque octet utile est codé sur 7 bits ASCII (`0x00` à `0x7F` selon la valeur d’essai).
4. Vérifier qu’aucun résidu non nul ne subsiste dans la zone de padding.

## Résultat attendu

Le contenu utile et le padding correspondent exactement au vecteur attendu, sans donnée résiduelle.
