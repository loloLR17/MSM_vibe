# Recovery fault injection — Transaction epoch V1.1

## 1. Statut

Compagnon de `ARCHITECTURE_TRANSACTION_EPOCH_V1_1.md`.

Ce document traduit les invariants V1.1-TRANSACTION-01 en scénarios falsifiables. Il ne modifie pas la baseline V1.

Les injections utilisent les abstractions normales de persistance/recovery ; aucun registre de test privé n'est requis par le protocole.

## 2. Frontières génériques

Séquence nominale :

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

L'injection doit pouvoir interrompre à chacune de ces frontières lorsque le mécanisme réel les matérialise.

## 3. Scénarios

### J-EPOCH-01 — Crash avant RESERVED

Oracle : N reste courante ; aucune transaction de renouvellement n'existe ; aucune N+1 n'existe.

### J-EPOCH-02 — Crash après RESERVED, avant STARTED

Oracle : N reste courante ; aucun effet d'epoch ; le renouvellement peut être recoveré comme transaction sans effet.

### J-EPOCH-03 — Crash après STARTED, avant préparation durable N+1

Oracle : N reste courante ; aucune activation ne peut être inférée de `STARTED` seul.

### J-EPOCH-04 — Crash après préparation N+1, avant activation

Oracle : N reste seule courante ; le candidat N+1 préparé n'a aucune autorité.

### J-EPOCH-05 — Crash immédiatement après activation N+1

Oracle : N+1 est restaurée comme seule epoch courante ; l'effet terminal est prouvé par `TransactionEpochStore` ; aucune activation supplémentaire n'est autorisée.

### J-EPOCH-06 — Crash après activation, avant `COMPLETED`

Oracle : le recovery finalise `(N,65535)` sans N+2 ; la corrélation de transition reste durable jusqu'à finalisation.

### J-EPOCH-07 — Reboots répétés au même point

Répéter le reboot après J-EPOCH-05/06.

Oracle :

```text
jamais N → N+1 → reboot → N+2
```

Le recovery est idempotent.

### J-EPOCH-08 — Retry `(N,65535)` après activation N+1

Oracle : retry K1 de la transaction de transition ; aucun nouvel effet ; N+1 reste courante ; jamais N+2.

### J-EPOCH-09 — Retry `(N,65535)` après finalisation durable

Oracle : aucune nouvelle activation. La restitution individuelle du résultat historique peut dépendre de la rétention retenue après la frontière de finalisation, mais aucun redispatch n'est permis.

### J-EPOCH-10 — Requête d'une ancienne epoch

Oracle : aucun `RESERVED`, aucun dispatch métier, aucun nouvel effet.

### J-EPOCH-11 — `transaction_epoch=0`

Oracle : rejet `TRANSACTION_EPOCH_INVALID` conceptuel ; aucune transaction créée.

### J-EPOCH-12 — Epoch différente non reconnue

Oracle : rejet `TRANSACTION_EPOCH_UNKNOWN` conceptuel ; aucune transaction créée.

### J-EPOCH-13 — TransactionEpochStore corrompu

Oracle : état interne `CORRUPTED`; aucune nouvelle transaction B5 admise ; aucune epoch inventée depuis le journal.

### J-EPOCH-14 — TransactionEpochStore indisponible

Oracle : état `UNAVAILABLE`; aucune nouvelle transaction B5 admise.

### J-EPOCH-15 — Format d'epoch non supporté

Oracle : état `UNSUPPORTED`; aucune nouvelle transaction B5 admise ; jamais normalisé en `UNINITIALIZED`.

### J-EPOCH-16 — Recovery indéterminé d'une transition

Précondition : les preuves disponibles ne permettent de démontrer ni N ni N+1 comme autorité unique.

Oracle : `INDETERMINATE`; aucune nouvelle transaction B5 admise ; aucun choix arbitraire.

### J-EPOCH-17 — Première initialisation légitime

Précondition : preuve positive `UNINITIALIZED`.

Oracle : création d'une epoch non nulle, commit durable, puis seulement admission B5.

### J-EPOCH-18 — Coupure pendant première initialisation

Oracle : jamais d'admission B5 tant qu'une epoch autoritative non nulle n'est pas récupérable ; une écriture partielle ne devient pas une epoch valide.

### J-EPOCH-19 — Corruption assimilée à tort à première initialisation

Oracle : interdit. `CORRUPTED` ne peut jamais être converti silencieusement en `UNINITIALIZED`.

### J-EPOCH-20 — Renouvellement alors qu'une autre transaction est non terminale

Oracle : rejet `TRANSACTION_EPOCH_RENEWAL_NOT_ALLOWED` conceptuel ; aucune N+1 préparée/activée.

### J-EPOCH-21 — Commande ordinaire avec txid 65535

Oracle : rejet avant effet métier ; 65535 reste réservé.

### J-EPOCH-22 — RENEW avec txid autre que 65535

Oracle : rejet ; aucune activation.

### J-EPOCH-23 — Même txid dans deux epochs successives

Exécuter `(N,42)` puis, après renouvellement, `(N+1,42)`.

Oracle : identités transactionnelles différentes ; la seconde est une nouvelle transaction et ne doit pas être confondue avec l'historique de N.

### J-EPOCH-24 — Collision dans une même epoch

Même `(N,T)` avec deux requêtes différentes.

Oracle : collision détectée ; aucun redispatch de la seconde requête.

### J-EPOCH-25 — Perte d'historique d'allocation côté centrale

Oracle côté centrale : ne pas deviner un txid libre dans N ; obtenir une nouvelle frontière d'epoch avant nouvelles transactions.

## 4. Critères transversaux

Pour tous les scénarios :

- aucune donnée B5 ne devient autorité de l'epoch ;
- aucune projection Modbus antérieure ne reconstruit l'autorité ;
- aucun `STARTED` seul ne prouve l'effet ;
- aucune ancienne epoch ne redevient active par reboot ;
- aucune collision n'est convertie en effet métier ;
- l'absence de preuve reste absence de preuve.
