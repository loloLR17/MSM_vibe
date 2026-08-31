# TT-STR-02-GEN-002 — Compatibilité type / taille

## Objectif

Vérifier que le nombre de registres occupés par un champ est structurellement compatible avec son type déclaré.

## Règles

| Type | Taille structurelle |
|---|---|
| `uint16` | 1 registre |
| `int16` | 1 registre |
| `enum16` | 1 registre |
| `bitfield16` | 1 registre |
| `uint32` | 2 registres |
| `ASCII fixe` | longueur normative / 2 caractères par registre |

Pour une notation documentaire `uint16[n]`, la portée doit contenir exactement `n` registres `uint16` consécutifs.

## Résultat attendu

La taille dérivée du champ est exactement compatible avec son type et sa définition normative.

## Hors périmètre

L'ordre des mots d'un `uint32`, le contenu ASCII et le comportement d'une lecture partielle sont vérifiés dans les sous-familles dédiées.

## Instanciation

Applicable à chacun des 183 champs logiques de GEL-MAP-V1.
