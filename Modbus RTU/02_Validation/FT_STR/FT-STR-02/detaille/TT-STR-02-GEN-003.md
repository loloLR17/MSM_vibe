# TT-STR-02-GEN-003 — Absence de typage implicite ou interdit

## Objectif

Vérifier globalement que les artefacts actifs n'introduisent aucun type non autorisé ni aucune interprétation implicite.

## Contrôles

- absence de `float`, `float32`, `float64` ;
- absence de `enum` utilisé comme type à la place de `enum16` ;
- absence de `bitfield` utilisé comme type à la place de `bitfield16` ;
- absence de type déduit uniquement d'une valeur observée ;
- absence de conversion cachée côté tests ;
- `uint16[n]` utilisé uniquement comme notation documentaire.

## Résultat attendu

Tous les champs sont interprétables sans heuristique à partir d'un type normatif explicitement autorisé.

## Échec

Tout type ou mécanisme implicite non prévu par la charte doit être corrigé dans l'artefact dérivé ou faire l'objet d'une évolution formelle de la norme.
