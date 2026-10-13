# M016 — GPIO statique DAT1/DAT2/DAT3, seconde NUCLEO uniquement

Référence officielle exacte : [STM32CubeU5 v1.9.0 / NUCLEO-U575ZI-Q / GPIO_IOToggle](https://github.com/STMicroelectronics/STM32CubeU5/tree/v1.9.0/Projects/NUCLEO-U575ZI-Q/Examples/GPIO/GPIO_IOToggle), SDK d88042df. Originaux/licences SLA0044 et HAL BSD-3-Clause conservés dans vendor/, hashes dans SOURCES.json ; HAL/CMSIS externes inchangés.

Adaptations : LED1/LED2 remplacées par PB15/PB3/PB4 seulement, faible vitesse GPIO, pas de pull logiciel, latch préchargé bas avant OUTPUT_PP. Fonction d'horloge ST conservée (CPU160MHz), SMPS/dead battery comme ST, ICACHE/BSP LEDs omis. Pas de SDMMC/DMA/UART/FRAM/capteur. UID exact seconde carte vérifié avant GPIO, sortie trace PB3 refusée si active. Aucune modification du portage SD.

**Précondition physique obligatoire avant flash/commutation :** alimentation coupée pour retirer la microSD, carte retirée, lignes sans autre pilotage, banc ensuite alimenté et confirmation utilisateur. Ne pas insérer de microSD tant que ce firmware configure ces lignes en sorties, y compris lorsque toutes sont basses. Aucun circuit principal utilisé.

## Build

```sh
cmake -S . -B /home/lolo/dev/msm/cache/gpio-dat123 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake \
  -DSTM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build /home/lolo/dev/msm/cache/gpio-dat123 -j4
```

Flash qualifié SN0032001F3235510C37333439 uniquement, UID001B0046/38425018/20333157 ; primaire refusée. Flash seconde jetable selon décision M015-M, sans mass erase/OB. Rehash BIN/code, verify et readback intégral. Aucun retour automatique à un ancien firmware SD/capteur.

## Séquence stable, à commander par Codex

|Phase|Commande RAM après phase précédente|D1 PB15/CN12-26|DAT2 PB3/CN12-31|D3 PB4/CN12-27|
|---|---|---|---|---|
|0 boot tout bas|aucune|0 V|0 V|0 V|
|1 DAT1 haut|47310001|~3,3 V|0 V|0 V|
|2 tout bas|47310002|0 V|0 V|0 V|
|3 DAT2 haut|47310003|0 V|~3,3 V|0 V|
|4 tout bas|47310004|0 V|0 V|0 V|
|5 DAT3 haut|47310005|0 V|0 V|~3,3 V|
|6 arrêt tout bas|47310006|0 V|0 V|0 V|

Commande `gpio_command` acceptée seulement dans l'ordre et après au moins20 secondes de phase ; pas d'auto-avancement vers un haut. Un haut expire vers arrêt/tout-bas après300 secondes de tickHAL (temps GDB halted exclu), aucune boucle répétée. STOP4731FFFF immédiat ; commande invalide conduit erreur/tout-bas. Reset recommence uniquement à tout-bas. Observer gpio_stage/phase/output et GPIOB ODR/IDR/MODER/OTYPER/PUPDR ; RCC SDMMC1/2 clocks doivent être off. États logiciels ne prouvent pas la tension au breakout.

## Mesure utilisateur au Brymen BM869s

Référence physique : [ST UM2861 Rev10 table18](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf), pinout H3h-B ; [Adafruit4682](https://learn.adafruit.com/adafruit-microsd-spi-sdio/pinouts). Masse commune, 3V3 ; PB15 partage UCPD CC2/JP7 selon révision, PB3/PB4 fonctions debug alternatives : vérifier absence de charge/strap avant pilotage. Ne déplacer aucun strap implicitement.

Mode **tension DC V⎓**, noir en COM, rouge en entréeV/Ω (pasA/mA), pointe noire fixée au GND breakout. Pointe rouge successivement D1, DAT2, D3, sans toucher les broches voisines. Pas d'ohmmètre/continuité sur le banc alimenté ; pas d'injection externe3,3V. Si pointes trop larges : arrêter et utiliser des pinces/sondes isolées adaptées.

Codex annonce la phase réellement observée, puis collecte les trois tensions. Exemple de réponse : `D1=3,29 V ; DAT2=0,003 V ; D3=0,002 V`. Bas proche0V, haut prochealimentation3,3V ; valeurs indicatives, pas calibration de l'instrument. Mesurer phase0 puis1/3/5 ; phases2/4/6 de repos contrôlées par Codex. Rester sur la phase annoncée ; si elle a expiré, aucune valeur n'est attribuée au haut sans vérifier l'état. Rapport distingue registreMCU, tensions rapportées et limite : fonctionnement statique ne qualifie pas SDMMC4bits dynamique.
