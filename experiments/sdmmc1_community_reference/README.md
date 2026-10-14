# P12/H3h — reproduction SDMMC1 communautaire

Projet source externe inchangé : https://github.com/VILLO88/NUCLEOU575/tree/1e3c9183bad023ebf7d234134823d9270a0e8b64. CMake vérifie HEAD exact et absence de modifications. Aucune copie des sources/firmwares tiers dans le dépôt MSM ; bibliothèques du dépôt tiers conservées, sans substitution du SDK MSM. HAL1.6.2, Cubepackage .ioc1.8.0, TinyUSB0.20.1. Licence globale non trouvée ; notices des composants conservées dans le checkout, aucune redistribution de l'image tierce ici. BIN/ELF/MAP locaux et leurs hashes publiés.

## Compilation Linux reproductible

Dans un checkout externe neuf : cloner la référence, sélectionner le commit exact ci-dessus. Ne pas écraser un checkout existant. Depuis la racine de ce worktree :

```sh
cmake -S experiments/sdmmc1_community_reference \
 -B /home/lolo/dev/msm/reports/SDMMC1-PoC-release-build \
 -DCMAKE_TOOLCHAIN_FILE="$PWD/experiments/sdmmc_st_reference/cmake/arm-none-eabi-gcc.cmake" \
 -DREFERENCE_ROOT=/home/lolo/dev/msm/cache/VILLO88-NUCLEOU575-SDMMC1-PoC \
 -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_RELEASE=
cmake --build /home/lolo/dev/msm/reports/SDMMC1-PoC-release-build -j4
```

GCC ARM14.2.1, Cortex-M33/FPUfpv5-sp-d16/ABIhard, `-Os` comme Release .cproject d'origine, `-g3` pour observation ; linker/startup et tous les Core sources inchangés. Debug utilise `-O0`. Flags effectifs conservés avec les preuves. Les sources périphériques HAL restent celles du tiers ; seuls le lanceur Linux et les outils d'observation sont nouveaux. Aucun flash automatique par CMake.

## Cible et protocole

Seconde SN0032001F3235510C37333439 / UID001B0046/38425018/20333157 seulement, jamais principal. Ancienne image M017 vérifiée, retrait SD indiqué au contrat puis insertion hors alimentation confirmée avant flash. Flash normal0x08000000/verify/readback intégral, reset logiciel qualifié après programmateur, GDBobserve HAL_SD_Init et HAL_SD_ConfigWideBusOperation sans modifier leurs arguments. CID/capacité stricts avant write. Original démarre1bit puis demande4bits avec fallback ; hsd1.Init.BusWide restant0 n'est pas la preuve du mode réel : CLKCR4002 et SSRDataBusWidth2 prouvent4bits, kernelHSI48/4=12MHz.

Câblage original PC8/D0,PC9/D1,PC10/D2,PC11/D3,PC12/CLK,PD2/CMD ; usage CN11/CN12 et SB19/SB22 pontés déclaré utilisateur. Brochage et risque de stubs documentés dans UM2861 tables19/20. Aucune soudure/modification imposée. CN15 USB-C natif distinct ST-LINK ; câble validé par Galaxy reconnu Windows puis Nucleo USB0483:572B reconnue. Buffer MSC actuel8192, README tiers4096 périmé.

Tests exécutés : Debug CRC, Release read8192exact puis GDB write16×8192; premiers2chunks relus exacts, CRC au3e. Carte DATA après CRC, récupération par reset d'origine sans paramètre changé. USB rawPhysicalDrive read refusé par droits, partition propre ciblée E: accessible en read ; média RAW sans filesystem, aucun format. Read intégral128KiB du jeuGDB après reset exact. Write natif via FileStream inadéquat rejeté, nouvelle tentative Win32 alignée VirtualAlloc/no-buffering/write-through gardée sur même partition,16×8192 et flush/relecture entière128KiB exacts après récupération de NoMedia. Aucun retry caché, aucune élévation/ACL ou configuration permanente modifiée. Les erreurs restent dans les preuves.

Les tests temporels viaGDB ne sont pas des benchmarks ; chrono hôte natif sansGDB seulement pour petits accès, aucune qualification de cadence/7jours. Dernier état : USB reconnu mais SDerror2005/R1B00, WindowsNoMedia revient. Image tierce Release laissée, debugger détaché ; pas de restaurationGPIO/SDMMC2. PoC fonctionnel par épisodes, aucune fiabilité durable établie ; le contrat interdit de modifier SD/code/bus pour masquer cette limite.
