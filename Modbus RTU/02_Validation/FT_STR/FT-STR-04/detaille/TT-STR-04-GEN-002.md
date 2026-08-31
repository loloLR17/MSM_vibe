# TT-STR-04-GEN-002 — Ordre des octets et reconstruction

## Objectif

Valider l’ordre normatif des deux caractères dans chaque registre ASCII fixe.

## Convention

- premier caractère → octet haut ;
- second caractère → octet bas.

## Vecteurs minimaux

| Texte connu | Octets attendus | Registre(s) attendu(s) |
|---|---|---|
| `AB` | `41 42` | `0x4142` |
| `ABCD` | `41 42 43 44` | `0x4142 0x4344` |
| `12` | `31 32` | `0x3132` |

## Étapes

1. Injecter ou exposer une valeur d’essai connue.
2. Lire les registres du champ.
3. Extraire d’abord l’octet haut puis l’octet bas de chaque registre.
4. Comparer la séquence reconstruite au vecteur attendu.
5. Vérifier qu’aucune inversion par registre ni heuristique de plausibilité n’est utilisée.

## Résultat attendu

La séquence reconstruite correspond exactement à l’ordre des caractères du texte connu.
