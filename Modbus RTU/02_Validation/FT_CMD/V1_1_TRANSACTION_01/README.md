# FT-CMD V1.1 — TRANSACTION-01

## Objet

Cette famille valide le gel fonctionnel **V1.1-TRANSACTION-01 — Cycle de vie normatif du `transaction_id`**.

Elle est volontairement séparée des cas FT-CMD V1 existants. Les tests V1 restent inchangés et continuent d'utiliser la spécification V1 comme oracle.

## Oracle

Références :

- `Modbus RTU/04_Architecture/ARCHITECTURE_TRANSACTION_EPOCH_V1_1.md` ;
- `Modbus RTU/04_Architecture/RECOVERY_FAULT_INJECTION_TRANSACTION_EPOCH_V1_1.md` ;
- `Modbus RTU/00_gouvernance/FREEZE_V1_1_TRANSACTION_01.md`.

Les offsets B5 et valeurs numériques des nouveaux codes n'étant pas encore gelés, les tests de cette famille utilisent les noms conceptuels des champs/résultats.

Ils deviendront exécutables au niveau protocolaire complet après la passe mapping/compatibilité.

## Contenu

- `FT-CMD-V11-TRANSACTION-01_detaille.md` : cas fonctionnels ;
- `FT-CMD-V11-TRANSACTION-01_matrice_couverture.csv` : couverture des invariants T01.

## Principe

Un test est `PENDING_MAPPING` lorsque son oracle fonctionnel est gelé mais que son exécution Modbus nécessite des adresses ou valeurs numériques encore non attribuées.

Ce statut ne doit pas être converti en `PASS`.
