# FT-SEQ-01 — Qualification initiale et contexte

## Objectif

Déterminer ce que la V1 impose réellement lorsqu'une centrale établit le contexte initial d'un capteur TR2 avant une opération métier.

## Résultat d'audit

La V1 met à disposition les informations nécessaires à une qualification initiale via les Blocs 0, 1, 2, 4, 5, 6 et 7.

En revanche, elle ne définit pas de handshake initial obligatoire, d'ordre exhaustif de lecture, de liste minimale de blocs à consulter, de marqueur « qualification terminée » ni d'interdiction générale de commander avant cette phase.

FT-SEQ-01 est donc essentiellement une sous-famille de **traçabilité et de frontière normative**.

## Couverture

- `COVERED` propriétaire FT-SEQ : 0
- `CONDITIONAL` : 0
- `DELEGATED` : 7
- `TRACE_ONLY` : 1
- `NOT_DEFINED` : 4

Aucun cas de test détaillé autonome n'est créé : fabriquer un test PASS/FAIL imposant un ordre de qualification reviendrait à inventer une exigence V1.

## Délégations

- identification et structure B0 : FT-BLK / FT-STR / FT-ACC ;
- état B1 : FT-BLK / FT-INT ;
- état temporel B2 : FT-BLK / FT-INT ;
- configuration B4 : FT-BLK / FT-INT ;
- moteur B5 : FT-CMD ;
- inventaire B6 : FT-BLK / FT-INT ;
- diagnostic B7 : FT-BLK / FT-INT.

## Usage aval

FT-SEQ-06 pourra utiliser une phase de qualification initiale comme préambule pratique d'un cycle nominal complet. L'ordre des lectures restera libre sauf dépendance normative explicite.

Voir `source/FT-SEQ-01_source.md` pour l'inventaire détaillé.
