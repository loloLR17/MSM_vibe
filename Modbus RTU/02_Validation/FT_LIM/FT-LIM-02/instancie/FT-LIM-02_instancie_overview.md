# FT-LIM-02 — Vue d'ensemble des instances

## Couverture active

FT-LIM-02 contient 23 instances actives réparties en deux contraintes normatives.

| Contrainte | Cas générique | Instances | Objet |
|---|---|---:|---|
| LIM02-C01 | LIM02-G01 | 20 | produit cartésien exhaustif des 4 fenêtres × 5 périodes, fréquence V1 fixée à 26667 Hz |
| LIM02-C02 | LIM02-G02 | 3 | borne basse conditionnelle, borne dynamique C, première valeur au-dessus C |
| **Total** | | **23** | |

## C01 — Cohérence combinatoire

Les 20 instances utilisent exclusivement des valeurs unitaires valides FT-LIM-01.

La combinaison la plus contraignante est :
- fréquence : 26667 Hz ;
- fenêtre : 32768 échantillons ;
- période : 2000 ms.

Elle satisfait encore l'invariant :

`2000 × 26667 = 53 334 000 >= 32 768 000 = 1000 × 32768`.

Par conséquent, toutes les autres combinaisons du produit cartésien satisfont également la relation. L'exhaustivité des 20 lignes est conservée pour la traçabilité et la non-régression si les domaines V1 sont ultérieurement révisés.

## C02 — Capacité dynamique

Les trois instances dépendent d'une capacité utilisable `C` caractérisée dans l'environnement d'essai :

- 1 MB : valide si C >= 1 ;
- C MB : borne dynamique valide ;
- C+1 MB : invalide si cette valeur reste représentable en uint32.

Aucune capacité de stockage fictive n'est intégrée aux tests.

## Relations non testées

La matrice conserve explicitement les relations tentantes mais non définies par la V1 (ordre des seuils, hystérésis, masque supervision, dimensionnement durée/stockage). Elles ne génèrent aucune instance active.

## Frontière avec FT-LIM-03

FT-LIM-02 établit la validité des relations de valeurs. La validation exhaustive du CRC, des états et du mécanisme d'application reste réservée à FT-LIM-03.
