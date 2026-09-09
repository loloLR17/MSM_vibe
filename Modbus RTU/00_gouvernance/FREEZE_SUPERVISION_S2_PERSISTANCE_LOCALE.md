# Projet MSM — Capteur de vibration TR2

## Gel S2 — Persistance locale durable de la supervision

Date de gel : 2026-09-09

Ce document clôture **S2 — persistance locale durable et reprise de l'état de supervision**. Il complète `FREEZE_SUPERVISION_S0_CADRAGE.md` et `FREEZE_SUPERVISION_S1_SOCLE_LOGICIEL.md`. Il ne modifie ni la spécification Modbus RTU V1 ni les gels firmware.

## 1. Baselines

Dernier état logiciel S2 validé localement :

`4c5278ae15727f59e1c3008d394fb54dde978967`

`Supervision: test S2-G integrated persistence recovery`

L'utilisateur a confirmé build et tests verts sur cette baseline.

Dernière baseline documentaire avant ce gel :

`30e51cb16ea4bf224c9d8696342c6df831ab756a`

`Supervision: close S2 PC prerequisites`

Le présent commit est documentaire.

## 2. Politique de persistance gelée

S2 gèle :

- SQLite local ;
- `Microsoft.Data.Sqlite` ;
- SQL explicite / ADO.NET ;
- absence d'EF Core ;
- base active sur filesystem local ;
- WAL ;
- `synchronous=FULL` ;
- `foreign_keys=ON` ;
- `busy_timeout` configurable, valeur par défaut actuelle 5000 ms ;
- transactions SQLite locales courtes ;
- absence de `System.Transactions` ;
- séparation stricte entre erreurs de stockage et erreurs Modbus.

Une transaction SQLite ne doit pas englober une communication Modbus, une attente opérateur, une temporisation de polling, un accès réseau externe ou un traitement analytique long.

## 3. Schéma gelé

Version courante :

```text
PRAGMA user_version = 5
```

Référence détaillée : `SUPERVISION_S2_PERSISTENCE_SCHEMA.md`.

Migrations :

```text
v1  marqueur de schéma
v2  réservation + journal B5
v3  archive B3
v4  journal communication
v5  Installation / Equipment / MeasurementPoint / affectations
```

Une version future non supportée est refusée explicitement. Aucune migration destructive silencieuse n'est autorisée.

## 4. B5 durable

S2 fournit les implémentations SQLite de `ICommandTransactionReservationStore` et `ICommandTransactionJournal`.

Invariants :

- allocation durable avant possibilité de submit ;
- `transaction_id` 1..65535 ;
- allocation lifetime-strict sous autorité Application ;
- journal append-only ;
- recovery depuis les données commitées ;
- transaction non terminale restaurée `Ambiguous` ;
- aucun replay automatique ;
- aucune réutilisation implicite du dernier `transaction_id` réservé ;
- une réservation durable sans journal n'invente aucune transaction active ; l'allocation suivante continue après la réservation persistée.

## 5. B3 durable

`SqliteB3ArchiveSink` persiste :

- `DeviceId` ;
- valeur B3 complète ;
- timestamp PC ;
- contexte B2 optionnel avec son propre timestamp PC.

L'archive est append-only dans S2. L'absence de B2 reste une absence réelle et n'est pas comblée. Une erreur d'archive reste une erreur de persistance et ne devient pas une erreur Modbus.

## 6. Journal communication durable

Le sink SQLite persiste bus, adresse Modbus, `DeviceId` nullable, opération, timestamp, type d'exception et message.

Aucun `DeviceId` n'est fabriqué depuis l'adresse Modbus.

La règle S1 reste applicable : dans les chemins best-effort, une panne du sink ne doit ni empêcher `Disconnected` ni masquer l'erreur de communication originale.

## 7. Modèle équipements durable

S2 persiste sans enrichissement implicite :

```text
Installation -> Equipment -> MeasurementPoint
DeviceId -> MeasurementPoint
```

avec historique `valid_from` / `valid_to`.

Invariants :

- clés étrangères hiérarchiques ;
- au plus un point actif par TR2 ;
- au plus un TR2 actif par point ;
- `Move` clôt l'ancienne affectation puis crée une nouvelle ligne ;
- `Unassign` clôt l'affectation active ;
- clôture strictement postérieure au début ;
- historique fermé conservé ;
- aucun couplage implicite avec B4.

## 8. Recovery intégré prouvé

Les tests S2-G prouvent sur vraie base SQLite temporaire :

- base vide sans état fantôme ;
- coexistence durable B5, B3, journal communication et affectations ;
- B5 non terminal restauré `Ambiguous` ;
- résolution puis allocation suivante sans réutilisation ;
- état partiel réservation-only sans transaction active inventée.

Cela qualifie fermeture/réouverture et restart logiciel host. Cela ne qualifie pas une coupure physique d'alimentation pendant une écriture disque.

## 9. Architecture préservée

Les frontières S1 restent valides et testées :

```text
TR2.Domain              -> aucune dépendance projet
TR2.Transport           -> aucune dépendance projet
TR2.Protocol            -> TR2.Domain + TR2.Transport
TR2.Application         -> TR2.Domain + TR2.Protocol
TR2.Persistence         -> TR2.Application + TR2.Domain
TR2.Campaigns           -> TR2.Application + TR2.Domain
TR2.Supervision.Service -> Application + Campaigns + Domain + Persistence + Protocol + Transport
```

S2 n'ajoute aucune dépendance directe de `TR2.Persistence` vers `TR2.Protocol`, le Web ou le transport physique. Les conversions SQLite restent confinées à `TR2.Persistence`.

## 10. Prérequis PC

`Modbus RTU/Supervision TR2/PREREQUIS_PC_SUPERVISION.md` fait partie des contrôles de clôture.

À l'issue de S2 : .NET 10, SQLite, `Microsoft.Data.Sqlite` et un stockage local persistant sont les dépendances confirmées. SQL Server, SQL Server Express, EF Core et un service DB séparé ne sont pas requis. Aucun transport RS-485 physique n'est encore requis par S2.

## 11. Hors périmètre après S2

Restent ouverts : configuration durable bus/endpoints, composition finale du service, cadences chiffrées, transport Modbus RTU physique, adaptateur RS-485, API/Web UI, authentification/rôles, journal opérateur, write path B4, provisioning adresse, import/parser SD, stockage brut SD, FFT, rétention, sauvegarde/export, chiffrement, synchronisation analytique/Grafana, packaging Windows final et qualification de coupure d'alimentation réelle.

## 12. Règle de poursuite

Toute évolution future du schéma doit incrémenter explicitement `user_version`, fournir une migration ordonnée, préserver ou arbitrer explicitement les invariants S2, ajouter des tests de migration/réouverture et mettre à jour les documents de schéma et prérequis PC concernés.
