# Projet MSM — Capteur de vibration TR2

## Supervision TR2 — S2 : schéma de persistance locale durable

Date : 2026-09-09

Statut : **référence documentaire de clôture S2**

Ce document décrit le schéma SQLite effectivement implémenté à l'issue de S2. Il ne crée aucune sémantique métier nouvelle : il documente les décisions S2-A et les implémentations S2-B à S2-G présentes sur `main`.

---

## 1. Moteur et politique d'ouverture

La persistance structurée locale utilise :

- SQLite ;
- `Microsoft.Data.Sqlite` ;
- SQL explicite / ADO.NET ;
- base locale au PC de supervision ;
- `journal_mode=WAL` ;
- `synchronous=FULL` ;
- `foreign_keys=ON` ;
- `busy_timeout` configurable, valeur par défaut actuelle : 5000 ms ;
- migrations ordonnées via `PRAGMA user_version`.

La version courante du schéma à la clôture de S2 est :

```text
user_version = 5
```

Une version supérieure à celle supportée doit provoquer un refus explicite d'ouverture en écriture. Aucune migration destructive silencieuse n'est autorisée.

---

## 2. Version 1 — marqueur de schéma

Table : `tr2_schema_marker`

Rôle : matérialiser l'initialisation d'une base TR2.

Champs :

- `singleton` : clé primaire, valeur imposée à 1 ;
- `created_utc` : timestamp de création.

---

## 3. Version 2 — transactionnel B5

### 3.1 `b5_transaction_reservation`

Autorité durable de la dernière allocation de `transaction_id` par `DeviceId`.

Champs :

- `device_id` : clé primaire, plage uint32 ;
- `transaction_id` : 1..65535 ;
- `persisted_utc`.

Invariant : la persistance de l'allocation doit être commitée avant qu'un submit Modbus correspondant puisse devenir possible.

La table ne remplace pas la politique Application : elle persiste l'allocation, mais la règle lifetime-strict et l'incrément restent sous autorité du `CommandCoordinator`.

### 3.2 `b5_transaction_journal`

Journal append-only des transitions nécessaires au recovery supervision.

Champs :

- `journal_id` : clé primaire autoincrémentée ;
- `device_id` ;
- `transaction_id` ;
- `request_identity` ;
- `event_kind` ;
- `observed_utc`.

Valeurs S2 de `event_kind` :

- `Prepared` ;
- `Submitted` ;
- `Ambiguous` ;
- `TerminalEvidenceObserved`.

Index : journal ordonné par device puis `journal_id`.

Invariant de recovery : une transaction non terminale reconstruite après restart est restaurée en état `Ambiguous`; aucun replay automatique n'est déclenché.

---

## 4. Version 3 — archive B3

Table : `b3_archive`

Rôle : archive append-only des observations B3 publiées par la supervision.

Chaque ligne contient :

- `device_id` durable ;
- timestamp PC `received_utc` ;
- ensemble complet des champs B3 V1 utilisés par `B3VibrationSupervision` ;
- contexte B2 optionnel ;
- timestamp PC de réception B2 optionnel.

L'absence de contexte B2 est représentée par SQL `NULL`; aucune heure TR2 n'est inventée.

Index : `(device_id, observation_id)`.

Invariants :

- append-only dans S2 ;
- aucune écriture Modbus associée ;
- aucune donnée brute de campagne SD dans cette table ;
- un échec de stockage B3 reste une erreur de persistance, pas une erreur de communication Modbus.

---

## 5. Version 4 — journal des échecs de communication

Table : `communication_failure_journal`

Champs :

- `failure_id` : clé primaire autoincrémentée ;
- `bus_id` ;
- `modbus_address` ;
- `device_id` nullable ;
- `operation` ;
- `observed_utc` ;
- `exception_type` ;
- `message`.

Valeurs actuellement admises de `operation` :

- `Polling` ;
- `ExplicitRefresh`.

L'identité `device_id` reste nullable lorsqu'elle n'est pas encore connue. La persistance ne fabrique jamais un `DeviceId`.

