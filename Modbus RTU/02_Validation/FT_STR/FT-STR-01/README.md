# FT-STR-01 — Structure des plages

## 1. Objet

FT-STR-01 valide la structure des plages Modbus définies par la spécification V1 et matérialisées dans GEL-MAP-V1.

Elle couvre :
- les bornes de début et de fin de chaque bloc ;
- la longueur de chaque bloc ;
- la continuité interne des plages exposées ;
- l'absence de chevauchement ;
- la cohérence des intervalles entre blocs.

Elle ne valide ni le typage, ni l'encodage, ni les droits d'écriture, ni le comportement des requêtes de lecture. Ces sujets relèvent des autres sous-familles FT-STR et de FT-ACC.

## 2. Architecture

```text
FT-STR-01/
├── README.md
├── source/
│   └── FT-STR-01.md
├── detaille/
│   ├── TT-STR-01-GEN-001.md
│   ├── TT-STR-01-GEN-002.md
│   └── TT-STR-01-GEN-003.md
└── instancie/
    ├── FT-STR-01_instancie_index.csv
    ├── FT-STR-01_instancie_overview.md
    ├── TT-STR-01-B0-001 ... TT-STR-01-B7-001
    └── TT-STR-01-GLOBAL-001
```

## 3. Référentiel de vérité

Ordre de priorité obligatoire :

1. spécification Modbus RTU V1 gelée ;
2. GEL-MAP-V1, dérivé de la V1 ;
3. `source/FT-STR-01.md` ;
4. `detaille/` ;
5. `instancie/`.

En cas de contradiction, la source de niveau supérieur prévaut. Aucune divergence ne doit être corrigée implicitement dans la source supérieure.

## 4. Règles propres à FT-STR-01

- un bloc doit avoir les bornes et la longueur prévues par V1 ;
- deux champs d'un même bloc ne doivent ni se chevaucher ni créer de trou interne non défini ;
- deux blocs ne doivent pas se chevaucher ;
- un intervalle non exposé entre deux blocs n'est pas un défaut lorsqu'il résulte des bornes définies par V1 ;
- l'accessibilité effective des lectures et le découpage des requêtes relèvent de FT-STR-06 ;
- les accès invalides relèvent de la doctrine GEL-GOV-02 et des familles concernées ;
- la stabilité temporelle de l'image relève de FT-STR-07.

## 5. Exécution

FT-STR-08 doit avoir établi la cohérence documentaire avant FT-STR-01.

Les tests génériques définissent les invariants. Les tests instanciés appliquent ensuite ces invariants aux huit blocs de GEL-MAP-V1.

## 6. Statut

Sous-famille reconstruite lors de l'audit FT-STR. La validation opérationnelle dépend de l'exécution et de l'enregistrement des résultats correspondants.
