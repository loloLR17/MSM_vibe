# V1.1-TRANSACTION-01 — Gel fonctionnel et mapping protocolaire B5

## 1. Statut

Ce document enregistre le gel **V1.1-TRANSACTION-01 — Cycle de vie normatif du `transaction_id` et mapping B5**, complété par les arbitrages de recovery **TRANSACTION-04 / TRANSACTION-05**.

Statut : **FUNCTIONALLY AND PROTOCOL-MAPPING FROZEN**.

Ce gel :

- ne modifie pas la baseline normative Modbus RTU V1 ;
- ne remplace pas la politique firmware V1 `lifetime strict` ;
- fige le modèle fonctionnel TRANSACTION-01 ;
- fige le mapping B5 V1.1, les offsets, les codes numériques et les règles de projection/recovery associées ;
- fige les résultats de recovery 28 et 29 pour les transactions déjà admises ;
- ne garantit pas la compatibilité transactionnelle d'une centrale V1 avec un capteur V1.1.

Références :

- `Modbus RTU/04_Architecture/ARCHITECTURE_TRANSACTION_EPOCH_V1_1.md` ;
- `Modbus RTU/04_Architecture/RECOVERY_FAULT_INJECTION_TRANSACTION_EPOCH_V1_1.md` ;
- `Modbus RTU/01_Specification_source/V1_1/bloc5_v1_1_transaction_epoch.md` ;
- `Modbus RTU/02_Validation/mapping_unifie/V1_1/tr2_mapping_unifie_logique_v1_1.csv` ;
- `Modbus RTU/02_Validation/FT_CMD/V1_1_TRANSACTION_01/`.

## 2. Identité transactionnelle

```text
TransactionIdentity = (transaction_epoch, transaction_id)
```

```text
transaction_epoch : uint32 MSW/LSW
0                  : invalide / réservé
1..0xFFFFFFFF      : valide

transaction_id 1..65534 : transactions ordinaires
transaction_id 65535    : RENEW_TRANSACTION_EPOCH uniquement
```

L'epoch est une autorité persistante du capteur. Reboot, perte de liaison, temps écoulé, changement de centrale et épuisement d'un txid n'autorisent jamais un changement implicite d'epoch.

## 3. Mapping B5 V1.1 gelé

```text
base      = 5000
taille    = 29 registres
offsets   = 0..28
adresses  = 5000..5028
```

Les registres V1 `5000..5019` restent aux mêmes adresses. L'extension V1.1 est :

```text
5020 cmd_request_transaction_epoch_msw    RW
5021 cmd_request_transaction_epoch_lsw    RW
5022 cmd_current_transaction_epoch_msw     RO
5023 cmd_current_transaction_epoch_lsw     RO
5024 cmd_active_transaction_epoch_msw      RO
5025 cmd_active_transaction_epoch_lsw      RO
5026 cmd_last_transaction_epoch_msw        RO
5027 cmd_last_transaction_epoch_lsw        RO
5028 cmd_transaction_epoch_status          RO enum16
```

## 4. Codes numériques gelés

### Commande

```text
12 = RENEW_TRANSACTION_EPOCH
13..65535 = réservés
```

### `cmd_status`

Les valeurs V1 `0..8` restent inchangées.

```text
9 = RECOVERY_INDETERMINATE
10..65535 = réservés
```

### `cmd_result_code`

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

Les résultats 28 et 29 sont terminaux avec `cmd_status=6`. Ils ne sont jamais des valeurs de repli face à l'incertitude : chacun exige une preuve positive conforme aux règles de recovery gelées.

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

`VALID` implique une `current_epoch` non nulle. Tout autre statut publie `current_epoch=0` et interdit l'admission de nouvelles transactions B5.

## 5. Recovery transactionnel gelé — TRANSACTION-04 / TRANSACTION-05

Classification autoritative :

