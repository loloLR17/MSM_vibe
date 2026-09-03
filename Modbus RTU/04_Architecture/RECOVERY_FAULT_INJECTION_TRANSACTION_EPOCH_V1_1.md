# Recovery fault injection — Transaction epoch V1.1

## 1. Statut

Compagnon gelé de `ARCHITECTURE_TRANSACTION_EPOCH_V1_1.md`. Il couvre TRANSACTION-01, TRANSACTION-04 / TRANSACTION-05 et TRANSACTION-06. Il ne modifie pas la baseline V1.

Les injections utilisent les abstractions réelles de persistance/recovery ; aucun registre de test privé n'est requis.

## 2. Frontières générales renewal

```text
epoch N active
↓ E1
renewal RESERVED durable
↓ E2
renewal STARTED durable
↓ E3
N+1 prepared durable
↓ E4
N+1 durable activation
↓ E5
final result durable
↓ E6
COMPLETED durable
↓ E7
B5 publication
```

Pour les terminalisations recovery 28/29, la frontière générique est :

```text
état RESERVED ou STARTED récupéré
↓ R1
preuve positive autoritative obtenue
↓ R2
COMPLETED + status final 6 + result final 28/29 durables
↓ R3
power-loss-safe barrier
↓ R4
B5 publication terminale
```

## 3. Scénarios et projections

### J-EPOCH-01 — Crash avant RESERVED
N reste courante ; aucune transaction renewal ; aucune N+1.

### J-EPOCH-02 — Après RESERVED avant STARTED
N reste courante ; renewal récupérable. Si le recovery prouve positivement que `STARTED` n'a jamais été franchi et qu'aucun effet significatif n'a pu commencer, la transaction est terminalisée `status=6 / result=28 TRANSACTION_ABORTED_BEFORE_EFFECT`, sans redispatch. Tant que cette terminalisation n'est pas durable, l'active peut publier `(N,65535)`.

### J-EPOCH-03 — Après STARTED avant préparation N+1
N reste courante ; `STARTED` seul ne prouve aucune activation. Si `ABSENCE_PROVEN` est établi par l'autorité métier, la transaction est terminalisée `status=6 / result=29 TRANSACTION_ABORTED_NO_EFFECT`. Si ni absence ni effet terminal ne peuvent être prouvés, `cmd_status=9 RECOVERY_INDETERMINATE`.

### J-EPOCH-04 — Après préparation N+1 avant activation
N reste seule courante ; N+1 préparée sans autorité. `STARTED` a été franchi : result 28 est interdit. La résolution suit `ABSENCE_PROVEN`, `TERMINAL_EFFECT_PROVEN` ou `RECOVERY_INDETERMINATE`.

### J-EPOCH-05 — Immédiatement après activation N+1
`epoch_status=VALID`, `current_epoch=N+1`; la transaction `(N,65535)` reste récupérable comme active tant qu'elle n'est pas terminale ; jamais N+2.

### J-EPOCH-06 — Après activation avant COMPLETED
Projection autorisée : `current_epoch=N+1`, `active_epoch=N`, `active_txid=65535`. Recovery finalise la même transaction selon l'effet d'activation N+1 positivement prouvé, sans nouvelle activation.

### J-EPOCH-07 — Reboots répétés
Jamais `N → N+1 → reboot → N+2`. Recovery idempotent.

### J-EPOCH-08 — Retry `(N,65535)` après activation
Retry K1 de la transition ; aucun nouvel effet ; N+1 reste courante.

### J-EPOCH-09 — Retry après finalisation durable
Aucune nouvelle activation. Si l'artefact spécial a été libéré, la requête de N peut être `STALE`; `last` peut encore afficher `(N,65535)` sans créer un droit à restitution.

### J-EPOCH-10 — Ancienne epoch
`status=5`, `result=24`, aucun `RESERVED`, aucun dispatch métier.

### J-EPOCH-11 — epoch=0
`status=5`, `result=23`, aucune transaction créée.

