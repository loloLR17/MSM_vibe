# FT-STR-01 — Fiche de spécification

## Structure des plages Modbus

## 1. Identification

- **ID** : FT-STR-01
- **Nom** : Structure des plages
- **Famille parente** : FT-STR
- **Criticité** : P0

## 2. Objectif

Valider que la structure d'adressage exposée par les blocs Modbus est conforme à la spécification V1 et à son mapping unifié dérivé GEL-MAP-V1.

FT-STR-01 porte exclusivement sur la géométrie des plages : bornes, longueurs, continuité interne, absence de chevauchement et cohérence inter-blocs.

## 3. Références d'entrée

Par ordre de priorité :

1. `Modbus RTU/01_Specification_source/bloc0.md` à `bloc7.md` ;
2. `Modbus RTU/01_Specification_source/charte_typage.md` lorsque pertinent ;
3. `Modbus RTU/02_Validation/mapping_unifie/` — GEL-MAP-V1 ;
4. gouvernance gelée applicable.

Le mapping est une représentation dérivée de la V1. Il ne peut pas modifier la V1.

## 4. Périmètre inclus

- présence des blocs 0 à 7 dans le mapping dérivé ;
- adresse de début et adresse de fin de chaque bloc ;
- longueur de chaque bloc en registres ;
- couverture continue à l'intérieur de la portée définie d'un bloc ;
- absence de chevauchement entre champs ;
- absence de chevauchement entre blocs ;
- identification explicite des intervalles non exposés entre blocs ;
- cohérence de l'ordre croissant des plages.

## 5. Hors périmètre

- typage des champs : FT-STR-02 ;
- encodage multi-registres : FT-STR-03 ;
- ASCII fixe : FT-STR-04 ;
- réservés et sentinelles : FT-STR-05 ;
- accessibilité et découpage des lectures Modbus : FT-STR-06 ;
- stabilité temporelle de l'image : FT-STR-07 ;
- conformité documentaire : FT-STR-08 ;
- droits d'écriture et refus d'accès : FT-ACC et GEL-GOV-02.

## 6. Invariants de conformité

### 6.1 Bloc individuel

Un bloc est structurellement conforme si :

- sa première adresse correspond à la borne V1 ;
- sa dernière adresse correspond à la borne V1 ;
- sa longueur calculée vaut `fin - début + 1` ;
- les plages de ses champs ne se chevauchent pas ;
- la couverture interne ne présente aucun trou non défini par V1.

### 6.2 Ensemble des blocs

La structure globale est conforme si :

- les huit blocs sont représentés ;
- leurs plages sont ordonnées sans chevauchement ;
- les intervalles entre blocs sont explicitement identifiables ;
- aucun intervalle inter-blocs n'est interprété comme un bloc ou un champ implicite.

Un intervalle non exposé entre deux blocs n'est pas une anomalie structurelle lorsqu'il découle directement des bornes définies par V1.

## 7. Cas génériques à couvrir

- **GEN-001** : conformité des bornes et de la longueur d'un bloc ;
- **GEN-002** : continuité interne et absence de chevauchement ;
- **GEN-003** : cohérence globale inter-blocs.

## 8. Instanciation GEL-MAP-V1

Les invariants sont appliqués aux huit blocs :

| Bloc | Début | Fin | Longueur |
|---|---:|---:|---:|
| B0 | 0 | 20 | 21 |
| B1 | 1000 | 1019 | 20 |
| B2 | 2000 | 2015 | 16 |
| B3 | 3000 | 3047 | 48 |
| B4 | 4000 | 4175 | 176 |
| B5 | 5000 | 5019 | 20 |
| B6 | 6000 | 6063 | 64 |
| B7 | 7000 | 7015 | 16 |

Ces valeurs sont celles de GEL-MAP-V1 et doivent rester cohérentes avec la V1.

## 9. Critères de réussite

FT-STR-01 est conforme lorsque :

- 100 % des blocs respectent leurs bornes et longueurs attendues ;
- aucun trou interne non défini n'est détecté ;
- aucun chevauchement intra-bloc ou inter-bloc n'est détecté ;
- tous les intervalles inter-blocs sont identifiés sans être assimilés à des plages exposées ;
- les tests instanciés sont traçables aux tests génériques et à GEL-MAP-V1.

## 10. Critères d'échec

Constituent notamment une non-conformité FT-STR-01 :

- bloc manquant dans la représentation dérivée ;
- borne de début ou de fin différente de V1 ;
- longueur incorrecte ;
- trou interne non prévu ;
- chevauchement de champs ou de blocs ;
- création implicite d'une plage dans un intervalle inter-blocs non exposé.

## 11. Dépendances

### Amont

- V1 gelée ;
- GEL-MAP-V1 ;
- FT-STR-08 pour le contrôle documentaire préalable.

### Aval

- FT-STR-02 à FT-STR-07 utilisent une structure de plages préalablement établie.

## 12. Traçabilité

Toute évolution des bornes ou longueurs doit provenir d'une évolution explicite de la V1, suivie d'une nouvelle dérivation/version du mapping. FT-STR-01 ne doit jamais être utilisée pour modifier silencieusement la spécification normative.
