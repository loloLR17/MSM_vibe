# FT-ACC-04 — README

## 1. Objet
Cette sous-famille valide le comportement des registres réservés :
- neutralité ;
- stabilité ;
- absence de sémantique cachée ;
- comportement cohérent en lecture et lors des tentatives d’écriture.

## 2. Convention d’identification des réservés
La gouvernance retenue est :
- tout registre réservé est identifié par un `logical_name` commençant par `reserved`.

## 3. Structure du dossier

```text
FT-ACC-04/
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/
```

## 4. Référentiel de vérité
1. `mapping_unifie/`
2. `source/FT-ACC-04.md`
3. `instancie/`
4. `detaille/`

## 5. Règles
- aucun test instancié ne doit exister sans cible `reserved*` du mapping ;
- aucune adresse dans `detaille/` ;
- `archive_pre_renforcement/` ne doit jamais être utilisée en validation ;
- la règle `reserved*` appartient à la gouvernance et doit rester stable.

## 6. Statut
Package industrialisé et exhaustif sur la base du mapping fourni.
