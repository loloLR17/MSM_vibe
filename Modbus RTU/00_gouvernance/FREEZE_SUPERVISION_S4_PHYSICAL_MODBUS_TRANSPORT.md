# Projet MSM — Capteur de vibration TR2

## Gel S4 — Transport Modbus RTU physique de la supervision

Date de gel : 2026-09-09

Ce document clôture **S4 — transport Modbus RTU physique de la supervision**. Il complète les gels S0 à S3. Il ne modifie ni la spécification Modbus RTU V1 ni les gels firmware.

## 1. Baseline logicielle validée

Dernier état logiciel S4 validé localement avant la clôture documentaire :

`2a83831cc38fb0b27fe89f6c3578babc78d148d4`

`Supervision: test physical runtime composition`

L'utilisateur a confirmé build et tests verts avec :

```bash
dotnet build TR2.Supervision.sln --no-restore
dotnet test TR2.Supervision.sln --no-build
```

Les commits de prérequis et de gel qui suivent sont documentaires.

## 2. Tranches S4 gelées

S4 couvre les tranches validées suivantes :

```text
S4-A   contrat transport physique et invariants de sécurité
S4-B   configuration runtime série
S4-C   adaptateur registres NModbus
S4-D   lifecycle des connexions physiques et mapping runtime
S4-E   déconnexion / reconnexion logicielle
S4-F   polling et découverte B0 physiques
S4-G   refresh prioritaire et B5 physique avec sécurité transactionnelle
S4-H   sélection B6, classification stable des erreurs et journalisation
S4-I   composition finale du runtime physique et clôture documentaire
```

## 3. Frontière transport

Les contrats existants restent l'autorité entre protocole et transport :

- `IRegisterTransport` pour les lectures ;
- `IRegisterWriteTransport` pour les écritures.

Ils restent indépendants de `SerialPort` et de NModbus. Les types NModbus restent confinés dans `TR2.Transport`.

Cette frontière permet l'ajout futur d'un autre transport derrière les mêmes contrats sans introduire aujourd'hui d'abstraction UDP spéculative.

## 4. Bibliothèque Modbus et politique de retry

S4 retient :

- NModbus `3.0.83` ;
- NModbus.Serial `3.0.83` ;
- accès série .NET via `System.IO.Ports`.

Le framing Modbus RTU, CRC et fonctions standard sont délégués à cette bibliothèque mature.

Politique de sécurité S4 :

```text
Retries = 0
WaitToRetryMilliseconds = 0
SlaveBusyUsesRetryCount = true
```

Aucun retry caché n'est autorisé. Cette règle est critique pour B5 : une écriture potentiellement émise ne doit jamais être rejouée automatiquement après résultat ambigu.

## 5. Configuration série

Un bus runtime peut porter une configuration série explicite comprenant :

- `PortName` ;
- `BaudRate` ;
- `DataBits` ;
- `Parity` ;
- `StopBits` ;
- `ResponseTimeout`.

Ces paramètres sont **SUPERVISION_POLICY**. S4 ne gèle aucune valeur réelle de baud/parité/stop bits/timeout et aucun nom de port COM. Les valeurs utilisées en tests ne sont pas normatives.

L'identité logique du bus reste indépendante du nom de port physique.

## 6. Lifecycle physique

`ModbusBusConnectionManager` possède les connexions physiques par `busId`.

Invariants :

- une seule connexion physique active par bus logique ;
- ouverture explicite ;
- doublon d'ouverture refusé ;
- fermeture et disposal explicites ;
- erreur de factory propagée ;
- incohérence de `BusId` retourné refusée avec nettoyage ;
- cancellation vérifiée avant l'opération lorsque possible.

La composition seule n'ouvre aucun port. L'ouverture des bus série configurés intervient au startup.

## 7. Polling et identification

Le planning S3 reste applicable :

```text
STATIC  B0
FAST    B1 + B3 + B5
MEDIUM  B2 + B7
SLOW    B4 + B6
```

`PhysicalPollingWorkRunner` exécute les lectures via la connexion physique courante.

B0 reste l'autorité d'identification/reconnaissance de compatibilité. Après une rupture I/O et une reconnexion physique, la session reste `Unidentified` jusqu'à une nouvelle lecture B0 réussie.

Le `BusWorkScheduler` reste l'autorité de sérialisation : un seul travail actif par bus.

## 8. Politique de défaillance et reconnexion

Taxonomie stable S4 :

```text
Unclassified
Timeout
Io
ModbusExceptionResponse
```

Politique :

- `Timeout` : échec de l'opération, connexion physique conservée, identité B0 conservée ;
- `Io` : connexion fermée, sessions du bus remises `Unidentified`, reconnexion possible selon cadence configurée ;
- `ModbusExceptionResponse` : réponse Modbus valide d'exception, non assimilée à une rupture physique, pas de déconnexion automatique ;
- `Unclassified` : aucune sémantique de transport supplémentaire inventée.

La cadence de reconnexion est opt-in et relève de **SUPERVISION_POLICY**. Aucun intervalle matériel/normatif n'est déduit de la V1.

Le `SerialBusReconnectScheduler` contient les échecs d'ouverture/reconnexion et les replanifie, tout en propageant une cancellation demandée. Ces échecs d'ouverture OS/driver ne sont pas artificiellement classés comme défaillances Modbus endpoint.

## 9. B5 physique et ambiguïté

Le chemin B5 physique conserve les invariants transactionnels gelés :

