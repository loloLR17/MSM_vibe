# FT_ACC — Validation des accès et permissions Modbus

## 1. Objet

La famille **FT_ACC** valide les droits d’accès Modbus du système TR2 ainsi que l’absence d’effets de bord non spécifiés.

Elle garantit notamment que :
- les droits `RO` / `RW` / réservés sont respectés ;
- les écritures interdites sont rejetées conformément à GEL-GOV-02 ;
- les écritures autorisées ne produisent que les effets explicitement prévus par la V1 ;
- les requêtes composites invalides sont rejetées atomiquement ;
- la couverture reste cohérente avec la spécification Modbus RTU V1 gelée et son mapping dérivé.

## 2. Position dans le plan de test

FT_ACC est une famille **P0 critique**, exécutée après **FT_STR — Conformité structurelle**.

L’accessibilité en lecture des zones exposées est couverte par **FT-STR-06 gelée**. FT-ACC ne rejoue pas cette couverture structurelle.

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

En cas de divergence mapping ↔ spécification V1, la V1 fait foi et la divergence doit être remontée sans correction silencieuse du référentiel gelé.

## 4. Périmètre couvert

FT_ACC couvre :
- écriture autorisée des zones RW ;
- refus d’écriture sur zones RO non réservées ;
- refus d’écriture sur zones réservées ;
- absence d’effets de bord **non spécifiés** ;
- rejet atomique des écritures composites invalides ;
- consolidation globale de la couverture des permissions.

Une lecture portant sur un sous-ensemble valide d’une zone exposée n’est pas invalide du seul fait qu’elle est partielle.

## 5. Décomposition active

| ID | Statut | Nom | Objectif |
|---|---|---|---|
| FT-ACC-01 | **RETIRÉE** | Lecture des zones exposées | Périmètre transféré à FT-STR-06 gelée |
| FT-ACC-02 | ACTIVE | Écriture RW | Vérifier les écritures autorisées sur les 35 champs RW |
| FT-ACC-03 | ACTIVE | Refus RO | Vérifier le refus d’écriture sur les 129 champs RO non réservés |
| FT-ACC-04 | ACTIVE | Réservés | Vérifier le refus d’écriture sur les 18 zones réservées |
| FT-ACC-05 | ACTIVE | Effets non spécifiés | Vérifier qu’une écriture autorisée ne produit aucun effet non prévu par la V1 |
| FT-ACC-06 | ACTIVE | Écritures composites invalides | Vérifier le rejet atomique des requêtes mêlant RW et accès interdits |
| FT-ACC-07 | ACTIVE | Consolidation | Vérifier la couverture globale et l’absence de contradiction entre sous-familles |

## 6. Doctrine de gouvernance

### 6.1 Accès Modbus invalides
Toute requête Modbus invalide doit :
- générer une exception Modbus standard appropriée ;
- ne produire aucune modification de registre ni d’état interne imputable à la requête ;
- ne jamais être exécutée partiellement ;
- être traitée de manière déterministe.

Aucune acceptation silencieuse d’une écriture interdite n’est conforme.

### 6.2 Champs réservés
Les champs réservés exposés doivent respecter :
- lecture : comportement structurel défini par la V1 et vérifié par FT-STR ;
- écriture : refus avec exception Modbus standard appropriée ;
- aucune modification de registre ni d’état interne imputable à la requête rejetée.

L’ancienne tolérance « écriture acceptée sans effet observable » est obsolète et interdite dans le référentiel actif.

### 6.3 Valeurs métier invalides dans un registre RW
Une valeur métier hors domaine écrite dans un registre explicitement RW ne constitue pas, à elle seule, un accès Modbus invalide.

Son traitement relève des règles fonctionnelles du bloc et des familles adaptées, notamment FT-LIM / FT-BLK.

## 7. Couverture primaire

La classification primaire active comprend :
- **35 champs RW** → FT-ACC-02 ;
- **129 champs RO non réservés** → FT-ACC-03 ;
- **18 zones réservées** → FT-ACC-04 ;
- **182 cibles logiques uniques** au total.

FT-ACC-05 et FT-ACC-06 apportent les couvertures complémentaires sur les effets non spécifiés et les écritures composites invalides.

## 8. Méthodologie et industrialisation

Chaque sous-famille suit, lorsque pertinent, la structure :

```text
FT-ACC-0X/
├── README.md
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/
```

Principes :
- index CSV détaillé lorsque la couverture est instanciée par cible ;
- overview synthétique ;
- traçabilité test ↔ mapping ↔ V1 ;
- conservation des anciens référentiels sous `archive_pre_renforcement/` sans valeur exécutable V1.

## 9. Critères de validation globale

La famille FT_ACC est validée si :
- aucun champ RO non réservé n’est modifiable ;
- aucune zone réservée n’est inscriptible ;
- les écritures valides sur champs RW se comportent conformément à la V1 ;
- seuls les effets explicitement spécifiés par la V1 sont admis ;
- les écritures composites invalides sont rejetées sans exécution partielle ;
- aucune erreur métier n’est arbitrairement reclassée en erreur d’accès ;
- les 182 cibles primaires sont couvertes sans doublon ni orphelin ;
- aucune doctrine historique incompatible avec GEL-GOV-02 ne subsiste dans le chemin actif.

## 10. Résultat attendu

À l’issue de FT_ACC, les règles d’accès Modbus sont déterministes, traçables et exploitables sans ambiguïté par la centrale.

La décomposition officielle et les critères de gel sont figés dans `Specifications.md`.
