---
mission_id: TR2-20261029-R3-1H-FAT32-ENDURANCE
status: IN_PROGRESS
base_ref: main
base_sha: c9d7333e5ea1747bd5744daabd0c87a247608b09
initial_head: d6bd8e45ba43a5736d6f7642ad1113cf49042d6d
result_sha: f76c3431c169d9580d23ec57b12dac3ffcf046e6
created_at_utc: 2026-10-30T10:52:35.826923+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-1H — audit temporel et référence indépendante, checkpoint avant nouveau gate

Même contrat/mission, reprise de l'instruction `REPRISE_R3_1H_QUALIFICATION_TEMPS_20261030.md` sur main39287c8c. Revue A-01 ChatGPT E4 acceptée pour l'intégrité ; réserve horaire maintenue. **Pas DONE et aucune nouvelle heure lancée.**

## Acquis conservés

Le code de capture historique **a7ca102f857310e139d26f52c6332032ca8e9381**, H1N0003/5, les57088872B utiles du fichier long et31554B du gate, tous CRC/index/footer/SHA amont relus sur PC, sixRUN/R2N etInfo inchangés restent acquis. Le rapport E4 précédent est conservé dans `preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/RAPPORT_E4_AVANT_AUDIT_TEMPS.md` ; ses reçus et extents demeurent intacts. L'audit ne transforme pas cette intégrité en qualification temporelle.

État reçu utilisateur : TR2-002 sous ST-LINK, microSD au PC, USB natif déconnecté. Cible SN0032001F3235510C37333439/UID001B0046/38425018/20333157 réellement vérifiée, ancienne imageA7 relue sur54212B exacte, WHO7B lu avant les probes. Aucun accès TR2-001/FRAM/COM4/serviceWindows, aucun writeSD ni nouveau fichier de capture.

## Audit indépendant des preuves passées — effectué sans matériel d'abord

Script reproductible `experiments/r3_time_reference/tools/audit_previous_hour.py` : intervalle UTChôte3584,726090s ; duréeMCUnominale3600,000743s ; différence15,274653s. Somme exacte des trois sautsWindows **−1,913166300s** ; modèle corrigeant ces seuls sauts **3586,639256300s**, différence résiduelle13,361486700s. Sous les seules hypothèses WSLsuivant exactementces sauts/aucunautreslew/overheadnul, ratio minimum1,00372535 (~160,596MHz). **Ce ne sont pas des bornes physiques prouvées ni une attribution causale.**

Ordre audité : datetimePC avantlancementGDB/sleep2s→connect/guards/RAMtrigger→relecturegate/préallocationFAT→FIFOstart→begin_us→polls→duration_us lu avantSTOP→STOP/drain/footer→breakterminal→prints/dumps/detach→datetimePC. Aucun couple QPC/monotone indépendant aux événements réels n'est enregistré. Les timeouts internes, sleeps, clocks DWT/SysTick et calibrationDEC8 ne remplacent pas cette preuve. Logs Windows originaux hachés ; seules dates ASCII interprétées, octets de locale nonUTF8 préservés. **Qualification rétroactive impossible avec les éléments conservés.**

## Mesure courte indépendante réellement exécutée

Probe isolé, sans codeFIFO/FIR/SD/FRAM/RTU, USART1 PA9/PA10/AF7/1152008N1. Route VCP par défaut documentée UM2861Rev10/table10/table18note1, distincte du LPUART PG7/8 deD0/D1. Aucune soudure/câblage changé. Identité du port **COM5** liée par ascendancePnP au STLINK002, handle exclusif ; COM4refusé. D'abord test QPC hôte sans ouverture de port : haute résolution, fréquence10000000Hz ; pas une qualification du STM32.

Premier probe d379e701 : flash normal vérifié, démarrage99/error0 avantUART/clocknominal0. Cause de raccordement : initialisationPWR/SMPS de R1 omise. Image/traces et readback exact11156B conservés ; aucun test ou transmission UART avant échec. Correctif minimal **f76c3431** : HAL_MspInit/PWRclock et ConfigSupplySMPS comme R1, sans changerPLL. Second flash standard, readback complet corrigé exact, stage2/error0/sequence0.

**Un seul couple de requêtes Q de calibration**, sansGDB pendant mesure : QPCdelta entre snapshots borné **10,0204539..10,0337022s**, deltaDWT **1615040000cycles**. Fréquencecore relativement àQPC : **160961524,252..161174335,626Hz**,160MHz nominal **hors intervalle**. QPCest une référence PC indépendante du STM32 et des sautsUTC, pas une traçabilitéSI. Chargeprobeidle différente de la capture ; cette mesure actuelle n'étalonne ni l'heure passée ni sa dérive thermique. Elle confirme que diviser aveuglément les cycles par160 ne qualifie pas la durée réelle.

