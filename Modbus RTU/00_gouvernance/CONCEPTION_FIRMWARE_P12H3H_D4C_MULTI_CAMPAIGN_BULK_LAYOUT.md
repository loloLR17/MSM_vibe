# P12-H3h-D4-C — Layout multi-campagnes et frontière d'intégrité bulk

## Statut

**CONCEPTION gelée avant implémentation du CampaignDataStore microSD.**

Baseline d'entrée :

```text
5237cee432667901304e3ac6e2d74d4fabfc42bf
Firmware: validate H3h-D4B metadata recovery
```

## 1. Constats vérifiés dans main

Le runtime réel impose les contraintes suivantes :

- `TR2_CAMPAIGN_REPOSITORY_CAPACITY = 8` ;
- au boot, `SystemRuntime::recover_campaigns()` parcourt chaque campagne
  historique du repository et appelle
  `CampaignDataStore::recover_campaign(campaign_id)` ;
- `CampaignService` ne possède qu'une campagne active à la fois ;
- le vieux `CampaignDataStorePersistent` possède 4 slots, mais ce nombre est
  propre à son ancien petit backend et n'est pas une autorité pour le nouveau
  bulk microSD ;
- le recovery historique sait distinguer EMPTY, VALID, CORRUPTED, UNAVAILABLE
  et UNSUPPORTED.

Conséquence : une seule paire de descriptors D4-B ne suffit pas au backend
microSD réel.

## 2. Répertoire de métadonnées bulk V1

Le layout bulk réserve une paire de descriptors D4-B par capacité de campagne
du repository :

```text
8 slots metadata
× 2 copies A/B
× 512 bytes
= 8192 bytes
```

Le nombre 8 n'est pas inventé par H3h : il est dérivé de
`TR2_CAMPAIGN_REPOSITORY_CAPACITY`.

Chaque slot metadata peut être EMPTY ou porter l'autorité d'un unique
`campaign_id`.

La recherche d'une campagne au recovery scanne les 8 slots et sélectionne le
slot dont l'autorité D4-B valide porte le `campaign_id` demandé.

Deux slots valides revendiquant le même `campaign_id` constituent une
corruption de layout et ne sont pas arbitrés silencieusement.

## 3. Allocation d'un slot metadata

`begin_campaign(campaign_id)` :

1. rejette un identifiant invalide ;
2. scanne toutes les autorités metadata ;
3. rejette un `campaign_id` déjà présent ;
4. choisit un slot EMPTY ;
5. détermine un `data_base` sûr ;
6. publie le descriptor génération 1, état OPEN, préfixe durable 0 ;
7. seulement après succès, la campagne devient active dans le DataStore.

Si aucun slot metadata n'est libre, le backend retourne NOT_AVAILABLE.

Aucune politique d'effacement/rétention n'est ajoutée implicitement.

## 4. Allocation de la zone data

Les campagnes sont physiquement append-only et une seule campagne peut être
active à la fois.

La zone metadata [0, 8192) est séparée de la zone data.

Le `data_base` d'une nouvelle campagne doit être calculé à partir des
autorités persistantes récupérables et aligné sur la granularité physique
retenue pour les records de stockage.

Une queue non checkpointée après power-loss n'est pas une autorité P8-B et peut
être abandonnée/écrasée. En revanche, aucun octet appartenant au préfixe durable
d'une campagne antérieure ne peut être réalloué.

La géométrie doit vérifier les additions 64 bits et la capacité réelle du média.

## 5. Pourquoi D4-B seul ne suffit pas

Le CRC32 D4-B protège le descriptor et donc la valeur
`durable_prefix_bytes`.

Il ne prouve pas que les octets du payload désignés par ce préfixe sont encore
intacts.

Le runtime existant possède déjà une sémantique où une corruption dans le
préfixe durable est remontée comme
`CAMPAIGN_DATA_RECOVERY_CORRUPTED`.

Le nouveau backend ne sera donc pas branché dans `SystemRuntime` tant qu'il
ne peut pas fournir une preuve équivalente d'intégrité du préfixe durable.

## 6. Format physique d'intégrité retenu

Le record logique P8-B reste strictement inchangé : 16 octets par sample.

Le média bulk utilise des **blocs physiques de données contrôlés par CRC32**.
Le CRC appartient au format physique du DataStore et non au record logique P8.

