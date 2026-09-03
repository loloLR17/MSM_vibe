# Architecture V1.1 — Transaction epoch et cycle de vie B5

## 1. Statut

Ce document formalise l'arbitrage fonctionnel **V1.1-TRANSACTION-01**.

Il est **FUNCTIONALLY FROZEN** mais ne constitue pas encore, à lui seul, la spécification protocolaire normative V1.1. Les offsets B5, codes numériques et règles de compatibilité V1/V1.1 restent à intégrer explicitement lors de la passe mapping.

La baseline V1 reste inchangée. Pour V1, la politique firmware actuelle `lifetime strict` reste applicable.

## 2. Problème traité

La V1 impose l'idempotence par `transaction_id` mais ne définit pas :

- la durée/profondeur de conservation de l'historique ;
- le moment où un identifiant ancien peut être réutilisé ;
- le wrap après épuisement du domaine 16 bits ;
- une frontière interopérable permettant à la centrale et au capteur de savoir si un identifiant appartient encore à l'histoire courante.

V1.1 doit fournir une frontière explicite et observable sans dépendre du `WallClock`.

## 3. Identité transactionnelle

La V1.1 retient :

```text
TransactionIdentity = (transaction_epoch, transaction_id)
```

`transaction_id` seul n'est donc plus suffisant comme identité globale.

Le même `transaction_id` peut être utilisé dans deux epochs différentes sans collision d'identité.

## 4. Autorité de l'epoch

L'epoch courante est possédée par une autorité persistante dédiée :

```text
TransactionEpochStore
```

ou un composant sémantiquement équivalent.

Le `CommandJournal` reste l'autorité transactionnelle des commandes et ne devient jamais l'autorité de l'epoch courante.

Séparation :

```text
renewal command transaction authority != current epoch business authority
```

## 5. Domaine de `transaction_epoch`

Le champ est un `uint32`, encodé MSW puis LSW.

```text
0              = invalide / réservé
1..0xFFFFFFFF  = valide
```

Aucun wrap implicite de l'epoch n'est autorisé.

La valeur initiale exacte de la première epoch reste `IMPLEMENTATION`, sous réserve qu'elle soit non nulle et durable avant toute admission B5.

## 6. Représentation conceptuelle B5

La V1.1 devra permettre à chaque soumission de porter explicitement l'epoch cible et devra exposer séparément l'epoch courante autoritative.

Champs conceptuels :

```text
cmd_request_transaction_epoch_msw  RW
cmd_request_transaction_epoch_lsw  RW
cmd_current_transaction_epoch_msw  RO
cmd_current_transaction_epoch_lsw  RO
```

Les offsets/adresses exacts sont différés à la passe mapping.

La requête immuable capturée doit contenir au minimum :

```text
transaction_epoch
transaction_id
command_code
param1
param2
param3
confirm_key
```

## 7. Admission transactionnelle

Ordre logique :

```text
capture coherent request
↓
Modbus syntax validation
↓
transaction_epoch validation
↓
transaction_id validation
↓
idempotence / collision lookup
↓
command / parameter / business validation
↓
RESERVED / processing
```

Les erreurs d'epoch sont rejetées avant tout dispatch métier et avant création d'une nouvelle transaction `RESERVED`.

## 8. Cycle de vie de l'epoch

L'epoch ne change jamais implicitement à cause :

- d'un reboot capteur ;
- d'un reboot centrale ;
- d'une perte de liaison ;
- du temps écoulé ;
- d'un changement de centrale ;
- d'un `transaction_id` atteignant sa valeur maximale.

Le renouvellement est une opération protocolaire explicite.

La centrale demande le renouvellement ; le capteur possède l'autorité d'attribution de la nouvelle valeur.

## 9. Namespace et commande de renouvellement

La V1.1 réserve :

```text
1..65534 → transactions ordinaires
65535    → RENEW_TRANSACTION_EPOCH uniquement
```

Règles :

- une commande ordinaire avec `transaction_id=65535` est rejetée sans effet métier ;
- `RENEW_TRANSACTION_EPOCH` avec un txid différent de `65535` est rejetée ;
- le renouvellement reste une transaction K1 complète avec `RESERVED`, `STARTED`, recovery et `COMPLETED` ;
- le renouvellement ne peut se produire qu'une fois dans une epoch ;
- un retry exact `(epoch N,65535)` après activation de N+1 ne déclenche jamais N+2 ;
- aucune consommation minimale préalable des txid ordinaires n'est imposée.

## 10. Précondition de renouvellement

