# FT-PER-05 — Cas détaillé

## TT-PER-B01B06B07-001 — Caractérisation état/campagnes/diagnostic avant et après RESET SOFTWARE

**Classification : `TRACE_ONLY`.**

### Objectif

Observer le comportement réel de l'état système, de l'inventaire campagnes et du diagnostic à travers un RESET SOFTWARE, sans fabriquer une politique de persistance absente de V1.

### Préconditions

- RESET SOFTWARE contrôlable selon FT-PER-01 ;
- accès B1, B6 et B7 avant/après reboot ;
- si possible, inventaire B6 non vide ;
- si possible, états diagnostics significatifs préexistants ;
- aucune modification métier volontaire entre les deux relevés hors reboot.

### Capture pré-reboot

B1 :
- `fault_flags`, `warning_flags` ;
- `storage_status`, `storage_usage_percent` ;
- `acquisition_state` ;
- `active_campaign_id` ;
- `error_code`, `warning_code`.

B6 :
- `total_campaign_count` ;
- `valid_campaign_count` ;
- `selected_campaign_index` ;
- `selected_campaign_valid` ;
- `storage_used_mb`, `storage_free_mb`, `storage_health_status` ;
- si sélection valide : métadonnées complètes de l'entrée sélectionnée.

B7 :
- `system_health_status`, `system_fault_flags` ;
- `last_fault_code`, `last_fault_timestamp` ;
- `selftest_status`, `selftest_result_code`, `selftest_detail`.

### Étapes

1. réaliser la capture pré-reboot ;
2. exécuter un RESET SOFTWARE conforme à FT-PER-01 ;
3. établir le reboot et reprendre les lectures Modbus ;
4. refaire la même capture ;
5. comparer les valeurs ;
6. consigner chaque champ comme conservé, réinitialisé, recalculé, modifié ou non déterminable ;
7. ne produire aucun FAIL FT-PER-05 sur ces différences faute d'oracle V1.

### Résultat

Trace de caractérisation exploitable pour :
- conception du simulateur ;
- préparation firmware ;
- arbitrages V1.1 ;
- diagnostic des choix d'implémentation.

### Limites

Ce test ne démontre pas :
- la conformité d'une politique de persistance ;
- la conservation des campagnes après coupure d'alimentation ;
- le sort normatif d'une acquisition interrompue ;
- la persistance des défauts historiques ;
- la portée de RESET STATISTICS.