### J-EPOCH-12 — Epoch inconnue
`status=5`, `result=25`, aucune transaction créée.

### J-EPOCH-13 — Store corrompu
`epoch_status=2 CORRUPTED`, `current_epoch=0`, aucune admission, aucune reconstruction depuis le journal.

### J-EPOCH-14 — Store indisponible
`epoch_status=3 UNAVAILABLE`, `current_epoch=0`, aucune admission.

### J-EPOCH-15 — Format non supporté
`epoch_status=4 UNSUPPORTED`, `current_epoch=0`, aucune admission, jamais normalisé en UNINITIALIZED.

### J-EPOCH-16 — Autorité de transition indéterminée
Ni N ni N+1 ne peuvent être prouvées autoritatives. Oracle : `epoch_status=5 INDETERMINATE`, `current_epoch=0`, aucune admission, aucun choix arbitraire.

### J-EPOCH-17 — Première initialisation légitime
Précondition `UNINITIALIZED` positivement prouvée. Avant activation durable : `epoch_status=0`, `current_epoch=0`. Après commit valide : `epoch_status=1`, epoch non nulle, puis seulement admission.

### J-EPOCH-18 — Coupure pendant première initialisation
Aucune admission tant qu'une epoch autoritative non nulle n'est pas récupérable ; une écriture partielle n'est jamais VALID.

### J-EPOCH-19 — Corruption assimilée à tort à première init
Interdit : CORRUPTED ne devient jamais silencieusement UNINITIALIZED.

### J-EPOCH-20 — Renewal avec autre transaction non terminale
`status=5`, `result=26`; aucune N+1 préparée/activée.

### J-EPOCH-21 — Commande ordinaire avec txid 65535
`status=5`, `result=2`; aucun RESERVED, aucun effet métier.

### J-EPOCH-22 — RENEW avec txid autre que 65535
`status=5`, `result=2`; aucune activation.

### J-EPOCH-23 — Même txid dans deux epochs
`(N,42)` et `(N+1,42)` sont deux identités différentes ; la seconde est une nouvelle transaction.

### J-EPOCH-24 — Collision même epoch
Même `(N,T)` avec requêtes différentes : `status=5`, `result=27`; aucun redispatch ; transaction originale et `last` inchangés par la tentative conflictuelle.

### J-EPOCH-25 — Perte historique allocation centrale
La centrale ne devine pas un txid libre ; elle obtient une nouvelle frontière d'epoch.

### J-EPOCH-26 — Transaction ordinaire RESERVED, crash avant STARTED
Précondition : transaction `(N,T)` admise et `RESERVED` durable ; crash avant `STARTED`. Recovery prouve positivement que `STARTED` n'a jamais été franchi et qu'aucun effet significatif n'a pu commencer.

Oracle : `COMPLETED`, `status=6`, `result=28`; aucun redispatch ; identité consommée ; après barrière durable active neutre ; la transaction peut devenir `last`.

### J-EPOCH-27 — Retry exact après result 28
Retransmettre la même requête canonique `(N,T)`.

Oracle : restitution du résultat terminal 6/28 ; aucun nouveau `RESERVED`, aucun effet métier.

### J-EPOCH-28 — Collision après result 28
Même `(N,T)` avec contenu canonique différent.

Oracle : `status=5`, `result=27`; transaction 6/28 originale inchangée ; aucun redispatch.

### J-EPOCH-29 — Transaction STARTED avec ABSENCE_PROVEN
Précondition : `(N,T)` a franchi `STARTED` durablement ; crash ; l'autorité métier établit positivement `ABSENCE_PROVEN`.

Oracle : `COMPLETED`, `status=6`, `result=29`; result 28 interdit ; aucun redispatch ; identité consommée ; active neutre après barrière durable.

### J-EPOCH-30 — STARTED sans preuve concluante
Après crash, ni `TERMINAL_EFFECT_PROVEN` ni `ABSENCE_PROVEN`.

