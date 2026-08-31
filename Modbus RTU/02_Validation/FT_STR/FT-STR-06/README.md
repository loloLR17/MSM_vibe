# FT-STR-06 — Accessibilité et découpage de lecture Modbus

## Objet

FT-STR-06 valide que les plages exposées par le mapping TR2 sont lisibles via Modbus FC03 sans hypothèse implicite de découpage des champs logiques.

## Référentiel de vérité

1. spécification V1 (`bloc0.md` à `bloc7.md` et `charte_typage.md`) ;
2. GEL-MAP-V1, dérivé de V1 ;
3. `source/FT-STR-06.md` ;
4. tests génériques de `detaille/` ;
5. validation instanciée.

Une lecture partielle d'une zone exposée reste valide si toutes les adresses demandées sont valides.

Une requête qui traverse une adresse inexistante est invalide et relève de GEL-GOV-02.

## Structure active

```text
FT-STR-06/
├── README.md
├── source/FT-STR-06.md
├── detaille/
│   ├── TT-STR-06-GEN-001.md
│   ├── TT-STR-06-GEN-002.md
│   ├── TT-STR-06-GEN-003.md
│   └── TT-STR-06-GEN-004.md
├── instancie/
│   ├── README.md
│   ├── COUVERTURE_FT-STR-06_GEL-MAP-V1.csv
│   └── RESULTAT_FT-STR-06_GEL-MAP-V1.md
└── archive_pre_renforcement/
    └── instancie_legacy/
```

Le validateur mécanique est `Modbus RTU/03_Automatisation/validate_ft_str_06_read_access.py`.

## Répartition avec les autres sous-familles

- géométrie et frontières du mapping : FT-STR-01 ;
- typage : FT-STR-02 ;
- encodage multi-registres : FT-STR-03 ;
- stabilité/cohérence temporelle des réponses : FT-STR-07 ;
- droits et refus d'écriture : FT-ACC.

FT-PER reste la famille persistance/recovery. La performance Modbus non spécifiée est hors périmètre actuel.

## Statut

Sous-famille reconstruite lors de l'audit FT-STR contre V1, GEL-MAP-V1 et GEL-GOV-02.
