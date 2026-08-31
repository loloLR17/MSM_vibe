# TT-ACC-05-GEN-002 — Configuration préparée et effets normatifs

## Objectif
Vérifier qu’une écriture dans la configuration préparée du Bloc 4 ne produit que les effets explicitement prévus par la V1.

## Préconditions
- cible RW de la configuration préparée ;
- valeur nominale maîtrisée ;
- état de la configuration connu.

## Étapes
1. Capturer le Bloc 4 avant écriture, y compris l’image active.
2. Écrire la cible préparée.
3. Capturer le Bloc 4 après stabilisation.
4. Vérifier la cible et classifier les autres différences.

## Résultat attendu
- la cible préparée évolue conformément à l’écriture ;
- `config_state` peut évoluer vers `BROUILLON` conformément à la V1 ;
- l’image active n’est pas appliquée ou modifiée implicitement ;
- aucun autre effet non spécifié n’apparaît.

## Criticité
P0
