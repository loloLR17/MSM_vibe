# FT-STR-02 — Typage des champs

## 1. Objet

FT-STR-02 valide le typage déclaré des champs Modbus TR2 et la compatibilité structurelle entre chaque type et son nombre de registres.

Elle vérifie la chaîne V1 → charte de typage → GEL-MAP-V1 → validation instanciée.

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
├── instancie/
│   ├── README.md
│   ├── RESULTAT_FT-STR-02_GEL-MAP-V1.md
│   └── COUVERTURE_FT-STR-02_GEL-MAP-V1.csv
└── archive_pre_renforcement/
    └── instancie_legacy/
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

## 5. Méthode d'instanciation

La validation active porte directement sur les **183 lignes logiques de GEL-MAP-V1**. Il n'est pas nécessaire de dupliquer ces 183 lignes en 183 fichiers Markdown : le mapping gelé constitue la table d'instanciation et le validateur mécanique applique les invariants GEN-001 à GEN-003.

Validateur : `Modbus RTU/03_Automatisation/validate_ft_str_02_typing.py`.

Les anciennes fiches champ-par-champ sont conservées en archive historique uniquement.

## 6. Répartition avec les autres sous-familles

- bornes et chevauchements : FT-STR-01 ;
- ordre MSW/LSW et cohérence des multi-registres : FT-STR-03 ;
- encodage ASCII : FT-STR-04 ;
- réservés/sentinelles : FT-STR-05 ;
- accessibilité et découpage de lecture : FT-STR-06 ;
- stabilité temporelle : FT-STR-07 ;
- conformité documentaire globale : FT-STR-08.

## 7. Statut

Sous-famille reconstruite et finalisée contre GEL-MAP-V1. Les artefacts historiques ne participent plus à la validation active.
