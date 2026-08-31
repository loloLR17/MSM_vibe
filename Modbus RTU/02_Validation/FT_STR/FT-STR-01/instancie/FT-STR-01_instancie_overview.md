# FT-STR-01 — Tests instanciés GEL-MAP-V1

## Objet

Appliquer les invariants génériques de FT-STR-01 aux huit blocs du mapping unifié gelé.

## Références

- V1 : `01_Specification_source/bloc0.md` à `bloc7.md`
- mapping : GEL-MAP-V1
- gel mapping : `ff948e5917becceed7637d9c7864ec9b279be0ca`
- génériques : `TT-STR-01-GEN-001` à `TT-STR-01-GEN-003`

## Principe

- une fiche instanciée par bloc applique GEN-001 et GEN-002 ;
- une fiche globale inter-blocs applique GEN-003 ;
- les intervalles inter-blocs sont explicitement recensés mais ne sont pas considérés comme des trous internes ;
- aucun comportement de lecture Modbus n'est déduit de FT-STR-01.

## Couverture GEL-MAP-V1

| Bloc | Portée | Registres | Trous internes | Chevauchements |
|---|---|---:|---:|---:|
| B0 | 0..20 | 21 | 0 | 0 |
| B1 | 1000..1019 | 20 | 0 | 0 |
| B2 | 2000..2015 | 16 | 0 | 0 |
| B3 | 3000..3047 | 48 | 0 | 0 |
| B4 | 4000..4175 | 176 | 0 | 0 |
| B5 | 5000..5019 | 20 | 0 | 0 |
| B6 | 6000..6063 | 64 | 0 | 0 |
| B7 | 7000..7015 | 16 | 0 | 0 |

## Convention d'identifiant

- `TT-STR-01-B<bloc>-001` : structure du bloc ;
- `TT-STR-01-GLOBAL-001` : cohérence inter-blocs.

## Statut documentaire

Les valeurs ci-dessus sont cohérentes avec `tr2_mapping_couverture.csv` de GEL-MAP-V1. L'exécution instrumentée éventuelle est distincte de ce constat documentaire.
