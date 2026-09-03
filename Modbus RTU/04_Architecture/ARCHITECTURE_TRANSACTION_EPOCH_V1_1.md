# Architecture V1.1 — Transaction epoch et cycle de vie B5

## 1. Statut

Ce document formalise **V1.1-TRANSACTION-01**.

Statut : **FUNCTIONALLY AND PROTOCOL-MAPPING FROZEN**.

La baseline Modbus RTU V1 reste inchangée. Pour V1, la politique firmware `lifetime strict` reste applicable. Le présent document définit la promotion V1.1 ; il n'établit pas de compatibilité transactionnelle avec une centrale V1.

## 2. Problème traité

La V1 impose une idempotence par `transaction_id` mais ne définit ni frontière de réutilisation, ni cycle de vie fini du namespace, ni mécanisme interopérable après perte de l'historique d'allocation côté centrale.

V1.1 introduit une frontière explicite, persistante et indépendante du `WallClock`.

## 3. Identité transactionnelle

```text
TransactionIdentity = (transaction_epoch, transaction_id)
```

`transaction_epoch` est un `uint32` MSW/LSW :

```text
0             = invalide / réservé
1..0xFFFFFFFF = valide
```

Aucun wrap implicite n'est autorisé.

Namespace txid :

```text
1..65534 = transactions ordinaires
65535    = RENEW_TRANSACTION_EPOCH uniquement
```

Le même txid peut être utilisé dans deux epochs différentes sans collision d'identité.

## 4. Autorités

L'epoch courante est possédée par `TransactionEpochStore` ou équivalent sémantique. Le `CommandJournal` reste l'autorité transactionnelle des commandes et ne devient jamais l'autorité de l'epoch.

```text
renewal command transaction authority != current epoch authority
```

La première epoch n'est créée que depuis `UNINITIALIZED` positivement prouvé, avec une valeur non nulle choisie par l'implémentation et rendue durable avant toute admission B5.

Recovery de l'autorité d'epoch :

```text
UNINITIALIZED
VALID
CORRUPTED
UNAVAILABLE
UNSUPPORTED
INDETERMINATE
```

`CORRUPTED`, `UNAVAILABLE`, `UNSUPPORTED` et `INDETERMINATE` n'autorisent aucune nouvelle transaction B5. Aucun journal ni registre B5 ne reconstruit implicitement l'epoch courante.

## 5. Mapping B5 V1.1

B5 V1.1 conserve les offsets V1 `0..19` et ajoute :

```text
20 / 5020 cmd_request_transaction_epoch_msw    RW
21 / 5021 cmd_request_transaction_epoch_lsw    RW
22 / 5022 cmd_current_transaction_epoch_msw    RO
23 / 5023 cmd_current_transaction_epoch_lsw    RO
24 / 5024 cmd_active_transaction_epoch_msw     RO
25 / 5025 cmd_active_transaction_epoch_lsw     RO
26 / 5026 cmd_last_transaction_epoch_msw       RO
27 / 5027 cmd_last_transaction_epoch_lsw       RO
28 / 5028 cmd_transaction_epoch_status         RO enum16
```

Taille totale : 29 registres, adresses `5000..5028`.

Les `uint32` sont MSW puis LSW et cohérents dans une même réponse Modbus.

## 6. Requête immuable et mailbox

À la prise en compte du `submit`, le firmware capture de façon cohérente :

```text
transaction_epoch
transaction_id
command_code
param1
param2
param3
confirm_key
```

`submit`, `cancel_request` et `clear_request_fields` ne font pas partie de l'identité canonique.

Une écriture normale ne vide pas automatiquement les champs d'identité/paramètres. Après prise en compte, `submit` revient automatiquement à 0 conformément à la V1.

`clear_request_fields` remet à zéro la mailbox entière, y compris l'epoch de requête, mais ne modifie jamais une transaction capturée, son journal, ses effets, `TransactionEpochStore`, `active` ou `last`.

Une centrale écrit complètement les deux mots d'epoch avant `submit`. Aucun FC16 n'est imposé normativement ; une valeur transitoire hybride de mailbox n'a aucune autorité tant qu'elle n'est pas soumise.

## 7. Canonicalisation

