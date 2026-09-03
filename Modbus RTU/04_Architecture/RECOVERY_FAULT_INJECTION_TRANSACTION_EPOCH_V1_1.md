# Recovery fault injection — Transaction epoch V1.1

## 1. Statut

Compagnon gelé de `ARCHITECTURE_TRANSACTION_EPOCH_V1_1.md`. Il ne modifie pas la baseline V1.

Les injections utilisent les abstractions réelles de persistance/recovery ; aucun registre de test privé n'est requis.

## 2. Frontières génériques

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

## 3. Scénarios et projections

### J-EPOCH-01 — Crash avant RESERVED
N reste courante ; aucune transaction renewal ; aucune N+1.

### J-EPOCH-02 — Après RESERVED avant STARTED
N reste courante ; renewal recoverable sans effet ; active peut publier `(N,65535)`.

### J-EPOCH-03 — Après STARTED avant préparation N+1
N reste courante ; `STARTED` seul ne prouve aucune activation.

### J-EPOCH-04 — Après préparation N+1 avant activation
N reste seule courante ; N+1 préparée sans autorité.

### J-EPOCH-05 — Immédiatement après activation N+1
`epoch_status=VALID`, `current_epoch=N+1`; la transaction `(N,65535)` reste récupérable comme active tant qu'elle n'est pas terminale ; jamais N+2.

### J-EPOCH-06 — Après activation avant COMPLETED
Projection autorisée : `current_epoch=N+1`, `active_epoch=N`, `active_txid=65535`. Recovery finalise la même transaction sans nouvelle activation.

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
- aucune ancienne epoch ne redevient active par reboot ;
- aucune collision n'est convertie en effet métier ;
- une tentative rejetée avant RESERVED ne remplace pas `last` ;
- l'absence de preuve reste absence de preuve ;
- le firmware bloque plutôt qu'inventer une continuité.
