# FT-OBS-05 — Source normative consolidée

## 1. Ownership

FT-CMD V1 est gelée et possède le comportement transactionnel B5. FT-OBS-05 ne vérifie que la capacité d'une centrale à **identifier et corréler ce qu'elle observe**.

## 2. Classifications

### OBS05-R01 — Corrélation requête / état B5 par transaction_id
**`COVERED`.**

B5 définit `cmd_request_transaction_id` comme obligatoire pour distinguer les commandes successives et permettre la corrélation stricte requête/réponse. `cmd_active_transaction_id` expose l'identifiant de la commande active / dernière prise en compte.

Test : `TT-OBS-B05-001`.

### OBS05-R02 — Corrélation avec la dernière commande terminée
**`COVERED`.**

`cmd_last_transaction_id`, associé à `cmd_last_code`, `cmd_last_status_final`, `cmd_last_result_code` et `cmd_last_timestamp`, fournit un historique minimal explicitement identifiable.

Test : `TT-OBS-B05-002`.

### OBS05-R03 — Distinction état courant / historique minimal
**`COVERED`.**

Les zones B5 « état courant » et « historique minimal » sont distinctes. La centrale peut donc distinguer l'état de traitement exposé et la dernière commande terminée sans réutiliser un registre unique ambigu.

Test : `TT-OBS-B05-002`.

### OBS05-R04 — Profondeur historique supérieure à une commande
**`NOT_DEFINED`.**

La V1 ne promet qu'un historique minimal de la dernière commande terminée. Aucune profondeur supplémentaire ne doit être supposée.

### OBS05-R05 — Durée de conservation de l'historique B5
**`NOT_DEFINED`.**

La durée de rétention n'est pas définie en V1. Le comportement post-reboot est en outre propriétaire FT-PER.

### OBS05-R06 — Même transaction_id avec payload différent
**`NOT_DEFINED`.**

La V1 impose l'idempotence d'un ID déjà traité mais ne définit pas exhaustivement le cas d'un même ID réutilisé avec un contenu de requête différent. Dette déjà connue FT-CMD ; FT-OBS n'invente aucun critère de corrélation supplémentaire.

### OBS05-R07 — Sélection B6 explicitement qualifiée
**`COVERED`.**

`selected_campaign_valid` distingue explicitement index invalide et campagne valide. Les métadonnées ne doivent pas être interprétées comme valides lorsque ce champ vaut 0.

Test : `TT-OBS-B06-001`.

### OBS05-R08 — Identité de campagne
**`COVERED`.**

`campaign_id` est défini comme identifiant unique de campagne et ne doit jamais être 0 pour une campagne valide. Il permet d'identifier la campagne exposée indépendamment de son index logique.

Test : `TT-OBS-B06-001`.

### OBS05-R09 — Distinction index / identifiant
**`COVERED`.**

`selected_campaign_index` est un index logique de navigation ; `campaign_id` est l'identifiant de campagne. FT-OBS-05 interdit de les confondre ou de supposer leur égalité.

Test : `TT-OBS-B06-001`.

### OBS05-R10 — Corrélation campagne / mission
**`COVERED` au niveau identifiant exposé.**

B6 expose `mission_id` dans l'entrée sélectionnée. Une centrale peut donc lire l'identifiant de mission associé à la campagne. L'unicité globale, la sémantique métier et les relations avec B4 restent hors ownership FT-OBS-05.

### OBS05-R11 — Relation stricte B4 mission_id ↔ B6 mission_id
**`DELEGATED → FT-INT`.**

Aucune nouvelle égalité inter-blocs n'est créée ici.

### OBS05-R12 — Effet d'une commande B5 sur B6
**`DELEGATED → FT-CMD / FT-SEQ / FT-INT`.**

FT-OBS-05 n'exige pas qu'une commande donnée crée, modifie ou termine une campagne au-delà des règles déjà propriétaires des familles gelées.

### OBS05-R13 — Mécanique transactionnelle B5
**`DELEGATED → FT-CMD`.**

Soumission, front montant, auto-clear, idempotence, concurrence, refus, résultats et exécution ne sont pas retestés.

### OBS05-R14 — Scénario complet centrale→commande→effet
**`DELEGATED → FT-SEQ`.**

FT-OBS vérifie les informations observables, pas le scénario fonctionnel complet.

### OBS05-R15 — Persistance de la corrélation après reboot
**`DELEGATED → FT-PER`.**

Aucune hypothèse de conservation de `cmd_last_*`, des IDs ou de l'idempotence après redémarrage.

### OBS05-R16 — Types/domaines/snapshot
**`DELEGATED → FT-STR / FT-LIM`.**

Encodage, domaines et atomicité ne sont pas dupliqués.

## 3. Dette V1.1 candidate

Sans modifier l'oracle V1 :
- profondeur/durée de rétention de l'historique B5 ;
- politique pour réutilisation d'un `transaction_id` avec payload différent ;
- politique de persistance de la corrélation/idempotence après reboot, déjà tracée FT-PER.

## 4. Conclusion

La V1 permet une corrélation opératoire déterministe **dans la fenêtre d'information explicitement exposée** : transaction B5 courante/dernière terminée et campagne B6 sélectionnée. Elle ne définit pas un journal transactionnel durable ni une corrélation historique multi-commandes.