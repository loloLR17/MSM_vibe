# Projet MSM — Capteur de vibration TR2

## S7-E1 — Fondation read-only détail TR2 et diagnostic B7

Date : 2026-09-09

Baseline de départ :

`5cc8e9ef238a9ebc148496a5407078b24a1826d3`

`Supervision: fix S7-D static web assets`

Cette tranche prépare le détail TR2 S7-E sans introduire de commande Web. Elle raccorde le diagnostic B7 déjà lu par le polling Medium à l'autorité de télémétrie existante, le projette vers l'IHM et expose une lecture HTTP par `device_id`.

Toute décision de présentation est une `SUPERVISION_POLICY` / `UX_POLICY`. Les valeurs et codes B7 restent ceux de la spécification Modbus RTU V1.

## 1. Constat de départ

Le protocole et `PollingExecutor` savent déjà lire le Bloc 7 :

- `B7Reader` et `B7DiagnosticState` existent ;
- le groupe `PollingGroup.Medium` lit B2 puis B7 ;
- `PollingReadSet.B7` transporte le résultat.

Cependant, `PhysicalPollingWorkRunner.ApplyReadSet` ne publiait jusque-là que B1, B2 et B3 dans `DeviceTelemetrySnapshotRegistry`.

Le snapshot B7 était donc lu physiquement puis perdu après le cycle de polling.

S7-E1 corrige cette lacune de flux sans créer de cache ou d'autorité parallèle.

## 2. Autorité de télémétrie

`DeviceTelemetrySnapshots` est étendu avec :

`ObservedSnapshot<B7DiagnosticState> DiagnosticState`

Les règles déjà appliquées à B1/B2/B3 restent valables :

- `NeverReceived` avant la première lecture ;
- conservation de la dernière valeur reçue ;
- date `ReceivedAt` côté PC ;
- `MarkUnavailable()` conserve la valeur et la marque indisponible.

`DeviceTelemetrySnapshotRegistry` reste l'unique autorité en mémoire de ces derniers snapshots et conserve sa synchronisation interne.

## 3. Publication du polling B7

Lorsque `PollingReadSet.B7` est non nul et que la session est `Compatible`, `PhysicalPollingWorkRunner` publie B7 dans `DeviceTelemetrySnapshotRegistry` avec le même `observedAt` que le cycle.

Aucune lecture Modbus supplémentaire n'est créée par le Web.

La cadence B7 reste donc celle définie par le polling Medium existant.

## 4. Projection IHM B7

Le read model ajoute `IhmDiagnosticStateReadModel` avec les champs V1 du Bloc 7 :

- version de structure ;
- état global de santé ;
- flags défaut ;
- dernier code défaut et timestamp ;
- état/résultat/détail SELFTEST ;
- uptime ;
- cause reset ;
- température interne ;
- tension alimentation.

`IhmDeviceTelemetryReadModel` expose ce diagnostic avec le même conteneur `IhmObservedValue<T>` que les autres snapshots.

La projection ne fusionne pas B1 et B7 et ne cherche pas à forcer une égalité entre leurs flags : la V1 n'impose pas de dérivation exhaustive entre ces blocs.

## 5. Lecture d'un TR2 par identité durable

`SupervisionReadProjection` fournit une lecture par `device_id`.

La résolution s'appuie sur le parc déjà projeté et sur l'invariant existant qui interdit deux sessions compatibles avec le même `device_id`.

Le port COM et l'adresse Modbus ne deviennent jamais l'identité durable du TR2.

## 6. API HTTP read-only

S7-E1 ajoute :

`GET /api/v1/devices/{deviceId}`

Réponse :

- `observedAt` ;
- le même `IhmDeviceReadModel` que la vue parc, enrichi du diagnostic B7.

Un `device_id` inconnu retourne `404 Not Found`.

Aucun endpoint `POST`, `PUT`, `PATCH` ou `DELETE` n'est ajouté.

## 7. Sémantique B7

Les codes B7 restent normatifs et distincts des politiques PC :

- `system_health_status` : 0 OK, 1 Warning, 2 Dégradé, 3 Critique ;
- `selftest_status` : 0 jamais exécuté, 1 en cours, 2 OK, 3 échec ;
- température en 0,1 °C ;
- tension en mV.

S7-E1 transporte les valeurs numériques sans inventer de nouveau niveau de santé ou score.

Les libellés opérateur de l'écran détaillé seront câblés dans la tranche UI suivante à partir de ces codes normatifs.

## 8. Indisponibilité communication

Une perte de communication continue d'appeler `MarkUnavailable()` sur l'autorité de télémétrie.

B7 suit désormais la même règle que B1/B2/B3 :

- dernière valeur B7 conservée si elle existe ;
- `IsAvailable=false` ;
- fraîcheur PC `Unavailable` ;
- aucune remise à zéro artificielle.

## 9. Tests

S7-E1 couvre :

- stockage/projection d'un B7 reçu ;
- conservation de B7 lors d'une indisponibilité ;
- publication réelle du B7 lu par le polling Medium ;
- lecture HTTP d'un device connu ;
- `404` pour un `device_id` inconnu ;
- maintien du caractère read-only de l'API.

Ces tests sont pré-matériel et ne qualifient pas le RS-485 physique ni le STM32.

## 10. Hors périmètre

S7-E1 ne réalise pas encore :

- écran détaillé complet et navigation S7-E2 ;
- projection B4 de configuration ;
- historique vibratoire ;
- campagnes B6 ;
- diagnostic de communication PC détaillé ;
- commandes B5 ;
- parcours `Ambiguous` ;
- authentification ou HTTPS.

## 11. Suite

Après validation locale de S7-E1 :

**S7-E2 — écran détail TR2 / vibrations / diagnostic**, alimenté uniquement par les projections read-only validées.