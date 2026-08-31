# FT-STR-03 — Encodage multi-registres

## 1. Identification

- **ID** : FT-STR-03
- **Nom** : Encodage multi-registres
- **Famille parente** : FT-STR
- **Criticité** : P0

## 2. Objectif

Vérifier que tout champ protocolaire déclaré `uint32` est représenté et décodé sans ambiguïté selon la convention normative TR2 : registre N = MSW, registre N+1 = LSW.

## 3. Références normatives

- V1 : `bloc0.md` à `bloc7.md` ;
- `charte_typage.md` ;
- GEL-MAP-V1 comme dérivé opérationnel gelé de V1.

En cas de contradiction, V1 prévaut. Une règle absente de V1 n'est pas inventée : `NON DÉFINI / À ARBITRER`.

## 4. Périmètre inclus

- identification exhaustive des champs `uint32` ;
- présence de deux registres consécutifs pour chaque `uint32` ;
- ordre N = MSW, N+1 = LSW ;
- reconstruction déterministe `(MSW << 16) | LSW` ;
- vecteurs de test capables de révéler une permutation de mots ;
- absence d'heuristique de plausibilité ou de permutation implicite côté centrale, firmware ou tests.

## 5. Hors périmètre

- typage général : FT-STR-02 ;
- géométrie globale des plages : FT-STR-01 ;
- lectures partielles et accessibilité Modbus : FT-STR-06 ;
- atomicité temporelle, cohérence MSW/LSW au même instant logique et stabilité de l'image : FT-STR-07 ;
- contenu et padding des chaînes : FT-STR-04 ;
- valeurs métier et domaines fonctionnels.

## 6. Règles de conformité

Pour chaque `uint32` :

1. `register_count = 2` ;
2. `address_end = address_start + 1` ;
3. `offset_end = offset_start + 1` ;
4. le premier mot est interprété comme MSW et le second comme LSW ;
5. le décodeur applique strictement `(MSW << 16) | LSW` ;
6. aucune décision d'ordre des mots ne dépend de la valeur obtenue.

Les champs dont GEL-MAP-V1 porte `kind = uint32_from_split_words` doivent en outre référencer exactement deux composants sources dans l'ordre documentaire MSW puis LSW. Les autres `uint32` restent soumis à la même convention normative, même si le mapping logique les représente `declared_as_is`.

## 7. Vecteurs génériques

Les vecteurs asymétriques sont obligatoires pour démontrer l'ordre :

| Valeur | MSW | LSW | Inversion |
|---|---:|---:|---:|
| `0x12345678` | `0x1234` | `0x5678` | `0x56781234` |
| `0x00010002` | `0x0001` | `0x0002` | `0x00020001` |
| `0x89ABCDEF` | `0x89AB` | `0xCDEF` | `0xCDEF89AB` |

`0x00000000` et `0xFFFFFFFF` peuvent contrôler les bornes de reconstruction mais ne prouvent jamais l'ordre des mots, car leur permutation produit la même valeur.

## 8. Critères de réussite

- 100 % des `uint32` GEL-MAP-V1 satisfont les invariants structurels d'encodage ;
- tous les vecteurs asymétriques sont reconstruits conformément à la convention ;
- aucune heuristique ou convention concurrente n'est utilisée dans les artefacts actifs.

## 9. Dépendances

- amont : FT-STR-02 et GEL-MAP-V1 ;
- complément temporel : FT-STR-07.