Chaque bloc physique contient :

- magic ;
- version ;
- taille de header ;
- campaign_id ;
- index monotone du bloc dans la campagne ;
- taille de payload utile ;
- payload constitué exclusivement de records P8-B complets ;
- CRC32 couvrant header significatif + payload.

Les champs sont encodés explicitement. Aucun layout C brut n'est persisté.

## 7. Granularité

La taille définitive du payload par bloc n'est pas gelée dans D4-C.

Elle devra être choisie dans une tranche de qualification en respectant
simultanément :

- multiples de 16 octets pour ne jamais couper un record P8 ;
- compatibilité avec la granularité SD ;
- overhead CRC/header faible devant 426672 bytes/s ;
- coût de recovery borné ;
- comportement face aux stalls mesurés en H3h-C ;
- RAM réellement disponible sur STM32U575.

En particulier, la valeur 64 KiB utilisée lors des essais H3h-C n'est pas
transformée silencieusement en constante de production.

## 8. Checkpoint avec blocs contrôlés

À une frontière P8-B :

1. tous les records logiques acceptés avant la frontière doivent être intégrés
   dans des blocs physiques complets ou dans un bloc finalisé de checkpoint ;
2. chaque bloc finalisé porte son CRC ;
3. les blocs sont écrits ;
4. le média est synchronisé ;
5. seulement ensuite D4-B publie le descriptor A/B avec le nouveau
   `durable_prefix_bytes` logique ;
6. le descriptor est synchronisé, relu et validé avant de devenir autorité RAM.

Ainsi, un descriptor ne peut jamais autoriser un payload qui n'a pas franchi
la barrière média précédente.

## 9. Recovery

Pour une campagne demandée :

1. retrouver exactement une autorité metadata valide correspondant au
   `campaign_id` ;
2. obtenir `data_base` et `durable_prefix_bytes` ;
3. parcourir uniquement les blocs nécessaires pour couvrir exactement ce
   préfixe logique ;
4. vérifier magic/version/campaign_id/index/taille/CRC de chaque bloc ;
5. vérifier que la somme des payloads validés est exactement égale au préfixe
   durable autorisé.

Résultats :

- aucun descriptor pour cet id : EMPTY ;
- descriptor supporté + tous blocs du préfixe valides : VALID ;
- descriptor valide mais bloc requis invalide/CRC faux/somme incohérente :
  CORRUPTED ;
- erreur de lecture média : UNAVAILABLE ;
- format reconnu mais version non supportée : UNSUPPORTED.

Aucun octet situé après le préfixe durable n'est utilisé pour reconstruire ou
deviner une queue non checkpointée.

## 10. Conséquence sur D2/D3

Le `CampaignBulkAggregator` D2 actuel agrège un flux brut et écrit directement
ce flux. Il reste une preuve utile du mécanisme d'agrégation mais ne suffit pas
tel quel au format physique CRC de D4-C.

Le checkpoint D3 reste l'invariant d'ordre `flush → sync → publication`, mais
son implémentation devra être composée avec le futur encodeur de blocs contrôlés.

D4-C n'altère donc pas rétroactivement les validations D2/D3 ; il précise la
couche physique requise avant leur composition dans le DataStore final.

## 11. Tranche suivante

**H3h-D5-A — codec de bloc bulk contrôlé**, indépendant du HAL et du runtime :

- format explicite ;
- encode/decode ;
- CRC32 ;
- validation campaign_id/index/taille ;
- payload multiple de 16 ;
- détection corruption/version inconnue ;
- aucune taille de buffer de production figée.

Puis :

- H3h-D5-B : writer/reader séquentiel de blocs ;
- H3h-D5-C : composition multi-slot metadata + data en CampaignDataStore ;
- seulement ensuite remplacement du vieux DataStore dans SystemRuntime ;
- H3h-E : backend SDMMC physique et qualification.

## 12. Invariants

- 8 slots metadata dérivés du repository courant, pas du vieux DataStore ;
- aucune auto-reprise d'une campagne OPEN ;
- aucune politique d'effacement implicite ;
- aucun filesystem imposé ;
- aucun record P8-B modifié ;
- aucune intégrité globale revendiquée sans validation des blocs du préfixe ;
- aucune taille de bloc de production inventée ;
- aucune composition SystemRuntime avant conformité recovery.
