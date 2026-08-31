# FT-STR-08 — Conformité documentaire

## Objet

FT-STR-08 vérifie la conformité et la traçabilité documentaire de la chaîne V1 → mapping unifié → artefacts de validation.

## Hiérarchie documentaire

Ordre de priorité obligatoire :

1. spécification Modbus RTU V1 gelée (`01_Specification_source/bloc0.md` à `bloc7.md` et `charte_typage.md`) ;
2. `mapping_unifie`, artefact dérivé ;
3. fiche source FT-STR-08 ;
4. tests détaillés génériques ;
5. résultats/tests instanciés.

En cas de divergence, le niveau supérieur fait foi. Aucune correction silencieuse de la V1 n'est autorisée.

## Structure

```text
FT-STR-08/
├── README.md
├── source/
│   └── FT-STR-08.md
├── detaille/
│   ├── TT-STR-08-GEN-001.md
│   ├── TT-STR-08-GEN-002.md
│   ├── TT-STR-08-GEN-003.md
│   └── TT-STR-08-GEN-004.md
├── instancie/
│   └── RESULTAT_FT-STR-08_GEL-MAP-V1.md
└── archive_pre_renforcement/
    └── auto_legacy/
```

## Règles

- `source/` définit la sous-famille de validation, sans devenir une norme indépendante.
- `detaille/` contient les contrôles génériques, sans adresse concrète ni dépendance à un champ particulier.
- `instancie/` matérialise l'application de ces contrôles au mapping gelé.
- l'archive conserve uniquement l'historique et n'est jamais utilisée pour valider ou générer.
- une information absente de la V1 est classée `NON DÉFINI / À ARBITRER`.

## Statut

Sous-famille reconstruite après GEL-GOV-01 et GEL-MAP-V1. Les anciens micro-tests champ-par-champ sont archivés car ils ne matérialisaient pas correctement la hiérarchie normative.
