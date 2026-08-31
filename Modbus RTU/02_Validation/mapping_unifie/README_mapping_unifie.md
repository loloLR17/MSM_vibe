# TR2 — Mapping unifié

## 1. Statut et autorité documentaire

Le mapping unifié est un **artefact dérivé de validation**.

La source normative est exclusivement constituée de la **TR2 — Spécification Modbus RTU V1 gelée** :

- `01_Specification_source/bloc0.md` à `bloc7.md` ;
- `01_Specification_source/charte_typage.md`.

Le mapping ne constitue pas une norme indépendante. En cas de divergence, la spécification V1 fait foi et la divergence doit être traitée comme une anomalie documentaire ; elle ne doit jamais être résolue en modifiant silencieusement la V1.

## 2. Contenu

- `tr2_mapping_unifie_brut.csv` : vue normalisée au niveau des champs source exposés par les blocs. Les couples `*_msw` / `*_lsw` et les registres composant les chaînes ASCII restent explicités lorsqu’ils existent dans la source.
- `tr2_mapping_unifie_logique.csv` : vue opérationnelle destinée notamment à l’instanciation des tests, avec regroupements logiques sûrs (`uint32`, chaînes ASCII fixes) et conservation exacte des types, accès et plages normatives.
- `tr2_mapping_couverture.csv` : synthèse mécanique de couverture par bloc, incluant désormais le contrôle des lacunes et des chevauchements/doublons d’adresses.
- `AUDIT_MAPPING_V1.md` : traçabilité de l’audit croisé ayant conduit à l’alignement sur la V1 gelée.
- `VERSION.md` : statut de maturité du mapping.

## 3. Hiérarchie de dérivation

```text
Spécification Modbus RTU V1 gelée
        ↓
tr2_mapping_unifie_brut.csv
        ↓
tr2_mapping_unifie_logique.csv
        ↓
tests instanciés
```

La vue logique ne doit jamais dégrader un type normatif (`enum16`, `bitfield16`, `int16`, etc.) en un type plus générique.

## 4. Conventions de nommage

- Tout champ réservé doit être identifiable sans ambiguïté et conserver le nom normatif de la spécification.
- Les champs réservés suivent la convention `reserved_*` lorsque le nom normatif utilise cette forme.
- Les noms particuliers normatifs, notamment `B3_RESERVED_0`, sont conservés tels quels.
- Les noms inventés lors d’une extraction historique ne doivent pas survivre lorsqu’un nom normatif existe.

## 5. Types

Les types du mapping doivent être compatibles avec `charte_typage.md` :

- `uint16` ;
- `int16` ;
- `uint32` ;
- `bitfield16` ;
- `enum16` ;
- `ASCII fixe` ;
- `uint16[n]` uniquement comme notation documentaire de regroupement de registres 16 bits.

Les échappements Markdown tels que `uint16\\[2]` ne sont pas des types de mapping valides et ne doivent pas apparaître dans les CSV.

## 6. Contrôles structurels obligatoires

Avant tout gel ou toute régénération massive de tests, il faut vérifier au minimum :

1. couverture complète de chaque plage normative ;
2. absence de trou ;
3. absence de chevauchement d’adresses ;
4. absence de plage ou adresse dupliquée ;
5. cohérence entre `register_count`, offsets et adresses ;
6. conformité des types ;
7. conformité des accès `RO` / `RW` ;
8. conformité des noms des champs réservés ;
9. audit croisé avec les huit blocs et `charte_typage.md`.

Le script :

```text
03_Automatisation/validate_mapping_structure.py
```

contrôle les invariants mécaniques. Il **ne remplace pas** la comparaison normative avec les fichiers de spécification.

## 7. Règle de déduplication

La simple suppression de doublons textuels exacts n’est pas suffisante : deux descriptions différentes d’une même adresse peuvent représenter un doublon documentaire.

La déduplication doit donc tenir compte au minimum du bloc, des offsets et des adresses, puis être arbitrée par rapport au mapping normatif du bloc concerné.

Aucune divergence ne doit être résolue par choix heuristique silencieux.
