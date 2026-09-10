# Gel S7-J — Intégration multi-client et concurrence Web

## 1. Objet

Ce document fige la phase **S7-J — Intégration multi-client et concurrence Web** de la supervision PC TR2.

Le périmètre S7-J complète S7-I en rendant explicites et testables les propriétés de concurrence entre plusieurs clients HTTP, le runtime de supervision, le scheduler de bus et l’unique moteur transactionnel B5.

## 2. Baseline gelée

État de référence validé avant ce gel :

- commit `7528d215ff4e867e4633a4b400f4893a3a60b10e`
- message `Supervision: add S7-J3 per-device web submission gates`
- validation locale : build et tests verts, **466/466**.

Les gels et arbitrages S0 à S7-I restent applicables.

## 3. Invariants gelés

### 3.1 Unicité du runtime et du transport

- Le serveur Web partage **le même `PhysicalSupervisionRuntime`** que le service de supervision.
- Aucun second moteur Modbus, aucun second scheduler et aucun second `CommandCoordinator` logique ne sont créés pour la Web IHM.
- Le navigateur ne possède jamais le bus RS-485 ni l’autorité B5.

### 3.2 Sérialisation physique du bus

- `BusWorkScheduler` reste l’unique arbitre de l’exécution physique des travaux par `SerialBus`.
- Ses collections, compteurs, publications, `BeginNext` et `Complete` sont protégés contre les accès concurrents.
- L’ordre existant priorité / échéance / séquence est conservé.
- Un seul travail est actif par bus à un instant donné.

### 3.3 Autorité transactionnelle B5 par device_id

- Un seul `CommandCoordinator` existe par `device_id`.
- La création/récupération du coordinator est atomique via `CommandCoordinatorRegistry`.
- Deux préparations concurrentes B5 pour un même TR2 ne peuvent pas produire deux transactions non terminales.
- Une commande déjà non terminale conserve son rôle de verrou métier : une nouvelle commande B5 concurrente est refusée, sans replay ni création d’une transaction parallèle.

### 3.4 Publication atomique du contexte de travail

- Le contexte associé à un travail prioritaire B5/B6 est enregistré avant publication du travail dans le scheduler.
- Le runtime ne peut donc pas consommer un work item visible sans son contexte associé.
- Les tables de contexte de `SupervisionOperationalFacade` sont protégées contre les lectures/écritures concurrentes.

### 3.5 Multi-client Web

- Les soumissions Web sont sérialisées **par `device_id`**, et non globalement.
- B5 et B6 visant le même TR2 passent par le même verrou de soumission Web afin de conserver un ordre déterministe au niveau de ce device.
- Deux clients visant deux TR2 différents ne sont pas artificiellement bloqués l’un par l’autre.
- La sérialisation physique finale reste néanmoins celle du `BusWorkScheduler` lorsqu’ils partagent le même bus.

### 3.6 requestIdentity

- `requestIdentity` reste unique par `device_id` dans l’historique B5.
- Deux requêtes HTTP concurrentes portant le même `requestIdentity` sur le même TR2 ne peuvent pas être acceptées toutes les deux.
- Une répétition est reportée comme conflit, sans replay automatique.

### 3.7 Recovery et Ambiguous

- Les règles gelées S7-H restent inchangées.
- Aucun mécanisme de concurrence S7-J ne résout implicitement un état `Ambiguous`.
- Reconnexion, multi-client, répétition HTTP ou relance de page ne constituent jamais une preuve terminale B5.
- Seule l’observation de la preuve B5 terminale autorisée résout la transaction.

### 3.8 B6 distinct de B5

- La sélection de campagne B6 reste un travail non transactionnel distinct de B5.
- Aucun `transaction_id`, journal B5 ou mécanisme de replay B5 n’est introduit pour B6.
- La concurrence Web n’altère pas cette séparation.

## 4. Éléments techniques consolidés par S7-J

S7-J a notamment consolidé :

- `CommandCoordinatorRegistry` : accès concurrent protégé et acquisition atomique par `device_id` ;
- `BusWorkScheduler` : protection des files, travaux actifs et identifiants de work ;
- `SupervisionOperationalFacade` : sérialisation de préparation B5 par device et publication atomique des contextes de travaux ;
- `FleetRegistry` : snapshots de sessions protégés ;
- `DeviceTelemetrySnapshotRegistry` : lectures et mises à jour protégées ;
- `RuntimeCommandSink` : verrou de soumission Web par `device_id` ;
- lifecycle B5 : absence de duplication de réconciliation concurrente d’une même transaction ;
- tests multi-client : un seul B5 non terminal par TR2, requestIdentity concurrent déterministe, indépendance entre devices.

## 5. Ce que S7-J ne change pas

S7-J ne modifie pas :

- le mapping Modbus V1 ;
- les règles métier des commandes B5 ;
- les registres B1 à B7 ;
- la politique `Ambiguous` ;
- la sémantique de B6 ;
- la politique de sécurité réseau S7-A ;
- le contenu fonctionnel de l’IHM S6/S7-I ;
- le périmètre `RESET_STATISTICS`, toujours absent.

Aucun comportement V1.1 n’est introduit silencieusement.

## 6. Limites explicites

Ce gel ne prétend pas démontrer :

- le comportement réel sous charge réseau importante ;
- les performances de plusieurs dizaines/centaines de clients ;
- le comportement matériel RS-485 réel ;
- la robustesse électromagnétique ou temporelle de la cible STM32 ;
- l’adéquation d’une exposition LAN sans authentification à un environnement de production définitif.

Ces points restent hors preuve tant que les essais correspondants ne sont pas réalisés.

## 7. Statut

**S7-J est gelée** sous réserve de la validation locale du commit documentaire de gel.

La prochaine phase pertinente est **S7-K — audit transversal final et gel de S7**, sans ajout fonctionnel par défaut : elle doit relire l’ensemble S7-A à S7-J, vérifier les invariants croisés, identifier les éventuels reliquats et produire le gel final de l’architecture Web de production.
