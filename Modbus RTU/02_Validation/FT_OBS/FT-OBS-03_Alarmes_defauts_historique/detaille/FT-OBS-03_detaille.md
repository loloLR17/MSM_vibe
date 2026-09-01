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

## TT-OBS-B01B07-001 — Non-généralisation des diagnostics B1 depuis B7

### Objectif
Vérifier la règle de méthode FT-OBS : une information détaillée définie dans B7 ne doit pas être utilisée pour fabriquer une table normative absente dans B1.

### Données à lire
- `B1.fault_flags` ;
- `B1.warning_flags` ;
- `B1.error_code` ;
- `B1.warning_code` ;
- `B7.system_fault_flags` ;
- `B7.last_fault_code`.

### Vérification
Le banc relève les valeurs simultanément ou dans une fenêtre documentée. Les valeurs B1 sont enregistrées mais aucun bit ni code B1 n'est interprété au-delà de ce que la V1 définit explicitement.

Même si une corrélation empirique apparaît avec B7, elle reste `TRACE_ONLY` tant qu'aucune relation normative ne l'établit.

### PASS
Le verdict de validation n'utilise aucune table inventée pour les flags/codes B1 et ne crée aucune égalité bit-à-bit B1/B7.

### FAIL
Le test ou l'implémentation de supervision prétend déduire normativement la signification détaillée des bits/codes B1 à partir des tables B7 ou des compléments métier informatifs.

### Remarque
Ce test est un test d'exploitabilité et de non-fabrication du référentiel, pas un test d'égalité inter-blocs. Les relations inter-blocs restent propriétaires FT-INT.

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

### Table détaillée B1 fault/warning
`NOT_DEFINED` malgré les exemples informatifs.