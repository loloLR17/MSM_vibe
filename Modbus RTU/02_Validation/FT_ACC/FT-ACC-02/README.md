# FT-ACC-02 — README

## 1. Objet

Cette sous-famille appartient au plan de validation **FT-ACC (Accès et permissions)**.

Elle a pour objectif de valider :
- l’écriture effective des zones déclarées `RW` ;
- la cohérence **write → read** ;
- l’absence de refus d’accès sur une zone documentée modifiable ;
- la cohérence entre mapping et comportement observé.

---

## 2. Structure du dossier

```text
FT-ACC-02/
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/
```

### source/
Contient la fiche de spécification officielle :
- `FT-ACC-02.md`

### detaille/
Contient les cas de test génériques :
- `TT-ACC-02-GEN-XXX.md`

### instancie/
Contient les cas de test appliqués au mapping réel.

👉 Niveau :
- exécutable terrain
- basé sur `tr2_mapping_unifie_logique.csv`
- stratégie hybride **Option C**

### archive_pre_renforcement/
Contient les anciennes versions non opérationnelles.

---

## 3. Référentiel de vérité

Ordre de priorité :

1. `mapping_unifie/`
2. `source/FT-ACC-02.md`
3. `instancie/`
4. `detaille/`

---

## 4. Règles

- aucun test instancié ne doit exister sans cible `RW` du mapping ;
- aucun champ logique `RW` ne doit être instancié deux fois comme test de champ ;
- les tests de plage bloc sont explicitement distincts des tests de champ ;
- `detaille/` ne doit jamais contenir d’adresse ;
- `archive_pre_renforcement/` ne doit jamais être utilisée en validation.

---

## 5. Utilisation

Ordre recommandé :

1. FT-STR validée ;
2. FT-ACC-01 validée ;
3. exécuter FT-ACC-02 sur simulateur déterministe ;
4. poursuivre avec FT-ACC-03.

---

## 6. Statut

Famille préparée pour usage industriel et instanciation terrain.
