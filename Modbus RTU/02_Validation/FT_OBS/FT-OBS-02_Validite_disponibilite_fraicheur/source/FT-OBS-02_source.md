# FT-OBS-02 — Source normative consolidée

## 1. Références

Sources normatives principales :
- `Modbus RTU/01_Specification_source/bloc2.md` V1 ;
- `Modbus RTU/01_Specification_source/bloc3.md` V1 ;
- familles FT-STR, FT-LIM, FT-BLK et FT-INT gelées.

Les compléments métier informatifs ne constituent jamais un oracle.

## 2. Exigences retenues

### OBS02-R01 — Validité explicite de la base de temps
**Classification : `COVERED`.**

`time_status` distingue normativement : non initialisé, temps invalide, temps valide non synchronisé, temps synchronisé et temps dégradé. Une centrale peut donc qualifier l'état de la base de temps sans interpréter la valeur numérique de `current_time`.

Test : `TT-OBS-B02-001`.

### OBS02-R02 — Drapeau explicite TIME_VALID
**Classification : `COVERED`.**

Le bit 0 de `time_flags` signifie explicitement `Temps valide`. La centrale doit utiliser ce discriminant et ne pas déduire la validité du seul fait que `current_time != 0`.

Test : `TT-OBS-B02-001`.

### OBS02-R03 — État de synchronisation explicite
**Classification : `COVERED`.**

`time_status`, `time_flags` et `sync_source` fournissent des informations normatives permettant de distinguer notamment temps valide non synchronisé et temps synchronisé. Les relations de cohérence détaillées entre ces champs restent propriétaires FT-INT.

Test : `TT-OBS-B02-001`.

### OBS02-R04 — Validité globale des valeurs B3
**Classification : `COVERED`.**

`B3_STATUS_GLOBAL` distingue explicitement valeurs valides, valeurs dégradées mais exploitables et valeurs invalides. `B3_VALIDITY_FLAGS.VALUES_VALID` fournit en outre un discriminant de validité détaillé.

Test : `TT-OBS-B03-001`.

### OBS02-R05 — Fraîcheur explicite B3
**Classification : `COVERED`.**

`B3_VALIDITY_FLAGS.VALUES_FRESH` indique explicitement si les valeurs sont considérées fraîches. `B3_VALUE_AGE_MS` expose l'âge de la dernière valeur valide et `B3_LAST_UPDATE_TR2` son horodatage de dernière mise à jour valide.

Test : `TT-OBS-B03-001`.

### OBS02-R06 — Dernière valeur conservée
**Classification : `COVERED`.**

La V1 autorise explicitement le maintien de la dernière valeur calculée en cas d'indisponibilité temporaire et définit `LAST_VALUE_HELD = 1` comme « dernière valeur conservée sans recalcul récent ».

Une centrale ne doit donc jamais considérer la simple présence d'une valeur RMS/crête comme preuve de fraîcheur.

Test : `TT-OBS-B03-002`.

### OBS02-R07 — Donnée dégradée
**Classification : `COVERED`.**

`B3_STATUS_GLOBAL = 4` signifie `Valeurs dégradées mais exploitables` et `DATA_DEGRADED=1` signifie `Données dégradées mais potentiellement utilisables`. Cette qualification est distincte de `Valeurs invalides`.

Test : `TT-OBS-B03-001`.

### OBS02-R08 — Erreur de calcul observable
**Classification : `COVERED`.**

`CALC_ERROR=1` indique explicitement une erreur de calcul détectée. FT-OBS-02 valide uniquement la discriminabilité de cette information ; la cause, les transitions et les effets appartiennent aux familles fonctionnelles propriétaires.

Test : `TT-OBS-B03-001`.

### OBS02-R09 — Seuil numérique de fraîcheur B3
**Classification : `NOT_DEFINED`.**

La V1 expose `B3_VALUE_AGE_MS` et le bit `VALUES_FRESH`, mais ne définit pas dans FT-OBS un seuil numérique universel permettant de recalculer le bit à partir de l'âge. Aucun seuil ne doit être inventé.

### OBS02-R10 — Sentinelle globale d'absence ou d'invalidité
**Classification : `NOT_DEFINED`.**

Aucune convention V1 globale ne permet d'interpréter systématiquement `0`, `0xFFFF` ou `0xFFFFFFFF` comme absent, inconnu ou invalide. Les sentinelles locales éventuelles ne peuvent pas être généralisées.

### OBS02-R11 — Distinction universelle absent / indisponible / non renseigné
**Classification : `NOT_DEFINED`.**

La V1 ne fournit pas un mécanisme transversal unique couvrant tous les blocs et tous les champs. FT-OBS-02 ne fabrique donc pas une sémantique universelle d'absence.

### OBS02-R12 — Domaines et réservés des codes/flags
**Classification : `DELEGATED`.**

La validité des valeurs enum, des bits réservés et des types est propriétaire FT-STR / FT-LIM.

### OBS02-R13 — Cohérence temporelle B2
**Classification : `DELEGATED`.**

Monotonie de `current_time`, mise à jour de `last_sync_time`, dérivation de `time_since_sync` et cohérences associées restent propriétaires FT-BLK / FT-INT.

### OBS02-R14 — Cohérence interne du snapshot B3
**Classification : `DELEGATED`.**

La cohérence d'une fenêtre de calcul, des uint32 et du snapshot appartient aux familles structurelles/fonctionnelles gelées. FT-OBS consomme ces champs comme informations qualifiantes sans reprendre leur ownership.

## 3. Règles anti-heuristique

FT-OBS-02 interdit explicitement :
- de déclarer une valeur B3 fraîche parce qu'elle change ;
- de déclarer une valeur valide parce qu'elle est non nulle ;
- de déclarer un timestamp valide parce qu'il est non nul ;
- de reconstruire un seuil `VALUES_FRESH` non défini ;
- de généraliser une sentinelle locale à tout le protocole ;
- de confondre `dégradé mais exploitable` avec `invalide`.

## 4. Dette V1.1 candidate

À documenter au backlog V1.1, sans modifier l'oracle V1 :
- préciser, si souhaité, la politique exacte produisant `VALUES_FRESH` à partir de l'âge ;
- définir seulement si nécessaire une convention transversale d'absence/non-renseignement ;
- documenter les combinaisons obligatoires/interdites entre `B3_STATUS_GLOBAL` et `B3_VALIDITY_FLAGS` si l'on souhaite un oracle inter-champs plus fort.