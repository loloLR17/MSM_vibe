---
mission_id: TR2-20261012-M015-J-CONTINUOUS-FIFO
status: BLOCKED
base_ref: main
base_sha: d25aa42a3d2c4850f4cedeeaca545da63aad0c29
initial_head: de173b229d4a2cbaf54aa9f9f208cd186443f73d
result_sha: 24ccec00698cd2e7f55297acc2bad0d22746e744
created_at_utc: 2026-10-12T19:00:28.025389+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-J — Arrêt après premier burst ; fenêtre4096 non qualifiée

**BLOCKED matériel : l’unique campagne est arrêtée au readback groupé postpremierburst, FIFO_CTRL3/09 attendu0A/reçu14.** Le mêmeRX contientFIFO_CTRL4/0A attendu06/reçu0C. Garde intact, fenêtreINVALID/0XYZaccepté ; aucunsecondessai. M014-H restauré àl’identique et repriseHP vérifiée. M015globaleINPROGRESS, acquisI256conservés sansrejeu.

## Tranche logicielle validée

Nouveau harness `stm32_iis3dwb_continuous.c/.h`, profil OFF/exclusif `TR2_STM32_CONTINUOUS_FIFO`, branchemain/CMake ettestmock. Types etgroupes I réutilisés, WHO7B/CTRL1A8/IF_INC/CTRL4=0, gardesHAL/error/compteurs/CS1→0→1 ; aucunreadsimple/vote/fallback/substitution. FIFOcontinu mode 6, watermark128/STOP_ON_WTM0/BDR0A, A8 maintenu pendantvidange ; contrôlesgroupés avant/aprèschaqueburst. Une fenêtre4096XYZownedRAM,32bursts128 maximum,192statuts/384traces,250 ms, stop explicite, EMPTY→ARMED→FILLING→COMPLETE ouINVALID. Aucunarrêtentrée avantfinfenêtre réussie ; sortie enrestauration aufirstrefus. Niveaux/flags FULL-OVR-latched/headroom128 surFIFO512, tags/parité/counter etindexRAM contrôlés. Conservation `niveauaprès−niveauavant+128` dansenveloppe annoncée ±5%ODRconfigurée/+2 mots phase, positive ; c’est untestcohérence, pasmesure indépendanteODR.

[Protocole/RAM/limites](../QUALIFICATION_M015_J_FIFO_CONTINUE.md) :RAM786432, XYZ24576/tags4096/raw28704/trace 35328/polls3072/meta2688. `.bss 101536/.data 12`, GNUsizebss104100/data 20 (réservesheap/stack/initarray), stackmin2048/heap512 etBSS→top684884 octets. Pasheapalloué, buffersstatiques exclusifs, prologues run456/io152/controls40/HALSPI64 ; pasmesurehighwaterpile. Rétentiondiagnostique complète×8 pour32768 =787712 octets horsautresglobals/stack, ne tientpas ; XYZ/tags32768 seuls229376 tiennent. Propositionfuture segmented/ring2×128/rawcompact/ownedstockage souscontratdistinct, pasmodificationfenêtresV1 niécrituremédia. InterfacesAcquisitionService/VibrationSource réelles inspectées, nonraccordées àcenouveauRAMdiag etinchangées. Pasrefactoringcore/mapping/validator/V1.

**Validations effectives** :initial18 scénarios ciblés/CTest1/1 etcrossdéfaut/profilDebugNinja réussis, text 37248/data 20/bss 104100/BIN 37268 ; warningsNewlibusuels. Nominalproducteuractif/window4096, incompletSPI, niveaux/overrun/timeout, absenceproduction, parité/gap, stop, freezeproducerpendantlecture refusé, backpressure, wrapDWT, RAMlimit, config/WHO/NVIC/DWT/deadline. Profilconcurrentrefusé ;symbmedia/production/UARTstart/anciensprobesabsents. WrapperHPJSteady-only/deux FC03, rejetsPrepare/Offline/Physical/parserPS validés sansport.

Après refusphysique, **un testhôte ajouté** simuleRXpostburst0A→14/06→0C, vérifieburst1/wordcount0/INVALID/restore 0/mask 23 :19 scénarios/1-1passés. Relance justifiée parcette nouvelle régression, aucunfirmwaremodifié niARMrebâti niessaimatérielrépété. TestI/Hinchangés etcampagnes antérieures nonrejoués. Choixproportionné :harness/mainisolé exclusivement ;codeportableV1 etinterfacesprodintacts.

