# Projet MSM — Capteur de vibration TR2

## S7-A — Architecture Web minimale de la supervision

Date : 2026-09-09

Ce document formalise l'arbitrage **S7-A — architecture Web / frontières / choix technique minimal** après le gel S6.

Il complète les gels S0 à S6. Il ne modifie ni la spécification Modbus RTU V1, ni les politiques firmware, ni les invariants transport/runtime déjà gelés.

Toute décision ci-dessous est une `SUPERVISION_POLICY` ou `UX_POLICY` sauf mention contraire.

## 1. Baseline

Baseline S6 de départ :

`414649ef745e25edbeebab457e54c1499ac33881`

`Supervision: freeze S6 web UX`

Référence UX :

`603984e4afe2176c005619584c88f6478a82ab83`

`Supervision: add S6-H navigable web mockup`

Maquette :

`Modbus RTU/Supervision TR2/ux/s6-h/`

La maquette S6-H reste la référence visuelle et fonctionnelle UX. Elle ne constitue pas une architecture Web de production.

## 2. Constat d'architecture au démarrage de S7

La solution .NET 10 courante contient :

```text
TR2.Domain
TR2.Protocol
TR2.Transport
TR2.Application
TR2.Persistence
TR2.Campaigns
TR2.Supervision.Service
```

Aucun projet Web de production n'existe au démarrage de S7.

Le runtime physique courant est construit autour d'une instance unique de `PhysicalSupervisionRuntime`, qui regroupe :

```text
Composition
Operations
Host
```

La composition existante possède déjà les autorités nécessaires à l'alimentation future de l'IHM, notamment :

- `FleetRegistry` ;
- `DeviceTelemetrySnapshotRegistry` ;
- `CommandCoordinatorRegistry` ;
- `SqliteB3ArchiveSink` ;
- `SqliteCommunicationJournalSink` ;
- `SqliteEquipmentModelStore` ;
- `RuntimeReadinessGate` ;
- `ModbusBusConnectionManager` ;
- la configuration runtime.

S7 ne crée pas une seconde composition équivalente pour l'IHM.

## 3. Choix du host Web

Le host HTTP de production S7 repose sur **ASP.NET Core sous .NET 10**.

Le choix vise :

- cohérence avec la solution .NET 10 existante ;
- absence de runtime serveur externe ;
- facilité de déploiement local/offline ;
- testabilité HTTP ;
- possibilité de packaging Windows ultérieur ;
- intégration naturelle avec le processus de supervision.

S7 n'introduit pas de framework serveur non-.NET.

## 4. Frontend

Le frontend S7 reste basé sur :

```text
HTML
CSS
JavaScript natif
```

La maquette S6-H est reprise comme base visuelle et progressivement raccordée au backend réel.

S7-A n'introduit pas :

- React ;
- Angular ;
- Vue ;
- Blazor ;
- Node.js ;
- npm ;
- bundler frontend ;
- pipeline de build JavaScript distinct.

L'introduction ultérieure d'un framework frontend nécessitera un besoin démontré et un nouvel arbitrage.

## 5. Projet Web

S7 prévoit un projet dédié :

```text
TR2.Supervision.Web
```

Ce projet porte uniquement les responsabilités Web :

- host HTTP ;
- fichiers statiques de l'IHM ;
- endpoints HTTP ;
- sérialisation des contrats IHM ;
- adaptation des erreurs HTTP ;
- politique réseau Web.

Le projet Web ne devient pas propriétaire :

- du transport Modbus ;
- de NModbus ;
- du polling ;
- des registres B0 à B7 ;
- des règles transactionnelles B5 ;
- de la persistance transactionnelle ;
- de l'identité durable des équipements.

## 6. Runtime unique

Il doit exister **une seule instance autoritative du runtime de supervision** pour un processus de supervision donné.

Le Web consomme cette instance et ses services/projections ; il n'instancie pas une seconde chaîne :

```text
Web -> Protocol -> Transport -> RS-485
```

Le navigateur ne possède aucun accès Modbus direct.

Le propriétaire unique du bus, du polling, des sessions et des opérations B5 reste le moteur de supervision existant.

S7-A interdit donc :

- une seconde `PhysicalSupervisionRuntime` dédiée au Web ;
- un second `ModbusBusConnectionManager` dédié au Web ;
- un polling Web parallèle ;
- une écriture B5 directe depuis un endpoint HTTP ;
- une source d'état en mémoire dupliquant sans nécessité les autorités existantes.

## 7. Frontière de lecture IHM

Les types domaine, protocole, transport ou persistance ne seront pas exposés directement comme contrat HTTP public.

S7 introduira en S7-B des projections/DTO adaptés à l'IHM.

Ces projections :

- dérivent exclusivement d'autorités existantes ;
- ne deviennent pas une nouvelle autorité métier ;
- ne contiennent aucune adresse de registre ;
- ne contiennent aucune séquence Modbus ;
- ne contiennent aucune clé de confirmation B5 ;
- ne doivent pas inventer de sémantique `NOT_DEFINED V1`.

Aucun `WebStateCache` générique n'est créé en S7-A.

## 8. Frontière de commande B5

Toute future commande Web doit passer par l'autorité transactionnelle existante.

La chaîne cible est conceptuellement :

```text
Navigateur
  -> endpoint HTTP
  -> service/projection applicative Web
  -> SupervisionOperationalFacade / CommandCoordinator
  -> BusWorkScheduler
  -> moteur de supervision
  -> Modbus RTU
```

