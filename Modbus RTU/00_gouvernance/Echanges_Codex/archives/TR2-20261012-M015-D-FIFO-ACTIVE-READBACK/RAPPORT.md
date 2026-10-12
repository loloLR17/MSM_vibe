---
mission_id: TR2-20261012-M015-D-FIFO-ACTIVE-READBACK
status: BLOCKED
base_ref: main
base_sha: 37b97f7fd8ea70c4ae8e377a3042bb3e2b8c5328
initial_head: 1ba8a5f7816eba37b9c45a4a1f7ff43cc3add178
result_sha: ca00da976de487ff6fd52fbd3973af105f07876a
created_at_utc: 2026-10-12T14:05:27.347172+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-D — Readback single CTRL2 non fiable pendant remplissage

**BLOCKED pour cause précise / fiabilité FIFO active complète.** Le défaut est reproduit et localisé à un contexte de lecture, mais son origine physique reste inconnue. M015 globale IN PROGRESS. Tranches audit, harness testé, campagne unique, restauration et contrôle minimal exécutées ; aucune campagne A/B/C ni validation inchangée rejouée.

## Constat discriminant

Code évalué **ca00da976de487ff6fd52fbd3973af105f07876a**, mode3/625kHz/HCLK160MHz. Une campagne,468transactions,28groupes alimentés en10ms puis32groupes figés. Aucun XYZ ou FIFO_DATA_OUT lu.

| Lecture | FIFO alimentée | FIFO conservée, entrée coupée |
|---|---|---|
|FIFO_CTRL2 single08|80×23 ;81×2 ;02×2 ;00×1|81×32|
|FIFO_CTRL2 dans burst07..0A|81×28|81×32|
|WHO /CTRL3 singles|7B /44,28fois chacun|7B /44,32fois chacun|
|Statuts3A/3B groupés|28niveaux monotones1→256|A100,32fois ; singles concordants32/32|

**26/28 divergences single↔burst durant remplissage,0/32 après arrêt de l’entrée.** Aucun writeCTRL2 entre les comparaisons ; configurationburst00/81/0A/01 stable. Autour du bit7 :127groupé→129en singles→137groupé ; aucun recul.128exact n’est pas échantillonné. OctetscommandeRX00/FF conservés mais non interprétés en données. CSV complet etcomparaisons indexées publiés.

HALstatus/error/RxCount/TxCount=0 etCSmarqueurs1→0→1 sur468transactions ; CFG1=70070067/CFG2=87400000, faults0. Ces faits n’établissent ni frontsCS/SCK ni exactitudewire ou flagsHAL internes. Le résultat enRAM différant seloncontexte ne démontre pas un défaut ducapteur.

**Limites** :Bfastpoll54,725µs entrestatuts, D364,575µs médian ; interleaving et writesduplex changent lecontexte. Pas comparaison àvariableunique, pas preuve du mécanisme des11reculsB. A100observé enD n’a pas bitsOVR_IA/LATCHED ; aucune qualification de l’overrunB. Pas cadenceODR/consecutivité/pertesvalidées. Ne pas attribuer au voilier un readback deconfiguration incorrect, ni remplacer heuristiquement80par81. Aucun correctif oucontournement deproduction appliqué.

## Tranche logicielle et validations

Audit ettable hypothèses/discriminants : `../AUDIT_M015_D_FIFO_ACTIVE.md`, référencesST héritées/relues pourFIFO/STOP_ON_WTM/BDU/framing. Nouveau `stm32_iis3dwb_active_readback.c/.h`, CMake et branchemain exclusifs/OFF par défaut ; driver/cœur/M014inchangés. Triggerunique4D313544,1024records×80octets, activité15ms/borne700préserve espace de restauration. Sept contrôles volatils sauvegardés ; configurationFIFO1/STOP256/BDR26.667/BDU/±4g,100msstabilisation enbypass. CoupureCTRL1 sansvidange puis32groupes figés ; retour exact des7contrôles, restauration même enéchec. Pas DMA/acquisitioncontinue/FFT/calibration/B5/SD/FRAM/UART.

Test neuf seulement :CMakehost/build `test_stm32_active_readback`, `ctest -R '^stm32_active_readback$' --output-on-failure` **1/1passé**,4scénarios nominal/injectionbit7tracepréservée/mismatchconfigurationrefusé+restauré/préexistantsanswrite ; pasDATA_OUT,CS/borne/uneactivation. Échec initialmock :128atteint enlecture simple plutôt queburst ; injectionpremierburst>=128 etassertionindexexact corrigées, sans modifier un résultatphysique ni affaiblir validation. Sortie d’échec conservée.

Cross-build défaut via `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only` etprofilDebugNinja/toolchainARM avec `-DTR2_STM32_ACTIVE_READBACK=ON` réussis. Text32136/data20/bss85460,BIN32156octets ; warningsNewlib habituels. Symboles média/production/UARTstart/acquisitions absents. Justification :seulharness dédié, pas régressiondu cœurplausible ; testsA/B/C/114firmware/539.NET nonrejoués. Guards nouveau nomHP `physical-m015-d-restored` Steadyseulement etparseurPS réussis sansport ; ancienpaquet.NET inchangé.

