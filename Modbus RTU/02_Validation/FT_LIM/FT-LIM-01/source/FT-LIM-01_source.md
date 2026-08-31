# FT-LIM-01 — Source normalisée

## 1. Référentiel

Source normative supérieure : `Modbus RTU/01_Specification_source/bloc4.md`.

Source opérationnelle d'adressage : `Modbus RTU/02_Validation/mapping_unifie/tr2_mapping_unifie_logique.csv`.

Frontières gelées :
- FT-STR : représentation et structure ;
- FT-ACC : permissions d'accès et atomicité des accès invalides.

## 2. Exigences FT-LIM-01

### LIM01-RQ-001 — Écriture métier invalide sur RW valide

Une valeur fonctionnellement invalide écrite dans un champ RW de la configuration préparée ne constitue pas une erreur d'adressage Modbus. L'écriture est acceptée au niveau Modbus ; la configuration ne doit toutefois pas pouvoir être appliquée.

### LIM01-RQ-002 — Effet d'une modification préparée

Toute modification d'un champ de la configuration préparée fait quitter un éventuel état `VALIDE` et repositionne la configuration en `BROUILLON`.

### LIM01-RQ-003 — Protection de l'image active

Une configuration invalide ne doit pas devenir active. En cas d'échec de validation/application, l'image active et son identifiant doivent rester ceux de la dernière configuration appliquée avec succès.

### LIM01-RQ-004 — Identifiants obligatoires

Pour la validation d'une configuration :
- `prepared_config_id != 0` ;
- `campaign_context_id != 0` ;
- `mission_id != 0`.

La valeur `0` signifie « non renseigné ».

### LIM01-RQ-005 — Fréquence d'échantillonnage

`sampling_frequency_hz = 26667` uniquement en V1.

### LIM01-RQ-006 — Axes actifs

`axes_enable_mask` :
- bits 0, 1 et 2 : X, Y, Z ;
- bits 3 à 15 : réservés à 0 ;
- `0x0000` invalide ;
- domaine valide V1 : `0x0001..0x0007`.

### LIM01-RQ-007 — Pleine échelle

`full_scale_code` :
- `0`, `1`, `2`, `3` valides ;
- `4..65535` réservés / invalides V1.

### LIM01-RQ-008 — Mode d'acquisition

`acquisition_mode` :
- `1` = acquisition de campagne standard ;
- `0` = non configuré, invalide pour une configuration validée ;
- `2..65535` réservés / invalides V1.

### LIM01-RQ-009 — Taille de fenêtre

`window_size_samples` appartient exactement à :
`{4096, 8192, 16384, 32768}`.

### LIM01-RQ-010 — Période d'indicateur

`indicator_period_ms` appartient exactement à :
`{2000, 5000, 10000, 30000, 60000}`.

La contrainte croisée avec la durée de fenêtre est hors FT-LIM-01 et sera traitée dans FT-LIM-02.

### LIM01-RQ-011 — Durée de campagne

`campaign_duration_s` appartient à l'intervalle inclusif `60..604800` secondes.

### LIM01-RQ-012 — Mode de stockage

`storage_mode` :
- `1` = stockage local store-and-forward ;
- `0` = non configuré, invalide pour une configuration validée ;
- `2..65535` réservés / invalides V1.

### LIM01-RQ-013 — Limite de stockage

`storage_limit_mb = 0` est invalide.

Les valeurs `1..4294967295` ne sont utilisables que si elles sont compatibles avec la capacité de stockage utilisable déterminée par le firmware. La borne dynamique n'est pas inventée par FT-LIM-01.

## 3. Champs sans domaine fonctionnel V1 instanciable

Aucune borne ou enum métier normative n'est définie en V1 pour :
- `supervision_enable_mask` ;
- `rms_warn_threshold_mg` ;
- `rms_alarm_threshold_mg` ;
- `peak_warn_threshold_mg` ;
- `peak_alarm_threshold_mg` ;
- `threshold_hysteresis_mg` ;
- `alarm_hold_time_ms` ;
- `operating_mode_code` ;
- `navigation_zone_code` ;
- `load_state_code` ;
- `sea_state_code`.

Ils restent tracés dans la matrice avec le statut `NOT_DEFINED`. Aucun test de domaine n'est inventé.

## 4. Champs volontairement renvoyés ailleurs

- `prepared_config_crc` : sémantique CRC, future FT-LIM-03 ;
- `campaign_label`, `mission_label` : contraintes d'ASCII fixe et de longueur déjà structurelles, FT-STR.

## 5. Règle de non-invention

Toute information non explicitement normative est classée `NOT_DEFINED`, `DEFERRED` ou `STRUCTURAL`. Elle ne produit aucune instanciation FT-LIM-01.
