# FT-CMD V1.1 — TRANSACTION-01

## Objet

Cette famille valide **V1.1-TRANSACTION-01 — Cycle de vie normatif du transaction_id et mapping B5**, incluant les branches de recovery TRANSACTION-04 / TRANSACTION-05 et l'observabilité temporelle historique TRANSACTION-06.

Elle reste séparée des FT-CMD V1. Les tests V1 continuent d'utiliser la spécification V1 comme oracle.

## Statut

Le mapping protocolaire V1.1 est gelé. Les cas qui étaient `PENDING_MAPPING` deviennent :

```text
SPECIFIED_NOT_EXECUTED
```

Ce statut signifie : oracle fonctionnel gelé + adresses/codes numériques gelés + test protocolaire spécifiable/exécutable, mais aucune exécution firmware n'est démontrée.

`SPECIFIED_NOT_EXECUTED` ne vaut jamais `PASS`.

## Oracle

Références :

- `Modbus RTU/00_gouvernance/FREEZE_V1_1_TRANSACTION_01.md` ;
- `Modbus RTU/01_Specification_source/V1_1/bloc5_v1_1_transaction_epoch.md` ;
- `Modbus RTU/04_Architecture/ARCHITECTURE_TRANSACTION_EPOCH_V1_1.md` ;
- `Modbus RTU/04_Architecture/RECOVERY_FAULT_INJECTION_TRANSACTION_EPOCH_V1_1.md` ;
- `Modbus RTU/02_Validation/mapping_unifie/V1_1/tr2_mapping_unifie_logique_v1_1.csv`.

## Contenu

- `FT-CMD-V11-TRANSACTION-01_detaille.md` : cas fonctionnels/protocolaires ;
- `FT-CMD-V11-TRANSACTION-01_matrice_couverture.csv` : couverture et statut ;
- fault injection : document d'architecture dédié.

## Valeurs V1.1 utilisées par les tests

```text
command 12 = RENEW_TRANSACTION_EPOCH
status 9  = RECOVERY_INDETERMINATE
result 23 = TRANSACTION_EPOCH_INVALID
result 24 = TRANSACTION_EPOCH_STALE
result 25 = TRANSACTION_EPOCH_UNKNOWN
result 26 = TRANSACTION_EPOCH_RENEWAL_NOT_ALLOWED
result 27 = TRANSACTION_IDENTITY_COLLISION
result 28 = TRANSACTION_ABORTED_BEFORE_EFFECT
result 29 = TRANSACTION_ABORTED_NO_EFFECT

epoch_status 0 UNINITIALIZED
             1 VALID
             2 CORRUPTED
             3 UNAVAILABLE
             4 UNSUPPORTED
             5 INDETERMINATE

cmd_engine_flags bit 11 = LAST_TIMESTAMP_VALID
```

Les résultats 28 et 29 sont terminaux avec `status=6` et ne sont valides que sous les conditions probatoires définies par l'architecture. En cas de preuve insuffisante après `STARTED`, l'oracle reste `status=9 RECOVERY_INDETERMINATE`.

Pour TRANSACTION-06, `LAST_TIMESTAMP_VALID=0` impose `cmd_last_timestamp=0x00000000` mais la valeur numérique zéro n'est pas, à elle seule, une sentinelle de validité. La validité historique est portée exclusivement par le bit 11. Le changement de l'état temporel courant B2 ne modifie jamais rétroactivement ce bit ni le timestamp du `LastCommandSnapshot`.
