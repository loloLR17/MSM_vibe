# FT-ACC-01 — README

## 1. Objet

Cette famille de tests appartient au plan de validation **FT-ACC (Accès et permissions)**.

Elle a pour objectif de valider :
- la lisibilité effective des zones exposées ;
- la cohérence entre mapping et lecture observée ;
- l’absence d’effet de bord d’une lecture nominale.

---

## 2. Structure du dossier

```text
FT-ACC-01/
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/
```

### source/
Contient la fiche de spécification officielle :
- `FT-ACC-01.md`

👉 Référence fonctionnelle

---

### detaille/
Contient les cas de test génériques :
- `TT-ACC-01-GEN-XXX.md`

👉 Niveau :
- logique
- indépendant du mapping

---

### instancie/
Contient les cas de test appliqués au mapping réel.

👉 Niveau :
- exécutable terrain
- basé sur `tr2_mapping_unifie_logique.csv`
- stratégie hybride **Option C**

---

### archive_pre_renforcement/
Contient les anciennes versions.

👉 Non utilisé en opérationnel

---

## 3. Référentiel de vérité

Ordre de priorité :

1. `mapping_unifie/`
2. `source/FT-ACC-01.md`
3. `instancie/`
4. `detaille/`

---

## 4. Règles

- aucun test instancié ne doit exister sans cible du mapping ;
- aucun champ logique ne doit être testé deux fois comme lecture unitaire ;
- les tests de plage bloc sont explicitement distincts des tests de champ ;
- `detaille/` ne doit jamais contenir d’adresse ;
- `archive_pre_renforcement/` ne doit jamais être utilisée en validation.

---

## 5. Utilisation

Ordre recommandé :

1. valider FT-STR ;
2. exécuter FT-ACC-01 sur simulateur déterministe ;
3. poursuivre avec FT-ACC-02 et suivantes.

---

## 6. Statut

Famille préparée pour usage industriel et instanciation terrain.
