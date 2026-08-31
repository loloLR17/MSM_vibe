# FT-BLK-05 — Inventaire et sélection de campagne

## 1. Objet

FT-BLK-05 valide les règles fonctionnelles intra-bloc du Bloc 6 relatives à la navigation dans l’inventaire de campagnes, à la validité de la sélection et aux invariants directement normés sur l’entrée sélectionnée.

## 2. Doctrine

La sous-famille distingue strictement :
- **COVERED** : oracle V1 directement exploitable ;
- **CONDITIONAL** : oracle V1 défini mais scénario d’exécution dépendant d’un état de campagne maîtrisable ;
- **NOT_DEFINED** : relation ou dérivation insuffisamment formalisée dans la V1 ;
- **TRACE_ONLY** : exigence déjà portée par une autre famille.

Les limitations identifiées sont conservées explicitement comme dette normative et ne sont pas comblées par hypothèse.

## 3. Périmètre actif

FT-BLK-05 couvre :
- sélection d’une campagne par `selected_campaign_index` ;
- validité de la sélection ;
- conséquences fonctionnelles d’un index hors plage ;
- invariant `campaign_id != 0` pour une campagne valide ;
- invariant `end_timestamp = 0` lorsqu’une campagne est en cours.

## 4. Limitations conservées

Ne sont pas transformés en oracles artificiels :
- égalité exacte `duration_s = end_timestamp - start_timestamp` pour tous les états ;
- relation normative `valid_campaign_count <= total_campaign_count` ;
- relation `storage_used_mb + storage_free_mb = capacité` ;
- dérivation de `storage_health_status` ;
- dérivation de `data_integrity_status` ;
- valeurs à imposer aux métadonnées lorsque `selected_campaign_valid = 0`.

## 5. Hors périmètre

- validité sémantique de l’index et comportement Modbus associé : FT-LIM / FT-ACC ;
- cohérence multi-registres, snapshot et absence de mélange entre campagnes dans une même réponse : FT-STR ;
- cohérences inter-blocs avec le temps, la campagne active ou la configuration : FT-INT ;
- persistance après reset / redémarrage : FT-PER.

## 6. Artefacts

- `source/FT-BLK-05_source.md`
- `detaille/FT-BLK-05_detaille.md`
- `detaille/FT-BLK-05_matrice_couverture.csv`

## 7. Statut

Sous-famille reconstruite sur le cadrage validé. Gel interdit avant audit croisé et validation explicite.
