# FT-LIM-08 — Source normative

## Références
V1 Blocs 0 et 1.

## Exigences

### LIM08-RQ-001 — Capacités
`device_capabilities` définit les bits 0..3. Les bits 4..15 sont réservés. Oracle : `(value & 0xFFF0) == 0`.

### LIM08-RQ-002 — Stabilité identification
Les informations du Bloc 0 doivent rester statiques pendant le fonctionnement normal et aucun champ ne doit dépendre d’un état dynamique du capteur.

### LIM08-RQ-003 — Identité
`device_id` doit être persistant. Son unicité est vérifiable seulement si plusieurs équipements sont disponibles.

### LIM08-RQ-004 — Cause du dernier reset
`last_reset_cause` appartient à {0,1,2,3,4,5,6}. Les valeurs 7..65535 sont réservées.

### LIM08-RQ-005 — État stockage
`storage_status` appartient à {0,1,2,3}. Les valeurs 4..65535 sont réservées.

### LIM08-RQ-006 — État acquisition
`acquisition_state` appartient à {0,1,2,3}. Les valeurs 4..65535 sont réservées.

### LIM08-RQ-007 — Uptime
`uptime_s` doit être monotone. Une diminution est acceptable uniquement en présence d’un redémarrage cohérent observé.

### LIM08-RQ-008 — Persistance conditionnelle
Les défauts et avertissements doivent rester persistants tant que la condition correspondante est présente. L’oracle nécessite une condition réelle, stable et identifiable ; aucune correspondance bit/cause non spécifiée n’est inventée.

## Domaines non définis / à arbitrer
- `system_status` : domaine détaillé non défini en V1 ;
- `system_flags`, `fault_flags`, `warning_flags` : tables de bits absentes ;
- `error_code`, `warning_code` : tables de codes absentes ;
- `internal_temp_dC` : aucune plage fonctionnelle normative ;
- `cpu_load_percent`, `memory_usage_percent`, `storage_usage_percent` : aucune borne fonctionnelle 0..100 explicitement normative.

## Relations non inventées
La règle générale « flags cohérents avec les états globaux » ne fournit pas une table assez précise pour créer des relations bit-à-bit ou état-à-état.
