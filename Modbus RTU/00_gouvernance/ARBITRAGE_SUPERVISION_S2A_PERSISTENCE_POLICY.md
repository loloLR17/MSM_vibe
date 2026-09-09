# Projet MSM — Capteur de vibration TR2

## S2-A — Politique et architecture de persistance locale

Date : 2026-09-09

Ce document formalise l'arbitrage S2-A de la supervision TR2.

Il complète les gels :

- `FREEZE_SUPERVISION_S0_CADRAGE.md` ;
- `FREEZE_SUPERVISION_S1_SOCLE_LOGICIEL.md`.

Il ne modifie ni la spécification Modbus RTU V1, ni l'architecture firmware gelée, ni les invariants S1. Toute décision ci-dessous est une `SUPERVISION_POLICY` sauf mention explicite contraire.

---

## 1. Objet de S2-A

S2-A choisit la technologie et les garanties de la persistance locale de la supervision, sans encore implémenter de stockage métier concret.

Le but est de préparer S2-B et les tranches suivantes en gelant :

- le moteur local ;
- la bibliothèque d'accès ;
- le modèle transactionnel ;
- la politique de durabilité ;
- la politique de journalisation SQLite ;
- la gestion des migrations ;
- la séparation des erreurs ;
- les frontières de dépendances ;
- les règles d'exploitation de la base locale.

S2-A n'ajoute encore :

- aucune table métier ;
- aucun sink B3 concret ;
- aucun journal B5 concret ;
- aucun store d'allocation de `transaction_id` concret ;
- aucun journal de communication concret ;
- aucun schéma d'affectation concret ;
- aucun transport Modbus physique ;
- aucun serveur Web ;
- aucune synchronisation Grafana/analytique.

---

## 2. Autorités existantes à préserver

S2-A ne redéfinit pas les contrats gelés dans S1.

Les interfaces applicatives restent les autorités de dépendance du code de production :

- `ICommandTransactionJournal` ;
- `ICommandTransactionReservationStore` ;
- `IB3ArchiveSink` ;
- contrats de journal de communication ;
- contrats futurs explicitement ajoutés côté Application lorsque nécessaires.

Le projet `TR2.Persistence` implémente ces contrats mais ne devient pas une autorité métier.

Le sens de dépendance gelé reste :

```text
TR2.Application -> contrats
        ^
        |
TR2.Persistence -> implémentations concrètes
```

Aucun type SQLite ne doit remonter dans `TR2.Domain` ou `TR2.Application`.

---

## 3. Choix du moteur local

### Décision

Le moteur local retenu pour S2 est **SQLite**.

La bibliothèque .NET retenue est **`Microsoft.Data.Sqlite`** utilisée directement via les abstractions ADO.NET.

### Classification

`SUPERVISION_POLICY`.

### Motifs

Le choix répond aux contraintes S0/S1 :

- fonctionnement offline-first ;
- stockage local autonome ;
- déploiement sans serveur de base externe ;
- transactions ACID ;
- fichier local portable ;
- testabilité host sans matériel TR2 ;
- contrôle explicite des transactions nécessaire pour les barrières B5.

### Non-choix

S2 n'utilise pas Entity Framework Core comme couche de persistance principale.

La raison n'est pas une incompatibilité fonctionnelle d'EF Core, mais le besoin de garder explicites et auditables :

- les transactions de sécurité B5 ;
- l'ordre exact des écritures ;
- les migrations ;
- les contraintes SQL ;
- les politiques de durabilité.

S2 utilise donc SQL explicite et `Microsoft.Data.Sqlite`.

---

## 4. Emplacement de la base

La base SQLite est une base **locale au processus de supervision**, stockée sur un filesystem local du même hôte.

Le chemin physique reste configurable et n'est pas codé en dur pour MSM.

La base n'est pas destinée à être ouverte directement depuis un partage réseau.

Cette règle est particulièrement importante si le mode WAL est activé, car WAL suppose que les processus accédant à la base se trouvent sur le même hôte et partagent les primitives de mémoire associées.

Le mécanisme de configuration concret du chemin sera traité lors de la composition du service.

---

## 5. Modèle de journal SQLite

### Décision

La base S2 utilise :

```sql
PRAGMA journal_mode = WAL;
```

L'activation doit être vérifiée à l'ouverture : la valeur retournée doit confirmer `wal`.

Le choix WAL est une `SUPERVISION_POLICY`.

### Raisons

- les lecteurs peuvent coexister avec le writer ;
- les écritures restent sérialisées par SQLite ;
- la supervision pourra ultérieurement exposer des lectures API sans imposer un blocage systématique des écritures ;
- les commits sont consignés dans le WAL avant checkpoint vers le fichier principal.

### Contraintes

Les fichiers associés `-wal` et `-shm` font partie de l'état opérationnel de la base pendant son utilisation.

Une copie naïve du seul fichier principal pendant qu'une connexion WAL est active n'est pas considérée comme une sauvegarde cohérente.

