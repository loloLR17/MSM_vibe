# Projet MSM — Capteur de vibration TR2

## Arbitrage S3-F — Façade opérationnelle B5 / refresh et dispatch prioritaire unifié

Date : 2026-09-09

Cette tranche complète S3-E. Elle ne modifie ni la spécification Modbus RTU V1 ni les gels firmware.

## 1. Objectif

S3-F introduit la première façade opérationnelle du host de supervision pour :

1. préparer et mettre en file une commande B5 ;
2. préparer les refresh explicites post-reconnexion B1 à B7 ;
3. conserver le contexte nécessaire à leur future exécution matérielle ;
4. permettre à la boucle host S3-E de dispatcher dans une même file les travaux de polling et les travaux prioritaires.

## 2. Classification

- transaction_id 1..65535, non-réutilisation et persistance avant submit : **architecture supervision déjà gelée / S1-S2** ;
- `CommandBusOrchestrator` et `FleetRefreshPlanner` : **socle applicatif S1 déjà validé** ;
- façade `SupervisionOperationalFacade` : **SUPERVISION_POLICY S3-F** ;
- contexte `B5CommandIntent` côté host : **SUPERVISION_POLICY S3-F**, sans modifier le mapping V1 ;
- priorité des travaux non-polling sur le polling : comportement déjà porté par `BusWorkScheduler`, désormais consommé par le host ;
- `IPriorityWorkRunner` : frontière host vers la future exécution réelle ;
- aucun comportement V1.1 n'est importé.

## 3. Barrière de readiness

Aucune commande B5 ni refresh explicite ne peut être mis en file avant l'ouverture de la readiness S3-C/D.

La façade appelle `RuntimeReadinessGate.EnsureReady()` avant toute opération.

## 4. Session et identité

La façade exige une session :

- `Compatible` ;
- identifiée par un `device_id`.

Le `CommandCoordinator` reste indexé par `device_id`, jamais par COM, bus ou adresse Modbus.

Si aucun coordinator n'existe encore pour un device compatible découvert après startup, la façade crée l'autorité avec les stores SQLite S2 déjà composés. Si un coordinator récupéré existe, il est réutilisé.

## 5. Commande B5

`QueueCommandAsync` :

1. vérifie readiness et session compatible ;
2. récupère ou crée le `CommandCoordinator` du `device_id` ;
3. appelle `CommandBusOrchestrator.PrepareAndQueueAsync` ;
4. la réservation durable du transaction_id est donc effectuée avant la mise en file ;
5. lie ensuite le transaction_id généré au `B5CommandIntent` pour produire le `B5CommandRequest` V1 exact ;
6. mémorise ce contexte par `work_id` pour le futur runner matériel.

S3-F ne réalise pas encore l'écriture Modbus B5 physique.

## 6. Refresh explicite

Le refresh post-reconnexion réutilise `FleetRefreshPlanner.QueuePostReconnectRefresh`.

La liste reste celle du socle S1 : B1 à B7, B0 ayant déjà été relu lors de la découverte/reconnexion.

Chaque `ScheduledBlockRefresh` est conservé par `work_id` afin que le futur runner puisse retrouver le bloc exact à exécuter.

## 7. Dispatch unifié

La boucle S3-E est étendue sans changer le scheduler :

- `BusWorkKind.Polling` -> `IPollingWorkRunner` ;
- tout travail prioritaire -> `IPriorityWorkRunner`.

Le choix de priorité reste entièrement celui de `BusWorkScheduler` : les travaux non-polling échus passent avant le polling échu sur un même bus.

Le runner qui exécute un travail reste responsable de la complétion du work auprès du scheduler, conformément aux orchestrateurs applicatifs existants.

## 8. Hors périmètre

S3-F n'introduit pas :

- port série / RS-485 ;
- driver ou COM ;
- exécution physique B5 ;
- cadence de post-submit monitoring ;
- timeout B5 arbitraire ;
- politique de retry ;
- reconciliation automatique des transactions récupérées ;
- Web/API/UI ;
- Windows Service ;
- V1.1.

Ces points restent pour les tranches suivantes ou la future intégration matérielle.

## 9. Validation attendue

Les tests doivent prouver au minimum :

- façade bloquée avant readiness ;
- commande B5 préparée avec transaction_id non nul et coordinator `Prepared` ;
- même contexte B5 récupérable par `work_id` ;
- refresh post-reconnexion B1 à B7 contextualisé ;
- travail B5 prioritaire dispatché avant un polling déjà échu sur le même bus ;
- aucune dépendance à un transport physique.
