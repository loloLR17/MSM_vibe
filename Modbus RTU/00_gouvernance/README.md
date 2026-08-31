# Gouvernance de validation TR2

Ce répertoire contient les règles méthodologiques transversales applicables à la validation du protocole Modbus RTU TR2.

## Référentiel supérieur

La gouvernance est subordonnée à la **Spécification Modbus RTU V1 gelée** située dans `01_Specification_source/` :

- `bloc0.md` à `bloc7.md` ;
- `charte_typage.md`.

Aucune charte de validation ne peut modifier implicitement une règle normative de la V1.

## Documents

- `CHARTE_ACCES_INVALIDES.md` — doctrine des accès Modbus invalides et distinction avec les valeurs métier invalides ;
- `CHARTE_ARBORESCENCE.md` — structure de référence des familles de validation et hiérarchie documentaire.

## Décisions gelées

### GEL-GOV-01 — Hiérarchie normative

```text
Spécification V1 gelée
    ↓
mapping_unifie dérivé
    ↓
fiches source de validation
    ↓
tests détaillés
    ↓
tests instanciés
```

### GEL-GOV-02 — Accès Modbus invalide vs valeur métier invalide

- adresse ou opération interdite → exception Modbus standard, aucune modification ;
- valeur métier hors domaine dans un registre RW valide → traitement fonctionnel défini par le bloc ;
- une lecture d’un sous-ensemble valide n’est pas invalide du seul fait qu’elle est partielle.

### GEL-GOV-03 — Numérotation FT-STR V1

- `FT-STR-05` = Réservés et sentinelles ;
- `FT-STR-06` = Accessibilité lecture.

## Éléments restant à auditer avant gel complet de la validation

- conformité de `mapping_unifie` à la V1 ;
- conformité détaillée de FT-STR ;
- conformité détaillée de FT-ACC ;
- reprise de FT-LIM ;
- développement des familles aval FT-BLK, FT-INT, FT-CMD, FT-SEQ, FT-RBT, FT-PER et FT-OBS.

Toute information nécessaire mais absente de la V1 doit être classée **NON DÉFINI / À ARBITRER**.
