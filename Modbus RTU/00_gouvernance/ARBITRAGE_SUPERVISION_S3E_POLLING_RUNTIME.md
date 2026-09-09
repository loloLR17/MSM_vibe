# Projet MSM — Capteur de vibration TR2

## Arbitrage S3-E — Boucle d’orchestration polling host

Date : 2026-09-09

Cette tranche complète S3-D. Elle ne modifie ni la spécification Modbus RTU V1 ni les gels firmware.

## 1. Objectif

S3-E introduit la première boucle métier long-running du host :

1. elle ne démarre qu’après ouverture de la readiness S3-C/S3-D ;
2. elle utilise le `BusWorkScheduler` S1 partagé ;
3. elle démarre chaque endpoint par le groupe `STATIC` B0 ;
4. les groupes `FAST`, `MEDIUM` et `SLOW` ne deviennent éligibles qu’après identification compatible ;
5. une perte de compatibilité ramène l’endpoint vers le polling `STATIC` de reconnexion ;
6. le `CancellationToken` S3-D arrête la boucle ;
7. aucune dépendance série/RS-485 physique n’est ajoutée.

## 2. Classification

- composition des groupes de registres : **S0/S1 gelé** via `TR2PollingPlan` ;
- sérialisation et priorité du travail par bus : **S1 gelé** via `BusWorkScheduler` ;
- cadences numériques : **SUPERVISION_POLICY S3-E** ;
- ordre `STATIC` avant polling opérationnel : application du modèle de session S0/S1 ;
- scan host et politique de reconnexion : **SUPERVISION_POLICY S3-E** ;
- aucune décision V1.1 n’est importée.

## 3. Politique de cadence initiale

Les valeurs par défaut S3-E sont volontairement configurables :

- retry `STATIC` : 5 s ;
- `FAST` : 1 s ;
- `MEDIUM` : 5 s ;
- `SLOW` : 30 s ;
- scan du scheduler host : 100 ms.

Ces nombres ne sont pas normatifs Modbus. Ils constituent une politique de supervision initiale et peuvent être remplacés dans le JSON runtime.

Toutes les cadences configurées doivent être strictement positives.

## 4. État par endpoint

La boucle conserve uniquement un état de scheduling volatile par endpoint : prochaine échéance `STATIC`, `FAST`, `MEDIUM`, `SLOW` et indicateur de travail déjà mis en file.

Cet état n’est pas persistant et n’est pas une nouvelle source d’identité.

Un endpoint non compatible (`Unidentified`, `Incompatible`, `Disconnected`) n’émet que du `STATIC`.

Après succès `STATIC` compatible, les trois groupes opérationnels deviennent immédiatement éligibles puis suivent leurs cadences respectives.

## 5. Autorité d’exécution

`SupervisionPollingLoop` choisit et ordonnance les `ScheduledBusWork` via le `BusWorkScheduler` partagé.

L’exécution concrète du polling est injectée par `IPollingWorkRunner`.

S3-E ne fournit volontairement aucun faux transport de production. Le runner host utilisé avec le futur transport devra s’appuyer sur les autorités Application/Protocol existantes et garantir la libération du work acquis sur le scheduler, comme le fait déjà `PollingBusOrchestrator`.

Les tests S3-E utilisent un runner déterministe uniquement pour qualifier l’orchestration host.

## 6. Intégration lifecycle

`SupervisionRuntimeHost` accepte désormais un `ISupervisionRuntimeLoop`.

Sans boucle fournie, le comportement S3-D reste inchangé via une boucle idle annulable.

Avec `SupervisionPollingLoop` :

- startup/recovery ;
- `Running` ;
- boucle polling ;
- cancellation ;
- fermeture readiness ;
- `Stopped`.

Une exception inattendue de la boucle conduit toujours à `Faulted` selon S3-D.

## 7. Frontière avec S3-F

S3-E ne traite que le travail `Polling`.

Les travaux prioritaires `ExplicitRefresh`, `CommandTransaction`, `CommandPostSubmitMonitoring` et `TransactionReconciliation` restent réservés à S3-F. La future tranche devra fournir un dispatch unifié sans dégrader l’ordre de priorité déjà porté par `BusWorkScheduler`.

## 8. Hors périmètre

S3-E n’introduit pas :

- port série/RS-485 réel ;
- choix COM/baud/parité ;
- retry Modbus naïf ;
- façade opérateur B5 ;
- refresh explicite ;
- Windows Service ;
- Web/API ;
- V1.1.

## 9. Validation attendue

Les tests doivent prouver au minimum :

- chargement et validation des cadences ;
- refus d’exécuter la boucle avant readiness ;
- `STATIC` exécuté avant les groupes opérationnels ;
- activation de `FAST` après identification compatible ;
- raccordement au lifecycle S3-D et arrêt propre par cancellation ;
- maintien de la readiness pendant `Running` puis fermeture à l’arrêt.
