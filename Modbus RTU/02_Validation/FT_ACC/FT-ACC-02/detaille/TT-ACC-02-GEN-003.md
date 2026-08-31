# TT-ACC-02-GEN-003 — Écriture complète d'un champ ASCII fixe RW

## Objectif
Vérifier qu'un champ `ASCII fixe` entièrement déclaré `RW` accepte une écriture nominale sur toute sa longueur.

## Préconditions
- champ ASCII RW confirmé par V1 et GEL-MAP-V1 ;
- longueur fixe et encodage confirmés ;
- chaîne de test sûre, ASCII uniquement, padding `0x00` conforme.

## Étapes
1. Lire le champ initial.
2. Encoder une chaîne de test sur la longueur complète.
3. Écrire tous les registres du champ avec FC16.
4. Vérifier l'absence d'exception.
5. Relire et décoder le champ.
6. Restaurer la valeur initiale si nécessaire.

## Résultat attendu
L'écriture complète est acceptée et la chaîne est observable conformément à V1.

## Règle
Aucune écriture partielle du champ ASCII n'est exigée sans règle V1 explicite.