Checkpointlogiciel **24ccec00698cd2e7f55297acc2bad0d22746e744**, `M015-J: add bounded live FIFO service and 4096-word window guards`, poussé et27 livrables entièrementrelus byte-identiques avantcampagne. BIN SHA **39534853cf49c1212f9a82510cd559f16cd19334b67fee94a50296e5827094f4** ; ELF **e2e33f06916fe1fca5754e009d2a38093de76e874396af888375cd1fc362d742**. Sourcecible inchangée ensuite ;seul testde régressionhôte/docs ajoutés.

## Observation de l’unique campagne

| Étape observée | Données |
|---|---|
| Préparation |07..0A=80/00/0A/00, 0F..14=7B/A8/00/44/00/00 |
| Avantpremierburst actif |trace 17 RX00 80 00 0A 06 ;trace 18 CTRL1A8/CTRL344/WHO7B conformes |
| LectureFIFO |unetransaction897 octets/128 mots reçus,HAL_OK/remaining0/CS1→0→1 |
| Niveaux |161→118 aprèsretirer128, productionobservée85, garde79..94 satisfaite |
| Readbackpostburst |**trace 25 TX87 00 00 00 00/RX00 80 00 14 0C** ;trace 26WHO7B/A8/44 conformes |
| Refusimmuable |phase 3/reason2/reg09/expected0A/observed14,validmask15, HALstatus0/error0/Rxleft0/Txleft0 |
| Fenêtre |stage 10/result 9/restore 0/windowstate4/accepted 0/wordcount0,burstcount1/pollcount6/lossflags4 |

Premierécart exact09=14, puis0A=0C visible dansmêmeframe ; ni écriturecorrective, fallback ou nouvellevariante. Lesdeuxoctets sont doublesdeleursattendus, observation arithmétique ne démontrantpas un décalageglobal ni causeSPI/HAL/CS/capteur. Octetwatermark80 reste80 ;pasmodèlecausalinventé, nepasrectifier lesbytes. Cause observable : readback de configuration incohérent après vidange active. La valeur réelle des registres du capteur n’est pas établie par une mesure indépendante. Lescauseprécise etfenêtre4096 ne sontpasqualifiées.

**Attentiontélémétrie** :`configured[]` est partiellementécrasé avantbreaksur09 ; FIFO_CTRL4 reste06 de la lectureantérieure danscetableau, alorsque ledernierraw25 contient0C. Lesbruts constituentla dernièreobservation. GlobalXYZ/tags4096 demeurentBSSàzéro, nonmesures ;first_failure_wordFFFFFFFF/capture_end0 nonrenseignés carparserXYZ etfinfenêtre nonatteints. Lescompteursfirmwaretag/parité/gap à0 n’établissentpas l’intégrité desdonnées.

Les897RX réels sontpubliéshex et128XYZ/tags **décodés horsligne/rejetés/acceptedfalse**, sansfenêtrevalide. ReconstructionCSV→raw exacte SHA **8527e290dcd9c1f94dcdd0a51dc5b5d5973ea8b303d9f6d7bacfb6b4649a9b7f** ;parseroffline128 tags/parité/counters/saturation ne trouvepasd’anomalie, mais nepeut paslever le gardeconfiguration. Payloadraw validecommeéchange, configurationpostburstrefusée. Metadata mask 23 atteste tentative/transfert/status/cohérence, bitsconfig8/intégritéfirmware32absents.36traces renseignées reconstruisent3312 octets du dump exact ;slotsnonrenseignés exclus.

