# Projet MSM — Capteur de vibration TR2

## Arbitrage S7-H3A — Fermeture du cycle B5 physique : politique de monitoring et réconciliation

Date : 2026-09-10

Cette tranche prépare la fermeture du cycle transactionnel B5 mise en évidence par S7-H1/H2. Elle ne modifie ni la spécification Modbus RTU V1 ni les gels firmware.

## 1. Constat sur l'état courant

Le socle applicatif contient déjà :

- `B5CommandExecutionService` ;
- `B5PostSubmitMonitor` ;
- `B5ReconciliationService` ;
- `CommandBusOrchestrator` avec les travaux `CommandPostSubmitMonitoring` et `TransactionReconciliation`.

Le runtime physique courant n'utilise cependant que l'émission B5 initiale dans `PhysicalPriorityWorkRunner`.

Conséquences actuelles :

- après un submit Modbus réussi, le coordinator reste `Submitted` faute d'observation terminale raccordée au runtime physique ;
- après résultat ambigu ou recovery au redémarrage, le coordinator reste `Ambiguous` faute d'exécution physique de la réconciliation ;
- S7-H2 reflète correctement ces états et bloque de nouvelles commandes B5, mais le runtime doit maintenant disposer du chemin permettant de les quitter sur preuve B5.

Ce constat est un gap d'intégration de supervision, pas une lacune du protocole V1 ni du firmware.

## 2. Invariants non négociables

Le raccordement H3 doit préserver :

1. un seul moteur transactionnel B5 et un seul `CommandCoordinator` par `device_id` ;
2. aucun replay automatique d'un submit B5 ;
3. aucune résolution par simple reconnexion ou par seule lecture B0 ;
4. `Ambiguous` signifie résultat incertain, jamais échec ;
5. seule une preuve B5 reconnue par `B5TransactionEvidence` peut terminer une transaction ;
6. un état non terminal récupéré après redémarrage est `Ambiguous` ;
7. la persistance SQLite existante reste l'autorité durable ;
8. le `BusWorkScheduler` reste l'autorité de sérialisation par bus ;
9. aucune action Web `Retry`, `Ignore`, `Force` ou équivalent n'est créée ;
10. B6 reste hors machine transactionnelle B5.

## 3. Décision de politique temporelle

S3-F avait explicitement laissé hors périmètre la cadence de post-submit monitoring et le timeout B5. S4 n'a pas défini ces valeurs non plus.

H3 classe donc ces paramètres comme **SUPERVISION_POLICY runtime**, distincte de toute valeur normative V1.

Ils doivent être fournis explicitement par la configuration locale sous un bloc dédié `b5` :

```json
"b5": {
  "postSubmitPollIntervalMilliseconds": 500,
  "postSubmitTimeoutMilliseconds": 30000,
  "reconciliationIntervalMilliseconds": 2000
}
```

Les valeurs ci-dessus sont les valeurs d'exemple retenues pour le fichier d'exemple uniquement. Elles ne sont ni normatives ni des propriétés du TR2.

Règle de mise en œuvre H3-B :

- aucune valeur métier B5 ne doit être déduite du `responseTimeoutMilliseconds` série ;
- aucune durée d'exécution de commande TR2 ne doit être inventée ;
- la configuration doit être validée fail-fast ;
- `postSubmitPollIntervalMilliseconds > 0` ;
- `postSubmitTimeoutMilliseconds >= postSubmitPollIntervalMilliseconds` ;
- `reconciliationIntervalMilliseconds > 0`.

Pour préserver la compatibilité avec les configurations existantes créées avant H3, l'absence du bloc `b5` conserve le runtime actuel sans monitoring/réconciliation automatique. Dès qu'un bloc `b5` est présent, le cycle H3 est activé. Cette absence doit être diagnostiquable et ne doit jamais être interprétée comme une valeur temporelle implicite.

## 4. Cycle post-submit décidé

Lorsque H3 est activé :

1. le travail `CommandTransaction` réalise exactement les deux écritures B5 existantes ;
2. si le submit n'est pas ambigu et que le coordinator est `Submitted`, le runtime programme un `CommandPostSubmitMonitoring` ;
3. chaque monitoring effectue uniquement une lecture B5 ;
4. si `B5TransactionEvidence` fournit une preuve terminale, le coordinator est résolu par le mécanisme existant ;
5. sinon, avant timeout, un nouveau monitoring est replanifié ;
6. au timeout sans preuve terminale, la transaction devient durablement `Ambiguous` puis une réconciliation est planifiée ;
7. aucune de ces étapes ne réémet la commande.