```text
RESERVED récupéré
+ preuve positive que STARTED n'a jamais été franchi
+ aucun effet métier significatif n'a pu commencer
→ COMPLETED
→ cmd_status=6
→ cmd_result_code=28 TRANSACTION_ABORTED_BEFORE_EFFECT

STARTED récupéré
+ ABSENCE_PROVEN
→ COMPLETED
→ cmd_status=6
→ cmd_result_code=29 TRANSACTION_ABORTED_NO_EFFECT

STARTED récupéré
+ TERMINAL_EFFECT_PROVEN
→ finalisation selon l'effet métier terminal prouvé

STARTED récupéré
+ preuve insuffisante
→ cmd_status=9 RECOVERY_INDETERMINATE
→ non terminal / bloquant
```

`ABSENCE_PROVEN` exige une preuve positive, autoritative et causalement pertinente que l'effet métier significatif attribuable à la transaction n'a pas eu lieu. Une absence de trace, un timeout, un reboot ou un état simplement compatible avec « pas d'effet » ne suffisent jamais.

Le résultat 28 est interdit dès lors que `STARTED` a été durablement franchi. Le résultat 29 est interdit si l'absence d'effet n'est pas positivement prouvée.

Pour 28 comme pour 29 :

- aucun redispatch métier n'est autorisé ;
- l'identité `(epoch,txid)` reste consommée ;
- un retry exact restitue le même résultat terminal ;
- même identité + requête canonique différente → collision 27 ;
- la terminalisation `COMPLETED` et son résultat final deviennent durables avant la publication B5 terminale ;
- après terminalisation durable, `active` est neutre ;
- la transaction peut devenir `LastCommandSnapshot` avec `last_status_final=6` et le résultat 28 ou 29 ;
- `last` reste une projection d'observabilité, jamais une autorité de recovery/idempotence ;
- aucun `last_timestamp` historique n'est fabriqué si l'instant fiable n'est pas disponible.

Si une coupure intervient avant la barrière de terminalisation durable, le recovery reprend la résolution sans redispatch métier. Si elle intervient après cette barrière, le résultat terminal durable reste autoritatif et la projection B5 est reconstruite depuis cette autorité.

## 6. Règles transversales gelées

- chaque nouvelle soumission porte explicitement `transaction_epoch` ; aucune epoch n'est inférée depuis l'epoch courante ;
- la requête canonique immuable comprend `transaction_epoch`, `transaction_id`, `command_code`, `param1`, `param2`, `param3`, `confirm_key` ;
- les champs non utilisés doivent être canoniquement à zéro ;
- même `(epoch,txid)` + même requête = retry, jamais second effet ;
- même `(epoch,txid)` + requête différente = `TRANSACTION_IDENTITY_COLLISION`, sans redispatch ;
- une requête rejetée avant `RESERVED` ne crée ni ne remplace `LastCommandSnapshot` ;
- la zone `active` V1.1 représente uniquement une transaction non terminale ; en absence de transaction non terminale, l'identité active est `(0,0)` ;
- `last` est une projection d'observabilité d'une transaction admise et terminale, jamais une autorité d'idempotence ;
- le retry d'un renouvellement `(N,65535)` après activation de N+1 ne peut jamais produire N+2 ;
- après activation N+1 mais avant finalisation du renouvellement, `current_epoch=N+1` et l'identité active peut rester `(N,65535)` ;
- `RECOVERY_INDETERMINATE` reste non terminal et bloquant ;
- aucune valeur B5 ne reconstruit à elle seule une autorité persistante.

## 7. Requalification de V1.1-TRANSACTION-02

TRANSACTION-01 absorbe le problème principal d'épuisement du namespace `transaction_id` grâce au renouvellement explicite d'epoch. TRANSACTION-02 reste limité aux sujets résiduels :

- renouvellement temporairement interdit ;
- éventuel signal d'approche/épuisement ;
- épuisement de `transaction_epoch` à `0xFFFFFFFF` ;
- maintenance/factory reset/reprovisioning éventuel.

## 8. Traçabilité

Les arbitrages fonctionnels A..D10, mapping M1..M28, corrections transversales CROSS-01/CROSS-02, ainsi que TRANSACTION-04 T04-A..T04-F/T04-CROSS-01 et TRANSACTION-05 T05-A..T05-F sont gelés.

Les tests V1.1 associés sont `SPECIFIED_NOT_EXECUTED` lorsqu'ils dépendent du mapping protocolaire ; aucun statut n'est assimilé à `PASS` avant exécution effective.
