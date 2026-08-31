# FT-STR-02 — Typage des champs

## 1. Objet

FT-STR-02 valide le typage déclaré des champs Modbus TR2 et la compatibilité structurelle entre chaque type et son nombre de registres.

Elle vérifie la chaîne V1 → charte de typage → GEL-MAP-V1 → tests instanciés.

Elle ne cherche pas à déduire un type à partir d'une lecture Modbus : un registre transporte toujours 16 bits et son interprétation provient du type déclaré.

## 2. Architecture

```text
FT-STR-02/
├── README.md
├── source/FT-STR-02.md
├── detaille/
│   ├── TT-STR-02-GEN-001.md
│   ├── TT-STR-02-GEN-002.md
│   └── TT-STR-02-GEN-003.md
└── instancie/
    ├── FT-STR-02_instancie_index.csv
    ├── FT-STR-02_instancie_overview.md
    └── TT-STR-02-B<bloc>-NNN__...md
```

## 3. Référentiel de vérité

1. spécification V1 et `charte_typage.md` ;
2. GEL-MAP-V1, dérivé de V1 ;
3. `source/FT-STR-02.md` ;
4. `detaille/` ;
5. `instancie/`.

Une incohérence se corrige dans l'artefact dérivé concerné, jamais en modifiant silencieusement une source supérieure.

## 4. Types protocolaires autorisés

- `uint16`
- `int16`
- `uint32`
- `bitfield16`
- `enum16`
- `ASCII fixe`

`uint16[n]` est uniquement une notation documentaire de regroupement de `n` registres `uint16` consécutifs ; ce n'est pas un type protocolaire supplémentaire.

Tout autre type est interdit sans évolution formelle de la charte.

## 5. Répartition avec les autres sous-familles

- bornes et chevauchements : FT-STR-01 ;
- ordre MSW/LSW et cohérence des multi-registres : FT-STR-03 ;
- encodage ASCII : FT-STR-04 ;
- réservés/sentinelles : FT-STR-05 ;
- accessibilité et découpage de lecture : FT-STR-06 ;
- stabilité temporelle : FT-STR-07 ;
- conformité documentaire globale : FT-STR-08.

## 6. Statut

Sous-famille reconstruite lors de l'audit FT-STR. Les 183 instanciations correspondent aux 183 champs logiques de GEL-MAP-V1 et servent à contrôler leur type déclaré et leur taille structurelle.