Le Web ne connaît ni `transaction_id` technique à générer manuellement, ni clé B5, ni ordre des écritures Modbus.

Les invariants S6 restent applicables :

- une seule transaction non terminale par `device_id` ;
- `Prepared`, `Submitted`, `Ambiguous` distincts des états B5 observés ;
- aucun replay automatique ;
- aucune résolution Web forcée de `Ambiguous` ;
- aucune reconnexion assimilée à une résolution ;
- persistance de l'ambiguïté à travers redémarrage ;
- `RESET_STATISTICS` absent avec le scope firmware courant.

## 9. Politique réseau HTTP

S7-A introduit la politique suivante :

- protocole initial : **HTTP** ;
- écoute par défaut : **loopback uniquement** ;
- exposition LAN : **explicitement configurable** ;
- l'exposition sur toutes les interfaces ne constitue jamais la valeur implicite par défaut ;
- UI et API sont servies en **same-origin** ;
- **CORS non activé par défaut** ;
- aucune hypothèse d'accès Internet n'est faite.

L'adresse ou les interfaces d'écoute exactes seront portées par la configuration runtime Web lors de l'implémentation correspondante.

## 10. Authentification et conséquences de sécurité

S7-A n'introduit pas encore :

- comptes utilisateurs ;
- authentification ;
- ACL ;
- rôles logiciels ;
- certificats ;
- HTTPS obligatoire.

Les personas S6 restent des personas UX et non des rôles de sécurité.

Conséquence explicite : lorsqu'un host sans authentification est volontairement exposé au LAN, tout client capable d'atteindre ce host pourra consulter les endpoints exposés et, lorsque S7-G existera, solliciter les commandes Web autorisées par cette API.

L'activation de l'écoute LAN est donc une décision opérateur explicite et non un défaut silencieux.

L'ajout futur d'authentification, de rôles ou de HTTPS fera l'objet d'un arbitrage de sécurité dédié.

## 11. Fichiers statiques et maquette S6-H

S6-H est conservée comme référence UX.

Son HTML/CSS/JavaScript pourra être déplacé ou intégré aux fichiers statiques du projet Web lors de S7-I, à condition de préserver les parcours et invariants validés.

Les fixtures S6-H restent explicitement non normatives jusqu'à leur remplacement par les données réelles.

Une fixture ne doit jamais être présentée comme provenant d'un TR2 physique ou comme une preuve de validation matérielle.

## 12. Données et fraîcheur

S7 conserve les autorités existantes pour les dernières observations.

En particulier, la perte de communication ne doit pas provoquer la remise à zéro artificielle des dernières valeurs connues.

Les contrats IHM devront préserver séparément :

- disponibilité/communication PC ;
- instant de réception PC ;
- fraîcheur de réception PC ;
- validité/fraîcheur définie par le TR2 lorsqu'elle existe ;
- valeur B3 réellement reçue.

Aucune projection Web ne doit transformer implicitement une dernière valeur connue en valeur actuelle.

## 13. Déploiement cible

S7-A vise un déploiement simple et local :

```text
processus de supervision .NET
+ host ASP.NET Core
+ fichiers statiques Web
+ base SQLite
+ fichier(s) de configuration
```

S7-A ne fige pas encore :

- Windows Service ;
- installer ;
- self-contained publish ;
- port HTTP définitif ;
- nom DNS local ;
- reverse proxy ;
- service discovery.

Ces points seront traités uniquement lorsqu'ils deviennent nécessaires.

## 14. Testabilité

L'architecture Web doit rester testable sans matériel réel.

Les tests pourront utiliser :

- runtime composé avec doubles de transport ;
- fixtures déterministes ;
- host HTTP de test ;
- SQLite temporaire.

Aucun test Web pré-matériel ne qualifie le RS-485, le STM32 ou le capteur physique.

## 15. Décomposition S7 retenue

La décomposition retenue après gap analysis est :

```text
S7-A  Architecture Web + politique d'exposition HTTP
S7-B  Contrats/projections IHM read-only
S7-C  Host HTTP + API read-only minimale
S7-D  Vue générale réelle
S7-E  Détail TR2 / vibration / diagnostic
S7-F  Campagnes / système / communications
S7-G  Write path B5 via autorité existante
S7-H  Ambiguous / erreurs / reconnexion
S7-I  Finalisation raccordement UX S6-H
S7-J  Intégration / concurrence multi-clients
S7-K  Audit transversal + gel
```

Cette décomposition reste une organisation de travail ; elle ne modifie pas les frontières normatives V1.

## 16. Hors périmètre S7-A

S7-A ne crée encore :

- aucun projet `.csproj` Web ;
- aucun endpoint HTTP ;
- aucun DTO ;
- aucune commande réelle ;
- aucune modification du polling ;
- aucune modification B5 ;
- aucune modification firmware ;
- aucune fonctionnalité B4 Web ;
- aucun import de données campagne ;
- aucune FFT ;
- aucune analyse vibratoire supplémentaire ;
- aucune qualification matérielle.

## 17. Suite

Après validation de S7-A, la tranche suivante est :

**S7-B — contrats de lecture IHM / DTO / projections applicatives read-only**.

S7-B devra définir les contrats nécessaires à la vue parc et au détail TR2 en réutilisant les autorités déjà présentes, avant de créer le host HTTP réel en S7-C.
