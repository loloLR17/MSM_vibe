# TR2 — Bloc 5 V1.1 : Commandes avec transaction epoch

## 1. Statut et portée

Ce document est le compagnon protocolaire V1.1 de `../bloc5.md` pour **V1.1-TRANSACTION-01**, incluant les arbitrages de recovery TRANSACTION-04 / TRANSACTION-05.

Il ne modifie pas la spécification normative V1. Les adresses V1 `5000..5019` sont conservées. La V1.1 étend B5 jusqu'à `5028` et modifie explicitement la sémantique transactionnelle nécessaire à l'identité `(transaction_epoch, transaction_id)`.

La compatibilité transactionnelle avec une centrale V1 n'est pas requise.

## 2. Taille et mapping

```text
base      = 5000
taille    = 29 registres
offsets   = 0..28
adresses  = 5000..5028
```

| Offset | Adresse | Nom | Type | Accès |
|---:|---:|---|---|---|
| 0 | 5000 | `cmd_request_code` | uint16 | RW |
| 1 | 5001 | `cmd_request_transaction_id` | uint16 | RW |
| 2 | 5002 | `cmd_request_param1` | uint16 | RW |
| 3 | 5003 | `cmd_request_param2` | uint16 | RW |
| 4 | 5004 | `cmd_request_param3_msw` | uint16 | RW |
| 5 | 5005 | `cmd_request_param3_lsw` | uint16 | RW |
| 6 | 5006 | `cmd_request_confirm_key` | uint16 | RW |
| 7 | 5007 | `cmd_request_control` | bitfield16 | RW |
| 8 | 5008 | `cmd_active_code` | uint16 | RO |
| 9 | 5009 | `cmd_active_transaction_id` | uint16 | RO |
| 10 | 5010 | `cmd_status` | enum16 | RO |
| 11 | 5011 | `cmd_result_code` | enum16 | RO |
| 12 | 5012 | `cmd_result_detail` | uint16 | RO |
| 13 | 5013 | `cmd_engine_flags` | bitfield16 | RO |
| 14 | 5014 | `cmd_last_code` | uint16 | RO |
| 15 | 5015 | `cmd_last_transaction_id` | uint16 | RO |
| 16 | 5016 | `cmd_last_status_final` | enum16 | RO |
| 17 | 5017 | `cmd_last_result_code` | enum16 | RO |
| 18 | 5018 | `cmd_last_timestamp_msw` | uint16 | RO |
| 19 | 5019 | `cmd_last_timestamp_lsw` | uint16 | RO |
| 20 | 5020 | `cmd_request_transaction_epoch_msw` | uint16 | RW |
| 21 | 5021 | `cmd_request_transaction_epoch_lsw` | uint16 | RW |
| 22 | 5022 | `cmd_current_transaction_epoch_msw` | uint16 | RO |
| 23 | 5023 | `cmd_current_transaction_epoch_lsw` | uint16 | RO |
| 24 | 5024 | `cmd_active_transaction_epoch_msw` | uint16 | RO |
| 25 | 5025 | `cmd_active_transaction_epoch_lsw` | uint16 | RO |
| 26 | 5026 | `cmd_last_transaction_epoch_msw` | uint16 | RO |
| 27 | 5027 | `cmd_last_transaction_epoch_lsw` | uint16 | RO |
| 28 | 5028 | `cmd_transaction_epoch_status` | enum16 | RO |

Tous les champs `uint32` sont exposés MSW puis LSW et doivent être cohérents dans une même réponse Modbus.

## 3. Identité de transaction

```text
TransactionIdentity = (transaction_epoch, transaction_id)
```

`transaction_epoch=0` est invalide. Domaine valide : `1..0xFFFFFFFF`.

```text
transaction_id 1..65534 = transactions ordinaires
transaction_id 65535    = RENEW_TRANSACTION_EPOCH uniquement
```

Chaque soumission porte explicitement l'epoch cible aux adresses 5020..5021. Le capteur ne déduit jamais l'epoch de requête depuis `cmd_current_transaction_epoch_*`.

## 4. Commandes

Les codes V1 `0..11` conservent leurs valeurs.

```text
12 = RENEW_TRANSACTION_EPOCH
13..65535 = réservés
```

La requête canonique de renouvellement est :

```text
transaction_epoch=N
transaction_id=65535
command_code=12
param1=0
param2=0
param3=0
confirm_key=0
```

Une commande 12 avec txid différent de 65535, ou une autre commande avec txid 65535, est rejetée `cmd_status=5`, `cmd_result_code=2`, sans `RESERVED`.

## 5. `cmd_status`

Les valeurs V1 `0..8` restent inchangées.

```text
9 = RECOVERY_INDETERMINATE
10..65535 = réservés
```

`RECOVERY_INDETERMINATE` est non terminal, reste associé à l'identité active et ne peut pas devenir `cmd_last_status_final` tant que le recovery n'est pas résolu.

## 6. `cmd_result_code`

Les valeurs V1 `0..22` restent inchangées.

```text
23 = TRANSACTION_EPOCH_INVALID
24 = TRANSACTION_EPOCH_STALE
25 = TRANSACTION_EPOCH_UNKNOWN
26 = TRANSACTION_EPOCH_RENEWAL_NOT_ALLOWED
27 = TRANSACTION_IDENTITY_COLLISION
28 = TRANSACTION_ABORTED_BEFORE_EFFECT
29 = TRANSACTION_ABORTED_NO_EFFECT
30..65535 = réservés
```

Les résultats 23, 24, 25 et 27 sont produits avant admission d'une nouvelle transaction et ne créent pas de `RESERVED`. Le résultat 26 ne permet aucune activation d'une nouvelle epoch.

