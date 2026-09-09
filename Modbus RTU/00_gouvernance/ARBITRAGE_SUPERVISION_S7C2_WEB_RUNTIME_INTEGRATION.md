# Projet MSM — Capteur de vibration TR2

## S7-C2 — Configuration Web et branchement au runtime autoritatif unique

Date : 2026-09-09

Baseline de départ :

`b2e859821e2da065aaf59f87aee259ea8ee18a03`

`Supervision: fix S7-C1 web host cancellation`

Cette tranche raccorde le host Web S7-C1 au processus de supervision existant sans créer de second runtime Modbus.

Toute décision ci-dessous est une `SUPERVISION_POLICY` / `UX_POLICY`. La spécification Modbus RTU V1, le firmware et les règles B5 restent inchangés.

## 1. Runtime unique

`TR2.Supervision.Service` reste l'unique propriétaire du processus et de `PhysicalSupervisionRuntime`.

Lorsque le Web est activé, `SupervisionReadProjection` reçoit directement :

- le `FleetRegistry` de la composition existante ;
- le `DeviceTelemetrySnapshotRegistry` de la même composition ;
- la politique de fraîcheur PC issue de la configuration Web.

Aucune seconde composition, aucun second `ModbusBusConnectionManager`, aucun polling Web et aucun cache Web autoritatif ne sont créés.

## 2. Configuration Web

La configuration runtime accepte une section optionnelle `web` :

```json
{
  "web": {
    "enabled": true,
    "listenUri": "http://127.0.0.1:5080",
    "allowRemote": false,
    "freshnessAgingAfterMilliseconds": 5000,
    "freshnessStaleAfterMilliseconds": 30000
  }
}
```

Si la section est absente :

- `enabled=false` ;
- `listenUri=http://127.0.0.1:5080` ;
- `allowRemote=false` ;
- fraîcheur PC : `Aging` après 5 s, `Stale` après 30 s.

Le point essentiel est que l'absence de configuration Web n'ouvre aucun port HTTP.

Les seuils 5 s / 30 s sont des valeurs `SUPERVISION_POLICY` configurables ; ils ne décrivent pas une validité TR2 et ne modifient aucune sémantique B3.

## 3. Exposition réseau

La validation S7-C1 reste applicable :

- HTTP uniquement dans cette tranche ;
- loopback autorisé par défaut ;
- toute URI non-loopback exige `allowRemote=true` ;
- pas de CORS ;
- pas d'authentification ;
- pas d'exposition Internet supposée.

Une configuration historique sans section `web` ne devient donc jamais accessible sur le réseau par effet de bord.

## 4. Cycle de vie

Quand `web.enabled=false`, le comportement du service reste celui de S5/S6 : seul le runtime de supervision s'exécute.

Quand `web.enabled=true` :

1. le runtime physique unique est construit et démarré ;
2. son état autoritatif est utilisé pour construire `SupervisionReadProjection` ;
3. le host Web S7-C1 est démarré dans le même processus ;
4. runtime et Web partagent un token d'arrêt coordonné ;
5. la défaillance inattendue d'un composant provoque l'arrêt de l'autre et remonte comme `Runtime failure`.

L'arrêt opérateur reste propre via le token de cancellation existant.

## 5. Concurrence lecture Web / polling

Avant activation du host de production, deux autorités en mémoire nécessitaient une protection explicite :

- `FleetRegistry` ;
- `DeviceTelemetrySnapshotRegistry`.

S7-C2 protège leurs accès par synchronisation interne.

`FleetRegistry.Sessions` renvoie désormais un snapshot de collection et non la vue vivante du `Dictionary` interne.

`DeviceTelemetrySnapshotRegistry` sérialise ses lectures et mises à jour, chaque valeur stockée restant un snapshot immuable.

Le Web peut ainsi projeter les données pendant que le polling met à jour sessions et télémétrie sans énumérer simultanément un `Dictionary` en mutation.

Cette synchronisation ne crée aucune nouvelle autorité et ne modifie pas les règles métier des registres.

## 6. Dépendances

`TR2.Supervision.Web` continue de dépendre uniquement de `TR2.Application`.

`TR2.Supervision.Service` référence désormais `TR2.Supervision.Web` pour démarrer le host HTTP dans le processus autoritatif.

Il n'existe aucune dépendance inverse Web -> Service et donc aucun cycle.

## 7. API

S7-C2 ne rajoute aucun endpoint.

Le seul endpoint applicatif reste :

`GET /api/v1/fleet`

Aucun write path B5 n'est introduit.

## 8. Tests

Les tests S7-C2 couvrent :

- Web désactivé par défaut quand la section est absente ;
- parsing explicite d'une configuration loopback ;
- rejet d'une écoute distante sans `allowRemote=true` ;
- cohérence des seuils de fraîcheur ;
- lecture de la projection pendant des mises à jour concurrentes du parc et de la télémétrie ;
- frontières de dépendances projet mises à jour.

Les tests S7-C1 continuent de couvrir le host Kestrel et `GET /api/v1/fleet`.

Aucun test pré-matériel ne qualifie RS-485, STM32 ou le capteur physique.

## 9. Hors périmètre

S7-C2 ne réalise pas encore :

- la vue générale UX réelle S7-D ;
- les pages de détail S7-E ;
- campagnes/système/communications S7-F ;
- le write path B5 S7-G ;
- les parcours Ambiguous S7-H ;
- le raccordement final des fichiers statiques S6-H S7-I ;
- l'authentification ou HTTPS.

## 10. Suite

Après validation locale de S7-C2, la tranche suivante est :

**S7-D — Vue générale réelle**, alimentée par l'API read-only désormais raccordée au runtime unique.