Tout champ non utilisé doit être exactement à 0. Une nouvelle identité présentant un champ inutilisé non canonique est rejetée `status=5 / result=2` avant `RESERVED`.

Requête canonique de renouvellement :

```text
transaction_epoch = N
transaction_id    = 65535
command_code      = 12
param1            = 0
param2            = 0
param3            = 0
confirm_key       = 0
```

## 8. Ordre normatif de validation/admission

```text
1. validation accès Modbus
2. capture cohérente mailbox
3. validation transaction_epoch
4. validation transaction_id / couple txid-command
5. lookup identité (epoch,txid)
6. retry ou collision
7. validation command_code
8. validation paramètres / confirm_key
9. validation contexte métier
10. préconditions spécifiques renewal
11. admission transactionnelle
12. RESERVED durable
13. exécution K1
```

Conséquences :

- `epoch=0` → `status=5 / result=23`, aucun `RESERVED` ;
- epoch ancienne reconnue → `5/24`, aucun `RESERVED` ;
- epoch valide mais inconnue → `5/25`, aucun `RESERVED` ;
- `txid=0` → résultat V1 `14` ;
- `code=12` avec txid != 65535 ou txid=65535 avec code !=12 → `5/2`, aucun `RESERVED` ;
- même identité + même requête → retry avant revalidation métier ;
- même identité + requête différente → `5/27`, aucune nouvelle transaction ;
- renewal canonique mais préconditions non satisfaites → `5/26`, aucune activation d'epoch.

Une tentative rejetée avant `RESERVED` ne crée ni ne remplace `LastCommandSnapshot`.

## 9. Codes gelés

### `cmd_request_code`

```text
12 = RENEW_TRANSACTION_EPOCH
13..65535 = réservés
```

### `cmd_status`

Valeurs V1 `0..8` inchangées.

```text
9 = RECOVERY_INDETERMINATE
10..65535 = réservés
```

`9` est non terminal. Il ne peut jamais être publié comme `cmd_last_status_final`.

### `cmd_result_code`

Valeurs V1 `0..22` inchangées.

```text
23 = TRANSACTION_EPOCH_INVALID
24 = TRANSACTION_EPOCH_STALE
25 = TRANSACTION_EPOCH_UNKNOWN
26 = TRANSACTION_EPOCH_RENEWAL_NOT_ALLOWED
27 = TRANSACTION_IDENTITY_COLLISION
28..65535 = réservés
```

### `cmd_transaction_epoch_status`

```text
0 = UNINITIALIZED
1 = VALID
2 = CORRUPTED
3 = UNAVAILABLE
4 = UNSUPPORTED
5 = INDETERMINATE
6..65535 = réservés
```

`VALID` implique `current_epoch != 0`. Tout autre statut publie `current_epoch=0`.

## 10. Sémantique `active`, `last`, `current`

La V1.1 resserre explicitement la zone `active` : elle représente uniquement une transaction autoritative non terminale.

Absence d'active :

```text
active_epoch = 0
active_transaction_id = 0
active_code = 0
```

Une transaction `RESERVED`, `STARTED` ou `RECOVERY_INDETERMINATE` reste publiée dans `active` avec son identité complète. `active` n'est jamais reconstruit depuis une ancienne projection B5 ; il provient du recovery transactionnel fiable.

`last` représente une transaction admise devenue terminale. Il est d'observabilité uniquement :

- il peut être restauré au reboot seulement depuis une preuve persistante fiable ;
- sinon il est neutralisé ;
- sa perte ne libère jamais un txid ;
- sa présence ne prouve pas qu'un retry détaillé est encore restituable ;
- un rejet pré-`RESERVED` ne remplace pas `last`.

`current_epoch`, `active_epoch` et `last_epoch` peuvent différer.

## 11. Renouvellement d'epoch

L'epoch ne change jamais implicitement à cause d'un reboot, d'une perte de liaison, du temps, d'un changement de centrale ou de l'épuisement du namespace ordinaire.

Le renouvellement est explicitement demandé par la centrale et attribué par le capteur.

Il est interdit tant qu'une autre transaction de l'epoch courante est non terminale.

Séquence durable :

