# S7-J2 — Concurrence du lifecycle B5 H3

## Statut

Tranche d'intégration / concurrence Web après S7-J1.

## Objet

S7-J2 sécurise les structures internes de `PhysicalB5LifecycleWorkRunner` utilisées pour :

- le monitoring post-submit ;
- la reconciliation des transactions `Ambiguous` ;
- l'anti-duplication des cycles de reconciliation.

## Constat

Après S7-J1, `BusWorkScheduler` est synchronisé et sait publier un work après initialisation atomique de son contexte. Le lifecycle H3 conservait cependant des `Dictionary` / `HashSet` mutables non synchronisés et publiait ses works avant d'enregistrer leur contexte.

Avec plusieurs sources concurrentes d'observation ou d'exécution, cela pouvait provoquer :

1. une course sur les collections internes H3 ;
2. un work visible par le scheduler avant disponibilité de son contexte ;
3. plusieurs tentatives concurrentes de planification d'une même reconciliation.

## Décisions

- Les contextes internes H3 deviennent des collections concurrentes.
- Le contexte monitoring/reconciliation est enregistré via `BusWorkScheduler.QueuePriority(..., initializeBeforePublish)` avant publication du work.
- L'anti-duplication reste strictement la clé `(device_id, transaction_id)`.
- La réservation de cette clé est atomique ; en cas d'échec de publication, elle est libérée.
- Aucun verrou global Web/runtime n'est ajouté.

## Invariants préservés

- un seul moteur B5 par TR2 ;
- aucun replay automatique ;
- monitoring et reconciliation uniquement en lecture B5 ;
- `Ambiguous` reste bloquant ;
- B0 compatible ne constitue jamais une preuve terminale ;
- B6 reste distinct de B5 ;
- aucune nouvelle règle V1 ou V1.1 ;
- `BusWorkScheduler` reste l'unique arbitre d'un work actif par bus.

## Validation attendue

Un test concurrent vérifie que plusieurs observations simultanées d'une même session compatible avec transaction `Ambiguous` ne publient qu'un seul work `TransactionReconciliation` pour la transaction concernée.