1. réservation durable du `transaction_id` avant submit ;
2. préparation puis submit comme deux écritures distinctes ;
3. aucun retry automatique ;
4. timeout pendant prepare : transaction reste `Prepared`, aucun submit ;
5. exception après tentative de submit : transaction durable `Ambiguous` ;
6. I/O après tentative de submit : `Ambiguous`, connexion fermée et session invalidée ;
7. reconnexion et nouveau B0 ne résolvent pas l'ambiguïté ;
8. une transaction ambiguë reconstruite continue de bloquer une nouvelle commande jusqu'à résolution explicite.

S4 ne crée aucun `transaction_epoch` V1.1 et ne modifie pas la machine d'état B5 pour faciliter le transport.

## 10. Refresh prioritaire et B6

`PhysicalPriorityWorkRunner` exécute :

- `ExplicitRefresh` ;
- `CommandTransaction` B5 ;
- `CampaignSelection` B6.

La sélection B6 écrit le `campaignIndex` via le chemin protocole existant au registre prévu par B6. S4 n'invente aucune plage sémantique supplémentaire sur cet index.

B6 n'est pas transformé en transaction B5 : pas de faux `transaction_id`, pas d'état durable d'ambiguïté B5, pas de retry automatique.

## 11. Journal communication durable

Le schéma SQLite courant devient `PRAGMA user_version = 6`.

Le journal de défaillances persiste :

- bus ;
- adresse Modbus ;
- `device_id` lorsqu'il est connu ;
- opération ;
- catégorie stable ;
- timestamp ;
- type technique d'exception ;
- message technique.

Opérations autorisées :

```text
Polling
ExplicitRefresh
CommandTransaction
CampaignSelection
```

Lors de la migration d'anciennes lignes, la catégorie devient `Unclassified` : aucune reclassification rétroactive n'est déduite du nom CLR ou du message d'exception.

## 12. Composition runtime physique

`PhysicalSupervisionRuntimeFactory` centralise l'assemblage :

```text
RuntimeConfiguration
        ↓
SupervisionRuntimeComposition
        ↓
SupervisionOperationalFacade
        ↓
PhysicalPollingWorkRunner
PhysicalPriorityWorkRunner
        ↓
SupervisionPollingLoop
        ↓
SupervisionRuntimeHost
```

`supportedProtocolVersion` reste un paramètre explicite de composition. S4 ne cache ni n'invente une version protocole implicite.

La factory ne déclenche pas d'I/O matériel ; l'ouverture reste sous l'autorité du startup.

## 13. Classification des décisions

S4 ne change aucun comportement normatif Modbus RTU V1.

Relèvent de **SUPERVISION_POLICY** :

- choix de NModbus côté PC ;
- absence de retry automatique ;
- configuration série runtime ;
- timeout de réponse configuré ;
- cadence de reconnexion ;
- mapping bus logique vers port série ;
- taxonomie stable du journal de communication ;
- comportement host face à timeout/I/O/réponse d'exception Modbus.

Restent normatifs/protocolaires uniquement les adresses, blocs, fonctions et comportements déjà définis par la spécification V1 et les couches `TR2.Protocol` existantes.

Aucune règle V1.1 n'est importée silencieusement.

## 14. Validation obtenue et limite matérielle

S4 est validé **côté logiciel** par les tests host, fakes/doubles et intégrations SQLite.

Sont notamment couverts :

- configuration série ;
- factory et lifecycle connexion ;
- polling B0 puis opérationnel ;
- I/O → déconnexion → reconnexion → nouveau B0 ;
- refresh physique ;
- B5 succès, timeout prepare, timeout submit, I/O submit et persistance de l'ambiguïté ;
- B6 physique ;
- classification et journalisation des erreurs ;
- composition du runtime physique.

S4 **ne constitue pas** une validation physique/électrique RS-485. Sans adaptateur et cible réels, il ne qualifie pas :

- niveaux électriques ;
- câblage A/B et référence commune ;
- terminaison/polarisation ;
- comportement réel du transceiver ;
- driver USB/RS-485 ;
- hot-unplug matériel réel ;
- paramètres série réels du TR2 ;
- timing réel sur bus ;
- compatibilité physique avec la NUCLEO/STM32.

Ces points devront faire l'objet d'une campagne de banc matériel distincte.

## 15. Hors périmètre après S4

Restent notamment ouverts :

- qualification et choix final de l'adaptateur USB/RS-485 ;
- validation physique avec STM32/TR2 réel ;
- valeurs série réelles et procédure de provisioning ;
- packaging CLI final ;
- Windows Service, installation et compte de service ;
- version minimale exacte de Windows ;
- Web API / Web UI ;
- authentification et rôles ;
- write path B4 s'il est requis ultérieurement ;
- import/parser des campagnes SD et stockage brut ;
- FFT et analyses ;
- rétention, sauvegarde/export et chiffrement ;
- synchronisation analytique/Grafana ;
- qualification de coupure d'alimentation PC réelle pendant écriture SQLite ;
- transport réseau/UDP éventuel, qui ne doit être introduit que lorsqu'un besoin concret le justifie.

## 16. Règle de poursuite

La suite doit préserver les invariants S0 à S4.

En particulier :

- le port COM ne devient jamais une identité d'équipement ;
- le scheduler reste l'autorité de sérialisation du bus ;
- aucun retry ne peut rejouer naïvement B5 ;
- une reconnexion physique exige une nouvelle identification B0 avant reprise opérationnelle ;
- une réponse d'exception Modbus ne doit pas être assimilée sans preuve à une rupture physique ;
- la validation matérielle future doit mesurer et documenter ce qui n'a été validé ici que par doubles logiciels ;
- toute nouvelle décision doit rester explicitement classée `normatif V1`, `SUPERVISION_POLICY`, `FW_POLICY`, `NOT_DEFINED V1` ou V1.1 selon son autorité.

S4 est considéré gelé après validation locale verte de la baseline logicielle indiquée en section 1 et intégration du présent document sur `main`.
