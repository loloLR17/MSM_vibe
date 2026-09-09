# Projet MSM — Capteur de vibration TR2

## S7-B — Contrats et projections IHM read-only

Date : 2026-09-09

Ce document formalise la tranche **S7-B — contrats/projections IHM read-only** après validation locale de S7-A.

Baseline de départ :

`1bdd6b0f71bd1eee7d0bb7cf1f03c89013237c6a`

`Supervision: arbitrate S7-A web architecture`

S7-B ne modifie ni la spécification Modbus RTU V1, ni le firmware, ni le transport, ni le polling, ni B5.

Toute décision de présentation ci-dessous est une `SUPERVISION_POLICY` / `UX_POLICY`.

## 1. Objet

S7-B crée une frontière de lecture destinée à la future IHM Web sans créer encore de host HTTP.

La projection s'appuie exclusivement sur les autorités runtime existantes :

- `FleetRegistry` pour les sessions/endpoints ;
- `DeviceTelemetrySnapshotRegistry` pour les dernières observations B1/B2/B3 ;
- `SnapshotFreshnessPolicy` pour la fraîcheur PC.

Aucun cache Web ni seconde autorité d'état n'est créé.

## 2. Contrats IHM

Les contrats IHM sont portés dans `TR2.Application` afin de rester indépendants d'ASP.NET Core en S7-B.

Ils n'exposent pas directement :

- `TR2Session` ;
- `TR2Endpoint` ;
- `DeviceId` ;
- `B1SystemState` ;
- `B2TimeState` ;
- `B3VibrationSupervision` ;
- des adresses de registres ;
- des détails Modbus/B5.

Les valeurs projetées utilisent des primitives et des types de présentation dédiés.

## 3. Identité et endpoint

Pour chaque session du parc, la projection fournit :

- `BusId` ;
- `ModbusAddress` ;
- `DeviceId` lorsque l'équipement a été identifié ;
- un état de session IHM dédié.

`DeviceId` reste l'identité durable de l'équipement. Le bus et l'adresse décrivent l'endpoint courant et ne deviennent pas une identité durable de remplacement.

Une session non identifiée ou incompatible ne reçoit pas de faux `DeviceId` et ne reçoit pas de télémétrie fabriquée.

## 4. États de session

Les quatre états runtime courants sont projetés explicitement vers `IhmSessionState` :

- `Unidentified` ;
- `Compatible` ;
- `Incompatible` ;
- `Disconnected`.

Le contrat IHM ne sérialise donc pas directement l'enum domaine `TR2SessionState`.

## 5. Observation et fraîcheur PC

Chaque bloc B1/B2/B3 associé à un équipement identifié est projeté sous forme d'observation comprenant :

- `HasValue` ;
- `IsAvailable` ;
- `ReceivedAt` ;
- `Freshness` ;
- `Value`.

La fraîcheur est celle calculée par la politique PC existante et est projetée vers `IhmSnapshotFreshness` :

- `NeverReceived` ;
- `Fresh` ;
- `Aging` ;
- `Stale` ;
- `Unavailable`.

Une observation jamais reçue ne doit pas être présentée comme une valeur disponible : la projection IHM expose alors `HasValue=false`, `IsAvailable=false`, `ReceivedAt=null`, `Freshness=NeverReceived`, `Value=null`.

## 6. Perte de communication

L'invariant S6 est conservé : une perte de communication ne remet pas les dernières valeurs connues à zéro.

Lorsque le registre de télémétrie marque une observation indisponible :

- `HasValue` reste vrai si une valeur avait été reçue ;
- la dernière `Value` est conservée ;
- `ReceivedAt` est conservé ;
- `IsAvailable=false` ;
- `Freshness=Unavailable`.

La future IHM pourra donc distinguer une dernière valeur connue d'une valeur actuellement disponible.

## 7. B1 / B2 / B3

S7-B projette les champs réellement présents dans les types protocole courants sans ajouter d'interprétation métier nouvelle.

En particulier :

- les valeurs B3 restent exprimées selon les unités/propriétés déjà définies (`Mg`, millisecondes, compteurs, flags) ;
- aucun RMS n'est recalculé ;
- aucun seuil, score de santé, vitesse mm/s, FFT, fréquence dominante ou sévérité ISO n'est inventé ;
- aucune valeur brute inconnue n'est transformée silencieusement en libellé métier non défini par V1.

## 8. Déterminisme du parc

`GetFleet` produit un ordre déterministe :

1. `BusId` en ordre ordinal ;
2. adresse Modbus croissante.

Cet ordre est une `SUPERVISION_POLICY` de présentation et ne modifie aucune priorité de bus ni aucun ordre de polling.

## 9. Hors périmètre

S7-B ne crée :

- aucun projet Web ;
- aucun host HTTP ;
- aucun endpoint REST ;
- aucun fichier statique de production ;
- aucune commande B5 ;
- aucun write path ;
- aucun nouveau stockage ;
- aucun cache IHM ;
- aucune lecture directe du bus ;
- aucune projection détaillée B4/B6/B7 au-delà des données B1/B2/B3 déjà présentes dans le snapshot runtime.

Les besoins complémentaires B4/B6/B7 seront raccordés dans les tranches S7-E/S7-F à partir de leurs autorités réelles, sans fabrication anticipée.

## 10. Tests S7-B

Les tests couvrent au minimum :

- ordre déterministe et projection de l'identité/session ;
- absence de valeur fabriquée avant première réception ;
- mapping des valeurs B1/B2/B3 et de l'horodatage PC ;
- conservation de la dernière valeur après perte de communication avec état `Unavailable`.

Ces tests restent pré-matériels et ne qualifient ni RS-485, ni STM32, ni capteur physique.

## 11. Suite

Après validation locale de S7-B, la tranche suivante est :

**S7-C — host ASP.NET Core + API read-only minimale**, consommant `SupervisionReadProjection` sans créer de second runtime.