Index :

- `(bus_id, modbus_address, failure_id)` ;
- `(device_id, failure_id)` pour les lignes identifiées.

Invariant : une défaillance du sink de journalisation ne doit pas être requalifiée en panne Modbus ni masquer l'erreur de communication originale dans les chemins Application qui l'utilisent en best-effort.

---

## 6. Version 5 — modèle équipements et affectations

### 6.1 `installation`

- `installation_id` : clé primaire texte non vide ;
- `name` : texte non vide.

### 6.2 `equipment`

- `equipment_id` : clé primaire texte non vide ;
- `installation_id` : FK vers `installation` ;
- `name` : texte non vide.

### 6.3 `measurement_point`

- `measurement_point_id` : clé primaire texte non vide ;
- `equipment_id` : FK vers `equipment` ;
- `name` : texte non vide.

### 6.4 `equipment_assignment`

Historique d'affectation d'un TR2 à un point de mesure.

Champs :

- `assignment_id` : clé primaire autoincrémentée ;
- `device_id` : uint32 ;
- `measurement_point_id` : FK vers `measurement_point` ;
- `valid_from_utc` ;
- `valid_to_utc` nullable.

Invariants :

- `valid_to_utc` nul signifie affectation active ;
- une clôture doit être strictement postérieure à `valid_from` ;
- au plus une affectation active par `device_id` ;
- au plus une affectation active par `measurement_point_id` ;
- `Move` ferme l'affectation précédente puis crée une nouvelle ligne ;
- `Unassign` ferme la ligne active ;
- l'historique fermé n'est pas réécrit en une autre affectation.

S2 n'introduit aucune liaison implicite entre ce modèle et B4.

---

## 7. Timestamps

Les timestamps de supervision persistés par S2 sont sérialisés en UTC au format round-trip `O` lorsque les stores les produisent depuis `DateTimeOffset`.

Ils doivent rester distingués des valeurs temporelles propres au TR2, notamment celles de B2.

Une date PC et une date TR2 ne deviennent jamais implicitement la même autorité temporelle.

---

## 8. Transactions SQLite

Les écritures significatives S2 utilisent des transactions SQLite locales courtes.

Une transaction SQLite ne doit jamais englober :

- une transaction Modbus physique ;
- une attente opérateur ;
- une temporisation de polling ;
- un accès réseau externe ;
- un traitement analytique long.

`System.Transactions` n'est pas utilisé.

---

## 9. Recovery prouvé à l'issue de S2

Les tests host réouvrent de vraies bases SQLite temporaires et prouvent notamment :

- base vide sans état fantôme ;
- reconstruction du dernier `transaction_id` réservé ;
- reconstruction B5 depuis le journal durable ;
- transaction B5 non terminale restaurée `Ambiguous` ;
- absence de réutilisation du `transaction_id` après restart ;
- persistance simultanée de B3, journal communication et affectations équipements ;
- état partiel contrôlé « réservation B5 présente / journal vide » : aucune transaction active n'est inventée et l'allocation suivante continue après la réservation durable.

Ces tests démontrent un restart logiciel / fermeture-réouverture de la base. Ils ne constituent pas une qualification physique d'une coupure d'alimentation pendant une écriture disque.

---

## 10. Hors périmètre S2

Restent explicitement hors S2 :

- transport Modbus RTU physique ;
- choix adaptateur RS-485 ;
- configuration durable des bus/endpoints de production ;
- composition finale du service ;
- API / Web UI / authentification / rôles ;
- journal métier/opérateur ;
- write path B4 ;
- import des campagnes SD ;
- stockage des fichiers bruts SD ;
- FFT / analyse avancée ;
- politique de rétention ;
- sauvegarde/export opérationnel final ;
- chiffrement de la base ;
- synchronisation analytique / Grafana ;
- packaging Windows final.

Ces sujets doivent être arbitrés dans leurs tranches dédiées et ne doivent pas être déduits du schéma S2.
