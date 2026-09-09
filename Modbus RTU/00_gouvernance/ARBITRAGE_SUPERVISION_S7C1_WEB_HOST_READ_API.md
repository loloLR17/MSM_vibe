# Projet MSM — Capteur de vibration TR2

## S7-C1 — Socle ASP.NET Core et API read-only minimale

Date : 2026-09-09

Baseline de départ :

`8e789d5645f296a85274483a16016a2c1141c1dd`

`Supervision: add S7-B read projections`

Cette tranche crée le socle Web de production prévu par S7-A sans le raccorder encore au cycle de vie du processus `TR2.Supervision.Service`.

Toute décision de cette tranche est une `SUPERVISION_POLICY` / `UX_POLICY`. Elle ne modifie ni la spécification Modbus RTU V1, ni le firmware, ni B5.

## 1. Nouveau projet Web

Le projet suivant est créé :

`src/TR2.Supervision.Web/TR2.Supervision.Web.csproj`

Il dépend uniquement de `TR2.Application` et du framework partagé `Microsoft.AspNetCore.App`.

Il ne référence directement ni `TR2.Protocol`, ni `TR2.Transport`, ni `TR2.Persistence`, ni `TR2.Campaigns`.

Le test d'architecture est étendu afin de figer cette frontière.

## 2. Host HTTP

`SupervisionWebHost` construit un host ASP.NET Core minimal.

S7-C1 n'introduit :

- aucun framework frontend ;
- aucun Node.js/npm ;
- aucun fichier statique de production ;
- aucun CORS ;
- aucune authentification ;
- aucun HTTPS ;
- aucune commande HTTP d'écriture.

## 3. Politique d'écoute

`SupervisionWebOptions` impose :

- une URI HTTP absolue ;
- aucune route, query string, fragment ou user-info dans l'URI d'écoute ;
- une écoute loopback autorisée sans option supplémentaire ;
- toute écoute non-loopback interdite tant que `allowRemote=true` n'a pas été fourni explicitement.

Cette règle matérialise l'invariant S7-A : l'exposition LAN ne doit jamais devenir implicite.

S7-C1 ne choisit pas encore de port de production ni de valeur de configuration runtime. Le branchement de ces options dans le fichier de configuration du service relève de S7-C2.

## 4. API read-only minimale

Un seul endpoint applicatif est créé :

`GET /api/v1/fleet`

Il produit :

- `observedAt`, horodatage PC de construction de la réponse ;
- `devices`, résultat courant de `SupervisionReadProjection.GetFleet(observedAt)`.

Les enums de présentation sont sérialisées sous forme de chaînes lisibles.

Aucun type Modbus brut ni adresse de registre n'est exposé.

`POST /api/v1/fleet` n'est pas accepté.

## 5. Autorités

L'API ne possède aucun cache Web et aucune copie autoritative de l'état.

Elle consomme exclusivement `SupervisionReadProjection`, donc indirectement les autorités S7-B existantes.

Le navigateur n'accède jamais au bus Modbus.

## 6. Tests

Les tests S7-C1 démarrent un vrai host Kestrel pré-matériel sur loopback avec port dynamique de test.

Ils vérifient :

- rejet d'une écoute distante sans opt-in explicite ;
- réponse HTTP réelle de `GET /api/v1/fleet` ;
- horodatage `observedAt` déterministe ;
- sérialisation de l'identité, de l'état de session, de la fraîcheur et d'une valeur B3 projetée ;
- refus de `POST /api/v1/fleet`.

Ces tests ne qualifient ni RS-485, ni STM32, ni capteur physique.

## 7. Hors périmètre

S7-C1 ne raccorde pas encore le host Web au processus console de production.

Il ne définit pas encore :

- la section `web` du fichier de configuration runtime ;
- le port opérationnel ;
- les seuils de fraîcheur PC opérationnels ;
- le démarrage/arrêt coordonné Web + runtime ;
- l'exposition LAN réelle ;
- les pages S6-H connectées aux données réelles.

## 8. Suite

Après validation locale de S7-C1 :

**S7-C2 — configuration Web + branchement au processus autoritatif unique**, avec traitement explicite de la concurrence lecture Web / mise à jour runtime avant activation du host en production.
