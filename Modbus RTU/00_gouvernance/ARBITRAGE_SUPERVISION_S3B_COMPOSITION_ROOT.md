# Projet MSM — Capteur de vibration TR2

## Arbitrage S3-B — Composition root host de la supervision

Date : 2026-09-09

Cette tranche complète S3-A sans modifier la spécification Modbus RTU V1, l'architecture firmware ou les gels S0/S1/S2.

## 1. Objet

S3-B introduit le point de composition du host de supervision. Son rôle est de construire une seule fois les autorités runtime déjà définies par les couches Application et Persistence, à partir de la `RuntimeConfiguration` validée en S3-A.

## 2. Autorités composées

La composition S3-B crée et expose une instance partagée de :

- `SqliteDatabase` ;
- `SqliteCommandTransactionReservationStore` ;
- `SqliteCommandTransactionJournal` ;
- `SqliteB3ArchiveSink` ;
- `SqliteCommunicationJournalSink` ;
- `SqliteEquipmentModelStore` ;
- `FleetRegistry` ;
- `CommandCoordinatorRegistry` ;
- `DeviceTelemetrySnapshotRegistry` ;
- `BusWorkScheduler`.

Tous les stores SQLite partagent la même instance logique de `SqliteDatabase`.

## 3. Enregistrement de flotte

Les endpoints configurés en S3-A sont enregistrés dans le `FleetRegistry` pendant la composition.

L'état initial reste celui défini par S1 : session `Unidentified`. Aucun `device_id` n'est inventé depuis l'adresse Modbus.

## 4. Invariant de pure composition

`SupervisionRuntimeCompositionRoot.Compose(...)` ne doit :

- ni ouvrir SQLite ;
- ni créer/migrer la base ;
- ni effectuer de communication Modbus ;
- ni lancer de polling ;
- ni restaurer de transaction B5 ;
- ni démarrer de tâche de fond.

L'ouverture/migration SQLite et la reconstruction durable B5 appartiennent à S3-C.

## 5. Dépendances

Aucun conteneur DI externe n'est introduit. La composition est explicite en code .NET et réutilise les constructeurs existants.

Aucune nouvelle dépendance NuGet n'est ajoutée.

## 6. Classification

- composition root : `SUPERVISION_POLICY` / implémentation host ;
- autorités Application et invariants Fleet/B5 : architecture S1 gelée ;
- persistance SQLite : politique S2 gelée ;
- transport physique RS-485 : hors S3-B ;
- lifecycle/startup : S3-C/S3-D ;
- polling actif : S3-E ;
- Web/API/Windows Service : hors S3-B.

## 7. Preuves S3-B

Les tests de service vérifient que :

- tous les endpoints configurés sont enregistrés ;
- les sessions initiales restent `Unidentified` ;
- aucun coordinateur B5 fantôme n'est créé ;
- les autorités partagées existent ;
- la composition seule ne crée pas le fichier SQLite.
