# Freeze P12-H3d3-D2-B — H3d2 physical format and persistent recovery on FRAM

## Statut

**GELÉ — validation matérielle réelle**

Baseline :
- P12-H3d3-D2-A read-only recovery gelé : `876d7e48def28bef0b523b507c0175db8083b09e`
- premier harnais D2-B de formatage gardé : `a82eac5e714d5feed8f2521a9f14e8633beb1a8e`
- correction backend FRAM pour transferts physiques longs : `efed212edf45be75350fe902804c9c9e0754bcb5`
- harnais de remise à EMPTY contrôlée : `9ac113225d111d7cd8956908fe1de739ccfb6e0e`
- désarmement du reset destructif avant preuve inter-boot : `6ed0b529b5cf00272eacd4fa936c20a621008e60`

## Objet

D2-B qualifie sur la FRAM physique STM32 le premier formatage H3d2 réel, sa publication initiale et le recovery persistant après un nouveau boot.

La tranche ne qualifie pas encore un commit transactionnel applicatif ni le basculement A/B.

## Incident découvert lors de la première tentative

La première tentative de `transactional_image_media_format_empty()` a donné :
```text
storage_init               = 0x0
geometry                   = 0x0
media_init                 = 0x0
recover                    = 0x0
recovery_status            = 0x0  # EMPTY
format_attempted           = 0x1
format_result              = 0x8  # TR2_ERROR_STORAGE
post_format_recover_result = 0x9  # sentinelle, non exécuté
post_format_status         = 0x4
post_format_generation     = 0x0
post_format_active_image   = 0xff
```

Cause identifiée dans le backend physique : le driver utilisait un unique `HAL_SPI_Transmit()` avec timeout 10 ms pour le payload H3d2 de 51 818 octets. À 2,5 MHz, le temps de transmission minimal du seul payload est d'environ 166 ms.

La tentative ayant retourné une erreur n'est pas requalifiée a posteriori comme réussite. Un boot ultérieur a néanmoins récupéré un état `VALID`, génération 1, image A, ce qui confirme le caractère physiquement ambigu d'une erreur sur une écriture en cours et justifie l'absence de retry automatique.

## Correction du backend physique

Le backend STM32 FRAM a été corrigé pour transférer les gros READ/WRITE en blocs HAL de 256 octets.

Pour un WRITE :
- une seule commande WRITE et une seule adresse sont émises ;
- CS reste actif pendant toute la commande et tous les morceaux du payload ;
- le découpage concerne uniquement les appels HAL bornés ;
- aucune réémission automatique de la commande WRITE n'est introduite ;
- la politique H3d2 n'est pas réimplémentée dans le driver.

Validation après correction :
- `./tr2_validate.sh` : vert ;
- cross-build STM32 : vert.

## Remise à EMPTY contrôlée pour la qualification

Afin d'obtenir une preuve propre du formatage avec le backend corrigé, un harnais destructif temporaire a mis à zéro uniquement les quatre enregistrements structurants de 64 octets utilisés par le recovery EMPTY :
- superblock A ;
- superblock B ;
- header image A ;
- header image B.

Les zones payload n'ont pas été effacées.

Le harnais était explicitement gardé et a été désarmé avant la preuve inter-boot.

## Résultat matériel — formatage physique complet

Sur le boot de qualification destructif :
```text
storage_init                    = 0x0
geometry                        = 0x0
media_init                      = 0x0

reset_attempted                 = 0x1
reset_result                    = 0x0

recover_result                  = 0x0
recovery_status                 = 0x0  # EMPTY

format_attempted                = 0x1
format_result                   = 0x0

post_format_recover_result      = 0x0
post_format_status              = 0x1  # VALID
post_format_generation          = 0x1
post_format_active_image        = 0x0
```

Cela démontre sur la FRAM réelle :
1. recovery EMPTY ;
2. exécution complète de `transactional_image_media_format_empty()` ;
3. écriture physique du payload initial de 51 818 octets ;
4. publication initiale H3d2 ;
5. recovery immédiat VALID ;
6. autorité génération 1 / image A.

## Résultat matériel — persistance inter-boot

Le mécanisme destructif a ensuite été désarmé, le firmware a été revalidé et recompilé, puis programmé avec reset.

Sondes GDB du nouveau boot :
```text
storage_init       = 0x0
geometry           = 0x0
media_init         = 0x0

reset_attempted    = 0x0
reset_result       = 0x9  # sentinelle : non exécuté

recover_result     = 0x0
recovery_status    = 0x1  # VALID
generation         = 0x1
active_image       = 0x0

format_attempted   = 0x0
format_result      = 0x9  # sentinelle : non exécuté
```

Le nouveau boot retrouve donc depuis la FRAM l'autorité H3d2 génération 1 / image A sans remise à zéro et sans nouveau formatage.

## Invariants préservés

D2-B ne modifie pas :
- le format H3d2 gelé ;
- la géométrie de qualification ;
- les offsets superblocks/images ;
- les règles de génération, CRC ou publication ;
- la logique transactionnelle portable ;
- la règle d'absence de retry automatique après résultat d'écriture ambigu.

La modification durable issue de la tranche est limitée au support des transferts physiques FRAM longs par le backend STM32. Le reset de métadonnées de qualification reste présent dans le code mais désarmé par `TR2_FRAM_D2B_RESET_METADATA_FOR_QUALIFICATION = 0`.

## Limites du gel

D2-B ne valide pas encore :
- un commit transactionnel applicatif réel ;
- le basculement image A vers image B ;
- le basculement superblock A vers superblock B ;
- les générations suivantes ;
- la coupure d'alimentation pendant les différentes phases d'un commit réel ;
- les scénarios de corruption physique ;
- le budget SRAM final du firmware complet.

## Conclusion

P12-H3d3-D2-B est validée et gelée comme qualification du formatage H3d2 initial sur FRAM physique et du recovery persistant VALID après nouveau boot.

La prochaine tranche peut qualifier un premier commit transactionnel réel sur FRAM, avant les essais de power-loss.
