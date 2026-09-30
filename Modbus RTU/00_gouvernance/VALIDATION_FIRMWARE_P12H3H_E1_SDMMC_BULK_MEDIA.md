# P12-H3h-E1 — Validation adaptateur CampaignBulkMedia SDMMC

## Statut

VALIDÉ ET GELÉ.

Baseline logicielle physiquement qualifiée :

```text
4303cfa0a4cc6232ae115e2d17a79444f5bd9aea
Firmware: fix H3h-E1 probe string include
```

## Objet

Valider physiquement l'adaptateur STM32 SDMMC du contrat byte-addressed
`CampaignBulkMedia` défini par D1, sur le chemin SDMMC2 déjà qualifié en
H3h-C : bus 1 bit, horloge 10 MHz, carte microSD sacrificielle.

Cette validation porte sur l'adaptateur E1. Elle ne constitue pas encore une
qualification physique de la composition D5-C complète.

## Validation logicielle préalable

La baseline ci-dessus a passé :

- la validation host/core complète ;
- le cross-build STM32U575 ;
- la compilation contre STM32CubeU5 v1.9.0 ;
- les warnings traités comme erreurs (`-Werror`).

## Zone physique de test

Le probe E1-P est destructif uniquement sur les blocs logiques :

```text
2048 .. 2051
```

soit 4 secteurs / 2048 octets.

Cette plage est entièrement incluse dans la zone sacrificielle déjà autorisée
et utilisée par H3h-C2c : blocs 2048 .. 3071.

Aucun filesystem, formatage ou effacement global n'est utilisé.

## Scénario physique

Le probe exécute via `CampaignBulkMedia` :

1. initialisation de l'adaptateur à partir du `SD_HandleTypeDef` réel ;
2. lecture de la capacité ;
3. écriture initiale alignée de 2048 octets avec motif déterministe ;
4. écriture partielle de 37 octets à l'offset +500, traversant la frontière
   entre deux secteurs de 512 octets et exerçant donc le read-modify-write ;
5. écriture alignée de 1024 octets à l'offset +1024 ;
6. `sync()` ;
7. relecture des 2048 octets complets via l'adaptateur ;
8. comparaison octet par octet avec l'image attendue.

La comparaison intégrale contrôle à la fois les données modifiées et la
préservation des octets voisins lors du RMW.

## Résultats physiques observés

```text
tr2_sdmmc2_stage                         = 16
tr2_sdmmc2_error_code                    = 0x00000000

tr2_sdmmc2_e1_init_result                = 0
tr2_sdmmc2_e1_capacity_result            = 0
tr2_sdmmc2_e1_capacity_bytes             = 511868665856

tr2_sdmmc2_e1_seed_write_result          = 0
tr2_sdmmc2_e1_partial_write_result       = 0
tr2_sdmmc2_e1_aligned_write_result       = 0
tr2_sdmmc2_e1_sync_result                = 0
tr2_sdmmc2_e1_read_result                = 0

tr2_sdmmc2_e1_mismatch_count             = 0
tr2_sdmmc2_e1_first_mismatch             = 0xffffffff
```

La capacité rapportée par le chemin réel est donc 511868665856 octets.

## Conclusion E1

La qualification démontre sur le matériel testé que l'adaptateur E1 sait :

- exposer la géométrie/capacité réelle ;
- traduire les accès byte-addressed alignés en accès SD multi-blocs ;
- réaliser physiquement un RMW non aligné traversant une frontière secteur ;
- préserver les octets voisins du RMW dans le scénario qualifié ;
- attendre l'état TRANSFER après les écritures ;
- exécuter la barrière média `sync()` ;
- relire sans mismatch les données écrites.

E1 est donc gelé pour ce périmètre.

## Limites maintenues

Ce gel ne démontre pas :

- la composition physique D5-C complète ;
- la sûreté power-loss d'un RMW partageant un secteur entre données déjà
  durables et nouvelles données ;
- le comportement sous stalls SD de pire cas ;
- une taille de buffer de production ;
- le découplage acquisition / écriture SD ;
- la publication de capacité dans `ConfigurationValidationEnvironment` ;
- le remplacement du bench par le runtime de production.

Avant de brancher le flux D5-C byte-packed sur E1, la règle d'isolation des
secteurs contenant un préfixe déjà durable doit être explicitement résolue :
une écriture ultérieure ne doit pas réécrire par RMW un secteur contenant des
octets appartenant au préfixe durable précédent.
