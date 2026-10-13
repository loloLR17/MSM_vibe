---
mission_id: TR2-20261013-M016-GPIO-DAT123-UNIT
status: IN_PROGRESS
base_ref: main
base_sha: 025e46367a2185401d47866f5cfe790936f1ec03
initial_head: 5e72a70b5ee220bb564e481fc5fc96826e022243
result_sha: 9b0ad67e8a981226d12fc48addf45ce35489e225
created_at_utc: 2026-10-13T21:30:46.032203+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — Test GPIO statique DAT1/DAT2/DAT3 terminé

**Les trois lignes reproduisent au breakout les niveaux DC attendus**, selon les mesures rapportées par l'utilisateur au BM869s. Les sept phases ont été contrôlées sur le MCU ; le banc est laissé à l'arrêt, toutes sorties basses. Statut provisoire IN_PROGRESS uniquement pour publication/relecture, puis DONE dans cette portée GPIO statique. **Aucune qualification dynamique SDMMC4bits n'en découle.** Aucun essai SDMMC répété ni portage SD modifié.

## Préconditions et identités

Avant tout flash ou sortie, l'utilisateur a confirmé explicitement : « microSD retirée, lignes libres, banc remis sous tension ». Les manipulations physiques restent à l'utilisateur ; aucune injection externe3,3V ni mesure de continuité sous tension demandée.

Seconde NUCLEO-U575ZI-Q seule sélectionnée : ST-LINK **0032001F3235510C37333439**, V3J15M7 ; UID GDB **001B0046/38425018/20333157** vérifié avant chaque programmation. Principal003500463235510B37333439/UID004A003D jamais sélectionné. Deux flashes standard après contrôle d'image, CLIverify et comparaison GDB du BIN intégral ; pas de mass erase/OB/RDP. Flash secondaire jetable selon décision permanente M015-M§4, donc aucune sauvegarde2MiB ou restauration historique répétitive.

## Référence et adaptation

Projet indépendant [experiments/gpio_dat123_st_reference](../../../experiments/gpio_dat123_st_reference/README.md), basé sur l'exemple exact [ST NUCLEO-U575ZI-Q/GPIO_IOToggle, CubeU5v1.9.0](https://github.com/STMicroelectronics/STM32CubeU5/tree/v1.9.0/Projects/NUCLEO-U575ZI-Q/Examples/GPIO/GPIO_IOToggle), SDK d88042df. Originaux main/MSP/README, licences exemples SLA0044 et HAL BSD-3-Clause conservés, hashes dans SOURCES.json ; HAL/CMSIS externes inchangés. L'adaptation n'est pas présentée comme validée par ST.

Adaptations minimales : LEDs remplacées par **PB15/PB3/PB4 uniquement**, latch bas avant OUTPUT_PP, faible vitesse, aucun pull logiciel ; horloge ST160MHz et initializationPWR conservées, BSP LEDs/ICACHE omis. Aucun SDMMC/UART/SPI/FRAM/capteur activé. Avancement RAM explicite dans l'ordre, minimum20s par phase, pas d'auto-avancement vers un haut. Un haut expire vers arrêt bas après300s de tickHAL (pauseGDB exclue), STOP immédiat, commande invalide→erreur/bas, UID étranger ou tracePB3 active refusés. Expiration/commande invalide revues dans le code, pas déclarées physiquement testées.