```text
epoch N active
→ RENEW RESERVED durable
→ preconditions validated
→ RENEW STARTED durable
→ N+1 prepared durable
→ DURABLE ACTIVATION N+1
→ final result durable
→ COMPLETED durable
→ final B5 publication
```

La frontière d'activation durable de N+1 est l'effet métier significatif. La préparation de N+1 n'a aucune autorité avant cette frontière.

Crash avant activation : N reste courante.

Crash après activation avant `COMPLETED` : N+1 est restaurée ; la même transaction `(N,65535)` est finalisée sans activation supplémentaire. Une corrélation durable minimale entre N+1 et `(N,65535)` est conservée jusqu'à finalisation.

Projection autorisée pendant cette fenêtre :

```text
current_epoch = N+1
active_epoch  = N
active_txid   = 65535
```

Aucun retry exact `(N,65535)` ne peut produire N+2.

Après `COMPLETED`, l'artefact spécial de transition peut être libéré immédiatement. Un retry ultérieur peut alors être classé `STALE`; la présence éventuelle de `(N,65535)` dans `last` ne constitue pas un droit à restitution détaillée.

## 12. Anciennes epochs et rétention

Dans l'epoch courante, tout txid admis reste connu pendant toute la durée de cette epoch. Un txid `1..65534` n'est jamais libéré par GC d'un résultat ou par reboot.

Après changement d'epoch :

- l'ancien namespace est fermé ;
- une ancienne requête ne peut jamais être redispatchée ;
- l'historique détaillé illimité n'est pas requis ;
- `STALE` ne se déduit pas d'une simple comparaison numérique `< current`.

L'epoch `0xFFFFFFFF` est valide. Aucun wrap vers une nouvelle epoch n'est autorisé ; un renouvellement à cette limite est refusé `result=26`.

## 13. Contrat centrale

- si l'epoch courante n'est pas connue de façon fiable, la centrale la lit avant de créer une transaction ;
- un retry conserve exactement `(epoch,txid,requête canonique)` ;
- une requête historique n'est jamais transformée en nouvelle transaction par substitution de l'epoch courante ;
- une centrale ayant perdu son historique d'allocation ne devine pas un txid libre : elle obtient une nouvelle frontière d'epoch ;
- une epoch constitue un domaine logique unique d'allocation ; plusieurs centrales physiques doivent coordonner les txid ;
- aucun `client_id` n'est ajouté par TRANSACTION-01.

## 14. Boot / Recovery

Ordre : récupérer l'autorité `TransactionEpochStore`, puis le recovery transactionnel, puis projeter B5. Aucune nouvelle admission n'est possible avant résolution.

Cas principaux :

```text
VALID(N), aucun non-terminal       → current=N, active neutre, admission oui
VALID(N), RESERVED/STARTED (N,T)   → current=N, active=(N,T), admission bloquée jusqu'à résolution
VALID(N), transaction INDETERMINATE→ current=N, active=(N,T), cmd_status=9, admission bloquée
VALID(N+1), renewal (N,65535) après activation → current=N+1, active=(N,65535), admission bloquée
UNINITIALIZED prouvé               → current=0, création durable première epoch avant admission
CORRUPTED/UNAVAILABLE/UNSUPPORTED  → current=0, aucune admission
INDETERMINATE autorité epoch       → current=0, aucune admission
```

`STARTED` seul ne prouve jamais un effet. Une transaction `STARTED` est résolue uniquement par `TERMINAL_EFFECT_PROVEN`, `ABSENCE_PROVEN` ou `INDETERMINATE`. Aucun choix du cas « le plus probable » n'est autorisé.

## 15. Invariants T01 conservés

Les invariants T01-01..T01-25 du gel fonctionnel restent applicables, notamment : identité `(epoch,txid)`, epoch 0 invalide, changement d'epoch explicite uniquement, txid 65535 réservé au renouvellement, pas de blind replay, corrélation de transition durable, pas de N+2 après retry du renouvellement, et pas de reconstruction d'autorité depuis le journal.

## 16. Points hors TRANSACTION-01

Restent séparés : factory reset/reprovisioning, politique d'échappement après `0xFFFFFFFF`, éventuel signal d'approche d'épuisement, et autres sujets résiduels TRANSACTION-02.
