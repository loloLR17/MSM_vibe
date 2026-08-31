# FT-STR-08 — README

## 1. Objet

Cette famille de tests appartient au plan de validation **FT-STR (Conformité structurelle)**.

Elle a pour objectif de valider :
- la conformité du mapping Modbus
- la structure des données
- leur encodage et accessibilité

---

## 2. Structure du dossier

```text
FT-STR-08/
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/ (optionnel)
```

### source/
Contient la fiche de spécification officielle :
- `FT-STR-XX.md`

👉 Référence fonctionnelle

---

### detaille/
Contient les cas de test génériques :
- `TT-STR-XX-XXX.md`

👉 Niveau :
- logique
- indépendant du mapping

---

### instancie/
Contient les cas de test appliqués au mapping réel.

👉 Niveau :
- exécutable terrain
- basé sur mapping_unifie

---

### archive_pre_renforcement/
Contient les anciennes versions.

👉 Non utilisé en opérationnel

---

## 3. Référentiel de vérité

Ordre de priorité :

1. mapping_unifie
2. source/FT-STR-XX.md
3. instancie/
4. detaille/

---

## 4. Règles

- aucun test instancié ne doit exister sans mapping
- aucun champ ne doit être testé deux fois dans instancie/
- detaille/ ne doit jamais contenir d’adresse
- archive ne doit jamais être utilisée en validation

---

## 5. Utilisation

Ordre recommandé :

1. FT-STR-08 (doc ↔ mapping)
2. FT-STR-01 → 07
3. exécution instanciée

---

## 6. Statut

Famille validée pour usage industriel.
