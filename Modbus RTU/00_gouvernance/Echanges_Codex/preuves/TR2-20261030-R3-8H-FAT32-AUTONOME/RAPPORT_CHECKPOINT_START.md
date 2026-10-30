---
mission_id: TR2-20261030-R3-8H-FAT32-AUTONOME
status: IN_PROGRESS
base_ref: main
base_sha: 7dcceb510862be2d6a25c2c9fa82d2a2e8741f70
initial_head: 395f9ccd5c31edeea6761455cf92ba340f984578
result_sha: 1d3a5763c34649d36b216b8ab1a1b894ae0a1210
created_at_utc: 2026-10-30T14:05:27.593055+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-8H — capture autonome lancée, Codex se déconnecte après ce checkpoint

**IN_PROGRESS, pas8h qualifiées et pasDONE.** Un uniqueSTARTlong a été réellement soumis le30/10/2026 ; entrée active enregistrée à **14:01:42.726861UTC** (annotation, pasétalon). Le STM32 seul exécute la capture et son STOP/flush. Après publication/relecture de ce checkpoint, **le tourCodex se termine effectivement** ; aucunmoniteurCodex/WSL, sessionGDB, terminaloutil ouautomatisationCodex n'est laissé en attente pendant8h. L'uniqueprocessus restant est l'instrumentQPC natifWindows passif autorisé par le contrat.

## État acquis au départ réel

Firmware **1d3a5763c34649d36b216b8ab1a1b894ae0a1210**, profil8H/QPC/ENDURANCE_1H=ON, archivev3/u64/Resultv4/E8N. BIN54536B SHA **7e9e9b331486c116770e1aeaa48eb0f34025c19f0a47b136f4c453bcbe51b3af**, ELF70b3cb03321f8dc90dd1a2cd20538878a782452c289b0b8ec1058070ce3a9db4. Unflashnormal/readbackBINentierexact ; ancienneimage24 WHO7B etUID/SN vérifiés avant. Aucunflash/reset supplémentaire après ; firmware deproduction TR2-001 intouché.

Cible002/STLINK **0032001F3235510C37333439**, UID **001B0046/38425018/20333157**. MicroSDtestFAT32/CID0353445344333247858bf0923c01927b/CSD400e00325b590000edc87f800a4040c3/62333952secteurs. DixanciensfichiersRUN/R2N/H1N/Q1N+Info re-hachésinchangés avantinsertion, E8N3/5 absents/espace suffisant ; aucunformat. Réinsertion utilisateur/STLINKseul/CN15off et disponibilité9h confirmées, CID/volume/calibration relus. SourceSPI3=8MHz/SDSPI2=4MHz, FIR/DEC8 /10 etfile64KiB/quanta64mots128B/groupes1..8/gardes1000/2500/5000 inchangés. V1-05 historique2000Hz inchangée, DCconservée,±8g nonétalonné.

Précondition avant trigger5 : UID attendu/run3/stage4/error0/clock_fault0/FIFOoff/writeroff/CShaute. `durable_verify_last` contrôle le gateCRC/SHA via vraielectureFatFs avant allocation du fichierlong neuf **E8N0005.TR2/640MiB** et ouvertureFIFO. TriggerRAM5ponctuel etGDB immédiatementdétaché ; ensuite observationUARTsanshalt :

- première trame **QPC2 ACTIVE/run5**, CRC/UID conformes, séquence **822**, session **8444551089417146** ;
- **12464XYZ acceptés**, tempsMCU1180226905µs ; dansceprofilACTIVE exige déjà `sd_written>0`, donc unpremiergroupeSDréel acquitté, pasjuste uneintentiond'écrire ;
- bracketQPC émission **12325779577523**, réception **12325780208623**, fréquence **10000000Hz**. Le secondpoint doit être émisauplus tôt28801sQPC aprèscettereception, mêmeUID/session/sourcecroissante ; aucune durée8h n'est encoreobservée.

## Autonomie et alimentation

Garde automatique **30600000000µs nominales MCU**, supérieureà8h avecmarge ; aucunéquivalentSI promis. STOPFIFO puisdrainbudget30s/timeoutsexistants, footerCOMPLETEseulementsifileentièrevidée/écriturescertaines ; sinonINCOMPLETE/erreurpréservée sansreprise. PuisWAITsansautostart. La pertePC/observateur/Codex neSTART/STOP/resetpas leMCU.

Observateur natifWindows **PID8968**, propriétaireégal àl'opérateur, processus lanceurterminé (parent14164 absent aucontrôle), preuve localeetdansstart/BUNDLE. CréationnativeCIM, aucuneWSLboucle résidente ; le lanceur etle déclencheur ontretourné. Instrumentversion3/SHAff4cb076a1bdb40486ae704c6a721932c07e11f7f61b11e406ff5502e7a77251, exclusifCOM5 lié auSTLINK002, COM4 refusé ; aucunréseau ni commande decontrôle. JournauxWindows natifs **`%TEMP%\TR2-R3-8H-20261030-e8-long\`**, ready.json/queries.jsonl/summary.json. Deuxtramesactives attendues à>=28801s intérieur, puisterminal1284 dansheuredegrâce ; encasd'échec pasdefaussequalification etpasderelanceautomatique.

