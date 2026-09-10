# S7-H3C2 — Restart B5 et projection Web

## Statut

Correction transversale issue de l'audit S7-H3 après validation locale de C1.

## Écart détecté

Au restart, `CommandCoordinatorRecoveryService` restaure volontairement toute transaction B5 non terminale (`Prepared`, `Submitted` ou déjà `Ambiguous`) sous forme d'une transaction runtime `Ambiguous`.

Conformément à S5, ce recovery ne crée aucun événement synthétique dans le journal SQLite : l'historique durable reste exactement celui observé avant l'arrêt.

La projection H1/H2 `RuntimeCommandSink.ReadAsync`, qui lisait uniquement le dernier événement durable, pouvait donc exposer `Prepared` ou `Submitted` alors que le `CommandCoordinator` avait déjà restauré la même transaction en `Ambiguous` et bloquait toute nouvelle commande.

Cette divergence est incorrecte pour l'IHM : l'état affiché doit représenter l'autorité runtime courante sans falsifier l'historique persistant.

## Décision C2

La projection Web conserve le journal SQLite comme source de l'historique et applique uniquement la règle de projection suivante :

- si le `CommandCoordinator` du `device_id` porte actuellement une transaction `Ambiguous` dont `(transaction_id, request_identity)` correspond à la transaction historique projetée, l'état IHM est `Ambiguous` ;
- aucun événement SQLite n'est ajouté au démarrage pour matérialiser cette projection ;
- `ObservedAt` reste le timestamp du dernier événement durable réellement observé ;
- dès qu'une preuve terminale B5 est observée, `ResolveTerminalAsync` ajoute normalement `TerminalEvidenceObserved` au journal et la projection redevient entièrement dérivée du dernier événement durable ;
- aucune nouvelle valeur d'état IHM n'est introduite.

## Invariants préservés

1. Les quatre états H1/H2 restent exactement : `Prepared`, `Submitted`, `Ambiguous`, `TerminalEvidenceObserved`.
2. Le recovery S5 reste inchangé et n'écrit aucun événement de journal synthétique.
3. Une transaction récupérée `Ambiguous` reste bloquante dans l'IHM avant toute réconciliation.
4. B0 compatible ne constitue jamais une preuve terminale ; il ne fait qu'autoriser l'amorçage de la réconciliation.
5. La réconciliation lit B5 uniquement.
6. Aucun replay de la commande B5 n'est effectué après restart.
7. La preuve terminale reconnue par `B5TransactionEvidence` reste l'unique voie de fermeture.
8. Le journal SQLite reste l'autorité durable ; le `CommandCoordinator` reste l'autorité runtime de la transaction active.

## Validation end-to-end attendue

Un test transversal doit démontrer la chaîne réelle suivante :

`Prepared -> Submitted -> restart -> recovery Ambiguous -> projection Web Ambiguous -> polling B0 compatible même device_id -> TransactionReconciliation -> preuve terminale B5 -> TerminalEvidenceObserved -> projection Web terminale`

Le test doit également démontrer :

- exactement deux écritures B5 lors de la soumission initiale ;
- zéro écriture B5 dans le runtime redémarré ;
- journal inchangé lors du recovery ;
- ajout d'un seul `TerminalEvidenceObserved` lors de la résolution ;
- aucune nouvelle commande ni replay pendant la récupération/réconciliation.

## Hors périmètre

- modification de la spécification Modbus V1 ;
- modification du recovery S5 ;
- ajout d'un état Web `Recovered` ou équivalent ;
- Retry/Ignore/Force ;
- B6 ;
- authentification ;
- V1.1.