La stratégie de backup/export sera arbitrée ultérieurement.

---

## 6. Politique de durabilité

### Décision

La base S2 utilise :

```sql
PRAGMA synchronous = FULL;
```

pour les connexions de production participant aux écritures durables de S2.

### Classification

`SUPERVISION_POLICY`, motivée par les garanties gelées de S1.

### Raison

Les barrières B5 exigent notamment que l'allocation d'un `transaction_id` soit durable avant la tentative de submit.

En mode WAL, `synchronous=NORMAL` peut préserver la cohérence tout en permettant la perte d'un commit après une coupure de courant ou un hard reset. Cette propriété est incompatible avec l'intention de sécurité du store d'allocation B5.

S2 préfère donc la durabilité à la performance maximale.

Toute optimisation future réduisant cette garantie nécessiterait un arbitrage explicite et ne peut pas être introduite silencieusement.

---

## 7. Transactions

Les transactions S2 sont des **transactions SQLite locales explicites** ouvertes sur `SqliteConnection`.

`System.Transactions` n'est pas utilisé.

Une opération métier qui exige plusieurs écritures atomiques doit les effectuer dans une transaction SQLite unique lorsqu'elles résident dans la même base.

Les transactions doivent rester courtes.

Aucune transaction longue ne doit englober :

- un échange Modbus ;
- une attente opérateur ;
- un délai de polling ;
- une opération réseau distante ;
- un traitement analytique long.

En particulier, la persistance préalable d'une allocation B5 est terminée et commitée **avant** l'appel ultérieur qui peut effectuer le submit Modbus.

La transaction SQLite ne traverse jamais la frontière physique Modbus.

---

## 8. Unité physique de stockage

S2 part d'une **base SQLite locale unique** pour les données structurées de supervision couvertes par S2 :

- état durable nécessaire aux transactions B5 ;
- journal transactionnel PC ;
- observations B3 ;
- journal de communication ;
- modèle Installation / Equipment / MeasurementPoint ;
- historique des affectations.

Cette décision ne signifie pas que toutes ces écritures partagent une transaction métier commune.

Les gros fichiers bruts de campagne SD restent hors de cette base structurée conformément à S0/S1.

Leur stockage physique sera traité dans une phase Campaigns dédiée.

---

## 9. Concurrence et autorité d'écriture

Le service TR2 reste l'autorité principale de la base.

S2 n'introduit pas de modèle multi-writer distribué.

Les écritures concurrentes doivent être courtes et maîtrisées. Les repositories/stores ne doivent pas conserver une transaction ouverte au-delà de leur opération atomique.

Une contention SQLite (`BUSY` / verrou temporaire) n'est jamais une erreur de communication TR2.

La politique chiffrée de `busy_timeout` sera gelée au moment où l'infrastructure de connexion S2-B sera implémentée et testée.

Aucun retry illimité ou caché ne sera autorisé.

---

## 10. Migrations et version du schéma

Le schéma physique est versionné.

S2-B introduira un mécanisme de migration **ordonné, monotone et explicite**.

Principes gelés :

1. une base neuve est initialisée automatiquement vers la version courante supportée ;
2. une base ancienne supportée est migrée dans l'ordre sans saut implicite ;
3. chaque migration est identifiée par une version entière monotone ;
4. une migration n'est marquée appliquée qu'après réussite de son transaction/lot de migration ;
5. une version de base supérieure à celle connue par le logiciel provoque un refus explicite d'ouverture en écriture ;
6. aucune migration destructive silencieuse n'est autorisée ;
7. les migrations ne doivent pas importer de règles V1.1 dans le modèle V1.

Le mécanisme concret de stockage du numéro de version sera implémenté en S2-B. `PRAGMA user_version` est le mécanisme privilégié sauf contrainte identifiée pendant cette tranche.

---

## 11. Typage et représentation

La persistance ne redéfinit pas la sémantique Modbus.

Les conversions entre types de domaine et représentation SQLite sont localisées dans `TR2.Persistence`.

Règles :

- les `DeviceId`, `TransactionId`, identifiants métier et enums restent validés par leurs types/constructeurs de domaine ;
- les timestamps PC sont stockés sans perte de l'instant représenté ;
- aucune valeur sentinelle SQLite générique n'est inventée pour remplacer les règles spécifiques du protocole ;
- `NULL` SQL signifie absence de donnée de supervision lorsque le schéma l'autorise, pas une convention Modbus implicite ;
- les données brutes nécessaires à une reconstruction fidèle ne sont pas remplacées par leur seul texte d'affichage.

Le format précis des colonnes sera défini tranche par tranche avec tests de round-trip.

---

## 12. Erreurs et classification

Les erreurs de persistance constituent une famille distincte des erreurs de communication TR2.

Une erreur SQLite :

- ne doit jamais être reclassée comme timeout/CRC/erreur Modbus ;
- ne doit pas, à elle seule, déconnecter une session TR2 ;
- peut être bloquante pour l'opération métier lorsque la persistance constitue une barrière de sécurité ;
- peut être best-effort uniquement lorsque le contrat S1 le dit explicitement.

