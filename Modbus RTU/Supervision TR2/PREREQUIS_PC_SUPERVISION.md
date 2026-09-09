# Projet MSM — Capteur de vibration TR2

## Prérequis du PC de supervision

Date de création : 2026-09-09

Ce document est un **registre vivant** des prérequis logiciels, système et matériels du PC hébergeant la supervision TR2.

Il doit être mis à jour à chaque tranche de supervision qui introduit, retire ou précise une dépendance d'exploitation.

Les entrées sont volontairement classées par statut afin de ne pas transformer une hypothèse ou un choix futur en exigence gelée.

---

## 1. Statuts

- **REQUIS** : nécessaire avec l'état logiciel courant ou une décision déjà gelée.
- **À FIGER** : besoin identifié mais valeur/version minimale pas encore arbitrée.
- **FUTUR** : dépendance probable d'une tranche ultérieure, non requise aujourd'hui.
- **NON REQUIS** : composant explicitement inutile avec l'architecture actuelle.

---

## 2. Système d'exploitation

| Élément | Statut | Exigence actuelle | Source / remarque |
|---|---|---|---|
| Système d'exploitation 64 bits | À FIGER | La supervision est destinée à un PC de supervision 64 bits. | La version minimale de Windows n'est pas encore gelée. |
| Windows | À FIGER | Cible d'exploitation envisagée pour MSM. | Ne pas inscrire Windows 10/11 comme minimum tant que le packaging du service n'est pas gelé. |
| Linux | NON REQUIS | Aucun besoin de Linux/WSL pour exploiter la supervision. | WSL peut rester un outil de développement local, pas un prérequis de production. |

---

## 3. .NET

| Élément | Statut | Exigence actuelle | Source / remarque |
|---|---|---|---|
| .NET 10 | REQUIS | Tous les projets de supervision ciblent `net10.0`. | Gel S0/S1 et `Directory.Build.props`. |
| .NET 10 SDK | REQUIS pour développement/build | Nécessaire pour `dotnet build` et `dotnet test`. | Non nécessairement requis sur le PC de production si le déploiement final n'est pas SDK-dependent. |
| .NET 10 Runtime | À FIGER pour production | Sera requis si l'application est publiée framework-dependent. | Le mode de publication final (framework-dependent ou self-contained) n'est pas encore gelé. |
| Visual Studio | NON REQUIS | Aucun besoin pour l'exploitation. | Optionnel pour le développement. |

---

## 4. Base de données locale

| Élément | Statut | Exigence actuelle | Source / remarque |
|---|---|---|---|
| SQLite | REQUIS à partir de S2 | Moteur local retenu pour la persistance structurée. | `ARBITRAGE_SUPERVISION_S2A_PERSISTENCE_POLICY.md`. |
| `Microsoft.Data.Sqlite` | REQUIS à partir de S2-B | Fournisseur ADO.NET retenu. | Dépendance NuGet de l'application, pas logiciel PC séparé. |
| SQL Server / SQL Server Express | NON REQUIS | Aucun serveur SQL externe local. | Architecture offline-first avec SQLite local. |
| Service de base de données Windows séparé | NON REQUIS | SQLite est embarqué/in-process. | Aucun daemon/service DB à administrer. |
| Outil CLI `sqlite3` | NON REQUIS | Pas nécessaire à l'exécution normale. | Diagnostic ponctuel seulement. |
| Entity Framework Core | NON REQUIS | SQL explicite via `Microsoft.Data.Sqlite`. | Décision S2-A. |

La base SQLite doit résider sur un **filesystem local du PC de supervision**. L'exploitation directe de la base active depuis un partage réseau n'est pas supportée.

À la clôture S4, le schéma structuré courant est `PRAGMA user_version = 6`. La migration S4 ajoute la catégorie stable du journal de défaillances de communication et étend les opérations journalisables à `Polling`, `ExplicitRefresh`, `CommandTransaction` et `CampaignSelection`.

---

## 5. Stockage disque

