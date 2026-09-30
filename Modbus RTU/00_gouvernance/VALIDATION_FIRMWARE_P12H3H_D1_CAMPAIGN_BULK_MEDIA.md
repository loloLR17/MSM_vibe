# P12-H3h-D1 — Frontière abstraite du média bulk de campagne

## Statut

Validation logicielle locale : **VERTE**.

Baseline validée :

```text
623ce82772972a9d966a5346807128d9f0171bdf
Firmware: fix H3h-D1 bool include
```

## Objet

Introduire une frontière média brute, indépendante du HAL STM32 et du layout
futur de campagne, afin que le backend bulk puisse être conçu et testé sur hôte
avant son raccordement physique à SDMMC2.

## Contrat introduit

`CampaignBulkMedia` expose uniquement :

- `capacity_bytes()` ;
- `read(offset, buffer, size)` ;
- `write(offset, buffer, size)` ;
- `sync()`.

Les offsets et capacités sont sur 64 bits.

`sync()` est la seule barrière de durabilité média exposée à la couche core.
Cette abstraction ne définit pas à elle seule la sémantique du checkpoint P8-B.

## Validation

Le test unitaire `test_campaign_bulk_media` couvre :

- délégation des opérations brutes ;
- lecture/écriture à taille nulle sans accès média ;
- propagation des erreurs du média ;
- rejet des arguments invalides ;
- rejet d'un intervalle 64 bits débordant.

La validation complète `tr2_validate.sh` a été exécutée localement après
correction de l'inclusion `<stdbool.h>` et déclarée entièrement verte.

## Invariants

- aucun changement du contrat `CampaignDataStore` P8-B ;
- aucun changement de `CampaignDataStorePersistent` ;
- aucune dépendance HAL/STM32 dans le core ;
- aucun filesystem imposé ;
- aucun layout microSD imposé ;
- aucune taille de buffer RAM imposée ;
- aucune affirmation supplémentaire sur le pire cas de latence microSD.

## Suite

H3h-D2 peut construire l'agrégation RAM au-dessus de cette frontière. Le choix
de capacité de buffering doit rester distinct de la taille élémentaire utilisée
pour agréger les écritures et doit tenir compte, ultérieurement, des latences
physiques observées et de la politique de service du writer.
