# S7-H3C — Passe transversale du cycle B5

## Statut

Passe de cohérence après validation locale de S7-H3B4 (`458/458`).

## Chaîne auditée

`Prepared -> Submitted -> Ambiguous -> TerminalEvidenceObserved`

Le cycle reste porté par un unique `CommandCoordinator` par `device_id`, le journal SQLite durable et le `BusWorkScheduler` existant.

## Constat C1 — réconciliation et perte de session

La première implémentation H3B3/H3B4 conservait un work `TransactionReconciliation` périodique lorsque la session n'était plus compatible ou lorsque le bus avait subi une erreur I/O.

Ce comportement n'écrivait jamais B5, mais il ne respectait pas strictement l'arbitrage H3A : après perte de connexion/identité, la réconciliation doit rester suspendue jusqu'à une nouvelle identification B0 compatible du même `device_id`.

## Correction C1

Pour une transaction `Ambiguous` :

- session non compatible, identité absente/différente ou connexion absente : le work courant est terminé, la clé d'anti-doublon est libérée et aucune réconciliation périodique n'est replanifiée ;
- erreur I/O pendant une lecture de réconciliation : le bus est marqué déconnecté, la clé d'anti-doublon est libérée et aucune réconciliation n'est replanifiée ;
- timeout de lecture avec session encore compatible : une nouvelle observation B5 read-only peut rester planifiée ;
- après reconnexion, seul un nouveau B0 compatible du même `device_id`, via `ObserveCompatibleSession`, peut réamorcer le cycle ;
- B0 ne constitue jamais une preuve terminale ;
- aucune écriture B5, aucun replay de commande, aucun Retry/Ignore/Force n'est ajouté.

## Anti-doublon

La clé `(device_id, transaction_id)` reste l'autorité locale d'anti-doublon pour les observations de réconciliation actives. Elle est libérée lorsqu'une observation doit être suspendue faute de session compatible, afin qu'un B0 compatible ultérieur puisse réamorcer exactement un cycle.

## Validation attendue

Les tests transversaux doivent démontrer notamment :

1. perte de session pendant `Ambiguous` => aucune boucle de réconciliation hors ligne ;
2. B0/session compatible ultérieure du même `device_id` => exactement une nouvelle réconciliation ;
3. zéro écriture B5 sur ce chemin ;
4. les invariants H1/H2 restent inchangés : `Ambiguous` est visible et bloquant tant qu'aucune preuve B5 terminale n'est observée.

## Hors périmètre

- modification de la spécification Modbus V1 ;
- replay automatique d'une commande ;
- nouvelles commandes B5 ;
- modification B6 ;
- authentification Web ;
- décision V1.1.
