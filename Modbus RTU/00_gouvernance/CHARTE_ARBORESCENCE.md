# Charte d’arborescence — Validation TR2

## 1. Objectif

Définir une structure unique, stable et industrialisable pour les familles de validation du protocole Modbus RTU TR2.

Cette charte généralise la méthode initialement mise en place pour FT-STR et réutilisée pour FT-ACC.

## 2. Hiérarchie documentaire

La hiérarchie de référence est :

```text
Spécification Modbus RTU V1 gelée
        ↓
mapping_unifie dérivé
        ↓
fiches source de validation
        ↓
tests détaillés génériques
        ↓
tests instanciés
```

Le mapping unifié est un artefact dérivé de la spécification V1 et ne constitue pas une source normative indépendante.

## 3. Arborescence de référence

```text
02_Validation/
└── FT_<FAMILLE>/
    ├── README.md
    └── FT-<FAMILLE>-0X/
        ├── README.md
        ├── source/
        │   └── FT-<FAMILLE>-0X.md
        ├── detaille/
        │   └── TT-<FAMILLE>-0X-GEN-XXX.md
        ├── instancie/
        │   └── ...
        └── archive_pre_renforcement/
            └── ...
```

Le répertoire `detaille/` doit être présent et rempli lorsqu’un niveau générique détaillé est applicable à la sous-famille.

Le répertoire `source/` doit contenir la fiche source active de la sous-famille.

Un répertoire vide n’étant pas conservé par Git, tout répertoire dont la présence doit être matérialisée dans le dépôt doit contenir au minimum un fichier documentaire approprié.

## 4. Rôle des niveaux

| Niveau | Rôle |
|---|---|
| `source` | spécification de validation de la sous-famille, subordonnée à la spécification Modbus V1 |
| `detaille` | logique de test générique, indépendante des adresses/champs instanciés |
| `instancie` | cas appliqués au mapping dérivé de la V1 |
| `archive_pre_renforcement` | historique uniquement, hors référentiel actif |

## 5. Règles strictes

### 5.1 Unicité

- une seule fiche source active par sous-famille ;
- aucune archive ne doit être utilisée comme référentiel actif.

### 5.2 Séparation des niveaux

- aucun test instancié dans `detaille/` ;
- aucune adresse ou champ particulier dans un test générique, sauf lorsque la nature même du test l’exige explicitement et que cela est documenté ;
- les tests instanciés doivent rester traçables vers leur test générique et vers le mapping utilisé.

### 5.3 Mapping

- `mapping_unifie` est dérivé de `bloc0.md` à `bloc7.md` et de `charte_typage.md` ;
- les tests instanciés sont dérivés du mapping unifié ;
- toute divergence mapping ↔ spécification doit être remontée comme anomalie documentaire ;
- la spécification V1 gelée fait foi tant qu’aucune évolution normative n’a été explicitement validée.

### 5.4 Archives

- les archives ne sont jamais utilisées pour exécuter la validation courante ;
- elles ne doivent pas être modifiées pour les faire correspondre au référentiel actuel ;
- leur rôle est uniquement historique et de traçabilité.

## 6. Règles de nommage

| Type | Format générique |
|---|---|
| Sous-famille | `FT-<FAMILLE>-0X` |
| Fiche source | `FT-<FAMILLE>-0X.md` |
| Test détaillé générique | `TT-<FAMILLE>-0X-GEN-XXX.md` |
| Test instancié | `TT-<FAMILLE>-0X-B<bloc>-XXX...md` selon la convention de la famille |

Les conventions particulières déjà gelées pour une famille priment lorsqu’elles sont plus précises sans contredire la présente charte.

## 7. Convention des réservés

Tout champ logique réservé doit être identifiable sans ambiguïté dans le mapping unifié, notamment au moyen de la convention `reserved_*` lorsqu’elle est applicable au mapping dérivé.

Cette convention documentaire ne modifie pas les règles normatives d’accès définies par la spécification V1.

## 8. Pipeline FT-STR de référence

Le pipeline FT-STR validé est :

1. FT-STR-08 → conformité documentaire
2. FT-STR-01 → structure des plages
3. FT-STR-02 → typage des champs
4. FT-STR-03 → encodage multi-registres
5. FT-STR-04 → ASCII fixe
6. FT-STR-05 → réservés et sentinelles
7. FT-STR-06 → accessibilité lecture
8. FT-STR-07 → stabilité d’image

## 9. Critères de qualité

- reproductibilité ;
- traçabilité ;
- absence de duplication active ;
- lisibilité ;
- exploitabilité terrain ;
- possibilité d’automatisation ;
- cohérence avec la spécification V1 gelée.

## 10. Interdictions

- duplication non justifiée de tests actifs ;
- mélange des niveaux `source` / `detaille` / `instancie` ;
- modification manuelle des instanciés sans traçabilité ;
- utilisation de fichiers d’archive comme référentiel courant ;
- promotion d’un artefact dérivé au-dessus de la spécification normative.

## 11. Objectif final

Permettre une validation :

- générable et automatisable ;
- fiable ;
- traçable ;
- industrialisable ;
- réutilisable pour FT-STR, FT-ACC, FT-LIM et les autres familles du plan de validation.