Timing :burstDWT **2,88586875 ms**, readbackerroné18127213→18131460/**26,54375 µs**, core160 MHz/SPI2,5 MHz calculéHAL/prescaler64/mode 3, CFG1=50070067/CFG2=87400000, SR A007→E001. FIFOenable→finrestaurationcapteur **9,72766875 ms**, pasdurée fenêtre4096. Polls0/4/53/106/161 puis118 aprèsburst, maxlevel161, aucunFULL/OVR détecté. Bilan85motsproduits compatibleaveclesmesures avant/après mais nequalifiepasleflux aprèsrefus. GPIOODR etDWT sont observationsMCU, pasmesureswire/instantsconversion ;ODRindépendante nonmesurée. NVICavant/après9champszéro/valid3/faultregs0, attendu dansceprofil sansIRQexternes ;signatureHOTPLUGISPRFFFFFFFF nonobservée.

## Restauration et reprise production

UID004A003D3842501820333157/STLINK003500463235510B37333439/V3J15M7/NUCLEO-U575ZI-Q/STM32U575-U5850x482RevU/3,29V reconnus. Image M014-H ethashes vérifiés avantflash, sauvegarde **fraîche** Flash2 Mio/RAM768 K conservée. Deuxflashs standardsNORMAL08000000/verify/reset (diagnostic puisrestaurationenfinally), secteurs0..4 puis0..17 ;pasHOTPLUG/masserase/OptionBytes/RDP/bootpersistant/câblage. Readback/resetGDB aprèsdernierCLI, ensuiteGDBseul ;untrigger4D31354A/detach pendantcampagne, dumpsRAM avantreflash. Aucun deuxièmeessai.

**Flash2 Mio restaurée entière byte-identique au backupfrais**, SHA **556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0** ;sept contrôlesrelus0/0/0/0/0/4/0. PermanentM014-H/source 0b2a901e185c9cbf8c02410bf2a29a0a3a75491e :BIN 143108 SHA **b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611**, ELF c37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de. Aucunmedia dansprofiltemp, FRAMgen12 retrouvée/configUNAVAILABLE/SDOFF ;pascomparaisonFRAM entière revendiquée, aucuneFRAM/SDwrite niaccèsLBA65536.

RepriseHP deux FC03 seulement B0/21 puisB1/20,COM7/1152008E1/unit1/retour0/retry0/connexionfermée,371 fichiers paquetH rehashés etgarde fraîcheurcalibréeSSHreadonly ; aucunGDBpendanttrafic. **Journal entier7 événements récupéré/hashrecalculé/lu**, SHA **77ce465d5c84dd69a75519192857cefd0eb39123731e52e322b00f40b2cac94d**, 2026-10-12T18:53:53.048199+00:00→2026-10-12T18:53:53.7993764+00:00, passedtrue/B0completconforme/B1uptime140s/status1/flags0/stockage0/acq0. Registresdécodés, paswirecapture ou requalificationgénéraleM014. GDBaprès sansreset :stage 2/result 0/pollerrors0/FRAMV2gen12/identité1/configUNAVAILABLE/IRQ49-66actives/UARTerror0/TXidle/fault0/filehead=tail20/CFSRHFSR0,MCUrunning/detach/serveursexit0. Aucun accèscible nouveauaprèsreprise.

## Publication, limites et prochaine action

Preuves publiques `preuves/TR2-20261012-M015-J/` :readbacks/refus/meta/polls/rawrejeté/CSVrejeté/analyse/validations/RAM/CLI/GDB/HPentier/restauration/hashes. OriginauxBIN/ELF/Flash/RAM/scripts conservés **/home/lolo/dev/msm/reports/TR2-mission015-J-20261012T184145Z**. Logs publicsUTF8/LF normalisés, deuxJSONHP sansBOM/LF, raw/journalexact ;manifest liebytespubliés/originaux. Ni erreur decompilation/test niéchecflash/restauration rencontré ;échecmatérielfonctionnel isoléauguardpostburst. Aucunevisu/signalphysique extérieur niimmobilité/calibration supposée.

Journal actualisé pourrefus/limites/restauration, roadmapJ BLOCKED/M015INPROGRESS. ArchivesI-physical exactesdepuisd25aa42, missionactiveinchangée, worktreeJisolé codex/m015-j. Autresworktrees/HEAD/stashes préservés ;permissionscommit/push main/flash/debug qualifiés/deuxlecturesderepriseexercées, aucune résolutionsdivergence/merge/rebase/resetGit/forcepush/configpermanente. Publication etrelecturefinale àconsigner aprèspush, SHAfinal/étatGit dansréponse ;publicationn’est pasvalidationcontinue. Revue indépendanteChatGPT àfaire.

Suite minimale **sousnouveaucontrat** :diagnosticborné du readbackgroupé07..0A aprèsFIFO_OUT actif(mode 6), conserverattendus0A/06 etrefus ; éventuellementcomparaisonAB dusecondmodule avecactionutilisateur hors tension etparamètresfigés si utile, sans attribuer une pannecapteur surce seulfait. Pasnouvellecampagneici, pasachatd’oscilloscope. V1ODR26667 Hz/4096..32768/continuité/store-and-forward inchangés, autresfenêtres/DMA/stockage/FFT/Web/B5/Modbusvibration/endurance nonqualifiés. AcquisI256figés préservés, nonassimilés aucontinu.
