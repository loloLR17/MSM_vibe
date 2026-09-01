# FT-OBS-03 — Source normative consolidée

## 1. Références

Sources normatives principales :
- `Modbus RTU/01_Specification_source/bloc1.md` V1 ;
- `Modbus RTU/01_Specification_source/bloc3.md` V1 ;
- `Modbus RTU/01_Specification_source/bloc7.md` V1 ;
- familles FT-INT et FT-PER gelées pour les frontières inter-blocs et reboot.

Les compléments métier informatifs ne constituent jamais un oracle.

## 2. Exigences retenues

### OBS03-R01 — Alarmes actives B3
**Classification : `COVERED`.**

`B3_ALARM_FLAGS` définit explicitement les bits `ALARM_GLOBAL_ACTIVE`, `ALARM_X_ACTIVE`, `ALARM_Y_ACTIVE`, `ALARM_Z_ACTIVE`. Une centrale peut donc identifier normativement les alarmes vibratoires actives.

Test : `TT-OBS-B03-003`.

### OBS03-R02 — Alarme mémorisée B3
**Classification : `COVERED`.**

`B3_ALARM_FLAGS.ALARM_LATCHED_PRESENT` et `B3_ALARM_LATCHED` exposent explicitement la présence d'une alarme mémorisée. La V1 autorise donc la distinction locale entre alarmes actives et mémorisées dans B3.

Test : `TT-OBS-B03-003`.

### OBS03-R03 — Sévérité globale B3
**Classification : `COVERED`.**

`B3_SEVERITY_GLOBAL` distingue normativement : non applicable, normal, information, avertissement, alarme, critique. FT-OBS-03 possède l'interprétation de cette sévérité comme observable ; son origine fonctionnelle reste déléguée.

Test : `TT-OBS-B03-003`.

### OBS03-R04 — Défauts système actifs B7
**Classification : `COVERED`.**

`system_fault_flags` définit explicitement des catégories de défauts actifs : capteur, acquisition, mémoire, SD, horloge, configuration, firmware interne, surconsommation, température hors plage, communication interne.

Test : `TT-OBS-B07-001`.

### OBS03-R05 — État santé global B7
**Classification : `COVERED`.**

`system_health_status` distingue `OK`, `Warning`, `Dégradé`, `Critique`. Cette table est exploitable côté centrale comme qualification globale B7.

Test : `TT-OBS-B07-001`.

### OBS03-R06 — Dernier défaut détecté B7
**Classification : `COVERED`.**

`last_fault_code` est défini comme le code interne du dernier défaut significatif détecté. Il constitue un observable historique distinct de `system_fault_flags`, qui représente les défauts courants.

Test : `TT-OBS-B07-001`.

### OBS03-R07 — Absence de défaut connu B7
**Classification : `COVERED`.**

La V1 définit explicitement et localement `last_fault_code = 0` comme `aucun défaut connu`.

Cette sentinelle ne doit pas être généralisée aux autres codes du protocole.

Test : `TT-OBS-B07-001`.

### OBS03-R08 — Timestamp associé au dernier défaut connu
**Classification : `COVERED`.**

Le champ `last_fault_timestamp` est défini comme le timestamp du dernier défaut, sur la même base temporelle que B2. Lorsque `last_fault_code` identifie un défaut connu, ce champ est interprétable comme l'horodatage associé au dernier défaut détecté.

Test : `TT-OBS-B07-001`.

### OBS03-R09 — `last_fault_timestamp` lorsque `last_fault_code = 0`
**Classification : `NOT_DEFINED`.**

Aucune sentinelle ou valeur obligatoire n'est définie. FT-OBS-03 n'impose ni `0`, ni conservation d'une ancienne valeur, ni valeur spéciale.

### OBS03-R10 — Détails `B1.fault_flags`
**Classification : `NOT_DEFINED`.**

B1 expose un bitfield de défauts actifs, mais aucune table normative détaillée des bits n'est définie dans la section normative. Les exemples des compléments métier sont informatifs seulement.

Test de non-généralisation : `TT-OBS-B01B07-001`.

### OBS03-R11 — Détails `B1.warning_flags`
**Classification : `NOT_DEFINED`.**

Même limite que pour `fault_flags` : la présence du champ est normative, mais la signification détaillée des bits ne l'est pas.

### OBS03-R12 — Tables `B1.error_code` et `B1.warning_code`
**Classification : `NOT_DEFINED`.**

Les champs existent, mais la V1 ne fournit pas de catalogue normatif permettant à une centrale d'interpréter leurs valeurs au-delà de leur rôle général.

### OBS03-R13 — Modèle global actif / mémorisé / acquitté
**Classification : `NOT_DEFINED`.**

La V1 définit un mécanisme latched explicite dans B3 et un dernier défaut dans B7, mais elle ne définit pas un modèle transversal commun à tous les blocs.

En particulier, `last_fault_code != 0` ne signifie pas que le défaut est encore actif.

### OBS03-R14 — Acquittement universel
**Classification : `NOT_DEFINED`.**

Aucun état global `acknowledged/unacknowledged` ni mécanisme transversal d'acquittement n'est défini en V1.

### OBS03-R15 — Cohérence B1 ↔ B7 des défauts
**Classification : `DELEGATED`.**

La relation entre champs de synthèse B1 et diagnostic B7 appartient à FT-INT. FT-OBS-03 ne crée aucune égalité ou correspondance bit à bit supplémentaire.

### OBS03-R16 — Apparition et disparition d'un défaut
**Classification : `DELEGATED`.**

Les règles fonctionnelles qui font apparaître ou disparaître un défaut relèvent des familles propriétaires. FT-OBS-03 valide l'interprétation de l'observable une fois l'état présent.

### OBS03-R17 — Persistance après reboot
**Classification : `DELEGATED`.**

La survie de `last_fault_code`, `last_fault_timestamp`, des alarmes mémorisées et des flags après reset/power cycle appartient à FT-PER et reste largement non définie en V1.

## 3. Règles anti-fabrication

FT-OBS-03 interdit explicitement :
- d'interpréter les exemples informatifs de B1 comme table de bits normative ;
- d'assimiler `last_fault_code` à un défaut actif ;
- d'assimiler une alarme B3 mémorisée à un défaut système B7 ;
- d'inventer un état d'acquittement ;
- d'imposer `last_fault_timestamp = 0` lorsqu'aucun défaut n'est connu ;
- d'exiger une équivalence B1/B7 absente de la V1.

## 4. Dette V1.1 candidate

- définir les bits normatifs de `B1.fault_flags` et `B1.warning_flags` si B1 doit être exploitable sans B7 ;
- définir les catalogues `B1.error_code` / `B1.warning_code` ;
- préciser le comportement de `last_fault_timestamp` quand aucun défaut n'est connu ;
- définir, si réellement nécessaire, un modèle transversal actif / latched / acknowledged ;
- documenter la persistance post-reboot de l'historique diagnostic dans FT-PER.