## Sauvegarde et restauration M014

CibleUID004A003D3842501820333157, STLINK003500463235510B37333439/V3J15M7/NUCLEO-U575ZI-Q,U575/U5850x482RevU/3,29V vérifiés. GDB initial reconnaît M014,FRAM génération12/IRQ49-66/configUNAVAILABLE/faults0 ; Flash2Mio etRAM768K sauvegardées/hashées avantflash. VoiequalifiéeNORMAL08000000/verify/reset ;2flashsstandard seulement, diagnostic puisrestauration, secteurs0..3 puis0..17. AucunHOTPLUG/masserase/OptionBytes/RDP/câblage. DernierCLI suivi dereadbackGDB/reset qualifié, ensuiteGDBseul ; debugger détaché durantcampagne, dumps avantrestauration, restaurationenfinally.

Imagepermanente M014-H/source0b2a901e185c9cbf8c02410bf2a29a0a3a75491e conservée. BIN143108 SHA **b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611**, ELF c37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de. **Flash2Mio entièrement byte-identique àl’initiale**, SHA556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0. Contrôlescapteurrelus restaurés0/0/0/0/0/4/0, restore_result0 ; cette vérification reste un readback, avec limites de chaîne décrites.

ProfilD sansaccèsFRAM/SD ni provisioning ; partitionLBA65536 protégée parabsenceaccès. Bootpermanent FRAMV2/gen12/identité1/configUNAVAILABLE/READY0/stockage0, pas acquisitionactive ni nouvelleimageproduction.

ContrôleminimalHP derepriseuniquement :paquetH371fichiershashésréutilisé,2FC03 B0/21puisB1/20,sansretry,COM7/1152008E1/unit1,retour0/connexionfermée ; gatefraîcheur0..30s calibréeSSHlectureseule, serveursfermés avanttrafic. Journalcomplet7événements **2026-10-12T14:00:13.6675979+00:00→2026-10-12T14:00:14.4393835+00:00**, SHA **8a1aab00264668fdbca38061edf7901e135a0f845cbad2962453a54e2882f507** comparéaudistant,récupéréetlu ;B0tous21registresconformes,B1uptime119s/status1/flags0/stockage0/acq0. Pas conclusionduseulhash etpas requalification M014. GDB finalsansreset :stage2/result0/pollerrors0/FRAMgen12/IRQ49-66/UARTerror0/TXidle/filehead=tail20/faults0 ; MCU détaché running/serveurterminé.

## Publication, limites et suite

Tranchecode/audit/archives publiée au **ca00da976de487ff6fd52fbd3973af105f07876a**,11livrables relus intégralement depuisorigin/main etbyte-identiques (`software-readback.json`). ArchivesC exactesdepuis37b97f7 ; missionDactiveinchangée. Journal actualisé pourdéfautsingle/burst/freeze/restauration ; roadmapM015INPROGRESS.

WorktreeisoléD codex/m015-d, destinationmainexplicite/pushnormal ;original82169af/H762c459/Aeab07d4/Bcfb9918/C37b97f7 et deux stashes préservés. Aucunmerge/rebase/reset/forcepush ni changementpermanent. Permissionscommit/push/flash/debugexercées danscontrat,2lecturesderepriseprévues. SHAfinal etétatGit donnés aprèspublication/relecturedistante, pas hashautoréférent ici.

Preuves publiques `preuves/TR2-20261012-M015-D/` :468TX/RX/CS/DWT/SR/CFG,analysegroupes/figé/127-129,validationsetlogsGDB,journalHPintégral,hachages/restauration. Preuvescomplètes **/home/lolo/dev/msm/reports/TR2-mission015-D-20261012T135359Z** :commandes/CLI/GDB/ELF/BIN/backupsFlash/RAM ;dumps mémoire etbinaires nonpubliés.

**SuiteChatGPT** : lecture singleCTRL2active nonfiable démontrée ; causephysique etmécanismefastpollB nonisolés, aucun deuxièmeessaiD. Pourla cause, comparerwire↔RAM descommandes88/87etBA pendantremplissagepuisfigé, aveccaptureCS/SCK/MOSI/MISO, instrument3,3V/GNDlogiqueidentifié. Protocolegestepréparé dansaudit, nonexécuté ; connexion seulementparutilisateur, pasbranchementsansinstrumentidentifié, nouvelleexécution souscontratdistinct. CibleactuellementM014, aucun essaiouflashàrelancer implicitement. M015-D BLOCKED technique ; publicationacquis ne vaut pasqualificationcomplète.

Publication des résultats vérifiée au **73cbdee155d88580b5db52372c84ac58abe2f40f** :30 livrables intégralement relus depuis la référence récupérée de GitHub, UTF8 et identiques octet par octet. Preuve `publication-readback-73cbdee.json`. Worktree D propre, cinq autres worktrees propres avec HEAD préservés, deux stashes intacts, archives C exactes et mission active inchangée. BLOCKED demeure technique ; la publication est effectuée. Le complément de clôture est vérifié après son push et son SHA est fourni dans la réponse finale.
