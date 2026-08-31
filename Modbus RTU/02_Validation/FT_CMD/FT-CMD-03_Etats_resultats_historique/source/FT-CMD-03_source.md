# FT-CMD-03 — Source normative

## 1. Sources V1

Source principale : `Modbus RTU/01_Specification_source/bloc5.md`.

Passages normatifs retenus :
- mapping `cmd_active_code`, `cmd_active_transaction_id`, `cmd_status`, `cmd_result_code`, `cmd_result_detail` ;
- table de valeurs de `cmd_status` ;
- table de valeurs de `cmd_result_code` ;
- définition de `cmd_last_code`, `cmd_last_transaction_id`, `cmd_last_status_final`, `cmd_last_result_code`, `cmd_last_timestamp` ;
- séquence d'utilisation recommandée distinguée des obligations normatives.

## 2. Exigences extraites

### CMD03-SRC-001 — Valeurs de `cmd_status`

Valeurs définies :
- `0` aucune commande ;
- `1` reçue ;
- `2` acceptée ;
- `3` en cours ;
- `4` terminée avec succès ;
- `5` refusée ;
- `6` échouée ;
- `7` inconnue ;
- `8` non autorisée dans l'état courant.

Classification : `COVERED` pour l'exposition des états finaux directement observables.

### CMD03-SRC-002 — Séquence des états intermédiaires

La V1 ne prescrit pas une progression obligatoire complète `1 -> 2 -> 3 -> final` et ne fixe aucun temps minimal de visibilité des états intermédiaires.

Classification : `NOT_DEFINED`.

### CMD03-SRC-003 — Valeurs de `cmd_result_code`

Les codes `0..22` sont définis, `23..65535` sont réservés.

La présence dans la table ne suffit pas à inventer une condition d'emploi pour chaque commande. Les oracles métier précis restent dans FT-CMD-05 à FT-CMD-07.

Classification : `COVERED` pour le domaine ; emploi métier : `DELEGATED`.

### CMD03-SRC-004 — Champs actifs

`cmd_active_code` expose la commande actuellement traitée ou la dernière prise en compte tant qu'aucune nouvelle commande n'a été acceptée. `cmd_active_transaction_id` expose l'identifiant correspondant.

Classification : `COVERED` en contexte nominal non concurrent.

### CMD03-SRC-005 — Historique minimal

La V1 définit la dernière commande terminée, quel que soit son résultat final, par :
- `cmd_last_code` ;
- `cmd_last_transaction_id` ;
- `cmd_last_status_final` ;
- `cmd_last_result_code` ;
- `cmd_last_timestamp`.

Classification : `COVERED`.

### CMD03-SRC-006 — Timestamp de fin

`cmd_last_timestamp` est le timestamp de fin de la dernière commande terminée, en secondes Epoch TR2.

Classification : `COVERED` pour la mise à jour fonctionnelle ; codage uint32 : `DELEGATED` FT-STR.

### CMD03-SRC-007 — `cmd_result_detail`

La V1 donne uniquement des exemples et `0` si non applicable, sans table exhaustive commande/cause/detail.

Classification : `NOT_DEFINED` pour une sémantique générique exhaustive.

## 3. Frontières

FT-CMD-03 ne transforme pas la séquence recommandée côté centrale en automate firmware obligatoire et ne crée aucun nouvel oracle métier à partir de la seule table de codes résultat.
