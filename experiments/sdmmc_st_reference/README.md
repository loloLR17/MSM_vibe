# M016 — référence SDMMC2 4 bits isolée (préparée, non qualifiée)

Projet indépendant du firmware TR2. Référence : [ST CubeU5 v1.9.0, SD_ReadWrite_DMALinkedList](https://github.com/STMicroelectronics/STM32CubeU5/tree/v1.9.0/Projects/STM32U575I-EV/Examples/SD/SD_ReadWrite_DMALinkedList), MCU U575 mais carte **STM32U575I-EV**, SDMMC1. Aucun exemple SD natif exact NUCLEO-U575ZI-Q trouvé dans le checkout v1.9.0. L'exemple ST n'est pas portable tel quel et n'est pas une preuve du banc NUCLEO.

## Provenance et écarts

`vendor/` conserve main/MSP/README originaux et licences, empreintes dans `SOURCES.json`. Exemples SLA0044, HAL BSD-3-Clause, CMSIS Apache-2.0 ; HAL/CMSIS restent dépendances externes du SDK qualifié. `clock.c` reprend la fonction SystemClock_Config ST (MSI→PLL, CPU160MHz et HSI48). Toolchain/linker/config HAL adaptés du projet autonome M015, sans son driver ni son code capteur.

Adaptations explicites : SDMMC1→2 et six GPIO, maintien clock falling/CLK48/div1 donc 24MHz nominal, bus4bit, pas de high-speed CMD6 facultatif, pas d'ICACHE/BSP LED, aucune commande erase/format, passage des deux nodes circulaires/24Mo à deux nodes contigus noncirculaires de2048octets chacun et trois répétitions. Écriture et relecture utilisent les API **HAL_SDEx_DMALinkedList** du SDK, DMA interne/IRQ. Toutes les données sont techniques synthétiques.

**Écart constaté dans le SDK :** les fonctions `HAL_SDEx_DMALinkedList_ReadCpltCallback/WriteCpltCallback` de l'exemple ne sont pas appelées par le HAL v1.9.0 présent. Son `HAL_SD_IRQHandler` appelle `HAL_SD_RxCpltCallback/TxCpltCallback` à DATAEND. L'adaptation utilise ces callbacks réels ; driver ST inchangé. Attentes bornées à2000ms et contrôle CMD13 sans erreur + TRANSFER + READY_FOR_DATA. Aucun délai dans IRQ. Pas de DCACHE ; buffers alignés32, SRAM normale. L'exemple adapté est à qualifier ; compilation ne démontre pas le DMA physique.

Aucun CampaignDataStore, FRAM, IIS3DWB, UART ou Modbus lié. Le firmware principal et ses volumes M016 restent hors périmètre. Aucune correction de production proposée avant preuve du banc.

## Build reproductible

Depuis ce dossier, avec GCC ARM et CMake :

```sh
cmake -S . -B /home/lolo/dev/msm/cache/sdmmc-reference-readonly \
  -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake \
  -DSTM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build /home/lolo/dev/msm/cache/sdmmc-reference-readonly -j4
```

Défaut **BENCH_WRITE_ENABLE=0**, aucun code writeblocks dans ELF après GC. Garde UID exact seconde carte historique001B0046/38425018/20333157 **avant toute initialisation SD**. Le SN historique0032001F3235510C37333439 doit être recontrôlé avant flash ; ne jamais sélectionner la carte principale003500463235510B37333439.

Un build avec `-DBENCH_WRITE_ENABLE=1 -DBENCH_TEST_LBA=<LBA autorisé>` doit être créé **après** décision sur le média et vérification de la capacité. Il n'autorise que huit secteurs à cette adresse, trois écrasements/relectures ; un CID RAM exact et un trigger unique sont aussi nécessaires. Le build logiciel LBA2048 des preuves est un test de compilation, **pas une allocation ni une autorisation de flash/exécution**. Il ne doit pas être utilisé pour une carte réelle sans nouvelle justification de ce LBA.

## Montage proposé à confirmer physiquement

[ST UM2861 Rev10, table18](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf) et pinout H3h-B du dépôt :

|Adafruit4682|STM32/AF|Morpho|
|---|---|---|
|CLK|PD6 AF11|CN11-43|
|CMD|PD7 AF11|CN11-45|
|D0|PB14 AF12|CN12-28|
|D1|PB15 AF12|CN12-26|
|DAT2|PB3 AF12|CN12-31|
|D3|PB4 AF12|CN12-27|
|3V|3V3|CN11-16|
|GND|GND|CN11-19 ou20|

[Adafruit pinouts](https://learn.adafruit.com/adafruit-microsd-spi-sdio/pinouts) : alimentation3,3V exclusivement, CMD et DAT bidirectionnels, pull-ups fournis. Leur [exemple sdioio](https://learn.adafruit.com/adafruit-microsd-spi-sdio/using-sdioio) concerne CircuitPython/SAMD51, pas une validation HALU5. Pas de code Adafruit copié. Vérifier contacts/longueur des fils, alimentation et éventuelles connexions aux fonctions partagées (notamment PB14/UCPD selon révision MB1549) ; ne pas déplacer de straps implicitement. Capteur/FRAM/RS485 et autres modules déconnectés du **second** banc par l'utilisateur. Ne pas utiliser le support TR2 principal.

## Protocole physique à reprendre

1. L'utilisateur connecte uniquement le second banc et confirme câblage, alimentation, média et huit secteurs sacrifiables sans donnée à préserver. Pas de formatage implicite. Relever SN/UID réels et Flash actuelle ; conserver son origine/empreinte et appliquer la règle secondeNUCLEO en vigueur dans son contrat, sans restauration automatique d'un firmware ancien.
2. Préflight : absence de session concurrente, SDK/source/BIN rehashés, UID attendu, voie flash qualifiée SN explicite/0x08000000/verify/reset uniquement. En cas d'incertitude conserver Flash/sauvegarde vérifiée avant programmation. Aucun flash principal.
3. Image readonly d'abord : observer stage2, initOK, CID, capacité512octets, ClockKernel/CLKCR/bus4bit et CMD13/R1. Un HAL_Init OK doit réellement correspondre à WIDBUS4bit (ACMD6 géré par HAL). Aucun write. `bench_clock_hz` est calculé à partir du registre/HAL ; **mesure électrique CLK** distincte à rapporter si disponible, jamais inventée.
4. Construire ensuite l'image write avec LBA explicitement autorisé et distinct des données protégées. Réexaminer CID/capacité/bus avant trigger ; sauvegarder et comparer la zone et les secteurs voisins en lecture si des données voisines doivent être préservées. Renseigner les quatre `bench_expected_cid[i]` depuis le CID lu puis `bench_trigger=0x4D313642` une seule fois à stage2 ; debugger détaché pendant l'essai, aucune pause DMA.
5. Trois rounds bornés dans huit secteurs, pattern différent par round, relecture intégrale et oracle **octet par octet**. Conserver stage/phase/resultats, rounds, mismatch, HALerror, CMD13/R1, DMAerror, timingswrite/read/total (millisecondes ; zéro = sous résolution, pas débit infini), CLKCR/CID/SN/UID/BIN/codeSHA et registresfault. Stage4 + rounds3 + mismatchFFFFFFFF + sans erreur = démonstration bornée uniquement. Stage99 refuse/échoue, pas de retry automatique.
6. En échec conserver les traces avant autres commandes carte, déterminer la première phase fautive. Une nouvelle hypothèse doit précéder tout changement. En succès comparaison des différences DMA/IRQ/clockedge/kernel/bus/alimentation avec TR2, proposition minimale pour mission suivante ; aucune intégration/flash TR2 ici. Laisser l'image isolée selon règle secondeNUCLEO, sans restauration non autorisée. Journal/rapport/proofs/push et relecture distante.

Préconditions actuelles non satisfaites : aucun ST-LINK détecté et montage/média/zone non confirmés. Aucun essai physique exécuté pour cette mission.

Correction de qualification physique : le premier essai à un seul node a échoué (CRC données/commande précédente). Le HAL initialize IDMALAR avec ULA et sizeof(node) et prépare donc head+12 ; adaptation revenue aux deux nodes contigus ST, liste finie sans modecirculaire. Aucune modification de HAL, clock ou bus. Essais et preuves distincts dans rapport de mission PHYSICAL.
