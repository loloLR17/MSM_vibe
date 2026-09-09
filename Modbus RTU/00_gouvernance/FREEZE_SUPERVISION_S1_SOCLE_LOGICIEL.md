# Projet MSM — Capteur de vibration TR2

## Gel S1 — Socle logiciel de supervision

Date de gel : 2026-09-09

Ce document clôture la phase **S1 — socle logiciel de supervision TR2** engagée après le gel de cadrage S0.

Il ne remplace ni la spécification Modbus RTU V1, ni les documents d'architecture et d'arbitrage firmware, ni `FREEZE_SUPERVISION_S0_CADRAGE.md`.

En cas de conflit, les documents normatifs Modbus et les gels firmware applicables restent autorités sur le comportement du TR2. Les décisions de supervision restent des décisions PC et ne doivent pas être relues comme des exigences firmware implicites.

---

## 1. Baseline gelée

Le dernier état logiciel S1 compilé et validé localement avant ce gel documentaire est :

`47f6a264b1024a8d6d54a78ff79cae8ea0ec3872`

Commit :

`Supervision: test S1 static B0 fleet publication closure`

Cet état inclut le correctif transversal final de publication du résultat de polling STATIC B0 dans le `FleetRegistry`, avec test du remplacement d'un capteur par un autre `device_id` au même endpoint.

La validation locale utilisateur correspondante est verte avec :

```bash
git pull --ff-only origin main
cd "Modbus RTU/Supervision TR2"
dotnet build TR2.Supervision.sln --no-restore
dotnet test TR2.Supervision.sln --no-build
```

Le commit créé par le présent document est documentaire et ne modifie pas le comportement logiciel gelé ci-dessus.

---

## 2. Nature de S1

S1 gèle un **socle logiciel**, pas une supervision produit complète.

S1 a pour objet de rendre explicites, testables et cohérentes les autorités logiques nécessaires à la future supervision multi-TR2 :

- modèle multi-bus / multi-capteurs ;
- identité durable des TR2 ;
- sessions et reconnexion ;
- modèle d'affectation aux points de mesure ;
- accès protocolaire B0 à B7 ;
- ordonnanceur de bus ;
- polling générique ;
- snapshots et fraîcheur ;
- transactions B5 côté PC ;
- récupération et réconciliation des commandes ;
- refresh explicites ;
- archivage logique B3 ;
- contexte temporel B2 associé aux observations B3 ;
- journalisation structurée des erreurs de communication PC ;
- frontières de projets .NET.

S1 ne choisit pas encore les technologies de persistance, d'IHM Web, de transport série physique ni de synchronisation analytique.

---

## 3. Architecture de solution gelée

La solution .NET 10 conserve les projets et responsabilités distincts suivants :

```text
TR2.Domain
TR2.Transport
TR2.Protocol
TR2.Application
TR2.Persistence
TR2.Campaigns
TR2.Supervision.Service
```

Les dépendances de production sont verrouillées par tests d'architecture :

```text
TR2.Domain              -> aucune dépendance projet
TR2.Transport           -> aucune dépendance projet
TR2.Protocol            -> TR2.Domain + TR2.Transport
TR2.Application         -> TR2.Domain + TR2.Protocol
TR2.Persistence         -> TR2.Application + TR2.Domain
TR2.Campaigns           -> TR2.Application + TR2.Domain
TR2.Supervision.Service -> Application + Campaigns + Domain + Persistence + Protocol + Transport
```

Le moteur métier reste indépendant de l'IHM.

Le projet `TR2.Supervision.Service` reste à ce gel un shell de composition minimal. L'absence de composition matérielle, de serveur Web et de stockage concret à ce stade est volontaire.

---

## 4. Identité, endpoints et sessions

Les concepts suivants sont gelés distinctement :

- `SerialBus` : bus physique logique ;
- `TR2Endpoint` : bus + adresse Modbus ;
- `TR2Device` : capteur physique identifié durablement par B0 `device_id` ;
- `TR2Session` : état logique vivant d'un endpoint ;
- `FleetRegistry` : registre central des sessions connues.

Le `device_id` est l'identité durable.

Le port série et l'adresse Modbus ne sont que des localisations de communication.

Une session compatible requiert B0 et la vérification de la version protocolaire supportée.

Le même `device_id` ne peut pas être actif comme `Compatible` sur plusieurs endpoints simultanément.

Un `device_id` différent détecté au même endpoint remplace l'identité courante de cet endpoint et représente un autre capteur.

Une perte de communication conserve le `device_id` connu et les dernières valeurs ; elle ne les remet jamais à zéro.

---

## 5. Modèle équipement / point de mesure

S1 gèle les concepts :

```text
Installation
  -> Equipment
      -> MeasurementPoint
```

ainsi que l'affectation historisée d'un TR2 à un point de mesure.

Un déplacement ultérieur du capteur ne réécrit pas l'historique précédent.

Le contexte d'affectation équipement est une donnée supervision et reste distinct du contexte de campagne B4.

Aucune installation spécifique MSM n'est codée en dur dans le modèle générique.

---

## 6. Accès protocolaire B0 à B7

