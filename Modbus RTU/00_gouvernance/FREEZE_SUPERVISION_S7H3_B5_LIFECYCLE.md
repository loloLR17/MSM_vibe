# FREEZE — Supervision S7-H3 — Cycle transactionnel B5

## Statut

**GELÉ après validation locale de S7-H3C2 : 460/460 tests verts.**

Ce gel ferme la tranche S7-H3 consacrée au cycle transactionnel B5 de la supervision PC, incluant le post-submit, l'ambiguïté, la reprise après perte de communication, le recovery après restart et la projection Web.

## Baseline de gel

Baseline logicielle validée localement avant ce gel :

`c4d506de885cfd5d617ad48d75ce1ba9f968281e`

Commit :

`Supervision: align recovered B5 state with web projection`

## Chaîne fonctionnelle gelée

La chaîne de référence est :

`Prepared -> Submitted -> Ambiguous -> TerminalEvidenceObserved`

`TerminalEvidenceObserved` signifie uniquement qu'une preuve terminale B5 reconnue a été observée pour la transaction concernée. Il ne doit jamais être présenté comme un synonyme générique de succès métier.

## Autorités

Les autorités restent uniques :

- un `CommandCoordinator` par `device_id` ;
- `ICommandTransactionJournal` SQLite pour l'historique durable ;
- `BusWorkScheduler` pour la sérialisation des travaux bus ;
- le runtime de supervision physique unique pour les accès Modbus ;
- aucune seconde machine d'état B5 côté Web.

## Invariants transactionnels gelés

1. Une commande B5 n'est jamais rejouée automatiquement après une incertitude de transport.
2. Les deux écritures constituant la soumission initiale restent les seules écritures B5 du chemin normal de commande.
3. Le post-submit et la réconciliation sont exclusivement en lecture B5.
4. Une absence de preuve terminale avant expiration du délai de supervision conduit à `Ambiguous`.
5. Une erreur de transport après tentative de soumission peut conduire à `Ambiguous` sans conclure à un échec TR2.
6. `Ambiguous` bloque toute nouvelle commande B5 pour le même `device_id`.
7. Aucun mécanisme Retry / Ignore / Force n'est introduit.
8. B6 reste strictement séparé de B5.
9. Aucun comportement V1.1 n'est injecté silencieusement dans ce cycle.

## Post-submit gelé

Lorsque la politique runtime B5 est configurée :

- une transaction effectivement `Submitted` reçoit des observations B5 périodiques ;
- l'intervalle de polling et le timeout sont des `SUPERVISION_POLICY`, pas des propriétés normatives du TR2 ;
- une preuve terminale reconnue appelle la résolution transactionnelle existante ;
- l'absence de preuve à l'échéance produit durablement `Ambiguous` ;
- aucun ré-envoi de la commande n'est effectué.

Lorsque la politique B5 est absente, aucun cycle post-submit automatique n'est activé.

## Réconciliation gelée

La réconciliation n'est définie que pour une transaction active `Ambiguous`.

Elle :

- lit B5 uniquement ;
- vérifie la même identité durable `device_id` ;
- utilise les règles existantes de `B5TransactionEvidence` / `B5ReconciliationDecider` ;
- résout uniquement sur preuve terminale correspondant à la transaction ;
- maintient `Ambiguous` si la preuve est non terminale ou insuffisante.

## Perte de communication et reprise

Après perte de session compatible, changement d'identité ou perte de connexion pendant `Ambiguous` :

- le cycle de réconciliation est suspendu ;
- aucun work périodique de réconciliation hors ligne n'est maintenu ;
- la clé locale d'anti-doublon `(device_id, transaction_id)` est libérée ;
- aucune écriture B5 n'est réalisée.

La reprise nécessite une nouvelle observation B0 compatible du **même `device_id`**.

B0 :

- autorise uniquement le réamorçage d'une observation B5 ;
- ne constitue jamais une preuve de résolution transactionnelle ;
- ne doit jamais, seul, faire sortir une transaction de `Ambiguous`.

## Recovery après restart

Le comportement S5 reste gelé :

- un historique non terminal (`Prepared`, `Submitted` ou déjà `Ambiguous`) est restauré comme transaction runtime `Ambiguous` ;
- aucun événement artificiel `Ambiguous` n'est ajouté au journal SQLite au démarrage ;
- aucune soumission B5 n'est rejouée ;
- aucune réconciliation n'est lancée avant réidentification physique compatible du même équipement.

Après B0 compatible du même `device_id`, une seule chaîne de réconciliation peut être amorcée pour la transaction récupérée.

## Projection Web gelée

L'IHM expose exactement les états :

- `Prepared` ;
- `Submitted` ;
- `Ambiguous` ;
- `TerminalEvidenceObserved`.

Pour une transaction récupérée après restart, l'état actif `Ambiguous` du `CommandCoordinator` prévaut dans la projection courante sur un dernier événement durable historique `Prepared` ou `Submitted`, sans modifier ce journal historique.

Après observation d'une preuve terminale, l'événement durable `TerminalEvidenceObserved` redevient naturellement l'état projeté.

L'IHM :

- bloque les commandes pour `Prepared`, `Submitted` et `Ambiguous` ;
- bloque également par prudence si la lecture de l'état transactionnel échoue ;
- se débloque après `TerminalEvidenceObserved` ;
- rappelle qu'une preuve terminale ne signifie pas nécessairement succès métier.

## Couverture de validation

La couverture obtenue à la baseline comprend notamment :

- écriture initiale B5 puis post-submit read-only ;
- timeout vers `Ambiguous` ;
- réconciliation terminale sans replay ;
- preuve insuffisante maintenant l'ambiguïté ;
- anti-doublon de réconciliation ;
- absence de réconciliation pour un autre `device_id` ;
- suspension hors ligne puis reprise sur B0 compatible ;
- restart SQLite avec restauration `Ambiguous` ;
- projection Web cohérente de l'état récupéré ;
- résolution terminale après restart ;
- conservation de l'historique durable ;
- blocage fail-safe côté Web.

Validation locale de fermeture :

`460/460 tests verts`

## Hors périmètre de ce gel

Restent hors S7-H3 :

- authentification / rôles / ACL Web ;
- HTTPS et politique certificats ;
- modification de la spécification Modbus V1 ;
- V1.1 ;
- RESET_STATISTICS tant que son scope reste `EMPTY` ;
- sélection B6, qui reste une transaction distincte de B5 ;
- port STM32 et validation matérielle ;
- finition générale de l'UX S6-H, traitée en S7-I ;
- audit transversal final complet de S7, traité ultérieurement en S7-K.

## Décision

S7-H3 est considérée **fonctionnellement fermée et gelée** sur la baseline ci-dessus, sous réserve de la validation locale du présent commit documentaire.

La tranche suivante est **S7-I — câblage final de l'UX S6-H sur les données et actions réelles**.