Le renouvellement est interdit tant qu'une transaction de l'epoch courante est non terminale (`RESERVED` ou `STARTED`).

La transaction de renouvellement elle-même est évidemment exclue de cette interdiction une fois admise.

## 11. Séquence durable de renouvellement

Ordre logique :

```text
epoch N active
↓
RENEW_TRANSACTION_EPOCH RESERVED
↓
preconditions validated
↓
RENEW STARTED
↓
N+1 prepared durably
↓
DURABLE ACTIVATION of N+1   ← significant business effect boundary
↓
final command result durable
↓
COMPLETED
↓
final B5 publication
```

La préparation de N+1 n'a aucune autorité avant la frontière d'activation durable.

## 12. Recovery autour de l'activation

### 12.1 Crash avant activation N+1

N reste la seule epoch courante. Une représentation préparée de N+1 ne possède aucune autorité.

### 12.2 Crash après activation N+1 mais avant `COMPLETED`

N+1 est restaurée comme epoch courante.

Le `TransactionEpochStore` fournit la preuve de l'effet terminal ; le recovery de la commande de renouvellement classe alors l'effet comme `TERMINAL_EFFECT_PROVEN` et finalise la transaction sans nouvelle activation.

### 12.3 Crash après `COMPLETED`

N+1 est restaurée et le résultat durable de la transaction est réutilisable conformément aux règles d'idempotence encore applicables.

### 12.4 Interdiction de reconstruction aveugle

Il est interdit d'inférer :

```text
old epoch=N + RENEW STARTED => epoch=N+1
```

`STARTED` prouve seulement qu'un effet a pu commencer.

## 13. Transition de renouvellement

L'activation N+1 et la finalisation de `(N,65535)` sont deux frontières distinctes.

Entre elles, N+1 est l'unique epoch courante mais l'état minimal nécessaire à la finalisation/restitution du renouvellement de N doit rester durablement récupérable.

Une autorité persistante conserve donc au minimum la corrélation causale :

```text
current_epoch = N+1
predecessor_epoch = N
renewal_transaction = (N,65535)
renewal_transition_state = PENDING_FINALIZATION
```

Ce mécanisme ne maintient pas N comme epoch courante et n'impose pas la conservation de tout son historique.

Un retry exact de `(N,65535)` pendant cette transition est traité comme retry K1 et ne peut jamais provoquer N+2.

L'obligation spéciale de conservation peut disparaître après `COMPLETED` durable.

## 14. Anciennes epochs et rétention

Dans l'epoch courante, tout txid admis reste connu pendant toute la durée de cette epoch.

Après changement d'epoch :

- l'ancien namespace est fermé ;
- aucune requête de l'ancienne epoch ne peut être redispatchée ;
- la conservation illimitée du résultat individuel de toutes les anciennes transactions n'est pas requise ;
- la centrale ne peut plus exiger la restitution individuelle d'un ancien résultat après la frontière de renouvellement, hors règle transitoire spéciale de la transaction de renouvellement elle-même.

États conceptuels :

```text
CURRENT_EPOCH
STALE_EPOCH
UNKNOWN_OR_INVALID_EPOCH
```

La classification `STALE` n'est pas fondée sur une simple comparaison numérique `< current`.

## 15. Résultats conceptuels d'epoch

Les résultats fonctionnels suivants sont retenus, sans valeur numérique encore attribuée :

```text
TRANSACTION_EPOCH_STALE
TRANSACTION_EPOCH_INVALID
TRANSACTION_EPOCH_UNKNOWN
TRANSACTION_EPOCH_RENEWAL_NOT_ALLOWED
```

Aucun de ces rejets ne crée une nouvelle transaction ni un état `RESERVED`.

Aucun résultat `NAMESPACE_EXHAUSTED` n'est introduit à ce stade sans besoin distinct démontré.

## 16. Contrat côté centrale

- une centrale sans connaissance fiable de l'epoch courante doit la lire avant de créer une nouvelle transaction ;
- un retry conserve strictement `(epoch, txid, request identity)` ;
- un retry historique n'est jamais transformé en nouvelle transaction en substituant l'epoch courante ;
- un reboot capteur n'autorise aucune réutilisation spéciale ;
- une centrale qui conserve un état fiable peut continuer à utiliser l'epoch courante après son reboot ;
- une centrale ayant perdu son historique d'allocation ne doit pas deviner un txid libre ; elle doit obtenir une nouvelle frontière d'epoch avant de créer de nouvelles transactions.

## 17. Domaine d'allocation / multi-master

Une epoch possède un unique domaine logique d'allocation des nouveaux txid.

