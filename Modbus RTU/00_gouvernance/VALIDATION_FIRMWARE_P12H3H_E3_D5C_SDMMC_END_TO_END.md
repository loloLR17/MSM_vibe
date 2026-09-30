# P12 / H3h-E3 — Validation physique D5-C -> E1 -> microSD

## Statut

VALIDÉ PHYSIQUEMENT.

Baseline testée :

```text
9585c64aa7fad317ce7b9df4b4cf6344b72dfa24
Firmware: bound H3h-E3 SD qualification window
```

## Montage et périmètre

- STM32U575 / NUCLEO-U575ZI-Q.
- SDMMC2 en mode 1 bit / 10 MHz.
- microSD physique.
- composition réelle `CampaignDataStoreBulk` D5-C au-dessus du
  `CampaignBulkMedia` SDMMC E1.
- vue média de qualification bornée aux blocs physiques 2048..3071.
- metadata D4-B, blocs D5-A/D5-B et isolation sectorielle E2 exercés par la
  chaîne réelle.
- carte de test neuve et intégralement disponible pour les essais destructifs;
  la fenêtre bornée est un garde-fou du harness, pas une limitation physique
  de la carte.

## Scénario

1. remise à zéro des 8192 octets de métadonnées dans la fenêtre de test ;
2. création de la campagne 0xE301 ;
3. append de 80 octets ;
4. checkpoint durable ;
5. append supplémentaire de 64 octets sans checkpoint ;
6. reconstruction d'un nouveau `CampaignDataStoreBulk` sur le même média,
   simulant un reboot logiciel ;
7. recovery de la campagne.

## Résultats physiques

```text
tr2_sdmmc2_stage                         = 28
tr2_sdmmc2_error_code                    = 0x0
tr2_sdmmc2_e3_window_clear_result        = 0
tr2_sdmmc2_e3_window_clear_sync_result   = 0
tr2_sdmmc2_e3_store_init_result          = 0
tr2_sdmmc2_e3_begin_result               = 0
tr2_sdmmc2_e3_append_checkpoint_result   = 0
tr2_sdmmc2_e3_checkpoint_result          = 0
tr2_sdmmc2_e3_post_checkpoint_offset     = 9216
tr2_sdmmc2_e3_append_tail_result         = 0
tr2_sdmmc2_e3_post_tail_offset           = 9728
tr2_sdmmc2_e3_reboot_store_init_result   = 0
tr2_sdmmc2_e3_recover_result             = 0
tr2_sdmmc2_e3_recovery_status            = 0
tr2_sdmmc2_e3_recovered_prefix_bytes     = 80
```

## Interprétation

Le checkpoint de 80 octets est matérialisé par deux extents physiques
sectorisés de 512 octets à partir de la data zone logique 8192, d'où
`next_offset = 9216`.

L'append post-checkpoint de 64 octets produit un troisième extent dans le
secteur suivant, d'où `next_offset = 9728`.

Après reconstruction du DataStore, le recovery retourne VALID et publie
exactement 80 octets. La queue de 64 octets présente physiquement après le
checkpoint n'est donc pas promue en autorité durable.

Cette observation valide physiquement la composition D5-C -> E1 pour
l'invariant P8-B exercé : seul le préfixe couvert par le dernier checkpoint
durable est récupéré.

## Limites

- le reboot est simulé par reconstruction logicielle du DataStore; aucune
  coupure d'alimentation réelle pendant une opération SD n'est revendiquée ici;
- les mesures de débit restent celles des qualifications H3h-C2a/C2b/C2c;
- aucune taille de buffer de production n'est gelée par E3;
- le découplage acquisition/écriture SD et le câblage du runtime STM32 restent
  des sujets d'intégration firmware, pas des résultats E3.
