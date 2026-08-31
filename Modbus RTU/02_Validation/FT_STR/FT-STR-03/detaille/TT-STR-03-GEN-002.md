# TT-STR-03-GEN-002 — Reconstruction déterministe

## Objectif

Démontrer que la reconstruction d'un `uint32` applique strictement `(MSW << 16) | LSW` et que les vecteurs utilisés permettent de révéler une inversion des mots.

## Vecteurs obligatoires

| Valeur attendue | MSW | LSW | Résultat inversé |
|---|---:|---:|---:|
| `0x12345678` | `0x1234` | `0x5678` | `0x56781234` |
| `0x00010002` | `0x0001` | `0x0002` | `0x00020001` |
| `0x89ABCDEF` | `0x89AB` | `0xCDEF` | `0xCDEF89AB` |

Les bornes `0x00000000` et `0xFFFFFFFF` peuvent compléter le test de reconstruction mais sont interdites comme preuve d'endianness, car elles sont symétriques à la permutation MSW/LSW.

## Critères d'acceptation

- chaque vecteur asymétrique produit exactement la valeur attendue ;
- la permutation volontaire produit la valeur inversée indiquée ;
- aucune troncature à 16 bits n'est admise.

## Hors périmètre

La répétabilité temporelle relève de FT-STR-07.
