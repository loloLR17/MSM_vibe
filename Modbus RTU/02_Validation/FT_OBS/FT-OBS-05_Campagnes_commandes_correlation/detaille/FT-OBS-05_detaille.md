# FT-OBS-05 — Procédures détaillées

## TT-OBS-B05-001 — Corrélation opératoire par transaction_id

### Objectif
Vérifier qu'une centrale peut associer sans heuristique une requête B5 à l'état observable correspondant grâce au `transaction_id`.

### Précondition
Utiliser une commande dont l'exécution est déjà validée par FT-CMD ou un simulateur/replay conforme.

### Étapes
1. Préparer une commande avec un `transaction_id = T1`.
2. Soumettre la commande selon la procédure FT-CMD, sans réévaluer ici sa conformité.
3. Lire `cmd_active_code`, `cmd_active_transaction_id`, `cmd_status` et `cmd_result_code`.
4. Vérifier que la couche centrale utilise `cmd_active_transaction_id = T1` comme clé de corrélation.
5. Soumettre ultérieurement une nouvelle commande avec `transaction_id = T2` et vérifier que la centrale distingue T1 et T2 même si `cmd_request_code` est identique.

### PASS
La corrélation repose sur l'identifiant de transaction et non sur le seul code commande ou sur l'ordre temporel supposé.

### Frontière
Les règles de soumission, d'acceptation et d'idempotence sont FT-CMD.

---

## TT-OBS-B05-002 — État courant et dernière commande terminée

### Objectif
Vérifier que la centrale distingue les informations B5 courantes de l'historique minimal de la dernière commande terminée.

### Étapes
1. Faire terminer une commande T1 par un scénario déjà conforme FT-CMD.
2. Lire `cmd_last_code`, `cmd_last_transaction_id`, `cmd_last_status_final`, `cmd_last_result_code`, `cmd_last_timestamp`.
3. Si une commande T2 est ensuite active, lire également la zone `cmd_active_*` et `cmd_status`.
4. Vérifier que l'IHM ou la logique centrale ne remplace pas l'identité de T1 par celle de T2 dans la zone « dernière commande terminée ».

### PASS
La centrale peut afficher/journaliser séparément la commande en cours ou dernière prise en compte et la dernière commande terminée.

### Limite
Aucune exigence sur une commande T-1 plus ancienne : historique > 1 `NOT_DEFINED`.

---

## TT-OBS-B06-001 — Sélection et identité de campagne

### Objectif
Vérifier qu'une centrale sait quelle campagne elle consulte sans déduire sa validité de métadonnées résiduelles.

### Étapes
1. Lire `total_campaign_count`.
2. Sélectionner un index valide connu.
3. Lire `selected_campaign_index`, `selected_campaign_valid`, `campaign_id`, `mission_id`, `campaign_state`.
4. Vérifier `selected_campaign_valid = 1` avant d'interpréter les métadonnées de l'entrée.
5. Mémoriser séparément l'index logique et `campaign_id` ; ne jamais supposer leur égalité.
6. Sélectionner un index hors plage.
7. Vérifier `selected_campaign_valid = 0`.
8. Même si des registres de métadonnées contiennent encore des valeurs non nulles, vérifier que la centrale ne les présente pas comme une campagne valide.

### PASS
La validité de sélection est pilotée par le discriminant normatif et l'identité métier de la campagne par `campaign_id`, sans heuristique sur le contenu résiduel.

### Frontière
La stabilité du snapshot pendant la lecture et les domaines sont déjà couverts par FT-STR/FT-LIM.

---

## Cas volontairement non testés par FT-OBS-05

- réexécution d'un même `transaction_id` ;
- concurrence de commandes ;
- priorités de refus ;
- détail des effets de START/STOP/RESET ;
- profondeur de l'historique au-delà de la dernière commande ;
- persistance après reboot ;
- égalités inter-blocs B4/B5/B6.

Ces sujets restent chez les familles gelées propriétaires.