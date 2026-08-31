# TT-STR-03-GEN-001 — Structure et ordre normatif uint32

## Objectif

Vérifier qu'un champ déclaré `uint32` occupe exactement deux registres consécutifs et que leur ordre protocolaire est N = MSW, N+1 = LSW.

## Entrées

- V1 et `charte_typage.md` ;
- GEL-MAP-V1.

## Vérifications

Pour chaque `uint32` :

1. vérifier `register_count = 2` ;
2. vérifier `address_end = address_start + 1` ;
3. vérifier `offset_end = offset_start + 1` ;
4. appliquer la convention normative N = MSW, N+1 = LSW.

## Résultat attendu

Aucun `uint32` ne présente une taille, une discontinuité ou un ordre documentaire contradictoire avec V1.

## Hors périmètre

La cohérence temporelle des deux mots relève de FT-STR-07.
