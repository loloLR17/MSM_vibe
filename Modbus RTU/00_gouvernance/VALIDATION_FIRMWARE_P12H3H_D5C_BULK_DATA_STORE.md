# P12-H3h-D5-C — Validation CampaignDataStore bulk multi-campagnes

## Statut

Validation logicielle locale : **VERTE — 101/101 tests**.

Baseline validée :

```text
d8c7411eaf8ac0462276f6e6261f30993dd08446
Firmware: test H3h-D5C physical campaign separation
```

## Objet

Composer les metadata transactionnelles D4-B et les blocs contrôlés D5-A/D5-B
dans une implémentation réelle de `CampaignDataStore`, indépendante du HAL
SDMMC et non encore substituée au backend historique de `SystemRuntime`.

## Composition validée

Le backend D5-C fournit :

- 8 slots metadata, dérivés de `TR2_CAMPAIGN_REPOSITORY_CAPACITY` ;
- une paire A/B D4-B par slot ;
- allocation append-only des zones data ;
- agrégation RAM fournie par l'appelant ;
- blocs physiques D5-A avec CRC32 ;
- writer/reader D5-B ;
- `begin_campaign`, `append`, `checkpoint`, `finish_campaign` et
  `recover_campaign` via l'interface `CampaignDataStore`.

## Sémantique checkpoint/recovery validée

Les tests valident notamment :

- recovery exact du préfixe checkpointé après reboot simulé ;
- queue non checkpointée non publiée comme autorité ;
- finish durable ;
- conservation de plusieurs campagnes successives ;
- corruption du payload durable détectée par CRC ;
- campagne absente classée EMPTY ;
- checkpoint refusé si la frontière logique coupe un record P8-B.

## Correction issue de la validation D5-C

La première implémentation calculait la base de la campagne suivante à partir de
`data_base + durable_prefix_bytes`.

Cette formule était incorrecte car `durable_prefix_bytes` est une taille
**logique**, alors que chaque bloc D5-A ajoute un header et un CRC.

Cas ayant révélé le défaut :

```text
metadata end             8192
logical payload            16
block header               32
block CRC                   4
physical block size        52
correct physical end     8244
incorrect old next base  8208
```

La seconde campagne pouvait donc chevaucher physiquement le bloc de la
première.

La correction validée détermine la fin physique en parcourant les blocs
contrôlés du préfixe durable. Un test de régression exige explicitement que la
campagne suivante commence après cette fin physique réelle.

## Validation

Après correction :

```text
101/101 tests verts
```

sur la baseline indiquée ci-dessus.

## Limites restantes

D5-C ne définit toujours pas :

- la taille de buffer de production STM32 ;
- la stratégie de découplage acquisition/écriture face aux stalls SD ;
- le backend physique SDMMC ;
- l'instanciation STM32 du nouveau DataStore ;
- la publication de capacité de stockage dans la validation de configuration.

Le backend D5-C n'est donc pas encore substitué au backend historique dans
`SystemRuntime`.

## Suite

La tranche suivante doit auditer la composition STM32 réelle avant
raccordement :

- création actuelle de `CampaignDataStore` ;
- backend persistant historique ;
- composition `SystemRuntime` ;
- mémoire statique réellement disponible ;
- contraintes d'exécution acquisition / stockage ;
- point d'insertion futur du `CampaignBulkMedia` SDMMC.

Aucune taille de buffer ou politique d'ordonnancement ne doit être inventée
avant cette revue.
