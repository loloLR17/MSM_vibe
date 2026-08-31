# FT_ACC — Validation des accès et permissions Modbus

## 1. Objet

La famille **FT_ACC** couvre la validation complète des règles d’accès au mapping Modbus du système TR2.

Elle garantit que :

- les droits d’accès (`RO` / `RW` / réservés) sont respectés ;
- les comportements sont déterministes ;
- aucune écriture illégitime n’est possible ;
- le comportement observé reste conforme à la spécification Modbus RTU V1 gelée et à son mapping dérivé.

## 2. Position dans le plan de test

FT_ACC est une famille **P0 critique**, exécutée après :

- FT_STR — Conformité structurelle.

Elle constitue le socle de validation des interactions Modbus avant les tests fonctionnels métier et les tests de robustesse avancés.

## 3. Référentiel

Hiérarchie applicable :

```text
Spécification Modbus RTU V1 gelée
        ↓
mapping_unifie dérivé
        ↓
FT_ACC source
        ↓
tests détaillés
        ↓
tests instanciés
```

Le mapping unifié constitue la source opérationnelle pour l’instanciation des tests. Il n’est pas une source normative indépendante.

En cas de divergence mapping ↔ spécification V1, la divergence doit être remontée et la spécification V1 fait foi tant qu’aucune évolution normative n’est explicitement validée.

## 4. Périmètre couvert

FT_ACC couvre :

- lecture des zones exposées ;
- écriture des zones RW ;
- refus d’écriture sur zones RO ;
- refus d’écriture sur registres réservés ;
- absence d’effets de bord ;
- accès hors plage ou autrement non autorisés ;
- conformité globale spécification ↔ mapping dérivé ↔ comportement.

Une lecture portant sur un sous-ensemble valide d’une zone exposée n’est pas invalide du seul fait qu’elle est partielle.

## 5. Décomposition

| ID | Nom | Objectif |
|---|---|---|
| FT-ACC-01 | Lecture | Vérifier que toutes les zones exposées sont lisibles |
| FT-ACC-02 | Écriture RW | Vérifier le comportement d’écriture des champs RW |
| FT-ACC-03 | Refus RO | Vérifier que les champs RO sont protégés |
| FT-ACC-04 | Reserved | Vérifier la neutralité en lecture et le refus d’écriture des réservés |
| FT-ACC-05 | Effets de bord | Garantir qu’aucune écriture n’impacte d’autres champs |
| FT-ACC-06 | Accès invalides | Vérifier les accès hors plage / non autorisés |
| FT-ACC-07 | Conformité globale | Vérifier la cohérence spécification ↔ mapping ↔ comportement |

## 6. Doctrine de gouvernance

### 6.1 Accès Modbus invalides

Toute requête Modbus invalide doit :

- générer une exception Modbus standard appropriée ;
- ne produire aucune modification de registre ni d’état interne ;
- ne jamais être exécutée partiellement ;
- être traitée de manière déterministe.

Aucun comportement implicite, aucune acceptation silencieuse et aucune correction automatique ne sont autorisés.

### 6.2 Champs réservés

Les champs réservés exposés doivent respecter :

- lecture : autorisée conformément au mapping ;
- valeur : neutre conformément à la spécification, notamment `0` lorsque cette valeur est imposée ;
- écriture : refusée par une exception Modbus standard ;
- aucune modification de registre ni d’état interne à la suite de la requête rejetée.

L’ancienne tolérance « écriture acceptée sans effet observable » n’est pas conforme à la V1 gelée et n’est plus admise.

### 6.3 Valeurs métier invalides dans un registre RW

Une valeur métier hors domaine écrite dans un registre explicitement RW ne constitue pas, à elle seule, un accès Modbus invalide.

Son traitement relève des règles fonctionnelles normatives du bloc concerné et des familles de validation adaptées, notamment FT-LIM / FT-BLK selon le cas.

## 7. Méthodologie de test

Chaque sous-famille suit, lorsque les niveaux sont applicables, la structure définie par `CHARTE_ARBORESCENCE.md` :

```text
FT-ACC-0X/
├── README.md
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/
```

Niveaux principaux :

- **GEN** : cas génériques logiques ;
- **instancié** : cas réels dérivés du mapping.

## 8. Règles d’industrialisation

- un test instancié par champ ou scénario réel lorsque pertinent ;
- index CSV systématique lorsque prévu par la sous-famille ;
- overview synthétique lorsque prévu ;
- traçabilité test ↔ mapping ↔ spécification ;
- nomenclature normalisée selon la sous-famille.

## 9. Critères de validation globale

La famille FT_ACC est validée si :

- aucun champ RO n’est modifiable ;
- les écritures valides sur champs RW se comportent conformément à leur spécification ;
- les écritures sur réservés sont rejetées conformément à la doctrine V1 ;
- aucune écriture ne génère d’effet de bord non prévu ;
- tous les accès Modbus invalides sont refusés correctement ;
- aucune erreur métier n’est arbitrairement reclassée en erreur d’adressage ;
- le comportement réel est conforme à la spécification V1 et au mapping dérivé validé.

## 10. Automatisation

FT_ACC est fortement automatisable et structurée pour une exécution batch sur banc de test Modbus.

## 11. Résultat attendu

À l’issue de FT_ACC, les règles d’accès Modbus sont déterministes, traçables et exploitables sans ambiguïté par la centrale.
