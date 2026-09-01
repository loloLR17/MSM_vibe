# FT-OBS-04 — Source normative consolidée

## 1. Règle propriétaire

FT-OBS-04 possède la question suivante : **la centrale peut-elle attribuer une signification normative à la valeur observée, sans connaissance privée du firmware ?**

La conformité du domaine numérique reste chez FT-LIM ; FT-OBS-04 traite l'interprétation.

## 2. Classifications

### OBS04-R01 — Code enum explicitement défini
**`COVERED`.**

Lorsqu'une table normative associe une valeur à une signification, la centrale peut utiliser cette signification. Cela couvre notamment B2 `time_status`, B3 `B3_STATUS_GLOBAL`/`B3_SEVERITY_GLOBAL`, B4 `config_state`, B5 `cmd_status`/`cmd_result_code`, B6 `campaign_state`/`data_integrity_status`/`storage_health_status`, B7 `system_health_status`/`selftest_status`/`reset_cause`.

Test : `TT-OBS-CODE-001`.

### OBS04-R02 — Valeur réservée
**`COVERED` pour l'interprétation.**

Une valeur explicitement réservée doit être traitée par la centrale comme **non supportée**, sans lui attribuer une signification métier implicite. La vérification que l'équipement ne produit pas certains réservés reste chez FT-LIM lorsque spécifiée.

Test : `TT-OBS-CODE-001`.

### OBS04-R03 — B1 `system_status`
**`NOT_DEFINED`.**

Le domaine détaillé est explicitement « À ARBITRER ». Les valeurs recommandées des compléments métier ne sont pas normatives.

### OBS04-R04 — B1 flags et codes non détaillés
**`NOT_DEFINED`.**

`system_flags`, `fault_flags`, `warning_flags`, `error_code`, `warning_code` ne disposent pas tous d'une sémantique normative détaillée suffisante pour une interprétation exhaustive côté centrale.

### OBS04-R05 — B4 `config_error_code`
**`NOT_DEFINED`.**

Le champ existe mais aucune table normative détaillée V1 n'est fournie. Une centrale peut journaliser la valeur brute, pas inventer sa signification.

### OBS04-R06 — B5 `cmd_result_detail`
**`NOT_DEFINED` exhaustivement.**

La V1 décrit ce champ comme détail complémentaire dépendant de la commande et donne des exemples, mais ne fournit pas un catalogue exhaustif de toutes les valeurs. Les exemples ne deviennent pas une table universelle.

### OBS04-R07 — B7 `selftest_result_code` et `selftest_detail`
**`NOT_DEFINED` exhaustivement.**

`selftest_status` est interprétable, mais les deux champs de détail ne disposent pas d'un catalogue normatif complet.

### OBS04-R08 — Sentinelle locale B4 ID=0
**`COVERED`.**

B4 définit explicitement `ID = 0 → non renseigné (interdit pour validation)`. Cette règle est locale aux identifiants visés par la convention B4 et ne doit pas être généralisée.

Test : `TT-OBS-SENT-001`.

### OBS04-R09 — Paramètres B5 non utilisés = 0
**`COVERED`.**

B5 définit explicitement que les paramètres optionnels non utilisés valent `0`. Cette convention est locale aux champs concernés.

Test : `TT-OBS-SENT-001`.

### OBS04-R10 — B6 `end_timestamp = 0`
**`COVERED`.**

B6 définit explicitement `end_timestamp = 0` si la campagne est en cours. Cette sentinelle ne signifie pas universellement « timestamp absent » dans le protocole.

Test : `TT-OBS-SENT-001`.

### OBS04-R11 — B6 `campaign_id != 0` pour campagne valide
**`COVERED`.**

B6 définit qu'un `campaign_id` ne doit jamais être `0` pour une campagne valide. La règle est locale à ce champ et à ce contexte.

Test : `TT-OBS-SENT-001`.

### OBS04-R12 — B7 `last_fault_code = 0`
**`COVERED`.**

B7 définit explicitement `0 = aucun défaut connu`. Cette sentinelle est locale.

Test : `TT-OBS-SENT-001`.

### OBS04-R13 — Sentinelle universelle zéro
**`NOT_DEFINED`.**

Les conventions locales précédentes sont différentes et ne permettent aucune règle transversale `0 = absent/invalide/inconnu`.

### OBS04-R14 — Base temporelle des timestamps B6/B7
**`COVERED`.**

B6 `start_timestamp`/`end_timestamp` et B7 `last_fault_timestamp` sont explicitement rattachés à la base temporelle du Bloc 2. Une centrale peut donc interpréter leur unité/référentiel conformément à B2 lorsque la valeur elle-même est applicable.

Test : `TT-OBS-TIME-001`.

### OBS04-R15 — Timestamp B7 sans défaut connu
**`NOT_DEFINED`.**

Lorsque `last_fault_code = 0`, aucune valeur obligatoire de `last_fault_timestamp` n'est définie.

### OBS04-R16 — Fraîcheur universelle des timestamps
**`NOT_DEFINED`.**

Le fait qu'un champ soit un timestamp ne définit pas automatiquement sa fraîcheur ni sa durée de validité. Les mécanismes spécifiques B2/B3 restent locaux.

### OBS04-R17 — Domaines numériques
**`DELEGATED → FT-LIM`.**

FT-OBS ne reprend pas les tests de plage, bits réservés ou codes interdits.

### OBS04-R18 — Encodage temporel et uint32
**`DELEGATED → FT-STR`.**

MSW/LSW, atomicité et cohérence structurelle ne sont pas retestés ici.

### OBS04-R19 — Relations entre timestamps
**`DELEGATED → FT-BLK / FT-INT`.**

Monotonie, égalités, calculs de durée et relations inter-blocs restent chez leurs propriétaires.

### OBS04-R20 — Interprétation des états/résultats B5
**`DELEGATED → FT-CMD` pour la mécanique, `COVERED` par OBS04-R01 pour le vocabulaire.**

Aucune nouvelle exigence transactionnelle n'est créée.

## 3. Règles anti-heuristique

- jamais utiliser une table informative comme oracle ;
- jamais attribuer un sens métier à un réservé ;
- jamais généraliser une sentinelle locale ;
- jamais interpréter un code de détail sans catalogue normatif ;
- jamais supposer qu'un timestamp non nul est valide/frais par nature.

## 4. Dette V1.1 candidate

- table normative `B1.system_status` ;
- tables B1 flags/error/warning ;
- table `B4.config_error_code` ;
- catalogue `B5.cmd_result_detail` si nécessaire à l'exploitation distante ;
- catalogue `B7.selftest_result_code/selftest_detail` ;
- convention de `last_fault_timestamp` en absence de défaut connu.