Exemples :

- échec de persistance de l'allocation B5 -> submit interdit ;
- échec du journal transactionnel sur une barrière requise -> comportement conforme aux garanties du `CommandCoordinator` ;
- échec d'archive B3 -> erreur propagée, snapshot déjà publié conservé, session non déconnectée ;
- échec du sink de journal communication -> ne masque pas l'erreur de communication originale et ne bloque pas la transition `Disconnected`.

---

## 13. Politique de reprise

La reprise après redémarrage repose uniquement sur des données commitée durablement.

La fermeture propre du processus ne doit pas être une précondition à la cohérence de la base.

Les tests S2 devront inclure des scénarios où :

- le store est fermé puis rouvert ;
- un autre objet store/repository relit la même base ;
- un état B5 est reconstruit à partir des écritures durables ;
- une migration est interrompue avant validation et ne laisse pas une version faussement avancée.

La simulation d'une coupure électrique réelle reste hors du périmètre host ; elle ne sera pas prétendue validée par un simple test unitaire.

---

## 14. Intégrité et clés

Les contraintes d'intégrité qui peuvent être garanties par SQLite doivent être exprimées dans le schéma en plus des validations applicatives lorsque pertinent :

- clés primaires ;
- unicité ;
- clés étrangères ;
- `NOT NULL` ;
- `CHECK` pour des invariants purement structurels.

L'application ne doit pas dépendre d'une désactivation implicite des contraintes de clés étrangères.

S2-B activera et vérifiera :

```sql
PRAGMA foreign_keys = ON;
```

sur chaque connexion concernée.

Les règles métier complexes restent dans le domaine/application et ne sont pas déplacées arbitrairement dans des triggers SQL.

---

## 15. Horodatage

Les timestamps de réception et d'événements PC restent des `DateTimeOffset` dans les contrats applicatifs lorsqu'ils le sont aujourd'hui.

La représentation SQLite doit permettre un round-trip déterministe de l'instant.

Le stockage en UTC est privilégié pour les colonnes temporelles de persistance ; la conversion d'affichage en heure locale appartient à la couche de présentation.

Cette règle ne modifie pas la base de temps B2 du TR2 et ne transforme jamais un timestamp PC en timestamp TR2.

---

## 16. Async et threads

Les API applicatives existantes conservent leurs signatures asynchrones lorsque présentes.

S2 ne doit cependant pas interpréter une API `Async` comme la garantie que SQLite réalise des I/O physiques non bloquantes à tous les niveaux.

L'objectif architectural reste :

- ne pas garder les transactions ouvertes inutilement ;
- isoler les accès persistence dans les stores/repositories ;
- permettre au runtime futur d'ordonner correctement polling, commandes et archivage.

Aucune politique de parallélisme agressif sur une même base n'est introduite en S2-A.

---

## 17. Ce que S2-A ne gèle pas encore

Restent ouverts pour S2-B ou les tranches suivantes :

- version NuGet exacte de `Microsoft.Data.Sqlite` ;
- chaîne de connexion complète ;
- `busy_timeout` chiffré ;
- stratégie précise de checkpoint WAL ;
- taille de page ;
- schéma des tables métier ;
- index ;
- politique chiffrée de rétention ;
- mécanisme de backup/export ;
- chiffrement de la base au repos ;
- stockage des fichiers bruts de campagne ;
- synchronisation analytique ;
- composition DI/service ;
- transport série physique.

Ces absences ne doivent pas être comblées implicitement.

---

## 18. Décomposition S2 confirmée

S2 reste découpée comme suit :

```text
S2-A — Politique et architecture de persistance
S2-B — Infrastructure physique de stockage
S2-C — Durabilité transactionnelle B5
S2-D — Archivage durable B3
S2-E — Journal de communication durable
S2-F — Persistance du modèle d'installation et des affectations
S2-G — Recovery intégré de la couche persistante
S2-H — Passe transversale et gel S2
```

La tranche immédiatement suivante après validation locale/documentaire de S2-A est **S2-B — Infrastructure physique de stockage**.

---

## 19. Résumé des décisions S2-A

```text
Moteur                SQLite
Provider .NET         Microsoft.Data.Sqlite
Accès                  ADO.NET / SQL explicite
EF Core                non retenu pour S2
Base                   locale, même hôte que le service
Journal mode           WAL
Durabilité             synchronous=FULL
Transactions           SQLite locales explicites
System.Transactions    non utilisé
Schéma                 versionné, migrations ordonnées
Version schéma         PRAGMA user_version privilégié
Foreign keys           ON et vérifiées
B5 safety barrier      commit durable avant submit
Erreurs persistence    distinctes des erreurs Modbus
Campagnes brutes SD    hors base structurée S2
```

Ces éléments constituent le cadrage de référence pour l'implémentation S2-B à S2-H.