Après mesure : GDBqualifié horsacquisition, stage2/error0/sequence2/UARTerrors0, detach. Le probe écoute sansTXpériodique, aucun capteur ou stockage démarré. Deuxflashs standards dans cette tranche, aucun reset logiciel supplémentaire. Firmware corrigé f76 conservé à ce checkpoint ; image recorderA7/backups toujours disponibles. Échec compilationinitial misleading-indentation corrigé sans baisse de warnings, source/traces conservés.

## Tranche suivante préparée — witness QPC2 indépendant

Code candidat **24aaf06a4f975e54b8a6199d6c625fab612cf005**, BIN **6cb4c7f93db96bca888a3950e99345acd7deedb1cd7e23d75296cdcac193f7d8**,55324B, **non encore flashé**. Profil explicite `R3_QPC_WITNESS=ON,R3_ENDURANCE_1H=ON`, nouveaux Q1N0003/Q1N0005. File64KiB/FIR/DEC8/SPI3=8MHz/SPI2=4MHz/quanta/gardes1000/2500/5000 inchangés ; trame archivev2 conserve les durées nominales. Garde autonome long **4000s nominales MCU** (4000000000µs,<u32), fichier72MiB ; aucune hypothèse d'équivalence4000MCU=3600physiques. Capacité à vérifier avantstart comme auparavant.

UARTnonbloquant un octet parpoll, checksum à la demande, coût inclus dansservice/gap ; packetQPC2/48B UID/sequence/tempsMCU/flagsactive/session/compteur source/CRC. Le contrôleurWindows attendA réellementactive aprèspremiersXYZ, relèveQPReceiveA, attend **3601sQPC** (+2ticks, margeobservation explicite1s au-dessus3600), relèveQPsendB puis demandeB encoreactive, même session et source croissante. START<snapshotA≤receiveA<sendB≤snapshotB<STOP : l'intervalle indépendant est **intérieur** à la capture ; les latencesUSB/OS ne peuvent artificiellement l'allonger. Le fichier final continu et conforme reste obligatoire.

MCU démarre partriggerRAM ponctuel et finit/flush seul à sa garde ; le témoinPC ne peut niSTART niSTOP. GDBsera détaché avantcapture, inspection seulement aprèspacketterminalSDquiet. Le PC ne sera nécessaire qu'à la preuve QPC, jamais à la progressionMCU. Futur8h autonome distinct, pas lancé. Courtgate neuf2sMCU avec1sQPC avantlong ; aucune répétitionH1N ni relâchement des gardes. Cette liaisond'observation est expérimentale, pas un protocoleModbus normatif.

## Validations et suite immédiate

Tests neufs hôte Cframe vsPythonCRC32, mauvaisesUID/CRC/reset/replay/brackets/temps, QPC2 intervalle intérieur tropcourt/inactif/earlySTOP : PASS. ParseurPowerShell natif etCRCcorrespondant à la fixtureC : PASS sansportouvert. ARMprobe etrecorderwitness : PASS ; text55300/data24/BSS546256 pourcandidat. Pas de recompilation du cœur scientifique/canonique inchangé ni répétition des150tests oucaptures acquises. Probes/handshake courts restent distincts de la qualification longue.

Rapports externes `/home/lolo/dev/msm/reports/TR2-r3-1h-time-qualification/` ; AUDIT.json, physical-reference.json/analysis, artifact-fixed/witness etlogs hardware. 77artefacts bruts compressés sansaltération +manifestesSHA dans `preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/time-reference/BUNDLE.json` ; limites/diagnostics/initéchec conservés.

Prochaine action autonome : publier cecheckpoint, charger candidat auWAIT aprèsidentité/readbackprobe, puis demander uniquementréinsertionSD horsalimentation (carteactuellementPC) quandfirmware sûr prêt. Aprèsretour utilisateur : nouvelle vérificationCID/volume/nomsQ1Nneufs, calibration/shortgateQPC etpreuveintégrité ; uniquementsiPASS, heurejustifiée avecwitnesscorrect etextractionPC finale. Aucun essailong avantcesportes. Siuserabsent, checkpoint explicite sans supposerinsertions/connexions.

Références primaires consultées : [MicrosoftQPC](https://learn.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps), [STUM2861Rev10](https://www.st.com/resource/en/user_manual/dm00789949.pdf). Méthode/commandes dans `experiments/r3_time_reference/README.md` ; aucune précisionSI/UTC/ppm certifiée revendiquée.

ÉtatGit : nouveauworktreeMSM_vibe-R3-1h-time basé39287c8c ; dépôt historique82169af, worktrees E4/préparations etdeuxstashes préservés. Commitd379e701 déjàpoussé ; correctiff76/candidat24 àpublier normalement aveccecheckpoint. Contrat inchangé/propriétéChatGPT respectée, pasforcepush/merge/rebase. Relecture distante aprèspublication ; statutIN_PROGRESS logiciel/qualificationpartielle, pasA01autoproclamée.