| Élément | Statut | Exigence actuelle | Source / remarque |
|---|---|---|---|
| Stockage local persistant | REQUIS | Nécessaire pour la base SQLite et les journaux durables. | S2-A. |
| Capacité disque minimale | À FIGER | Non définie à ce stade. | Dépendra de la rétention B3, journaux et campagnes. |
| Système de fichiers / volume précis | À FIGER | Doit supporter correctement les opérations de fichiers locaux requises par SQLite. | Le volume de production sera qualifié ultérieurement. |
| Stockage des campagnes SD | FUTUR | Hors base SQLite structurée. | Architecture S0/S1 ; phase Campaigns dédiée. |

En mode WAL, les fichiers `*.db`, `*.db-wal` et `*.db-shm` peuvent faire partie de l'état opérationnel d'une base ouverte. Une sauvegarde ne doit pas être conçue comme une simple copie arbitraire du seul fichier principal pendant l'activité.

---

## 6. Interfaces de communication TR2

| Élément | Statut | Exigence actuelle | Source / remarque |
|---|---|---|---|
| Interface série RS-485 | REQUIS pour exploitation Modbus RTU physique | S4 raccorde effectivement le transport série physique derrière les contrats neutres `IRegisterTransport` / `IRegisterWriteTransport`. | Validation électrique et matérielle encore à réaliser sur banc réel. |
| Adaptateur USB/RS-485 précis | À FIGER | Modèle, chipset et driver non sélectionnés/qualifiés. | Le logiciel S4 ne donne aucune autorité d'identité au port COM. |
| Driver OS de l'adaptateur | À FIGER | Dépendra de l'adaptateur retenu. | À qualifier avec le matériel réel. |
| Port série disponible | REQUIS pour un bus configuré en série | Le nom du port est fourni explicitement par la configuration runtime. | Aucun nom de COM n'est normatif ni fourni par défaut. |
| Paramètres série | REQUIS par bus série | `PortName`, `BaudRate`, `DataBits`, `Parity`, `StopBits`, `ResponseTimeout` doivent être fournis. | Les valeurs réelles restent à déterminer/qualifier ; aucune valeur de test n'est normative. |
| `System.IO.Ports` | REQUIS à partir de S4 | Accès série .NET utilisé par l'adaptateur physique. | Dépendance applicative, pas logiciel opérateur séparé. |
| NModbus | REQUIS à partir de S4 | Bibliothèque Modbus utilisée pour le framing/CRC et les transactions RTU. | Version NuGet retenue : `3.0.83`. |
| NModbus.Serial | REQUIS à partir de S4 | Adaptation NModbus au port série. | Version NuGet retenue : `3.0.83`. |
| Retry automatique NModbus | INTERDIT PAR POLITIQUE S4 | `Retries = 0`, `WaitToRetryMilliseconds = 0`; aucun replay caché. | Invariant particulièrement critique pour B5. |

S4 qualifie le **comportement logiciel** du transport : ouverture/fermeture, polling réel derrière l'adaptateur, refresh prioritaire, B5, sélection B6, déconnexion/reconnexion et classification stable des erreurs. S4 **ne qualifie pas** encore un adaptateur USB/RS-485 réel, le câblage, les niveaux électriques, la terminaison/polarisation, les paramètres série réels du TR2 ou les comportements de hot-unplug propres à un driver donné.

---

## 7. Réseau

| Élément | Statut | Exigence actuelle | Source / remarque |
|---|---|---|---|
| Accès Internet permanent | NON REQUIS | La supervision est offline-first. | S0. |
| Réseau local | FUTUR | Probablement nécessaire pour Web UI et/ou consommateurs locaux. | API/Web non encore gelés. |
| Accès plateforme analytique / Grafana | FUTUR | Synchronisation prévue de manière asynchrone et reprenable. | Ne doit pas conditionner l'acquisition locale. |

---

## 8. Interface utilisateur / navigateur

| Élément | Statut | Exigence actuelle | Source / remarque |
|---|---|---|---|
| Navigateur Web | FUTUR | S0 prévoit une Web UI locale future. | Aucun navigateur précis/minimum n'est encore gelé. |
| Accès navigateur direct au Modbus | NON REQUIS / INTERDIT PAR ARCHITECTURE | Le navigateur ne devient jamais maître Modbus. | Le moteur de supervision reste l'autorité du bus. |

---

## 9. Services et droits Windows