Les résultats 28 et 29 sont réservés au recovery de transactions déjà admises :

```text
28 TRANSACTION_ABORTED_BEFORE_EFFECT
→ transaction durablement RESERVED
→ recovery prouve positivement que STARTED n'a jamais été franchi
→ recovery prouve qu'aucun effet métier significatif n'a pu commencer
→ cmd_status=6
→ terminal

29 TRANSACTION_ABORTED_NO_EFFECT
→ transaction durablement STARTED
→ recovery établit ABSENCE_PROVEN
→ cmd_status=6
→ terminal
```

`ABSENCE_PROVEN` exige une preuve positive, autoritative et causalement pertinente que l'effet métier significatif attribuable à cette transaction n'a pas eu lieu. L'absence de trace, un timeout, un reboot ou un état simplement compatible avec « pas d'effet » ne constituent pas cette preuve.

Le résultat 28 est interdit si `STARTED` a été durablement franchi. Le résultat 29 est interdit si l'absence d'effet ne peut pas être positivement démontrée. Dans ce dernier cas, si aucun effet terminal ne peut non plus être prouvé, la transaction reste non terminale avec `cmd_status=9 RECOVERY_INDETERMINATE`.

Pour 28 et 29 : aucun redispatch métier ; identité consommée ; retry exact = restitution du même résultat terminal ; même identité avec requête canonique différente = résultat 27 sans modification de la transaction originale.

## 7. `cmd_transaction_epoch_status`

```text
0 = UNINITIALIZED
1 = VALID
2 = CORRUPTED
3 = UNAVAILABLE
4 = UNSUPPORTED
5 = INDETERMINATE
6..65535 = réservés
```

Projection :

```text
VALID        → current_epoch ∈ 1..0xFFFFFFFF
sinon        → current_epoch = 0
```

Toute valeur autre que `VALID` interdit l'admission de nouvelles transactions B5.

## 8. Ordre de validation

```text
1 accès Modbus
2 capture cohérente mailbox
3 transaction_epoch
4 transaction_id / couple txid-command
5 lookup (epoch,txid)
6 retry / collision
7 command_code
8 paramètres / confirm_key
9 contexte métier
10 préconditions renewal
11 admission
12 RESERVED durable
```

Priorités principales :

```text
epoch=0          → status 5 / result 23
stale epoch      → status 5 / result 24
unknown epoch    → status 5 / result 25
txid=0           → résultat V1 14
collision        → status 5 / result 27
renewal interdit → status 5 / result 26
```

Un retry exact est reconnu avant toute revalidation métier courante.

## 9. Mailbox et canonicalisation

La capture immuable comprend : `transaction_epoch`, `transaction_id`, `command_code`, `param1`, `param2`, `param3`, `confirm_key`.

Les champs inutilisés doivent être exactement à 0. Aucune normalisation silencieuse n'est autorisée.

`submit` s'auto-remet à 0 après prise en compte. `clear_request_fields` remet à zéro l'ensemble de la mailbox, y compris l'epoch de requête, sans toucher aux transactions capturées ni aux autorités persistantes.

Après activation N+1, le firmware ne réécrit pas automatiquement la mailbox de renouvellement de N vers N+1.

## 10. Zone active V1.1

La zone `active` représente uniquement une transaction non terminale autoritative.

Absence d'active : `active_epoch=0`, `active_transaction_id=0`, `active_code=0`.

Une transaction récupérée `RESERVED`, `STARTED` ou `RECOVERY_INDETERMINATE` reste active. `cmd_status=9` implique une identité active complète et bloque l'admission de nouvelles transactions.

Après terminalisation durable d'un recovery 28 ou 29, la zone active devient neutre avant/avec la projection terminale B5 cohérente.

Cette sémantique V1.1 est une évolution explicite par rapport au texte V1 où les champs active pouvaient aussi refléter la dernière commande prise en compte.

## 11. Zone last V1.1

`last` est la dernière transaction **admise** devenue terminale. Une tentative rejetée avant `RESERVED` ne crée ni ne remplace `LastCommandSnapshot`.

Une transaction terminalisée par recovery avec résultat 28 ou 29 peut devenir `last` avec `cmd_last_status_final=6` et le résultat correspondant. Aucun timestamp historique ne doit être fabriqué si l'instant fiable de terminalisation n'est pas disponible.

`last` n'est jamais une autorité d'idempotence. Sa restauration après reboot n'est requise que si une source persistante fiable permet de reconstruire un snapshot cohérent ; sinon il est neutralisé.

## 12. Renouvellement / recovery

Après activation durable de N+1 mais avant `COMPLETED` du renouvellement, la projection suivante est valide :

```text
current_epoch = N+1
active_epoch  = N
active_txid   = 65535
```

Un retry exact de `(N,65535,code12)` reste le retry de cette même transition et ne peut jamais produire N+2.

Si l'activation de N+1 ne peut être ni prouvée ni exclue, `cmd_transaction_epoch_status=5 INDETERMINATE`, `current_epoch=0` et aucune nouvelle transaction B5 n'est admise.

Après `COMPLETED`, l'artefact spécial de transition peut être libéré ; une requête de N peut alors être classée `STALE` même si `last` affiche encore le renouvellement de N.

## 13. Barrière durable de terminalisation recovery

Pour les résultats 28 et 29 :

```text
recovery concluant
→ écriture durable COMPLETED + status final 6 + result 28/29
→ barrière power-loss-safe
→ publication B5 terminale
```

Une coupure avant la barrière reprend le recovery sans redispatch métier. Une coupure après la barrière conserve le résultat terminal comme autorité et la projection B5 est reconstruite depuis cette autorité durable.
