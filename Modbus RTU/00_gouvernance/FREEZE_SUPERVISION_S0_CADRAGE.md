# Projet MSM — Capteur de vibration TR2

## Gel S0 — Cadrage fonctionnel et architectural de la supervision

Date de gel : 2026-09-09

Ce document conserve les décisions de cadrage prises avant le démarrage de S1. Il ne remplace pas la spécification Modbus RTU V1, l'architecture firmware gelée ni les arbitrages firmware existants. En cas de conflit, les documents normatifs et de gel firmware applicables restent autorités sur le comportement du TR2.

## 1. Finalité

Le système TR2 mesure les vibrations du navire en différents points (machine, locaux passagers, restaurants, passerelle, etc.) pendant des phases et manoeuvres choisies afin de permettre un post-traitement visant notamment à comparer des modes de conduite plus souples.

La supervision TR2 ne collecte pas, dans cette version, les autres données du navire (moteurs, stabilisateurs, propulseurs, etc.). La corrélation vibration / données navire est réalisée en aval dans le Grafana global.

L'archivage fiable et exploitable des données TR2 est une fonction coeur de la supervision.

## 2. Principes généraux

- Socle technologique : .NET 10 LTS.
- Architecture modulaire : domaine, protocole, transport, application, campagnes, persistance et présentation sont séparés.
- L'IHM reste volontairement agile : les pages, widgets, parcours et présentations détaillées ne sont pas gelés en S0.
- L'IHM ne connaît pas directement les adresses de registres Modbus et ne porte pas les règles transactionnelles.
- L'application est conçue multi-TR2 dès l'origine.
- Le système est offline-first : la supervision locale reste opérationnelle sans Grafana, sans base distante et sans Internet.

## 3. Architecture multi-postes

Un moteur de supervision TR2 unique possède les bus RS485, le polling, les sessions et les autorités de commande B5.

L'interface principale est prévue comme interface Web locale accessible depuis plusieurs postes du réseau du navire (par exemple passerelle, PC machine, maintenance).

Aucun navigateur ou client Web n'accède directement au Modbus.

Les commandes provenant de plusieurs postes passent toutes par la même autorité transactionnelle centrale.

Les droits de consultation / opération / maintenance / administration pourront être différenciés ultérieurement ; leur mécanisme précis n'est pas gelé en S0.

## 4. Identité, bus et sessions

Concepts distincts :

- `SerialBus` : ressource physique de communication ;
- `TR2Endpoint` : couple bus + adresse Modbus ;
- `TR2Session` : session logique vivante ;
- `TR2Device` : capteur physique identifié durablement par le `device_id` B0.

Le port COM et l'adresse Modbus localisent le capteur ; le `device_id` l'identifie durablement.

Un seul contrôleur émet des transactions sur un bus donné ; les trames d'un même bus sont sérialisées.

Une session compatible n'est établie qu'après lecture B0, contrôle minimal de cohérence, identification du `device_id` et vérification de la version protocolaire.

La perte de communication conserve l'identité et les dernières valeurs, marquées comme périmées ; elle ne les remplace pas par zéro.

Après reconnexion, B0 est revérifié puis les états nécessaires B1-B7 sont relus. Un `device_id` différent au même endpoint représente un autre capteur.

## 5. Adresse Modbus

L'adresse esclave est considérée comme une configuration locale persistante du TR2 située hors du mapping métier Modbus V1.

Le mécanisme concret de provisioning reste à définir côté firmware ; la supervision ne doit pas inventer un registre V1 pour cette fonction.

Le profil série physique détaillé (notamment baud/parité) n'est pas gelé par S0.

## 6. Polling et fraîcheur

Le polling est centralisé et sérialisé par bus.

Groupes conceptuels :

- FAST : B1, B3, B5 ;
- MEDIUM : B2, B7 ;
- SLOW / événementiel : B4, B6 ;
- STATIC : B0.

Les fréquences exactes sont `SUPERVISION_POLICY`, configurables. Le dimensionnement initial envisagé est de l'ordre de 1 s / 5 s / 30-60 s selon le groupe, sans valeur normative V1.

B5 en transaction/réconciliation, les actions opérateur et les refresh explicites ont priorité sur le polling ordinaire.

Chaque snapshot possède une fraîcheur côté PC distincte de la validité métier TR2 : Fresh, Aging, Stale, Unavailable ou NeverReceived (noms conceptuels).

La fréquence de polling, la fréquence d'acquisition vibration et la fréquence de calcul des indicateurs B3 sont des notions distinctes.

## 7. Politique B5 côté PC

Un `CommandCoordinator` par `device_id` est l'unique autorité de soumission B5 côté supervision.

- une seule transaction B5 non terminale par TR2 ;
- `transaction_id` dans 1..65535 ;
- allocation monotone et persistance avant submit ;
- aucun recyclage automatique ;
- timeout après submit = état ambigu, jamais échec métier implicite ;
- aucun retry naïf avec un nouveau transaction_id ;
- réconciliation B5 après reconnexion avant nouvelle commande ;
- une transaction non réconciliée bloque les nouvelles commandes sur ce TR2 ;
- état transactionnel indexé par `device_id`, jamais par COM/adresse ;
- journal local de toutes les commandes et preuves observées ;
- persistance de sécurité du transaction_id synchrone, persistance analytique asynchrone ;
- RESET_STATISTICS n'est pas proposé comme commande opérationnelle avec le firmware actuellement gelé ;
- épuisement de 1..65535 bloque explicitement les commandes : aucun wrap automatique ;
- aucune règle V1.1, notamment `transaction_epoch`, n'est importée silencieusement en V1.

