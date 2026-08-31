# Audit mapping unifié ↔ Spécification Modbus RTU V1

## Statut

**AUDIT TERMINÉ — CANDIDAT GEL-MAP-V1**

Référence normative :

- `01_Specification_source/bloc0.md` à `bloc7.md` ;
- `01_Specification_source/charte_typage.md`.

Hiérarchie applicable :

```text
Spécification V1 gelée
        ↓
mapping_unifie dérivé
        ↓
tests
```

Toute divergence constatée a été corrigée dans le mapping dérivé. Aucun fichier normatif de la V1 n’a été modifié pour faire correspondre la spécification à un artefact ancien.

## Résultat final par bloc

| Bloc | Plage | Couverture | Chevauchement | Résultat audit mapping |
|---:|---|---:|---:|---|
| 0 | 0-20 | complète | 0 | conforme après correction `reserved_0` |
| 1 | 1000-1019 | complète | 0 | conforme |
| 2 | 2000-2015 | complète | 0 | conforme |
| 3 | 3000-3047 | complète | 0 | conforme après correction du type de `B3_RESERVED_0` |
| 4 | 4000-4175 | complète | 0 | conforme après déduplication de 4017 et restauration des types V1 |
| 5 | 5000-5019 | complète | 0 | conforme |
| 6 | 6000-6063 | complète | 0 | conforme après normalisation `uint16[n]` |
| 7 | 7000-7015 | complète | 0 | conforme après correction `reset_cause = enum16` et normalisation `uint16[n]` |

## Registre des anomalies

| ID | Bloc | Élément | Anomalie initiale | Correction | Statut |
|---|---:|---|---|---|---|
| MAP-01 | 0 | adresse 20 | `reserved` au lieu de `reserved_0` | nom aligné V1 dans brut et logique | **CLOS** |
| MAP-02 | 3 | `B3_RESERVED_0` | type `réservé` | `uint16[8]` | **CLOS** |
| MAP-03 | 4 | adresse 4017 | deux entrées historiques | une entrée unique `reserved_4B_0` | **CLOS** |
| MAP-04 | 4 | `axes_enable_mask` | `uint16` | `bitfield16` | **CLOS** |
| MAP-05 | 4 | `full_scale_code` | `uint16` | `enum16` | **CLOS** |
| MAP-06 | 4 | `acquisition_mode` | `uint16` | `enum16` | **CLOS** |
| MAP-07 | 4 | `storage_mode` | `uint16` | `enum16` | **CLOS** |
| MAP-08 | 4 | `active_axes_enable_mask` | `uint16` | `bitfield16` | **CLOS** |
| MAP-09 | 4 | `active_full_scale_code` | `uint16` | `enum16` | **CLOS** |
| MAP-10 | 4 | `active_acquisition_mode` | `uint16` | `enum16` | **CLOS** |
| MAP-11 | 4 | `active_storage_mode` | `uint16` | `enum16` | **CLOS** |
| MAP-12 | 6 | `reserved_6A` | type échappé `uint16\\[2]` | `uint16[2]` | **CLOS** |
| MAP-13 | 6 | `reserved_6B` | type échappé `uint16\\[6]` | `uint16[6]` | **CLOS** |
| MAP-14 | 7 | `reset_cause` | `uint16` | `enum16` | **CLOS** |
| MAP-15 | 7 | `reserved_7A` | type échappé `uint16\\[2]` | `uint16[2]` | **CLOS** |

## Contrôles finaux

Les vues `brut` et `logique` corrigées ont été vérifiées sur les invariants suivants :

1. huit blocs présents ;
2. plages d’adresses conformes aux blocs V1 ;
3. couverture complète de chaque plage ;
4. zéro trou à l’intérieur de chaque plage ;
5. zéro chevauchement d’adresse ;
6. aucune plage dupliquée ;
7. cohérence `register_count` ↔ étendue d’offsets ↔ étendue d’adresses ;
8. relation `adresse absolue = base bloc + offset` ;
9. types limités aux types autorisés de la charte, avec `uint16[n]` comme notation documentaire de regroupement ;
10. accès limités à `RO` / `RW` et alignés sur les mappings normatifs ;
11. restauration des types spécialisés `enum16`, `bitfield16` et `int16` ;
12. noms des réservés alignés sur la spécification ;
13. regroupements logiques `uint32` et ASCII conservés sans perte de portée ni d’accès ;
14. chaque `source_field` logique existe dans la vue brute ;
15. chaque champ brut est représenté exactement une fois dans la vue logique ;
16. cohérence de `source_file` entre vues brute et logique ;
17. cohérence du fichier `tr2_mapping_couverture.csv` avec les plages attendues.

Le fichier `tr2_mapping_couverture.csv` expose explicitement les comptes de lacunes, chevauchements et plages dupliquées.

Le script `03_Automatisation/validate_mapping_structure.py` formalise et permet de rejouer ces invariants mécaniques. Il contrôle désormais :

- la vue brute ;
- la vue logique ;
- la relation brut ↔ logique ;
- la synthèse de couverture.

Il **ne remplace pas** l’audit normatif manuel contre les fichiers `bloc0.md` à `bloc7.md` et `charte_typage.md`.

## Cause racine retenue

La génération historique reposait en partie sur une déduplication de doublons textuels exacts. Cette approche était insuffisante : deux lignes documentaires différentes pouvaient décrire la même adresse et survivre à l’extraction, comme observé à l’adresse 4017.

Le contrôle historique de couverture vérifiait les trous mais ne détectait pas suffisamment les chevauchements. Il pouvait donc annoncer une couverture complète malgré un doublon d’adresse.

La doctrine corrigée impose désormais un contrôle structurel par plage/adresse, une vérification brut ↔ logique et une comparaison normative.

## Conclusion d’audit

Aucune contradiction résiduelle n’a été identifiée entre le mapping corrigé et les éléments normatifs de structure contrôlés dans la V1 :

- adresses ;
- offsets ;
- noms de champs ;
- types ;
- nombre de registres ;
- accès `RO` / `RW` ;
- regroupements multi-registres ;
- chaînes ASCII fixes ;
- registres réservés.

Les domaines métier, valeurs limites, codes d’énumération et comportements fonctionnels restent définis par les fichiers normatifs des blocs ; le mapping unifié ne se substitue pas à ces règles.

## Critère de gel

Tous les critères techniques de `GEL-MAP-V1` sont satisfaits sur la branche d’audit :

- MAP-01 à MAP-15 clos ;
- mapping brut aligné sur la V1 ;
- mapping logique cohérent avec le brut et la V1 ;
- couverture sans trou ni chevauchement ;
- aucun type hors charte identifié lors de la passe finale ;
- contrôles mécaniques reproductibles ;
- audit croisé final terminé.

Le statut `GEL-MAP-V1` devient effectif après validation et intégration de cette branche dans `main`.