Oracle : `cmd_status=9 RECOVERY_INDETERMINATE`, transaction non terminale et active, admission bloquée ; result 28 et 29 interdits.

### J-EPOCH-31 — Fausse absence de preuve
Recovery ne voit aucune trace de l'effet mais aucune autorité ne peut prouver son absence.

Oracle : ne jamais conclure `ABSENCE_PROVEN`; rester dans le chemin indéterminé si aucun effet terminal n'est prouvé.

### J-EPOCH-32 — Second crash avant terminalisation durable 28/29
Une preuve concluante a été obtenue mais la barrière R3 n'a pas été franchie.

Oracle : reprendre uniquement la résolution recovery ; aucun redispatch métier ; ne pas supposer le résultat terminal si sa durabilité n'est pas prouvée.

### J-EPOCH-33 — Second crash après terminalisation durable 28/29
La barrière R3 est franchie mais la projection B5 terminale n'a pas été publiée.

Oracle : restaurer le résultat terminal durable 6/28 ou 6/29 ; active neutre ; reconstruire la projection terminale depuis l'autorité persistante ; aucun redispatch.

### J-EPOCH-34 — Preuve ABSENCE_PROVEN perdue avant barrière
Le recovery a observé des éléments compatibles avec l'absence, mais la preuve autoritative n'est plus suffisamment disponible avant terminalisation durable et aucun résultat probatoire fiable n'a été capturé.

Oracle : result 29 interdit ; ne pas terminaliser par supposition ; suivre le chemin indéterminé si aucun autre résultat terminal n'est prouvé.

## 4. Recovery K1 indéterminé distinct de l'autorité epoch

Deux diagnostics distincts existent :

```text
cmd_status=9 RECOVERY_INDETERMINATE
→ transaction active non terminale, identité active conservée

epoch_status=5 INDETERMINATE
→ autorité epoch elle-même indéterminée, current_epoch=0
```

Ils ne doivent jamais être confondus.

## 5. Critères transversaux

- aucune donnée B5 ne devient autorité de l'epoch ;
- aucune projection Modbus antérieure ne reconstruit une autorité ;
- aucun `STARTED` seul ne prouve un effet ;
- result 28 exige la preuve que `STARTED` n'a jamais été franchi et qu'aucun effet significatif n'a pu commencer ;
- result 29 exige `STARTED` durable + `ABSENCE_PROVEN` positif et autoritatif ;
- result 28 est interdit après `STARTED` durable ;
- l'absence d'observation, un timeout, un reboot ou un état ambigu ne constituent jamais `ABSENCE_PROVEN` ;
- aucune ancienne epoch ne redevient active par reboot ;
- aucune collision n'est convertie en effet métier ;
- une tentative rejetée avant RESERVED ne remplace pas `last` ;
- l'absence de preuve reste absence de preuve ;
- la terminalisation durable précède toute projection B5 terminale 28/29 ;
- le firmware bloque plutôt qu'inventer une continuité.

## 6. TRANSACTION-06 — Fault injection du timestamp terminal historique

Frontière générique T06 :

```text
résultat terminal déterminé
↓ T1
construction de l'état terminal
↓ T2
COMPLETED + final_status + final_result
+ terminal_timestamp_valid
+ terminal_timestamp si valide
↓ T3
power-loss-safe barrier
↓ T4
publication B5 terminale
```

La classification K1 reste l'autorité de terminalité. T06 ne permet jamais de déduire `COMPLETED` à partir du timestamp.

### J-T06-01 — Terminalisation avec temps civil fiable
Précondition : transaction admise, terminalisation autoritative possible et temps civil fiable disponible à cet instant.

Oracle : `COMPLETED`, résultat final correct, `terminal_timestamp_valid=1`; B5 publie le `LastCommandSnapshot` cohérent, `LAST_TIMESTAMP_VALID=1` et le timestamp de la terminalisation autoritative. Ne pas dater l'effet métier ou la publication B5 à la place.

