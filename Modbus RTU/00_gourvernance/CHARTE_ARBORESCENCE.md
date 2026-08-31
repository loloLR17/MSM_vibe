# Charte d’arborescence — FT-STR

## 1. Objectif

Définir une structure unique, stable et industrielle pour toutes les familles de tests FT-STR.

---

## 2. Arborescence de référence

```text
02_Validation/
└── FT_STR/
    └── FT-STR-0X/
        ├── source/
        │   └── FT-STR-0X.md
        ├── detaille/
        │   └── TT-STR-0X-XXX.md
        ├── instancie/
        │   └── ...
        └── archive_pre_renforcement/
```

---

## 3. Rôle des niveaux

| Niveau | Rôle |
|------|------|
| source | spécification officielle |
| detaille | logique de test générique |
| instancie | test appliqué au mapping |
| archive | historique uniquement |

---

## 4. Règles strictes

### Unicité
- un seul fichier `FT-STR-0X.md` actif

### Séparation
- aucun fichier instancié dans detaille
- aucun champ dans detaille
- aucune adresse dans detaille

### Mapping
- instancie doit être dérivé du mapping_unifie

### Archive
- jamais utilisée en test
- jamais modifiée

---

## 5. Règles de nommage

| Type | Format |
|------|--------|
| Spécification | FT-STR-0X.md |
| Test détaillé | TT-STR-0X-XXX.md |
| Test instancié | TT-STR-0X-BY-XXX.md |

---

## 5.1 Mapping

- tout registre réservé doit être nommé avec le préfixe logique `reserved_` dans le mapping unifié, afin de permettre son identification automatique et non ambiguë dans les familles de validation (notamment FT-STR et FT-ACC).

---

## 6. Pipeline de validation

1. FT-STR-08 → validation doc ↔ mapping
2. FT-STR-01 → structure globale
3. FT-STR-02 → typage
4. FT-STR-03 → encodage
5. FT-STR-04 → ASCII
6. FT-STR-05 → accessibilité
7. FT-STR-06 → réservés
8. FT-STR-07 → stabilité

---

## 7. Critères de qualité

- reproductibilité
- traçabilité
- absence de duplication
- lisibilité
- exploitabilité terrain

---

## 8. Interdictions

- duplication de tests
- mélange des niveaux
- modification manuelle des instanciés sans traçabilité
- utilisation de fichiers archive en production

---

## 9. Objectif final

Permettre :
- génération automatique
- validation fiable
- industrialisation
- réutilisation sur autres familles (FT-ACC, FT-BLK, etc.)