Alimentationphysique002 viaSTLINKUSB/T1000→régulateurNUCLEO3,3V→capteur/SD, montageinchangé ; utilisateurconfirmebancbranché/dispo9h. T1000PowerOnlineTrue lu, veille/hibernationAC0 ; optionglobaleUSBsélectiveactivée, maispiloteCOM5 **usbser/IdleUsbSelectiveSuspendPolicy absent** (Microsoftdocumentefonctiondésactivée pardéfaut pourusbser). Aucunréglage/politique/exceptionchangés, aucunessaisleep/reset/powerloss. Cette configurationetconfirmation n'estpas unegarantieénergétiquecontreaccident ; nepasdébrancher/mettreenveille/redémarrer. SourceMicrosoft : https://learn.microsoft.com/en-us/windows-hardware/drivers/usbcon/usb-driver-installation-based-on-compatible-ids .

Finestiméevers **22:29UTC/23:29Paris le30/10** d'aprèslefacteurhorlogeobservé1h ; ce n'estpasunepreuvedefin. **Ne couperl'alimentationetne retirerSDqu'aprèsretourCodex/relevéRAMduSHAproducteur etcontrôleSTOP/SDquiet.** Simpleécoulement8h oudisparitionduprocessus n'estpasunSTATUTSDsûr.

## Validations antérieures de cette mission, non répétées

Quatretests cibléshôte PASS/u64DWT+SysTick+24hsimulées/fat640MiBcontigu20480clusters/lecteurv1v2v3/corruption/full/CID/nom/busy150/500/grappes/failclosed, ARMprofil etcanoniquePASS. Fixturev3creux640MiB/78XYZ=teststructure uniquement, pascampagne8h. `./tr2_validate` refusépermissions initialement ; BashdocumentéPASSsanschangerpermissions. Les150tests/campagne1h/scienceinchangés ne sontpasrejoués.

Gateunique **E8N0003** PASSsurcible :2,000456sMCU/52592XYZ/5260sorties/31560B, borneQPC1,0476086s, 103oraclesFIRpréfixeexactes, format3+u64+CRCheaderfooterconformes, zérofaute. FIFO46/file5120B, work712/service1303/gap1562/quantumSD492µs ; STOP149/drain20735µs. RelecturePC deE8N3 encoreàfaire avecE8N5 ; contrôleSD/FatFs/CRC/SHA prélong exécuteavantFIFO5.

Incidentlanceur initial : enfantStart-Process survivant,parentterminé maishandleWSLexpirant20s ; aucunecapturetriggeràcetteétape. Listenerseul proprementarrêté, méthodeCIMcorrigée/smokeseulretour0,584s+gatephysiquePASS. Aucuncontournement/élévation/nouveauservice/tâche niParseInput/fixturebloquée rejoué. Protectionsrealtime/behavior/tamperactives, aucune1116nouvelle entre13:18 et13:52:55UTC ; ancien1061 seul. Pasd'attestationglobaledesécuritéPC.

## Reprise après campagne, sans relancer

Document **`experiments/r3_durable_fat32/REPRISE_8H.md`** ethelpersconservés dans `/home/lolo/dev/msm/reports/TR2-r3-8h-autonomous/` :

1. Reliremain/contratID/cecheckpoint, préserverétatslocaux etimage ; aucunecommandeSTART/flash/reset.
2. `python3 .../read_native_after_run.py` : lirepreuvesexistantes, exigersecondpointactif/mêmesession/CRC/comptecroissant/borne>=28801s puisterminal1284 ; sinoninspecterlesfaits,sanssupposerSTOP. AucunmonitorCodex enattentedansl'intervalle.
3. Seulementaprèsce terminal : `collect_after_terminal.py e8-long-data 5 e8-long` (GDBqualifiéUIDgarde), conserverRAM/histogrammes/pile/SHAproducteurindépendant **avantpoweroff**, puisdetach.
4. Demanderalorsgesteutilisateurpoweroff002/SD→lecteurPC. Puislecturecopieexclusive E8N3/5,tousCRC/index/temps/comptes/session/footer/shaXYZcontreRAMproducteur, hashentiercarte=copie, dixanciensfichiers+Infoinchangés. Helperspréparés nonencoreexécutés pourlong, relireavantusage ; casINCOMPLETEàdocumenter sansretry.
5. Publierextentsréelsseulement, avecchunkss'ilsdépassentlimiteGitHub ; jamaisdezonepréallouéeinutilisée/privée niqueuezeroreconstituéecommeoriginal. RapportfinalaprèsrelectureGitHub, archivesexactesetseuleensuiteA-01ChatGPT.

Aucunrésultatlong/sha/durée/comptefinalencoredisponible. PasDONE/8hqualifiées/SI/24h/production/V1/FRAM/RTU/AC1/power-loss. Lesacquis1hqualifiés etarchivesdemeurentintacts ; aucunaccèsTR2-001/COM4/serviceWindows/FRAM/cartedeproduction. Deuxstashesetanciensworktreespréservés.

## Publication et état Git

Checkpointlogiciel **1d3a5763** publié/relu59fichiers/10artefacts ; gate/lanceur/documentreprise **5407696cef0ba7280a31d65b4031359dd15af1a3** publié/relu163fichiers/111artefacts. Firmwarephysique1d resteidentique aprèslecommitoutils540 ; aucunreflash inutile. RapportgateexactconservédansRAPPORT_AVANT_START_8H.md, preuves software/gate/start sousmissionID. CecheckpointSTART estpoussé normalement surmain puis intégralementrelu ; sonSHA estfourni danslaréponseCodex. Journalactualiséavecstateautonome/donnéesinitiales/reprise. WorktreedepublicationMSM_vibe-R3-8h/branchecodex/r3-8h-autonomous propreaprèscommit ; mainhistorique82169af ettravauxantérieurs préservés. **Fin du tourCodexaprèscontrôledistant, retourhumainultérieur pourphaseterminale dela même mission.**
