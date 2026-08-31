# FT-STR-03 — Encodage multi-registres

## Objet

FT-STR-03 valide la convention d'encodage des champs `uint32` du protocole TR2.

La règle normative est unique :

- registre N = MSW ;
- registre N+1 = LSW ;
- reconstruction = `(MSW << 16) | LSW`.

FT-STR-03 ne déduit jamais l'ordre des mots de la plausibilité d'une valeur observée.

## Référentiel de vérité

Ordre de priorité :

1. spécification V1 (`bloc0.md` à `bloc7.md` et `charte_typage.md`) ;
2. GEL-MAP-V1, dérivé de V1 ;
3. `source/FT-STR-03.md` ;
4. tests génériques de `detaille/` ;
5. validation instanciée.

Une incohérence se corrige dans l'artefact dérivé concerné, jamais en modifiant silencieusement V1.

## Structure active

```text
FT-STR-03/
├── README.md
├── source/FT-STR-03.md
├── detaille/
│   ├── TT-STR-03-GEN-001.md
│   ├── TT-STR-03-GEN-002.md
│   └── TT-STR-03-GEN-003.md
├── instancie/
│   ├── README.md
│   └── RESULTAT_FT-STR-03_GEL-MAP-V1.md
└── archive_pre_renforcement/
    └── instancie_legacy/
```

Le validateur mécanique est `Modbus RTU/03_Automatisation/validate_ft_str_03_multireg.py`.

## Répartition avec les autres sous-familles

- typage et taille structurelle : FT-STR-02 ;
- accessibilité et lectures partielles : FT-STR-06 ;
- cohérence temporelle, snapshot et atomicité de l'image : FT-STR-07 ;
- ASCII fixe : FT-STR-04.

La contiguïté physique des deux registres d'un `uint32` est vérifiée ici uniquement comme précondition nécessaire à son encodage, sans reprendre l'audit global de géométrie de FT-STR-01.

## Statut

Sous-famille reconstruite lors de l'audit FT-STR contre V1 et GEL-MAP-V1.
