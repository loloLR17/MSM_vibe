---
mission_id: TR2-20261013-M016-GPIO-DAT123-UNIT
status: IN_PROGRESS
base_ref: main
base_sha: 025e46367a2185401d47866f5cfe790936f1ec03
initial_head: 5e72a70b5ee220bb564e481fc5fc96826e022243
result_sha: 9b0ad67e8a981226d12fc48addf45ce35489e225
created_at_utc: 2026-10-13T21:24:05.686039+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — GPIO DAT1/DAT2/DAT3, checkpoint de mesure

Projet ST isolé préparé, cross-build réussi et démarrage sur seconde NUCLEO vérifié. **Mesures en cours**, aucune nouvelle qualification SDMMC. MicroSD retirée et lignes libres confirmées explicitement par l'utilisateur avant tout flash ou pilotage. Aucun essai SDMMC repris, aucun accès au TR2 principal, FRAM, Modbus ou média.

Référence officielle exacte : NUCLEO-U575ZI-Q/GPIO_IOToggle, CubeU5v1.9.0/d88042df. Originaux main/MSP/README et licences SLA0044/HAL BSD-3-Clause conservés et hashés. Adaptation minimale : PB15/PB3/PB4 au lieu des LEDs, latch bas avant OUTPUT_PP, GPIO faible vitesse, pas de pull logiciel, phases stables commandées viaRAM, minimum20s entre phases, timeouthaut300s de tickHAL→arrêt bas, STOPimmédiat. UIDseconde carte contrôlé avant_GPIO ; tracePB3 active refusée. Aucun driverSD/DMA/UART/SPI utilisé. [Projet et protocole](../../../experiments/gpio_dat123_st_reference/README.md).

Sonde0032001F3235510C37333439/V3J15M7, UIDGDB001B0046/38425018/20333157. Flashsecondeseulement, programmationnormale/verify et readbackintégral ; pasmass erase/OB/RDP/sauvegarde2MiB ni restauration historique de ce bancjetable. BINfinal 8732octets, SHA256 `90f3b24cc36f854669fbd251ca1d00f041475e274f40a72e2ebd47b7bbb73b4a` ; ELF/MAP/configurations et images danspreuves. Deuxcross-builds isolés (initial puiscorrectionMSP), pas de tests/cross-builds anciens rejoués. ELFfinal sansHAL_SD/HAL_DMA/FRAM/IIS3DWB/Modbus ; RCC_AHB2ENR1=C0000002, bitsSDMMC1/2 off.

Erreur de portage corrigée : HAL_MspInit du fichier officiel activaitPWRclock, omission dansnotre premierport. Initiale39b1d6 s'arrêtegpio_error2/stage99 **avant GPIO_initialized**, PWRclockoff, donc ses lecturesGPIOclockoff ne sontpas interprétées comme sorties réelles. Ajout de l'activationPWR exacteST, code9b0ad67 ; nouveauflash nécessaire, stage2/error0/PWR_UCPDR1 observés. Les échecs et logsinitiaux conservés. Le stdout du scriptfixed imprimait encorel'ancienfilename ; les nouvellespreuves GDBfixed et leursguards établissent les résultats réels, sansrelancer leurvalidation.

## Observations et mesures rapportées

|Phase|État attendu D1/DAT2/D3|ContrôleMCU|Mesure utilisateur au breakout|
|---|---|---|---|
|0 tout-bas|0/0/0V|ODR=IDR=0000, MODEOUTPUT_PP, error0|D1=0V, DAT2=0V, D3=0V|
|1 DAT1 haut|~3,3/0/0V|ODR=IDR=8000, phase1/error0|D1=3,3V, DAT2=0V, D3=0V|
|2 tout-bas|0/0/0V|ODR=IDR=0000, phase2/error0|pas de mesure supplémentaire requise|
|3 DAT2 haut|0/~3,3/0V|contrôle en cours|PENDING|
|4 tout-bas|0/0/0V|à faire|pas de mesure supplémentaire prévue|
|5 DAT3 haut|0/0/~3,3V|à faire|PENDING|
|6 arrêt bas|0/0/0V|à faire|état final à contrôler|

Mesures **rapportées par l'utilisateur**, BM869s en tensionDC, noirGND breakout ; précision/résolution non spécifiée, aucune décimale ajoutée. Le contrôleMCU n'estpas une preuve de continuité externe ; concordanceDC statique1ligne observée, pas une mesureohmique ni qualification4bits dynamique. Pointe rouge D1/DAT2/D3 successivement, modeVDC/entréeV (jamaisA oucontinuité sous tension), sondesisolées si risquecourt-circuit. MicroSD doitresterretirée tantquefirmwareGPIO présent, ycompris étatbas.

## Références physiques et préservation

[ST UM2861 Rev10 table18](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf) : PB15/CN12-26, PB3/CN12-31, PB4/CN12-27 ; [schémaC05](https://www.st.com/resource/en/schematic_pack/mb1549-u575ziq-c05-schematic.pdf), partagéPB15/UCPD_CC2 et debugPB3/PB4 ; pulls/rail3,3V du [breakoutAdafruit4682](https://learn.adafruit.com/adafruit-microsd-spi-sdio/pinouts). Étatjumpers/révision réels non inspecté indépendamment ; lignes libres confirméesutilisateur, aucun strap déplacé. Les tableaux du manuel présentent une incohérenceJP7/JP8 CC1/CC2 selon section ; pas de modification de jumper fondée sur cetteambiguïté. GardePWRdeadbattery/trace et mesures réelles priment pour ce test isolé ; nepas transformer cettelecture eninspection physique.

Ancienne missionSDMMC-PHYSICAL archivée byte-identique à025e463, preuves/échecs conservés. PortageSD nonmodifié. JournalETAT_COURANT_TR2 actualisé pour préparationGPIO/SDretirée/mesures acquises. Principalnonaccédé : dernierprobeSTORAGE2 historique conservé, cumulatifA04-02 nonrestauré, aucun état actuel relu. GelsV1, gouvernance, mission et dettesA04inchangés ; stashes6251b988/f3b829e6 et tousworktreespréservés.

## Git et reprise

Worktreeisolé `MSM_vibe-M016-gpio-dat123` àpartir de5e72a70, branchecodex/m016-gpio-dat123 ; dépôtoriginal et anciensworktrees inchangés. Commitslogiciel e39b1d6 (portinitial) puis9b0ad67 (MSPcorrectif). Checkpointreport/proofs puispushnormalHEAD:refs/heads/main, fetch et relectureintégrale. Permissionsexercées : développement/build/docs, flash/debugcibleautorisée, commit/pushmain ; aucuneextension d'autorisation.

[Preuves](preuves/TR2-20261013-M016-GPIO-DAT123/artifacts-pwr-fixed.json), logs/scripts et manifeste adjacents ; local `/home/lolo/dev/msm/reports/TR2-M016-GPIO-DAT123-20261013T211410Z`. Reste : DAT2haut et DAT3haut avecmesuresDC rapportées et retourfinalbas ; publication et relecture du rapportfinal. La collecte exige des gestes physiques utilisateur (AGENTS§9), pas une validationintermédiaire d'opérationlogicielle.
