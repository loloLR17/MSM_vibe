# FT-PER-04 — Cas détaillé

## TT-PER-B05-001 — Caractérisation du moteur transactionnel avant/après RESET SOFTWARE

**Classification : `TRACE_ONLY`.**

### Objectif

Documenter le comportement observé du Bloc 5 autour d'un reboot logiciel sans transformer ce comportement en exigence V1.

### Préconditions

- Bloc 5 lisible ;
- au moins une commande terminée avant le reboot afin que l'historique minimal soit observable ;
- RESET SOFTWARE exécutable selon FT-CMD-07 ;
- reboot établi selon FT-PER-01.

### Capture avant reboot

Relever au minimum :

- `cmd_request_code` ;
- `cmd_request_transaction_id` ;
- paramètres de requête ;
- `cmd_request_confirm_key` ;
- `cmd_request_control` ;
- `cmd_active_code` ;
- `cmd_active_transaction_id` ;
- `cmd_status` ;
- `cmd_result_code` ;
- `cmd_result_detail` ;
- `cmd_engine_flags` ;
- `cmd_last_code` ;
- `cmd_last_transaction_id` ;
- `cmd_last_status_final` ;
- `cmd_last_result_code` ;
- `cmd_last_timestamp`.

### Étapes

1. exécuter une commande de référence et attendre sa terminaison nominale ;
2. relever l'image B5 complète exploitable ;
3. exécuter RESET SOFTWARE dans ses préconditions normatives ;
4. établir la frontière réelle de reboot via FT-PER-01 ;
5. dès que Modbus redevient exploitable, relire B5 ;
6. comparer champ par champ l'état avant/après ;
7. si le banc le permet, tenter séparément un rejeu contrôlé du `transaction_id` pré-reboot et consigner le résultat sans verdict V1 de persistance.

### Résultat attendu

Aucune valeur post-reboot précise n'est exigée par V1 pour les champs transactionnels et historiques B5.

### Verdict

- aucune observation de conservation ou réinitialisation n'est un PASS de persistance ;
- aucune observation divergente n'est un FAIL FT-PER-04 ;
- les propriétés nominales sans reboot restent jugées par FT-CMD.

### Interdictions d'interprétation

Ne pas conclure, à partir d'une implémentation observée, que :

- l'historique doit survivre au reboot ;
- l'historique doit être effacé au reboot ;
- l'idempotence doit survivre au reboot ;
- l'idempotence doit être réinitialisée au reboot ;
- les champs de requête doivent être remis à zéro ;
- une commande interrompue doit être marquée échouée ou rejouée.

---

## Scénarios non transformables en PASS/FAIL V1

- rejeu post-reboot d'un transaction_id déjà traité ;
- commande active interrompue par watchdog ;
- commande active interrompue par brown-out ;
- commande active interrompue par reset externe ;
- commande active interrompue par coupure d'alimentation ;
- conservation ou perte de cmd_last_* ;
- valeur initiale de cmd_status après boot.

Tous nécessitent un arbitrage ou une évolution normative V1.1 avant de devenir des tests de conformité.