### J-T06-02 — Terminalisation sans temps civil fiable
Précondition identique mais aucun instant civil fiable disponible.

Oracle : `COMPLETED` n'est ni retardé ni bloqué ; final_status/result restent autoritatifs ; `LAST_TIMESTAMP_VALID=0`, `cmd_last_timestamp=0`. Aucune heure approximative ou synchronisation future ne remplace l'instant absent.

### J-T06-03 — Crash avant l'autorité terminale durable
Injection après détermination du résultat mais sans preuve que T3 a été franchie.

Oracle : ne pas considérer le nouveau last ni son timestamp comme autoritatifs ; reprendre le recovery transactionnel applicable. Le timestamp ne prouve jamais `COMPLETED`.

### J-T06-04 — Crash après barrière avant publication B5
Injection après T3 et avant T4.

Oracle : aucun redispatch ; restaurer depuis l'autorité durable identité, status/result, `terminal_timestamp_valid` et timestamp éventuel. valid=0 publie timestamp=0.

### J-T06-05 — Reboots répétés après timestamp valide
État durable terminal avec `valid=1`, timestamp=t.

Oracle à chaque boot : même identité/résultat, valid=1, même t. Interdit de redater au reboot ou d'invalider arbitrairement t.

### J-T06-06 — Reboots répétés après timestamp indisponible
État durable terminal avec `valid=0`.

Oracle : valid reste 0, timestamp reste 0, y compris si le temps civil devient ensuite fiable.

### J-T06-07 — Corruption isolée du timestamp
Identité/status/result du last restent positivement fiables ; la composante temporelle est corrompue de façon isolable.

Oracle : conserver le LastCommandSnapshot ; `LAST_TIMESTAMP_VALID=0`, timestamp=0. Aucun timestamp reconstruit depuis l'heure courante.

### J-T06-08 — Corruption compromettant l'autorité globale
L'intégrité ne permet plus de prouver identité/status/result comme snapshot cohérent.

Oracle : neutralisation complète du last : code/txid/status/result/timestamp/last_epoch à zéro, valid=0. Aucun best effort.

### J-T06-09 — Flag valide mais timestamp non prouvable
Stockage récupéré présentant `timestamp_valid=1` mais valeur temporelle non fiable.

Oracle : si la corruption temporelle est isolable, conserver le last et dégrader en valid=0/timestamp=0 ; sinon appliquer J-T06-08.

### J-T06-10 — Timestamp lisible mais snapshot non prouvable
La valeur temporelle paraît intègre mais identité/status/result ne sont pas autoritatifs.

Oracle : neutralisation complète. Un timestamp orphelin ne constitue jamais un LastCommandSnapshot.

### J-T06-11 — Retry exact après terminal avec timestamp valide
État terminal `(N,T)`, status S, result R, valid=1, timestamp=t. Effectuer retry exact.

Oracle : même S/R/valid/t ; aucun nouveau RESERVED, aucun nouvel effet, aucune nouvelle terminalisation.

### J-T06-12 — Retry exact après terminal sans timestamp et sync ultérieure
Terminal valid=0/timestamp=0, puis synchronisation temporelle et retry exact.

Oracle : même résultat terminal ; valid reste 0, timestamp reste 0.

### J-T06-13 — Changement de l'état temporel courant
Avec last valid=1 puis valid=0, faire varier TIME_VALID, synchronisation, perte de continuité et nouvelle synchronisation.

Oracle : identité/status/result, flag et timestamp du last restent inchangés. `LAST_TIMESTAMP_VALID` n'est jamais un miroir de B2.

### J-T06-14 — Nouveau last valide remplaçant un last sans timestamp
Last A valid=0 ; transaction B terminalise avec temps fiable.

Oracle : last=B, valid=1, timestamp=tB.

### J-T06-15 — Nouveau last sans timestamp remplaçant un last valide
Last A valid=1/tA ; B terminalise sans temps fiable.