## 8. Journalisation

Trois niveaux distincts :

1. journal opérateur / métier ;
2. journal transactionnel / technique ;
3. trace Modbus bas niveau, diagnostic-only et à rétention limitée.

Les événements sont structurés avant génération de leur texte d'affichage.

Les erreurs de communication PC sont distinctes des faults/warnings remontés par le TR2.

Les opérations peuvent utiliser un `OperationId` local ; les commandes B5 conservent en plus leur `transaction_id` protocolaire.

Le stockage local des journaux ne dépend pas d'une base distante. Un export diagnostic complet devra être possible.

## 9. Equipements et points de mesure

Concepts distincts :

- `TR2Device` : capteur ;
- `TR2Endpoint` : emplacement réseau ;
- `EquipmentAssignment` : équipement / point de mesure actuellement surveillé.

Le modèle métier prévoit :

`Installation -> Equipment -> MeasurementPoint`.

Un TR2 peut avoir zéro ou un point de mesure actif. Un point de mesure possède au plus un TR2 actif dans le périmètre initial.

Les affectations sont historisées avec des bornes temporelles (`valid_from` / `valid_to`). Un déplacement ultérieur d'un TR2 ne réécrit jamais le contexte historique d'une campagne.

L'affectation équipement est une métadonnée supervision/base, pas une extension implicite du mapping Modbus V1.

Le contexte d'équipement et le contexte de campagne B4 restent deux notions distinctes.

Le modèle prévoit une `Installation` générique et ne code pas MSM en dur.

## 10. Archivage et Grafana

Deux flux vibration sont distingués :

1. données de supervision B3, adaptées aux tendances et à l'historisation continue ;
2. données détaillées/brutes de campagne, récupérées physiquement depuis les cartes SD.

Le Modbus V1 ne fournit pas de mécanisme de téléchargement des données brutes de campagne. L'import SD est donc un canal distinct du Modbus.

Un `CampaignImportService` conceptuel devra détecter, valider, associer, dédupliquer et ingérer les fichiers de campagne. Son contrat détaillé dépendra du format de fichier réellement défini par le firmware et devra être vérifié dans `main` avant implémentation.

L'identité métier normative d'une campagne est `(device_id, campaign_id)`. Une empreinte de contenu pourra compléter la déduplication des fichiers importés.

Les fichiers SD bruts originaux sont conservés sans transformation destructive. Les données et indicateurs dérivés sont séparés de la source brute afin de permettre des recalculs futurs.

Les gros fichiers bruts sont conceptuellement séparés des tables analytiques.

La supervision conserve suffisamment d'information temporelle pour permettre une corrélation ultérieure avec les autres données du navire : temps TR2, temps de réception/ingestion lorsque pertinent et informations de synchronisation/qualité disponibles via B2.

La qualité et la validité pertinentes accompagnent les mesures historisées.

La fréquence de stockage est distincte de la fréquence de polling. B3 est la principale série temporelle Modbus ; configurations, commandes, diagnostics et inventaires sont plutôt historisés sur événements/changements.

La synchronisation vers une base analytique est asynchrone, persistante et reprenable. Une indisponibilité de Grafana ou de la base analytique n'affecte pas la communication TR2.

Grafana global est consommateur analytique. Il réalise la corrélation avec les données navire ; il ne commande pas les TR2 dans le périmètre actuel.

Les données restent exportables indépendamment de Grafana, notamment en formats tabulaires/structurés appropriés.

## 11. Frontières à préserver

Architecture conceptuelle :

```text
Postes Web du navire
        |
        v
Interface Web / API
        |
        v
TR2 Supervision Service
  |      |       |
Fleet  B4/B5  Archivage
  |      |       |-- B3
Polling  |       `-- Import SD
  |      |
  `------+- autorité Modbus unique
         |
       RS485
         |
       TR2 Fleet

Supervision Service
        |
Stockage local / archives
        |
Ingestion / export
        |
Base analytique
        |
Grafana global
        ^
        |
Autres données navire
```

L'IHM peut évoluer librement tant qu'elle consomme les services applicatifs et ne reprend pas la logique Modbus, transactionnelle ou d'archivage.

## 12. Points volontairement ouverts après S0

Ne sont notamment pas gelés :

- organisation détaillée de l'IHM Web ;
- choix définitif du framework Web .NET ;
- mécanisme d'authentification et rôles ;
- moteur exact de stockage local ;
- moteur exact de base analytique ;
- schéma SQL détaillé ;
- format et parser des fichiers campagne SD avant vérification du firmware ;
- algorithmes FFT / traitements vibration avancés ;
- politique chiffrée de rétention ;
- mécanisme réseau exact de synchronisation ;
- profil UART/RTU physique final ;
- mécanisme concret de provisioning de l'adresse Modbus.

## 13. Suite

La phase suivante est S1 — socle logiciel de supervision.

La première tranche proposée est S1-A : créer le socle de solution .NET, les frontières de projets, les dépendances autorisées et les tests d'architecture/build, sans commencer par l'IHM détaillée, le Modbus physique ou la base de données.