Plusieurs centrals physiques sont compatibles uniquement si leur allocation est coordonnée.

V1.1 n'introduit pas de `client_id` dans l'identité transactionnelle.

La responsabilité est séparée :

```text
CENTRAL / infrastructure de contrôle
→ éviter les doubles allocations

SENSOR
→ détecter toute collision
→ garantir qu'aucune collision ne devient un effet métier
```

Le multi-master indépendant non coordonné pour créer des transactions B5 est hors contrat V1.1-TRANSACTION-01.

## 18. Initialisation et recovery de l'autorité d'epoch

Le recovery distingue au minimum :

```text
VALID
UNINITIALIZED
CORRUPTED
UNAVAILABLE
UNSUPPORTED
```

`UNINITIALIZED` exige une preuve positive de première initialisation. Il ne signifie jamais simplement « aucune epoch lisible ».

La première epoch :

- n'est créée que depuis un état `UNINITIALIZED` démontré ;
- est non nulle ;
- est durable avant toute admission de transaction B5.

En cas `CORRUPTED`, `UNAVAILABLE` ou `UNSUPPORTED`, aucune nouvelle transaction B5 n'est acceptée.

Le `CommandJournal` ne reconstruit jamais l'epoch courante par simple inférence.

Si un renouvellement interrompu ne permet pas de prouver l'activation ou son absence, l'autorité d'epoch est `INDETERMINATE` et aucune nouvelle transaction B5 ne peut être admise.

Un éventuel factory reset transactionnel est un arbitrage séparé.

## 19. Invariants gelés

- **T01-01** — `TransactionIdentity = (transaction_epoch, transaction_id)`.
- **T01-02** — `transaction_epoch=0` est invalide.
- **T01-03** — une epoch courante possède un namespace transactionnel propre.
- **T01-04** — dans l'epoch courante, un txid admis reste connu pendant toute la durée de cette epoch.
- **T01-05** — même `(epoch,txid)` + même requête implique retry et jamais second effet.
- **T01-06** — même `(epoch,txid)` + requête différente implique collision et jamais redispatch.
- **T01-07** — une requête d'une ancienne epoch ne produit jamais de nouvel effet métier.
- **T01-08** — un changement d'epoch est uniquement explicite.
- **T01-09** — reboot, perte de liaison, temps écoulé et wrap txid ne changent jamais implicitement l'epoch.
- **T01-10** — l'epoch courante est une autorité persistante du capteur.
- **T01-11** — une activation d'epoch possède une frontière durable unique.
- **T01-12** — un recovery ne déduit jamais une activation de la seule présence d'un `STARTED`.
- **T01-13** — une autorité d'epoch indéterminée interdit l'admission de nouvelles transactions B5.
- **T01-14** — txid 65535 est réservé au renouvellement d'epoch.
- **T01-15** — txid 1..65534 constitue le namespace métier ordinaire.
- **T01-16** — le renouvellement est lui-même une transaction K1.
- **T01-17** — une epoch possède un seul domaine coordonné d'allocation des nouveaux txid.
- **T01-18** — un retry conserve son identité d'epoch originale.
- **T01-19** — une centrale ayant perdu son état d'allocation ne devine jamais un txid libre.
- **T01-20** — une ancienne epoch peut perdre son historique détaillé mais jamais redevenir implicitement active.
- **T01-21** — l'activation N+1 ne permet pas de perdre l'état nécessaire à la finalisation de `(N,65535)`.
- **T01-22** — une autorité persistante conserve la corrélation causale entre N+1 et la transaction de renouvellement qui l'a activée jusqu'à finalisation durable.
- **T01-23** — après activation N+1 mais avant `COMPLETED`, un retry exact du renouvellement de N reste un retry K1 et ne produit jamais de nouvelle activation.
- **T01-24** — cette conservation transitoire ne maintient pas N comme epoch courante et n'impose pas de conserver tout son historique.
- **T01-25** — après `COMPLETED` durable du renouvellement, l'obligation spéciale de conservation peut disparaître et les règles ordinaires d'ancienne epoch s'appliquent.

## 20. Frontières différées

Restent à traiter dans la passe mapping/compatibilité :

- offsets/adresses B5 ;
- code de commande `RENEW_TRANSACTION_EPOCH` ;
- valeurs numériques des résultats d'epoch ;
- stratégie exacte de compatibilité avec une centrale V1 ;
- évolution des champs `cmd_active_*` / `cmd_last_*` pour porter ou corréler l'epoch ;
- règles de négociation/capability si nécessaires.