| Élément | Statut | Exigence actuelle | Source / remarque |
|---|---|---|---|
| Exécution en service Windows | À FIGER | Le runtime long-running et sa composition physique existent, mais installation/packaging Windows Service restent hors S4. | À traiter dans une phase dédiée. |
| Compte de service dédié | FUTUR | Politique de compte et droits non définie. | À figer avec le packaging/service. |
| Droits d'écriture sur le dossier de données | REQUIS conceptuellement | Le processus doit pouvoir créer/modifier sa base et ses fichiers associés. | Chemin et ACL précis à figer avec le packaging. |
| Droits d'accès au port série | REQUIS conceptuellement | Le processus doit pouvoir ouvrir le port série configuré. | ACL/driver exacts à qualifier sur l'OS et l'adaptateur retenus. |
| Droits administrateur permanents | NON REQUIS à ce stade | Aucun invariant courant ne justifie un runtime administrateur. | Installation/driver pourront éventuellement demander une élévation. |

---

## 10. Horloge et temps

| Élément | Statut | Exigence actuelle | Source / remarque |
|---|---|---|---|
| Horloge système PC fonctionnelle | REQUIS | Les timestamps PC sont utilisés par la supervision et l'archivage. | S1/S2. |
| Synchronisation NTP/heure Windows | À FIGER | Recommandée mais politique opérationnelle non encore décidée. | Ne pas la considérer comme autorité Modbus implicite. |
| Fuseau horaire | À FIGER | Politique d'affichage à confirmer lors de la composition/UI. | Les timestamps persistés sont sérialisés en UTC et restent distincts du temps TR2 B2. |

---

## 11. Développement et validation uniquement

Ces éléments ne sont **pas** des prérequis d'exploitation du futur PC livré, sauf décision ultérieure :

- Git ;
- GitHub CLI ;
- WSL ;
- Visual Studio / VS Code ;
- CMake et toolchains firmware STM32 ;
- Python ;
- Arduino IDE ;
- outils de flash/debug STM32.

Ils appartiennent au poste de développement ou au banc firmware, pas à la supervision de production.

---

## 12. État à la clôture de S4

À l'issue de S4, les points certains pour le futur PC sont :

1. application de supervision basée sur **.NET 10** ;
2. persistance locale structurée via **SQLite**, schéma courant **6** ;
3. accès SQLite via **`Microsoft.Data.Sqlite`**, sans SQL Server ni EF Core ;
4. base active sur disque local, en WAL, `synchronous=FULL`, `foreign_keys=ON` ;
5. transport Modbus RTU physique implémenté derrière des interfaces neutres ;
6. accès série via `System.IO.Ports`, NModbus `3.0.83` et NModbus.Serial `3.0.83` ;
7. aucune répétition automatique NModbus autorisée ;
8. bus série configuré explicitement avec port, baud, bits de données, parité, stop bits et timeout ;
9. déconnexion I/O, reconnexion cadencée, nouvelle identification B0 et reprise opérationnelle testées côté logiciel ;
10. polling, refresh, B5 et sélection B6 disposent d'un chemin physique logiciel ;
11. les défaillances de communication sont classées de façon stable (`Unclassified`, `Timeout`, `Io`, `ModbusExceptionResponse`) et journalisées durablement ;
12. fonctionnement **offline-first** maintenu ;
13. aucune validation électrique RS-485 ni qualification d'adaptateur/driver réel n'est revendiquée ;
14. version minimale exacte de Windows, capacité disque, modèle d'adaptateur RS-485, paramètres série réels, packaging .NET, Windows Service, sauvegarde et rétention restent à figer.

---

## 13. Règle de maintenance

À chaque nouvelle tranche de supervision :

1. identifier les nouvelles dépendances PC introduites ;
2. mettre à jour ce document dans le même commit ou dans un commit documentaire immédiatement associé ;
3. changer explicitement le statut d'un prérequis lorsqu'il est gelé ;
4. indiquer une version minimale uniquement lorsqu'elle est justifiée par le code, une documentation officielle ou un arbitrage ;
5. ne jamais ajouter silencieusement un logiciel externe au PC de production.

Ce registre fait partie des contrôles transversaux de clôture de chaque future tranche de supervision.
