# Projet MSM — Capteur de vibration TR2

## Gel S3 — Composition runtime et orchestration du service de supervision

Date de gel : 2026-09-09

Ce document clôture **S3 — composition runtime et orchestration du service de supervision**. Il complète `FREEZE_SUPERVISION_S0_CADRAGE.md`, `FREEZE_SUPERVISION_S1_SOCLE_LOGICIEL.md` et `FREEZE_SUPERVISION_S2_PERSISTANCE_LOCALE.md`. Il ne modifie ni la spécification Modbus RTU V1 ni les gels firmware.

## 1. Baseline logicielle validée

Dernier état logiciel S3 validé localement :

`913bea37e0975d2146c351bddf8ad09f1a0830ca`

`Supervision: add S3-G restart recovery observability`

L'utilisateur a confirmé build et tests verts sur cette baseline avec :

```bash
dotnet build TR2.Supervision.sln --no-restore
dotnet test TR2.Supervision.sln --no-build
```

Le présent commit est documentaire uniquement.

## 2. Tranches S3 gelées

S3 est constitué des tranches validées suivantes :

```text
S3-A  politique de configuration runtime
S3-B  composition root host
S3-C  startup, reconstruction durable et barrière B5
S3-D  lifecycle runtime, cancellation et shutdown
S3-E  boucle d'orchestration polling host
S3-F  façade opérationnelle B5 / refresh et dispatch prioritaire
S3-G  recovery/restart intégré et observabilité minimale
```

Les arbitrages détaillés S3-A à S3-G restent applicables.

## 3. Configuration runtime

La supervision dispose désormais d'une configuration locale JSON pour :

- chemin SQLite ;
- bus logiques ;
- endpoints par bus ;
- cadences de polling host.

Invariants :

- chargement fail-fast ;
- membres JSON inconnus refusés ;
- identifiants de bus uniques ;
- adresse endpoint unique à l'intérieur d'un même bus ;
- une même adresse peut exister sur plusieurs bus ;
- aucun `device_id` n'est dérivé du bus ou de l'adresse Modbus ;
- le domaine courant ne définissant pas de plage Modbus normative plus étroite, S3 ne fabrique pas arbitrairement une règle 1..247 et valide seulement la représentabilité `byte` existante.

Les cadences sont **SUPERVISION_POLICY**, non normatives Modbus V1.

Valeurs par défaut S3 :

```text
STATIC retry  5 s
FAST          1 s
MEDIUM        5 s
SLOW          30 s
scan host     100 ms
```

Elles restent configurables et ne constituent pas des exigences firmware.

## 4. Composition root

`SupervisionRuntimeCompositionRoot` construit les autorités runtime partagées sans déclencher d'activité opérationnelle.

La composition seule :

- n'ouvre pas SQLite ;
- ne crée ni ne migre la base ;
- ne communique pas en Modbus ;
- ne démarre pas de polling ;
- ne restaure pas B5 ;
- ne lance aucune tâche de fond.

Les endpoints configurés sont enregistrés initialement `Unidentified` dans `FleetRegistry`.

Aucun `CommandCoordinator` fantôme n'est créé à partir d'une adresse Modbus.

## 5. Startup et readiness

Le startup S3-C est one-shot.

Ordre gelé :

1. ouverture SQLite ;
2. application/vérification des migrations S2 ;
3. lecture des `device_id` présents dans le journal B5 ;
4. reconstruction d'un `CommandCoordinator` par `device_id` concerné ;
5. recovery des transactions durables ;
6. seulement ensuite ouverture de la readiness.

Une transaction B5 non terminale reconstruite reste `Ambiguous` conformément aux invariants S1/S2.

Aucun replay automatique, aucun retry naïf et aucune réallocation silencieuse ne sont autorisés.

Une réservation durable sans événement de journal ne crée pas artificiellement une transaction active.

En cas d'échec du startup ou du recovery, la readiness reste fermée.

## 6. Lifecycle host

`SupervisionRuntimeHost` porte le lifecycle long-running :

```text
Created -> Starting -> Running -> Stopping -> Stopped
                         \
                          -> Faulted
```

Invariants :

- host one-shot ;
- `Running` seulement après startup/recovery réussi ;
- `CancellationToken` est l'autorité S3 d'arrêt ;
- une annulation demandée conduit à un arrêt normal, pas à un fault ;
- la readiness est fermée au shutdown et au fault ;
- aucun restart automatique n'est introduit.

S3 ne définit pas encore de timeout matériel de shutdown ni de drain RS-485, puisqu'aucun transport physique n'est raccordé.

## 7. Polling host

Le planning logique S1 reste gelé :

```text
STATIC  B0
FAST    B1 + B3 + B5
MEDIUM  B2 + B7
SLOW    B4 + B6
```

Après une identification B0 compatible, les groupes opérationnels deviennent éligibles selon leur cadence.

Un endpoint non compatible, non identifié ou déconnecté retourne au chemin STATIC/reconnexion avant reprise du polling opérationnel.

Le `BusWorkScheduler` reste l'autorité de sérialisation logique par bus : un seul travail actif par bus.

Aucun faux port série, faux adaptateur RS-485 ou producteur matériel n'a été introduit pour rendre S3 exécutable sur host.

## 8. Priorités et façade opérationnelle

La façade S3-F exige la readiness et une session compatible identifiée avant :

- préparation d'une commande B5 ;
- création du contexte `B5CommandRequest` ;
- planification d'un refresh post-reconnexion.