Le timeout est un timeout de décision de la supervision PC : il ne définit pas un timeout normatif d'exécution du TR2.

## 5. Cycle de réconciliation décidé

Une réconciliation :

- exige une transaction active `Ambiguous` ;
- exige une session de nouveau `Compatible` et identifiée pour le même `device_id` ;
- effectue uniquement une lecture B5 ;
- délègue l'interprétation à `B5ReconciliationService` / `B5ReconciliationDecider` existants.

Résultats :

- preuve terminale : résolution terminale durable ;
- transaction encore active côté B5 : reste `Ambiguous` ;
- preuve insuffisante : reste `Ambiguous`.

Tant que la transaction reste ambiguë, une nouvelle observation de réconciliation peut être replanifiée selon `reconciliationIntervalMilliseconds`, mais **aucune écriture B5 n'est autorisée par ce cycle**.

Une rupture I/O pendant observation conserve l'ambiguïté et suit la politique transport S4 : connexion fermée, session invalidée, puis reprise des observations seulement après reconnexion et nouveau B0 compatible.

## 6. Recovery au démarrage

Le recovery S5 reste inchangé : le journal durable est relu avant ouverture des bus et tout état non terminal est reconstruit `Ambiguous` sans écriture Modbus.

H3-B devra seulement raccorder la reprise d'observation : après identification B0 compatible d'un `device_id` possédant une transaction `Ambiguous`, une réconciliation pourra être mise en file.

Le B0 lui-même ne constitue jamais la preuve qui résout la transaction.

## 7. Observabilité et IHM

S7-H1/H2 reste l'interface de lecture opérateur :

- `Prepared` ;
- `Submitted` ;
- `Ambiguous` ;
- `TerminalEvidenceObserved`.

Aucun état supplémentaire n'est inventé par H3-A.

`TerminalEvidenceObserved` signifie seulement qu'une preuve terminale B5 a été observée. Cela ne doit pas être transformé en libellé générique « commande réussie » sans lecture/interprétation normative supplémentaire.

## 8. Périmètre H3-B

La tranche d'implémentation suivante devra au minimum :

- étendre la configuration runtime avec le bloc `b5` optionnel et validé ;
- composer `B5Reader`, `B5PostSubmitMonitor` et `B5ReconciliationService` sur le transport physique existant ;
- faire exécuter au runner physique `CommandPostSubmitMonitoring` et `TransactionReconciliation` ;
- programmer le monitoring après submit réussi ;
- programmer/reprogrammer la réconciliation d'un `Ambiguous` sans replay ;
- amorcer la réconciliation après B0 compatible pour les transactions ambiguës récupérées ou reconnectées ;
- journaliser les erreurs de lecture selon la taxonomie de communication existante sans créer de fausse conclusion métier ;
- tester Submitted -> preuve terminale ;
- tester Submitted -> timeout -> Ambiguous ;
- tester reconnexion/B0 -> transaction encore Ambiguous avant preuve B5 ;
- tester Ambiguous -> preuve terminale ;
- tester preuve insuffisante -> Ambiguous et aucune écriture B5 ;
- tester recovery restart -> réconciliation sans submit rejoué.

## 9. Hors périmètre

H3-A/H3-B n'introduisent pas :

- nouveau registre ou nouvelle commande Modbus ;
- `transaction_epoch` V1.1 ;
- retry d'écriture B5 ;
- résolution opérateur forcée ;
- acquittement de l'ambiguïté sans preuve ;
- changement de sémantique B6 ;
- valeur temporelle normative du TR2 ;
- conclusion de qualification matérielle RS-485.

## 10. Classification

- preuve terminale B5 et machine transactionnelle existante : architecture/protocole déjà gelés ;
- persistance/recovery : architecture supervision S1/S2/S5 déjà gelée ;
- absence de replay : politique de sécurité S4 déjà gelée ;
- paramètres temporels du cycle d'observation : **SUPERVISION_POLICY S7-H3** ;
- activation explicite via configuration `b5` : **SUPERVISION_POLICY S7-H3** ;
- aucun changement `FW_POLICY` ;
- aucune règle V1.1 importée.
