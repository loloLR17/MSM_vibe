# FT-STR-04 — ASCII fixe

## Objet

FT-STR-04 valide l’encodage des champs de type `ASCII fixe` du protocole TR2.

Règles normatives :

- 2 caractères ASCII par registre ;
- premier caractère dans l’octet de poids fort (bits 15..8) ;
- second caractère dans l’octet de poids faible (bits 7..0) ;
- padding terminal avec `0x00` ;
- longueur fixe définie par V1 ;
- ASCII uniquement.

Exemple : `"AB"` → `0x4142` ; `"A"` suivi du padding → `0x4100`.

## Référentiel de vérité

Ordre de priorité :

1. spécification V1 (`bloc0.md` à `bloc7.md` et `charte_typage.md`) ;
2. GEL-MAP-V1, dérivé de V1 ;
3. `source/FT-STR-04.md` ;
4. tests génériques de `detaille/` ;
5. validation instanciée.

Une incohérence se corrige dans l’artefact dérivé concerné, jamais en modifiant silencieusement V1.

## Structure active

```text
FT-STR-04/
├── README.md
├── source/FT-STR-04.md
├── detaille/
│   ├── TT-STR-04-GEN-001.md
│   ├── TT-STR-04-GEN-002.md
│   └── TT-STR-04-GEN-003.md
├── instancie/
│   ├── README.md
│   ├── COUVERTURE_FT-STR-04_GEL-MAP-V1.csv
│   └── RESULTAT_FT-STR-04_GEL-MAP-V1.md
└── archive_pre_renforcement/
    └── instancie_legacy/
```

Validateur mécanique : `Modbus RTU/03_Automatisation/validate_ft_str_04_ascii.py`.

## Répartition avec les autres sous-familles

- typage et taille structurelle générale : FT-STR-02 ;
- géométrie et absence de chevauchement : FT-STR-01 ;
- accessibilité et lectures partielles : FT-STR-06 ;
- stabilité temporelle de l’image : FT-STR-07.

FT-STR-04 contrôle la capacité du champ comme précondition de son encodage ASCII, sans reprendre l’audit global de géométrie.

## Statut

Sous-famille reconstruite lors de l’audit FT-STR. Les artefacts historiques sont conservés en archive uniquement.