Brochage confirmé par [UM2861 Rev10 table18](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf) : PB15/CN12-26→D1, PB3/CN12-31→DAT2, PB4/CN12-27→D3. [SchémaMB1549 C05](https://www.st.com/resource/en/schematic_pack/mb1549-u575ziq-c05-schematic.pdf) consulté pour fonctions partagées, notamment PB15/UCPD_CC2 et debugPB3/PB4. Révision/jumpers du banc non inspectés indépendamment ; aucune modification de strap. Lignes libres confirmées utilisateur, garde trace/PWR et mesures décrites ci-dessous. [Adafruit4682](https://learn.adafruit.com/adafruit-microsd-spi-sdio/pinouts) documente rail3,3V/GND et pull-ups. Pas de changement silencieux du pinout gelé.

## Mesures acquises

Méthode communiquée : BM869s en **tension DC**, noirCOM sur GND breakout, rouge sur D1/DAT2/D3 successivement, entréeV (pasA/mA). Pas d'ohmmètre/continuité sur banc alimenté ; sondes isolées adaptées si risque de toucher deux broches. Valeurs **rapportées par l'utilisateur**, pas mesurées par Codex ; précision/résolution/calibration non spécifiées.

| Phase | Sorties attendues D1/DAT2/D3 | Mesures utilisateur au breakout | ODR/IDR MCU |
|---|---|---|---|
|0 tout-bas|0/0/0V|**0/0/0V**|0000/0000|
|1 DAT1 haut|3,3/0/0V|**3,3/0/0V**|8000/8000|
|2 tout-bas|0/0/0V|repos contrôlé MCU, pas de mesure supplémentaire|0000/0000|
|3 DAT2 haut|0/3,3/0V|**0/3,3/0V**|0008/0008|
|4 tout-bas|0/0/0V|repos contrôlé MCU, pas de mesure supplémentaire|0000/0000|
|5 DAT3 haut|0/0/3,3V|**0/0/3,3V**|0010/0010|
|6 arrêt tout-bas|0/0/0V|état final contrôlé MCU|0000/0000|

GPIOB_MODER=7FFFFD7F, OTYPER=0, PUPDR=0 : sorties push-pull prévues. RCC_AHB2ENR1=C0000002 dans chaque relevé, bitsSDMMC1/2 désactivés. Analyse hors ligne des ticks de début phase1→6 : intervalles **80498, 54285, 149259, 48433, 111628ms**, minimum **48433ms**, supérieur aux20s demandées. Aucun haut simultané observé ; phases basses intercalées. Les réponses utilisateur ont été associées aux phases annoncées avant leur expiration.

**Conclusion proportionnée :** concordance statique de chacun des trois chemins GPIO→breakout étayée par étatsMCU et mesuresDC rapportées. Pas de mesure ohmique de continuité, de caractérisation des fronts, de test avec carte insérée ou de validation SDMMC4bits. Le précédent échec CRC4bit n'est pas expliqué par ce seul résultat ; pas de conclusion automatique « câblage parfait à24MHz ».

## Erreur corrigée et validations

Port initial e39b1d6 : activation de l'horlogePWR du MSP officiel omise. gpio_error2/stage99 avant GPIO_initialized et avant toute phase haute ; lecturesGPIO avec horloge désactivée non utilisées comme preuve de sortie. Correction exacte HAL_MspInit/__HAL_RCC_PWR_CLK_ENABLE, code **9b0ad67e8a981226d12fc48addf45ce35489e225** ; nouveau cross-build et flash justifiés. Démarrage corrigé stage2/error0, PWR_UCPDR1, tracePB3 off. Première tentative et logs conservés. Le scriptfixed affichait en stdout l'ancien log ; les logs GDBfixed et guards fournissent la preuve réelle, sans répéter leur validation.

Deux cross-builds isolés réussis, GCC ARM14.2.1/O2/g3 ; pas de compilation ou tests anciens rejoués. Validation proportionnée au harness autonome, sans interface fonctionnelle partagée ni firmwareproduction modifié. ELFfinal sans HAL_SD/HAL_DMA/FRAM/IIS3DWB/Modbus ; contrôle physique des registres RCC complète la vérification de non-activation SDMMC. Aucune compilation assimilée à une preuve de tension au breakout.

BIN exécuté final : **8732octets**, SHA256 **90f3b24cc36f854669fbd251ca1d00f041475e274f40a72e2ebd47b7bbb73b4a**. ELF/MAP/hashes dans [artifacts-pwr-fixed.json](preuves/TR2-20261013-M016-GPIO-DAT123/artifacts-pwr-fixed.json). Logs de build, CLI/GDB, scripts réellement utilisés, imageshex initiale/finale, réponses utilisateur et analyses indépendantes dans [preuves](preuves/TR2-20261013-M016-GPIO-DAT123/gpio-phase-analysis.json). Racine locale : `/home/lolo/dev/msm/reports/TR2-M016-GPIO-DAT123-20261013T211410Z`. Aucun test supprimé ou résultat affaibli.

## État final, Git et publication

Seconde carte : firmware GPIO9b0ad67, **stage4/phase6, command0/error0, ODR=IDR=0 sur les trois lignes**, SDMMCoff, debugger détaché. Sorties restent configuréesGPIO : **ne pas réinsérer la microSD tant que ce firmware est présent**, même tout-bas. Aucun retour automatique au firmware SD/capteur. Reset recommence uniquement à tout-bas.

Principal totalement non accédé : dernier probeSTORAGE2 historique/code38c0273 conservé, cumulatifA04-02 non restauré, pas de nouvelle lecture de son état. FRAM, médias, spécificationsV1 et dettesA04 inchangés ; aucun Modbus/essaiSDMMC dans cette mission. Contrat/rapport SDMMC-PHYSICAL archivés byte-identiques à025e463. Journal ETAT_COURANT_TR2 actualisé : SDretirée, test GPIO et mesures DC, fin tout-bas, limite dynamique.

Dépôt original propre à82169af ; worktrees historiques et stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2/f3b829e6188f491920edf74a5f9ed0f559d08b87 conservés. Worktree `MSM_vibe-M016-gpio-dat123`, branchecodex/m016-gpio-dat123 basée5e72a70 ; commits e39b1d6 (initial),9b0ad67 (MSPcorrectif),80291b68db8cde30ffe72fc05761c59658d8681b (checkpoint). Ce checkpoint a été poussé et relu intégralement :88fichiers byte-identiques, manifeste vérifié. Résultats finaux publiés parpushnormalHEAD:refs/heads/main puisfetch/relecture ; miseàjourDONE après cettepreuve. Aucunforce/merge/rebase/reset/clean ; aucun changement de gouvernance ou contrat. SHAfinal fourni dans réponseCodex.

**À transmettre à ChatGPT :** mesures statiques conformes pour les trois lignes ; examiner les preuves et décider de la prochaine investigation dynamique. La mission GPIO peut être clôturée dans sa portée ; stockageV1, CRC4bit, production et restauration du principal restent distincts. Aucunequalification indépendante ni tag créé par Codex.
