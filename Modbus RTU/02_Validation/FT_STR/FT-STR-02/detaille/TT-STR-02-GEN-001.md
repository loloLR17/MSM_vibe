# TT-STR-02-GEN-001 — Type déclaré autorisé

## Objectif

Vérifier qu'un champ possède un type explicite identique à la référence normative et appartenant à la liste autorisée par `charte_typage.md`.

## Contrôles

1. Identifier le type normatif du champ.
2. Vérifier que GEL-MAP-V1 déclare le même type.
3. Vérifier que ce type appartient à : `uint16`, `int16`, `uint32`, `bitfield16`, `enum16`, `ASCII fixe`.
4. Si `uint16[n]` apparaît, vérifier qu'il s'agit uniquement d'une notation documentaire de regroupement.

## Résultat attendu

Type explicite, autorisé et identique entre la source normative et le mapping dérivé.

## Échec

Type absent, ambigu, renommé sans autorité normative ou non autorisé.

## Instanciation

Applicable à chacun des 183 champs logiques de GEL-MAP-V1.
