# FT-OBS-03 — Procédures détaillées

## TT-OBS-B03-003 — Distinction alarme active / mémorisée

### Objectif
Vérifier que la centrale peut distinguer une alarme vibratoire active d'une alarme mémorisée à partir des champs B3 prévus par la V1.

### Données à lire
- `B3_ALARM_FLAGS` ;
- `B3_ALARM_LATCHED` ;
- `B3_SEVERITY_GLOBAL`.

### Scénarios
Selon les possibilités du banc :
1. état sans alarme ;
2. alarme active ;
3. état où une alarme mémorisée est présente ;
4. si réalisable sans hypothèse supplémentaire, état après disparition de la condition active avec mémorisation encore présente.

Les étapes 2 à 4 sont `CONDITIONAL` si le banc ne permet pas de provoquer les conditions correspondantes de façon déterministe.

### PASS
Les bits `ALARM_*_ACTIVE`, `ALARM_LATCHED_PRESENT` et le champ `B3_ALARM_LATCHED` permettent d'identifier les situations exposées conformément à leur définition normative.

### FAIL
Un état réellement provoqué ne peut pas être distingué via les indicateurs prévus, ou l'implémentation confond nécessairement alarme active et mémorisée.

### Limite
FT-OBS-03 ne définit pas la politique de déclenchement, d'effacement ou d'acquittement de l'alarme mémorisée au-delà de ce que les familles propriétaires spécifient.

---

## TT-OBS-B07-001 — Défauts actifs et dernier défaut connu

### Objectif
Vérifier que B7 permet de distinguer l'état de santé courant, les catégories de défauts courants et l'information historique minimale « dernier défaut détecté ».

### Données à lire
- `system_health_status` ;
- `system_fault_flags` ;
- `last_fault_code` ;
- `last_fault_timestamp`.

### Étapes
1. Lire l'état nominal ou courant du bloc B7.
2. Si le banc permet d'injecter un défaut connu, provoquer une catégorie couverte par `system_fault_flags`.
3. Vérifier que le bit correspondant est interprétable selon la table normative.
4. Relever `last_fault_code` et `last_fault_timestamp`.
5. Après disparition éventuelle du défaut, relire les champs sans supposer que `last_fault_code` représente encore un défaut actif.
6. Si `last_fault_code = 0`, interpréter uniquement « aucun défaut connu » ; ne pas imposer de valeur à `last_fault_timestamp`.

### PASS
Les défauts courants exposés sont décodables via `system_fault_flags`; `system_health_status` est interprétable; `last_fault_code` est exploitable comme information historique minimale et la valeur 0 est interprétée localement comme absence de défaut connu.

### FAIL
Un bit défini ou un état de santé défini ne peut pas être décodé conformément à la V1, ou `last_fault_code = 0` ne peut pas être utilisé avec sa sémantique normative.

### TRACE_ONLY
- valeur de `last_fault_timestamp` quand `last_fault_code = 0` ;
- durée de conservation du dernier défaut ;
- ordre exact de mise à jour des champs.

Aucun de ces éléments ne reçoit de verdict V1 lorsqu'il n'est pas spécifié.

---

## TT-OBS-B01B07-001 — Décodage B1 sans équivalence implicite avec B7

### Objectif
Vérifier que les catégories normatives de `B1.fault_flags` et `B1.warning_flags` sont exploitables par la centrale, sans fabriquer de correspondance bit-à-bit avec B7 ni de catalogue pour les codes B1.

### Données à lire
- `B1.fault_flags` ;
- `B1.warning_flags` ;
- `B1.error_code` ;
- `B1.warning_code` ;
- `B7.system_fault_flags` ;
- `B7.last_fault_code`.

### Vérification
1. Présenter des motifs couvrant chacun des bits définis de `B1.fault_flags` et vérifier le décodage `SENSOR_FAULT`, `ACQUISITION_FAULT`, `STORAGE_FAULT`, `TIME_FAULT`, `CONFIG_FAULT`, `SYSTEM_INTERNAL_FAULT`.
2. Présenter des motifs couvrant chacun des bits définis de `B1.warning_flags` et vérifier le décodage `STORAGE_WARNING`, `TIME_WARNING`, `TEMPERATURE_WARNING`.
3. Vérifier qu'aucune signification n'est attribuée aux bits réservés.
4. Relever éventuellement B7 dans la même fenêtre, mais ne créer aucune égalité ou correspondance bit-à-bit B1/B7 absente de la V1.
5. Enregistrer `B1.error_code` et `B1.warning_code` sans interpréter leurs valeurs au moyen d'un catalogue non normatif.

### PASS
Les bits B1 définis sont décodés conformément à leur table normative, les bits réservés ne reçoivent aucune signification, et aucune relation B1/B7 ou table de codes B1 n'est inventée.

### FAIL
Un bit B1 défini est mal interprété, un bit réservé reçoit une signification métier, ou le test/implémentation prétend déduire normativement une équivalence B1/B7 ou un catalogue de codes absent de V1.

### Remarque
Ce test porte sur l'exploitabilité des bitfields B1 et la non-fabrication de relations supplémentaires. Les relations inter-blocs restent propriétaires FT-INT.

---

## Cas volontairement non instanciés

### Acquittement général d'un défaut
`NOT_DEFINED` en V1.

### `last_fault_code != 0` implique défaut encore actif
Faux comme oracle : la V1 définit le champ comme dernier défaut détecté, pas comme défaut courant.

### `last_fault_timestamp = 0` quand aucun défaut connu
`NOT_DEFINED`.

### Persistance du dernier défaut après reboot
`DELEGATED` à FT-PER ; politique V1 largement non définie.

### Catalogues B1 `error_code` / `warning_code`
`NOT_DEFINED` en V1.