# FT-STR-08 — Fiche source

## Conformité documentaire et traçabilité

## 1. Identification

- **ID** : FT-STR-08
- **Famille parente** : FT-STR
- **Nom** : Conformité documentaire
- **Criticité** : P0

## 2. Objectif

Vérifier que les artefacts dérivés de validation sont traçables, complets et non contradictoires avec la spécification Modbus RTU V1 gelée.

FT-STR-08 ne valide pas l'implémentation réelle du capteur. Cette comparaison relève des familles d'exécution appropriées. Ici, la référence supérieure reste la V1 documentaire.

## 3. Référentiel

Hiérarchie obligatoire :

1. V1 gelée : `bloc0.md` à `bloc7.md` et `charte_typage.md` ;
2. `mapping_unifie` dérivé ;
3. présente fiche source ;
4. tests détaillés ;
5. résultats/tests instanciés.

Le mapping est une source opérationnelle d'instanciation, jamais une norme indépendante.

## 4. Périmètre inclus

- présence des huit blocs normatifs ;
- couverture des plages documentées ;
- absence de trou, chevauchement ou duplication documentaire non justifiée ;
- cohérence des noms, adresses, offsets, tailles, types et accès entre V1 et mapping ;
- cohérence raw ↔ logique ;
- traçabilité des artefacts de validation ;
- détection des règles inventées ou héritées d'anciennes versions ;
- signalement de toute information nécessaire mais absente de la V1.

## 5. Périmètre exclu

- comportement réel du firmware ;
- lecture physique du capteur ;
- validation des droits d'accès par transaction Modbus ;
- cohérence métier des valeurs ;
- performances.

## 6. Règles

Une divergence V1 ↔ mapping est une anomalie documentaire à corriger dans l'artefact dérivé, jamais dans la V1 sans arbitrage formel.

Une règle absente de la V1 ne peut pas être déduite d'un ancien test, d'un mapping ou d'une implémentation : elle est `NON DÉFINI / À ARBITRER`.

Les tests génériques ne doivent pas contenir d'adresse concrète ni être dupliqués champ par champ.

## 7. Critères d'entrée

- V1 gelée disponible ;
- mapping unifié gelé disponible ;
- gouvernance disponible.

## 8. Critères de réussite

- couverture documentaire complète ;
- aucune contradiction non résolue entre V1 et mapping ;
- aucune règle implicite ajoutée ;
- traçabilité complète V1 → mapping → validation ;
- tout point absent classé `NON DÉFINI / À ARBITRER`.

## 9. Livrables

- résultat FT-STR-08 appliqué à la version gelée du mapping ;
- liste des écarts éventuels ;
- références vers l'audit du mapping et son commit de gel.

## 10. Dépendances

### Amont

- spécification Modbus RTU V1 gelée ;
- gouvernance GEL-GOV-01 à GEL-GOV-03 ;
- mapping unifié dérivé GEL-MAP-V1.

### Aval

Toutes les autres sous-familles FT-STR et, par extension, les familles qui utilisent le mapping pour instancier leurs tests.
