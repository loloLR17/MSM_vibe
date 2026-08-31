# FT-ACC-07 — Consolidation de conformité des accès et couverture croisée

## Statut
Sous-famille active V1 — contrôle final de cohérence de FT-ACC.

## Objet
FT-ACC-07 ne rejoue pas les essais élémentaires déjà couverts par FT-ACC-02 à FT-ACC-06. Elle vérifie statiquement que la couverture des permissions Modbus est complète, non contradictoire et traçable jusqu'au mapping unifié dérivé de la V1.

## Hiérarchie documentaire
1. spécification Modbus RTU V1 gelée ;
2. mapping unifié gelé, source opérationnelle d'instanciation ;
3. `source/` ;
4. `detaille/` ;
5. `instancie/`.

## Répartition de référence
- 35 champs logiques RW → FT-ACC-02 ;
- 129 champs logiques RO non réservés → FT-ACC-03 ;
- 18 zones logiques réservées → FT-ACC-04 ;
- effets de bord des écritures autorisées → FT-ACC-05 ;
- écritures composites invalides et atomicité → FT-ACC-06.

Soit 182 cibles logiques uniques pour la classification primaire RW / RO / réservé.

La lecture des zones exposées reste couverte par FT-STR-06 gelée et n'est pas rejouée ici.

## Règle GEL-GOV-02
Toute écriture interdite doit produire une exception Modbus standard appropriée, sans modification de registre ou d'état interne, sans exécution partielle et avec un comportement déterministe. La tolérance historique « écriture acceptée mais ignorée » est interdite.

## Structure
```text
FT-ACC-07/
├── README.md
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/
```

## Principe d'exécution
FT-ACC-07 est un audit de consolidation documentaire. Les index détaillés actifs de FT-ACC-02, FT-ACC-03 et FT-ACC-04 restent les listes de référence par cible ; FT-ACC-07 les agrège sans recopier 182 fiches redondantes.

Un verdict PASS exige :
- aucune cible primaire orpheline ;
- aucune cible classée simultanément dans plusieurs classes primaires ;
- aucun doublon d'adresse logique non justifié ;
- aucune doctrine obsolète dans les tests actifs ;
- cohérence des couvertures complémentaires FT-ACC-05 et FT-ACC-06 avec leur périmètre.

Les anciens tests exhaustifs de FT-ACC-07 sont conservés uniquement dans `archive_pre_renforcement/`.