Pour B5 :

- le `CommandCoordinator` reste indexé par `device_id` ;
- l'allocation du `transaction_id` est durable avant mise en file du travail de commande ;
- une transaction active non terminale bloque une nouvelle préparation ;
- le `transaction_id` n'est pas dérivé du transport ou de l'adresse Modbus.

Le refresh post-reconnexion conserve l'ordre logique B1 à B7, B0 ayant déjà servi à réidentifier la session.

Les travaux prioritaires :

```text
ExplicitRefresh
CommandTransaction
CommandPostSubmitMonitoring
TransactionReconciliation
```

passent devant le polling lorsqu'ils sont dus sur le même bus. Leur ordre relatif reste déterminé par `DueAt`, puis la séquence d'insertion, conformément au scheduler S1.

L'exécution Modbus physique de ces travaux n'est pas ajoutée par S3.

## 9. Recovery/restart intégré

S3-G prouve sur une vraie base SQLite temporaire :

1. création et journalisation d'une transaction B5 ;
2. passage à `Ambiguous` ;
3. abandon du premier graphe runtime ;
4. recréation complète du composition root sur le même fichier SQLite ;
5. startup/recovery ;
6. restauration du même `device_id` et du même `transaction_id` en `Ambiguous` ;
7. maintien du blocage d'une nouvelle commande jusqu'à résolution explicite.

Cette preuve qualifie un **close/reopen et restart logiciel host**.

Elle ne qualifie pas :

- une coupure physique pendant écriture disque ;
- corruption média ;
- perte d'alimentation PC ;
- comportement d'un adaptateur série réel.

## 10. Observabilité minimale

`SupervisionRuntimeStatusReader` fournit un snapshot local en lecture seule :

- readiness ;
- nombre d'endpoints configurés ;
- nombre d'endpoints compatibles ;
- nombre d'endpoints déconnectés ;
- nombre de `CommandCoordinator` ;
- nombre de transactions B5 actives `Ambiguous`.

Cette observabilité :

- n'altère aucun état ;
- ne remplace pas les journaux S2 ;
- n'est ni une API Web ni une métrique Prometheus ;
- n'introduit aucun framework de logging supplémentaire.

## 11. Architecture et frontières préservées

Les dépendances projet S1 restent inchangées :

```text
TR2.Domain              -> aucune dépendance projet
TR2.Transport           -> aucune dépendance projet
TR2.Protocol            -> TR2.Domain + TR2.Transport
TR2.Application         -> TR2.Domain + TR2.Protocol
TR2.Persistence         -> TR2.Application + TR2.Domain
TR2.Campaigns           -> TR2.Application + TR2.Domain
TR2.Supervision.Service -> Application + Campaigns + Domain + Persistence + Protocol + Transport
```

S3 ajoute l'orchestration dans `TR2.Supervision.Service` sans déplacer les autorités de domaine, protocole, persistance ou transport.

## 12. Classification des décisions

S3 ne change aucun comportement normatif Modbus RTU V1.

Relèvent notamment de **SUPERVISION_POLICY** :

- format de configuration runtime ;
- cadences de polling ;
- scan interval host ;
- lifecycle one-shot ;
- readiness host ;
- organisation de la boucle runtime ;
- snapshot d'observabilité locale.

Restent sous les invariants déjà gelés S1/S2 :

- identité durable par `device_id` ;
- sérialisation par bus ;
- transaction B5 lifetime-strict ;
- persistance avant submit ;
- recovery ambigu ;
- absence de replay automatique ;
- priorités des travaux non-polling sur le polling.

Aucune règle V1.1 n'est importée pour combler un manque V1.

## 13. Hors périmètre après S3

Restent explicitement ouverts :

- transport Modbus RTU physique ;
- choix/adaptateur RS-485 ;
- mapping bus logique vers port COM ;
- paramètres série réels : baud, parité, stop bits et contraintes driver ;
- stratégie de timeout/retry transport ;
- provisioning persistant de l'adresse Modbus côté TR2 ;
- découverte matérielle/gestion branchement-débranchement réelle ;
- exécution réelle des runners polling, refresh et B5 sur port série ;
- Windows Service et compte de service ;
- politique OS minimale définitive ;
- CLI/packaging/installation ;
- Web API / UI ;
- authentification, rôles et sécurité opérateur ;
- journal opérateur ;
- write path B4 ;
- import/parser fichiers SD et stockage brut ;
- FFT et traitements analytiques ;
- rétention, sauvegarde/export et chiffrement ;
- synchronisation Grafana/analytics ;
- qualification de coupure d'alimentation réelle pendant écriture SQLite.

## 14. Règle de poursuite

La prochaine phase ne doit pas contourner les invariants S3 pour raccorder le matériel.

En particulier :

- le transport physique doit implémenter les contrats existants sans donner l'autorité d'identité au port COM ;
- un défaut de communication ne doit pas être transformé en défaut de persistance ;
- le scheduler unique par bus doit rester l'autorité de sérialisation ;
- le recovery B5 doit rester terminé avant ouverture opérationnelle ;
- aucun retry transport ne doit produire un replay naïf d'une commande B5 potentiellement soumise ;
- toute nouvelle décision de configuration matérielle doit être classée explicitement `SUPERVISION_POLICY`, `FW_POLICY`, `NOT_DEFINED V1` ou V1.1 selon son autorité réelle.

S3 est considéré gelé après validation locale verte de la baseline logicielle ci-dessus et intégration du présent document sur `main`.