S1 dispose de chemins de lecture typés pour les blocs B0 à B7 suivant le mapping Modbus V1 courant.

Les règles essentielles sont :

- B0 : identification et compatibilité de session ;
- B1 : état système ;
- B2 : temps et synchronisation ;
- B3 : supervision vibration ;
- B4 : configuration lue suivant le mapping courant ;
- B5 : état transactionnel de commande ;
- B6 : inventaire campagne ;
- B7 : diagnostic.

Les écritures ajoutées dans S1 restent limitées aux besoins explicitement supportés par le protocole :

- soumission B5 selon la séquence préparée puis front `submit` ;
- sélection d'index B6 selon le registre prévu.

S1 n'invente aucun registre de provisioning d'adresse Modbus.

---

## 7. Ordonnancement bus et polling

Un seul ordonnanceur logique arbitre les travaux d'un même bus.

Les familles de polling gelées sont :

```text
FAST   -> B1 + B3 + B5
MEDIUM -> B2 + B7
SLOW   -> B4 + B6
STATIC -> B0
```

Les commandes, refresh explicites, monitoring post-submit et réconciliations ont priorité sur le polling ordinaire.

Les fréquences chiffrées de polling ne sont pas gelées par S1 ; elles restent `SUPERVISION_POLICY`.

Le polling STATIC B0 publie maintenant explicitement la nouvelle session B0 dans le `FleetRegistry`.

Aucun retry caché n'est introduit par les exécutants de polling.

---

## 8. Reconnexion et refresh explicite

Après reconnexion, B0 est relu en premier par l'autorité de découverte.

Le plan de refresh explicite couvre ensuite B1 à B7.

B0 n'est pas relu une seconde fois dans ce plan post-découverte.

Les refresh explicites utilisent le même ordonnanceur de bus et libèrent le bus même en cas d'erreur.

Les refresh B1/B2/B3 publient les snapshots correspondants.

Les refresh B4/B5/B6/B7 ne créent pas artificiellement de snapshots de télémétrie.

Une erreur classée comme erreur de communication pendant un refresh marque les snapshots indisponibles et la session déconnectée, en conservant l'identité durable connue.

---

## 9. Snapshots et fraîcheur

S1 gèle un registre de snapshots côté supervision pour B1, B2 et B3.

Chaque snapshot conserve :

- la dernière valeur reçue ;
- son timestamp de réception PC ;
- son état de disponibilité.

La fraîcheur PC est distincte de la validité métier fournie par le TR2.

Les états conceptuels supportés comprennent :

- Fresh ;
- Aging ;
- Stale ;
- Unavailable ;
- NeverReceived.

Une perte de communication ne détruit jamais la dernière valeur connue.

---

## 10. Autorité transactionnelle B5 côté supervision

S1 gèle un `CommandCoordinator` par `device_id`.

Les invariants suivants restent applicables :

- une seule transaction non terminale par TR2 ;
- `transaction_id` valide dans 1..65535 ;
- allocation persistée avant soumission ;
- allocation monotone sans recyclage automatique ;
- aucun wrap automatique après 65535 ;
- aucun retry naïf avec un nouveau transaction_id ;
- timeout après tentative de submit = ambigu ;
- transaction ambiguë non résolue = blocage des nouvelles commandes ;
- récupération depuis le journal PC ;
- réconciliation B5 après reprise ;
- absence de replay automatique d'une transaction ambiguë ;
- aucune importation implicite d'un mécanisme V1.1 tel que `transaction_epoch`.

La séquence de soumission B5 reste :

```text
écriture mailbox B5 avec request_control = 0
        -> écriture du seul request_control avec submit = 1
```

Une erreur de préparation empêche le submit.

Une erreur lors de la tentative de submit conduit à l'état local ambigu.

Le monitoring post-submit et la réconciliation utilisent le même ordonnanceur central.

Une décision de réconciliation ne relance jamais automatiquement la commande métier.

`RESET_STATISTICS` reste non opérationnelle avec le firmware actuellement gelé : aucun faux service, aucun succès no-op et aucun stockage fictif de statistiques n'est introduit.

---

## 11. Journal transactionnel B5

S1 dispose du contrat de journal PC nécessaire aux garanties transactionnelles B5 et à la reconstruction de l'état des transactions.

Le journal technique B5 est distinct des faults/warnings du TR2.

Les barrières de journalisation nécessaires à l'allocation, à la préparation, au submit, à l'ambiguïté et à la résolution terminale restent des garanties du coordinateur côté PC.

La technologie concrète de stockage durable de ce journal n'est pas gelée par S1.

---

## 12. Journal de communication PC

S1 gèle un événement structuré `CommunicationFailureEvent` distinct des diagnostics TR2.

Il transporte au minimum :

- endpoint ;
- `device_id` lorsqu'il est connu ;
- nature de l'opération (`Polling` ou `ExplicitRefresh`) ;
- timestamp PC ;
- type d'exception ;
- message d'exception.

Le `device_id` n'est jamais déduit de l'adresse Modbus.

Sur erreur classée communication :

