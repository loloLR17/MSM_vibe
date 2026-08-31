# FT-ACC-07 — Fiche de spécification

## 1. Identification
- **ID** : FT-ACC-07
- **Nom** : Consolidation de conformité des accès et couverture croisée
- **Famille parente** : FT-ACC
- **Criticité** : P0
- **Nature** : audit statique de consolidation

## 2. Objectif
Démontrer que le référentiel actif FT-ACC couvre sans trou, doublon logique injustifié ni contradiction les permissions d'accès définies par la V1 et instanciées par le mapping unifié.

FT-ACC-07 n'est pas une nouvelle campagne exhaustive de lecture/écriture de chaque champ : ces comportements sont déjà validés par les sous-familles spécialisées.

## 3. Références
Ordre de priorité :
1. spécification Modbus RTU V1 gelée ;
2. mapping unifié gelé, dérivé de la V1 ;
3. sources FT-ACC-02 à FT-ACC-06 ;
4. tests détaillés ;
5. tests instanciés.

La doctrine GEL-GOV-02 s'applique à toute opération interdite.

## 4. Classification primaire attendue
Chaque cible logique exposée appartient à une et une seule classe primaire :
- **RW** : 35 champs → FT-ACC-02 ;
- **RO non réservé** : 129 champs → FT-ACC-03 ;
- **réservé** : 18 zones → FT-ACC-04.

Total attendu : **182 cibles logiques uniques**.

Une zone réservée n'est pas comptée une seconde fois comme RO ordinaire, même si sa représentation documentaire porte un attribut d'accès RO.

## 5. Couvertures complémentaires
- FT-ACC-05 vérifie l'absence d'effets de bord non spécifiés des écritures autorisées, tout en acceptant les effets explicitement normatifs.
- FT-ACC-06 vérifie le rejet atomique des écritures composites invalides ciblées par son référentiel.
- FT-STR-06 gelée couvre l'accessibilité structurelle en lecture ; elle n'est pas dupliquée.

## 6. Contrôles
L'audit doit vérifier :
1. le total et l'unicité des 182 cibles primaires ;
2. l'affectation exacte à FT-ACC-02, 03 ou 04 ;
3. l'absence de cible primaire orpheline ;
4. l'absence de double classification RO/réservé ;
5. l'absence de doublon d'adresse logique non justifié ;
6. la cohérence des index avec les sources actives ;
7. l'absence de la doctrine obsolète « refusée ou sans effet observable » dans le chemin actif ;
8. l'application de GEL-GOV-02 aux écritures interdites ;
9. la cohérence des couvertures complémentaires FT-ACC-05 et FT-ACC-06 ;
10. l'absence de réintroduction d'un périmètre déjà gelé dans FT-STR.

## 7. Cas particuliers connus à contrôler
- l'adresse 4017 ne doit apparaître qu'une seule fois dans la classification primaire des réservés ;
- `B3_RESERVED_0` doit être classé comme zone réservée et non comme RO ordinaire.

## 8. Critères d'acceptation
PASS uniquement si tous les contrôles sont satisfaits et si la somme des classes primaires vaut 182 cibles uniques.

Toute divergence avec la V1 est une anomalie et ne doit pas être corrigée silencieusement dans un artefact gelé.

## 9. Automatisation
Automatisation recommandée : parser les index actifs des sous-familles et produire une matrice de consolidation avec détection des doublons, trous et conflits de classification.

## 10. Conclusion
FT-ACC-07 constitue le contrôle final de complétude et de cohérence de la famille FT-ACC ; elle ne constitue pas un quatrième passage fonctionnel sur les mêmes permissions.