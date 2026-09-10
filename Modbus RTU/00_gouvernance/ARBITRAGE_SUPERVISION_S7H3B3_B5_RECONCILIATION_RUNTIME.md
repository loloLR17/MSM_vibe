# Projet MSM — Capteur de vibration TR2

## Arbitrage S7-H3B3 — Réconciliation physique B5 d’une transaction Ambiguous

Date : 2026-09-10

Cette tranche prolonge S7-H3B2 en raccordant uniquement l’exécution physique des travaux `TransactionReconciliation` déjà prévus par l’architecture applicative.

## 1. Périmètre

Lorsqu’un monitoring post-submit atteint le timeout configuré sans preuve terminale, le `CommandCoordinator` passe durablement à `Ambiguous` puis un travail prioritaire `TransactionReconciliation` est planifié à `observedAt + ReconciliationInterval`.

La réconciliation utilise exclusivement :

- le `BusWorkScheduler` existant ;
- le `CommandCoordinator` existant du `device_id` ;
- `B5Reader` ;
- `B5ReconciliationService` ;
- `B5ReconciliationDecider` ;
- `RuntimeB5LifecyclePolicy.ReconciliationInterval`.

Aucun second moteur transactionnel ni cache d’autorité n’est créé.

## 2. Invariants

Une réconciliation :

1. exige une transaction active `Ambiguous` correspondant au `transaction_id` mémorisé dans le contexte du travail ;
2. exige une session `Compatible`, identifiée, portant le même `device_id` ;
3. effectue uniquement une lecture B5 ;
4. ne réalise aucune écriture B5 ;
5. ne rejoue jamais le submit initial ;
6. ne transforme jamais une absence de preuve en succès ou en échec métier.

## 3. Résultats de lecture

Le résultat est délégué au `B5ReconciliationService` existant :

- `TerminalEvidence` : la transaction est résolue durablement par `ResolveTerminalAsync` ;
- `StillNonTerminal` : la transaction reste `Ambiguous` et une nouvelle observation est planifiée ;
- `InsufficientEvidence` : la transaction reste `Ambiguous` et une nouvelle observation est planifiée.

La nouvelle observation est due après `ReconciliationInterval`.

## 4. Transport et identité

Si la connexion n’est plus disponible, si la session n’est plus compatible ou si l’endpoint ne correspond plus au même `device_id`, aucune lecture B5 n’est effectuée et l’ambiguïté n’est pas résolue.

Pour une défaillance `Io`, le mécanisme de déconnexion/invalidation S4 existant reste appliqué. Une défaillance de lecture ne provoque jamais de réémission B5.

L’amorçage explicite après nouveau B0 compatible ou après recovery startup reste hors de cette tranche et sera traité dans H3B4.

## 5. Interaction avec H3B2

Le chemin post-submit devient :

`CommandTransaction -> Submitted -> CommandPostSubmitMonitoring -> TimedOutAmbiguous -> TransactionReconciliation`.

Une preuve terminale peut donc maintenant fermer une transaction devenue ambiguë après timeout, sans écriture supplémentaire.

## 6. Validation attendue

Les tests H3B3 vérifient au minimum :

- `Submitted -> timeout -> Ambiguous -> réconciliation -> preuve terminale` ;
- disparition de la transaction active uniquement sur preuve terminale ;
- preuve insuffisante : maintien de `Ambiguous` ;
- replanification d’une nouvelle `TransactionReconciliation` ;
- nombre d’écritures B5 inchangé à deux écritures initiales pendant tout le cycle de réconciliation.

## 7. Hors périmètre

Restent pour H3B4 :

- amorçage de la réconciliation après B0 compatible d’un équipement reconnecté ;
- reprise automatique d’une transaction `Ambiguous` reconstruite au startup ;
- prévention explicite des amorçages doublons lors de ces événements ;
- observabilité finale de l’activation/désactivation H3 si nécessaire.

Aucun `Retry`, `Ignore`, `Force`, replay de submit, V1.1, changement B6 ou qualification matérielle n’est introduit.
