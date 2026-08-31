# FT-RBT-04 — Lectures sous transition et sollicitations rapprochées

## 1. Objet

FT-RBT-04 compose l'oracle de cohérence temporelle FT-STR avec une situation dynamique : plusieurs lectures sont effectuées pendant qu'un état ou une donnée évolue.

## 2. Principe de propriété

FT-RBT-04 ne redéfinit pas la cohérence d'une réponse Modbus. FT-STR-07 impose déjà qu'une réponse multi-registres représente un même instant logique, y compris pour des données dynamiques.

La propriété FT-RBT est uniquement la composition : provoquer ou exploiter une transition contrôlée et solliciter le capteur pendant cette évolution afin de vérifier que chaque réponse reste individuellement conforme à l'oracle FT-STR.

## 3. Cas actif

- `TT-RBT-GEN-020` — lectures répétées pendant une transition contrôlée ; chaque réponse multi-registres doit rester cohérente en elle-même.

Classification : `CONDITIONAL`, car le moyen d'essai doit pouvoir provoquer ou identifier une transition suffisamment contrôlée pour exercer le scénario sans inventer d'exigence temporelle.

## 4. Interdictions

Le test ne doit pas imposer :
- l'observation obligatoire d'un état intermédiaire particulier ;
- un nombre minimal de transitions observées ;
- une durée maximale ou minimale de transition ;
- l'égalité de deux réponses successives portant sur des données dynamiques ;
- une fréquence minimale ou maximale de polling ;
- une latence de réponse non définie par V1.

## 5. Frontières

- cohérence interne d'une réponse : FT-STR-07 ;
- cohérence métier intra/inter-blocs : FT-BLK / FT-INT ;
- séquence fonctionnelle attendue : FT-SEQ ;
- performance temporelle et cadence admissible : non définies en V1.

## 6. Artefacts

- `source/FT-RBT-04_source.md` ;
- `detaille/FT-RBT-04_detaille.md` ;
- `detaille/FT-RBT-04_matrice_couverture.csv`.

## 7. Statut

Sous-famille reconstruite pour revue. Gel interdit avant validation explicite.
