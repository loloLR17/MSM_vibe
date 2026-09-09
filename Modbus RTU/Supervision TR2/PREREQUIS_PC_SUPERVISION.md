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
| Windows | À FIGER | Cible d'exploitation envisagée pour MSM. | Ne pas inscrire Windows 10/11 comme minimum tant que la composition et le packaging du service ne sont pas gelés. |
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
| `Microsoft.Data.Sqlite` | REQUIS à partir de S2-B | Fournisseur ADO.NET retenu. | Installé comme dépendance NuGet de l'application, pas comme logiciel PC séparé. |
| SQL Server | NON REQUIS | Aucun serveur SQL externe local. | Architecture offline-first avec SQLite local. |
| SQL Server Express | NON REQUIS | Aucun besoin. | Idem. |
| Service de base de données Windows séparé | NON REQUIS | SQLite est embarqué/in-process. | Aucun daemon/service DB à administrer. |
| Outil CLI `sqlite3` | NON REQUIS | Pas nécessaire à l'exécution normale. | Peut être utilisé ponctuellement en diagnostic, sans devenir un prérequis. |
| Entity Framework Core | NON REQUIS | S2 utilise SQL explicite via `Microsoft.Data.Sqlite`. | Décision S2-A. |

La base SQLite doit résider sur un **filesystem local du PC de supervision**. L'exploitation directe de la base active depuis un partage réseau n'est pas supportée par la politique S2.

À la clôture S2, le schéma structuré courant est `PRAGMA user_version = 5`. Le schéma est documenté dans `SUPERVISION_S2_PERSISTENCE_SCHEMA.md`.

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
| Interface série RS-485 | FUTUR | Nécessaire lorsque le transport Modbus RTU physique sera raccordé. | S1 n'a pas encore gelé l'adaptateur physique. |
| Adaptateur USB/RS-485 précis | À FIGER | Modèle, chipset et driver non sélectionnés. | À qualifier lors de la phase transport matériel. |
| Driver Windows de l'adaptateur | FUTUR | Dépendra de l'adaptateur retenu. | Non requis pour les tranches host actuelles. |
| Port COM disponible | FUTUR | Nécessaire au runtime physique. | Pas encore consommé par S2. |

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
| Exécution en service Windows | À FIGER | Le projet `TR2.Supervision.Service` existe, mais la composition/installation du service n'est pas gelée. | Phase de composition runtime ultérieure. |
| Compte de service dédié | FUTUR | Politique de compte et droits non définie. | À figer avec le packaging/service. |
| Droits d'écriture sur le dossier de données | REQUIS conceptuellement | Le processus doit pouvoir créer/modifier sa base et ses fichiers associés. | Chemin et ACL précis à figer avec le packaging. |
| Droits administrateur permanents | NON REQUIS à ce stade | Aucun invariant courant ne justifie un runtime administrateur. | L'installation initiale pourra éventuellement demander une élévation selon le packaging futur. |

---

## 10. Horloge et temps

| Élément | Statut | Exigence actuelle | Source / remarque |
|---|---|---|---|
| Horloge système PC fonctionnelle | REQUIS | Les timestamps PC sont utilisés par la supervision et l'archivage. | S1/S2. |
| Synchronisation NTP/heure Windows | À FIGER | Recommandée mais politique opérationnelle non encore décidée. | Ne pas la considérer comme autorité Modbus implicite. |
| Fuseau horaire | À FIGER | Politique d'affichage à confirmer lors de la composition/UI. | Les timestamps S2 persistés depuis `DateTimeOffset` sont sérialisés en UTC et restent distincts du temps TR2 B2. |

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

## 12. État à la clôture de S2

À l'issue de S2, les points certains pour le futur PC sont :

1. application de supervision basée sur **.NET 10** ;
2. persistance locale structurée via **SQLite** ;
3. accès SQLite via **`Microsoft.Data.Sqlite`**, sans SQL Server ni EF Core ;
4. base active sur disque **local**, en WAL, `synchronous=FULL`, `foreign_keys=ON` ;
5. schéma SQLite versionné, version courante S2 = **5** ;
6. stockage durable actuellement implémenté pour B5, B3, journal communication et modèle Installation/Equipment/MeasurementPoint/affectations ;
7. reprise après fermeture/réouverture host testée sur ces stores ;
8. fonctionnement **offline-first** ;
9. aucun transport série physique encore requis pour S2 ;
10. version minimale exacte de Windows, capacité disque, adaptateur RS-485, packaging .NET, politique de service, sauvegarde et rétention restent à figer.

Aucun nouveau logiciel PC externe n'a été introduit par S2-C à S2-H au-delà des dépendances déjà enregistrées en S2-A/S2-B.

---

## 13. Règle de maintenance

À chaque nouvelle tranche de supervision :

1. identifier les nouvelles dépendances PC introduites ;
2. mettre à jour ce document dans le même commit ou dans un commit documentaire immédiatement associé ;
3. changer explicitement le statut d'un prérequis lorsqu'il est gelé ;
4. indiquer une version minimale uniquement lorsqu'elle est justifiée par le code, une documentation officielle ou un arbitrage ;
5. ne jamais ajouter silencieusement un logiciel externe au PC de production.

Ce registre fait partie des contrôles transversaux de clôture de chaque future tranche de supervision.