```text
préserver les dernières valeurs
-> snapshots Unavailable
-> session Disconnected
-> tentative de journalisation communication
-> propagation de l'erreur de communication originale
```

La journalisation de communication est diagnostic best-effort dans ce chemin : une panne du sink de journal ne doit ni empêcher la transition `Disconnected`, ni masquer l'exception de communication originale.

La taxonomie utilisateur/opérateur détaillée et l'identité d'utilisateur ne sont pas gelées en S1.

---

## 13. Archivage logique B3

S1 gèle le contrat logique d'archivage B3 sans choisir de moteur de base de données.

Une observation B3 archive :

- le `device_id` durable ;
- la valeur B3 complète ;
- le timestamp de réception PC du B3 ;
- éventuellement le dernier contexte B2 connu et son propre timestamp de réception PC.

Le contexte B2 n'est jamais présenté comme simultané au B3 s'il provient d'un snapshot précédent.

Une absence de B2 n'empêche pas l'archivage B3.

Une observation B3 ne peut pas être attribuée à un endpoint non identifié ou à une session non compatible.

Le polling FAST et le refresh explicite B3 sont tous deux raccordés au contrat d'archive.

Une erreur du sink d'archive :

- est propagée ;
- ne devient pas une erreur de communication TR2 ;
- ne déconnecte pas la session ;
- ne remet pas en cause un snapshot B3 déjà publié.

La fréquence de stockage reste distincte de la fréquence de polling et n'est pas chiffrée en S1.

---

## 14. Campagnes B6 et données brutes SD

S1 fournit l'accès logique B6 et la sélection d'index prévue par le mapping.

L'identité métier d'une campagne reste `(device_id, campaign_id)`.

Le téléchargement des données brutes de campagne n'est pas inventé sur Modbus V1.

Le futur import SD reste séparé du polling Modbus.

Aucun parser de fichier brut n'est gelé tant que le format firmware réel n'a pas été vérifié dans `main`.

---

## 15. Points explicitement non gelés par S1

Les éléments suivants restent volontairement ouverts et appartiennent aux phases suivantes ou à des arbitrages dédiés :

- profil série physique final : port, baud, parité et paramètres RTU ;
- implémentation concrète de `IRegisterTransport` pour le matériel réel ;
- classification de production exacte des erreurs du futur transport concret ;
- moteur de stockage local ;
- choix SQLite, autre SQL, fichiers ou autre technologie ;
- schémas physiques de persistance ;
- politique chiffrée de rétention ;
- cadence chiffrée de polling ;
- cadence chiffrée d'archivage B3 ;
- reprise durable concrète du sink B3 ;
- stockage durable concret des journaux PC ;
- journal opérateur / métier complet ;
- trace Modbus bas niveau et sa politique de rétention ;
- framework Web final ;
- API HTTP détaillée ;
- authentification et rôles ;
- composition finale du service ;
- provisioning de l'adresse Modbus ;
- import et parser des fichiers SD ;
- stockage et traitement des données brutes ;
- synchronisation vers la base analytique ;
- schéma analytique ;
- intégration Grafana ;
- FFT et traitements vibration avancés.

L'absence de ces éléments n'empêche pas le gel S1 car S1 est le socle logiciel, non la livraison finale de la supervision.

---

## 16. Dette et contraintes connues à préserver

Les points suivants ne doivent pas être oubliés lors des phases suivantes :

- la date de préparation d'une commande et son `dueAt` pourront nécessiter une séparation plus explicite dans les appels futurs ;
- l'identité métier opaque actuellement portée par certaines requêtes B5 ne remplace pas une validation future de la correspondance exacte commande/paramètres ;
- les exécutants supposent que le travail reçu est celui obtenu via l'ordonnanceur ; une future composition de service devra préserver cette discipline ;
- les erreurs de lecture post-submit ne provoquent pas de retry implicite ;
- une transaction ambiguë nécessite une réconciliation explicite ;
- la sélection B6 n'effectue pas actuellement de read-after-write ;
- aucune configuration B4 préparée n'est écrite par la supervision S1 ; toute future écriture devra respecter exactement les règles CRC, accès et activation V1 ;
- les moteurs `TR2.Persistence` et `TR2.Campaigns` restent essentiellement des frontières architecturales dans S1 ;
- la composition réelle du host/service reste à réaliser ;
- l'archivage logique S1 ne constitue pas encore une garantie de durabilité disque tant qu'un sink concret n'est pas fourni.

Ces éléments sont des frontières de travail futures et ne doivent pas être silencieusement convertis en comportements supposés.

---

## 17. Conclusion de gel

La phase **S1 — socle logiciel de supervision TR2** est considérée clôturée à la baseline logicielle :

`47f6a264b1024a8d6d54a78ff79cae8ea0ec3872`

sous réserve du maintien de la validation locale verte déjà obtenue sur cet état.

Le socle S1 fournit désormais les autorités et contrats nécessaires pour engager une phase S2 sans remettre en cause les invariants centraux d'identité, de bus, de transactions B5, de reconnexion, de télémétrie et d'archivage logique.

La phase suivante devra commencer par un cadrage S2 explicite avant de choisir les technologies encore ouvertes.