Oracle : last=B, valid=0, timestamp=0. Interdit de conserver tA.

### J-T06-16 — `clear_request_fields` et activité mailbox
Avec un last établi, vider/modifier la mailbox sans installer de nouveau terminal.

Oracle : last, flag et timestamp inchangés.

### J-T06-17 — Cohérence intra-réponse du LastCommandSnapshot
Pendant le remplacement de A par B, lire plusieurs champs historiques dans une même réponse Modbus.

Oracle : snapshot A complet ou B complet ; jamais identité B + résultat A, valid B + timestamp A, ou autre mélange.

### J-T06-18 — Cohérence uint32 du timestamp
Lire MSW/LSW du timestamp au moment où le last peut changer.

Oracle : les deux mots proviennent du même uint32 et du même snapshot logique.

### J-T06-19 — Recovery result 28 avec temps fiable
`RESERVED` → crash → preuve positive que `STARTED` n'a jamais été franchi → terminalisation 6/28 avec temps civil fiable.

Oracle : last status=6/result=28, valid=1, timestamp de la terminalisation recovery ; ne pas dater RESERVED ou crash.

### J-T06-20 — Recovery result 28 sans temps fiable
Même scénario sans temps civil fiable à la terminalisation.

Oracle : status=6/result=28, valid=0, timestamp=0 ; identité consommée et terminalité inchangées.

### J-T06-21 — Recovery result 29 avec/sans temps fiable
`STARTED` → crash → `ABSENCE_PROVEN` → terminalisation 6/29.

Oracle : avec temps fiable, valid=1 et timestamp de terminalisation recovery ; sans temps fiable, valid=0/timestamp=0. La classification 6/29 ne dépend jamais du timestamp.

### J-T06-22 — Synchronisation après 28/29 sans timestamp
Après 6/28 ou 6/29 avec valid=0/timestamp=0, effectuer une synchronisation temporelle fiable.

Oracle : result inchangé ; valid reste 0 ; timestamp reste 0. Aucune datation rétroactive.

### J-T06-23 — Renouvellement d'epoch terminal
Le renewal `(N,65535)` active N+1 puis devient terminal et last.

Oracle : `current_epoch=N+1`, `last_epoch=N`, `last_txid=65535`. Si temps fiable à la terminalisation : valid=1/timestamp terminal ; sinon valid=0/timestamp=0. Aucun invariant `last_epoch==current_epoch` n'est requis.

### J-T06-24 — Absence de LastCommandSnapshot
Boot/recovery sans last autoritatif reconstructible.

Oracle :

```text
cmd_last_code                  = 0
cmd_last_transaction_id        = 0
cmd_last_status_final          = 0
cmd_last_result_code           = 0
cmd_last_timestamp             = 0
cmd_last_transaction_epoch     = 0
LAST_TIMESTAMP_VALID           = 0
```

Distinction : `last_epoch=0,valid=0` = aucun last ; `last_epoch!=0,valid=0` = last existant sans timestamp ; `last_epoch!=0,valid=1` = last existant avec timestamp fiable.

## 7. Critères transversaux TRANSACTION-06

- terminalité et disponibilité temporelle sont indépendantes ;
- `LAST_TIMESTAMP_VALID=0` impose timestamp zéro mais zéro n'est pas une sentinelle autonome ;
- aucune synchronisation ultérieure ne fabrique un timestamp terminal passé ;
- retry exact et reboot ne redatent jamais une transaction ;
- perte de la seule composante temporelle ne détruit pas des faits terminaux indépendamment fiables ;
- perte de l'autorité globale du snapshot neutralise la zone last ;
- une même réponse Modbus doit publier un snapshot last cohérent ;
- la terminalisation durable incluant validité/timestamp éventuel précède la publication B5 ;
- les résultats 28/29 gardent leur classification quel que soit l'état de disponibilité du timestamp ;
- `LAST_TIMESTAMP_VALID` n'est jamais calculé depuis l'état temporel B2 courant.
