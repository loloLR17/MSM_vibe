# TT-ACC-02-GEN-002 — Écriture complète d'un champ uint32 RW

## Objectif
Vérifier qu'un champ logique `uint32` entièrement déclaré `RW` accepte une écriture nominale sur ses deux registres.

## Préconditions
- champ `uint32` RW confirmé par V1 et GEL-MAP-V1 ;
- ordre MSW puis LSW confirmé ;
- valeur de test sûre.

## Étapes
1. Lire la valeur initiale.
2. Écrire les deux registres du champ en une requête FC16.
3. Vérifier l'absence d'exception.
4. Relire les deux registres et reconstruire le `uint32`.
5. Restaurer la valeur initiale si nécessaire.

## Résultat attendu
L'écriture complète est acceptée et la valeur est prise en compte conformément à V1.

## Règle
Ce test ne crée aucune exigence d'écriture partielle d'un `uint32`.
