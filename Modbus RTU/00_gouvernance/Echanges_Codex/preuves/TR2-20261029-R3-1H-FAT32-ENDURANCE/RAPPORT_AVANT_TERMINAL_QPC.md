---
mission_id: TR2-20261029-R3-1H-FAT32-ENDURANCE
status: IN_PROGRESS
base_ref: main
base_sha: c9d7333e5ea1747bd5744daabd0c87a247608b09
initial_head: d6bd8e45ba43a5736d6f7642ad1113cf49042d6d
result_sha: 24aaf06a4f975e54b8a6199d6c625fab612cf005
created_at_utc: 2026-10-30T10:52:35.826923+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-1H — capture QPC2 longue réellement lancée, qualification en cours

## État actuel — essai long lancé le 30/10/2026 à 11:44 UTC

Intégration isolée autorisée publiée à **a6e4cad14404075a15695c483b322147bb608529**. Relecture par connecteur GitHub : HEAD distant et rapport intégral/blob f5fe18211f4a9445b82470faa5a5466009062fd5 conformes. 426 objets locaux de cette identité contrôlés,356 artefacts décompressés/hachés. Le fetch Git après push a temporisé ; la confirmation distante a donc été obtenue par API GitHub, sans prétendre que ce fetch avait réussi. Les preuves sécurité ont été publiées et relues avant l'essai.

**Q1N0005.TR2 est maintenant réellement en acquisition**, déclenchement RAM puis GDB détaché à11:44:05.370713UTC (annotation seulement). UID attendu lu, run3/stage4/error0/SDquiet/CS haute contrôlés avant trigger5. Première trame QPC2 active run5/CRC conforme sauvegardée immédiatement : même UID, session nouvelle, source déjà croissante. Port exclusif COM5/STLINK002 ; COM4 exclu. Firmware24aaf06a/BIN6cb4…f7d8 inchangé, aucun flash/reset/test court supplémentaire. Le firmware relit l'intégrité du court puis préalloue le nouveau fichier sans toucher aux anciens.

Le témoin attend maintenant3601s QPC avant seconde trame active, puis terminal autonome ; aucune inspection GDB pendant capture. Garde4000sMCU nominales, pas d'affirmation préalable de durée physique ni de succès long. Protections Windows intactes, commande AST/fixture détectée jamais rejouée ; contrôleur de mesure distinct déjà qualifié sur court. Journal incrémental local `qpc-long.journal.jsonl`, lancement/commande exacte/preuves dans `qpc-long-start`. L'acquisition continue et s'arrête sans dépendre du PC. **Ne pas retirer la carte ni couper le banc avant STOP/SDquiet réellement observés.**

À faire : seconde borne QPC, STOP/flush et métriques terminales, puis retrait physique indispensable par l'utilisateur et contrôle PC intégral des deux Q1N face au SHA producteur RAM et empreintes historiques. Statut **IN_PROGRESS**, aucune heure indépendante/DONE/8h revendiquée. TR2-001/FRAM/COM4/service Windows préservés.

## Historique des checkpoints conservé

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

## Avancement suivant — nouveau gate QPC2 physique PASS, avant répétition longue

Checkpoint c565c4cd03720717ab83000fd5887a071fe23c63 récupéré/119fichiers relus/77artefacts décompressés-hachés à10:53:28UTC. Candidat24aaf06a désormais réellement flashé (troisièmeflash standard de cette reprise temporelle), UID/probeprécédentf76etreadback exact55324B SHA6cb4c7…93f7d8 ; bootWAIT/SDnoninitialisée/detach, QPC2surCOM5répondsequence1/flags0/session0/accepted0. PasSTARTautomatique.

HuitanciensfichiersRUN/R2N/H1N+Info re-hachés conformes avantnouveauwrite, Q1N3/5absents. User confirme ensuite réinsertionSD/TR2-002STLINK/CN15débranché. Nouvelleidentité/WHO7B/CID/CSD/OCR/capacité etFAT32existante conformes ; calibration193brackets/6577ancres/résidu38µs, aucunformat. Ce contrôle est nécessaire après changement de firmware et cycle physique, pas un rejeu de l'ancien essai acquis.

**Q1N0003.TR2 nouveau gate** : triggerRAM parGDB puisdetach immédiat ; aucune sessionGDB pendantcapture. DeuxsnapshotsactifsQPC2, mêmeUID/session/sourcecroissante, intervalleintérieur **1,0547623sQPC** (minimum1s), puis terminalUARTflags772=run3/COMPLETE, SDquiet avantGDB. Capture **2,000336sMCUnominales**,52600XYZ/5260sorties/31560B, aucunFIFO/SD/queue/temps/clip/rail/UARTfaute. FIFO47/file5120B, maxwork682,331/service1341,569/gap1597/SDquantum429,981µs sousgardesinchangées.103sortiespréfixe indépendantes exactes. Métadonnées/dumps etwitnessbruts dans qpc-gate/BUNDLE.json. IntégritéPC finale de ce nouveau gate encoreàfaire ; relectureFatFs+SHA avantlong parfirmware.

**Heurelongue pasencorelancée àcepoint.** Méthode désormaistestée surcourt, cohérente avecleplanjustifié : Q1N0005 gardeautonome4000sMCU, deuxsnapshotsactifs séparés≥3601sQPC (marge1s), terminal etfichierPCconforme ensuite. Les4000s ne sontpas présentéescomme3600sSI ; onmesurera un intervalleindépendantcontenu danscapture. ContrôleurWindows neSTART/STOPpas leMCU ; son journalQPC incrémental surPC préserve chaquerelevé avantl'attente longue. Si défaillance témoin, nepas conclureSTOPMCU et nepasflasher/reset/retirercarte avantterminal réel. Pas8h.

État actuel : recorder24aaf06a/run3/stage4/error0, FIFOstoppé/SDquiescente/CShaute/GDBdétaché ; microSDdansbanc selonconfirmationutilisateur. Donnéesantérieures/backups/worktrees/stashes inchangés. RapportIN_PROGRESS ; prochaineactionautonomeessailongjustifié aprèspublicationdecespreuves. Aucunevalidationintermédiairesoftrequise.

## Évolution distante, incident Defender et intégration isolée —30/10/2026

**L'essai long QPC2 n'a pas été lancé.** Lors de la publication du gate, main avait avancé de c565c4cd àe7810f71 avec trois ajouts sur `ALERTE_DEFENDER_QPC_T1000_20261030.md`. Le push de bf1ce5c2 a été refusé non-fast-forward. Commit bf1ce5c21a6972e1d9fb39672ede421ad4a4f6a0 et worktreeR3-1h-time conservés. AGENTS§7 appliqué : escalade, puis utilisateur **«Oui, intégration isolée et publication»**. Nouveau worktreeR3-1h-security basé e7810f71 ; réapplication exacte de la seule tranche gate, aucun merge/rebase/reset/force. La note ChatGPT est préservée sans modification.

### Collecte de sécurité réellement effectuée en lecture seule

Événement Defender1116/Record1061 **10:47:04.8466455UTC**, détection **Trojan:Win32/Injuke.C!MTB**, ThreatID2147957028 ; puis1117/Record1063 **10:47:22.9389433UTC**, action **Supprimer**, opération réussie. Get-MpThreatDetection : ActionSuccess=True, statut4, ProcessNameUnknown ; PID et parent non exposés, aucun PID du service journaliseur n'est attribué à la commande détectée. Les ressources d'événements sont tronquées à1023caractères ; **Get-MpThreatDetection restitue la ressource complète5159caractères**.

Commande complète conservée comme texte non exécutable, comparée **sans la rejouer** à la génération du diagnostic et au script Git. Identité littérale exacte : SHA256 **953ec00740ab88198f1863fcd946d6aa183c42793613bb39df93e72db34df9e2** pour la commande réelle **et** reconstruite. La chaîne littérale passée à ParseInput correspond exactement au blob Git **2e5e6f8e16b3d41ad0e11724e7ff85279a62eb10**, fichier witness_capture.ps1 au commit24aaf06a. **Cette commande est bien issue de notre diagnostic de validation**, et non un script d'origine inconnue.

Le flux réellement exécutable configure l'encodage de sa console, construit l'AST d'un texte détenu dans une chaîne, vérifie l'absence d'erreurs de syntaxe, définit une fonctionCRC32, décode une constante **48octets de données QPC2** et compare CRC IEEE **0x51D01B2D**, puis écrit du JSON. Le script COM contenu dans la chaîne **n'est pas invoqué** ; aucune ouverture COM ou attente3601s n'est exécutée par cette commande. Aucun Invoke-Expression/Add-Type/téléchargement/réseau externe/registre/persistance/élévation/injection/credential-access/modification de protections dans les instructions hors chaîne. Comparaison/CRC reproductibles par `audit_defender.py` Python, sans PowerShell incriminé.

État Defender lu : AMService/Antivirus/Antispyware/RealTimeProtection/BehaviorMonitor/IoavProtection/IsTamperProtected **True**, moteur1.1.26100.4, plateforme4.18.26100.4, signatures1.463.13.0. La fenêtre d'événements1116 consultée du30/10à09:30UTC jusqu'à la collecte contient **une seule détection**, celle ci-dessus ; ce n'est **pas** un audit complet ni une garantie d'absence de compromission. Aucun scan complet du PC n'est revendiqué. L'état Supprimé est conservé ; aucune restauration, exclusion, allow-list, modification de Defender/SAC/ASR/politique, ni signalement externe envoyé.

**Conclusion circonscrite : faux positif probable pour cette commande déterministe inoffensive, sans annuler le verdict du produit ni déclarer le PC globalement sain.** Le diagnostic ParseInput+fixture n'a pas été relancé, aucune réécriture/encodage d'évasion entrepris. Le témoin réel QPC2 est une commande distincte, sans ParseInput ni fixtureBase64 : son gate court a déjà fonctionné sous protections actives, sans autre détection1116 dans la fenêtre consultée. Le journal incrémental préparé avant lecture de l'alerte vise uniquement la conservation des relevés, pas l'évasion d'une signature. Toute nouvelle détection lors d'une commande de qualification impose l'arrêt Windows et un nouveau checkpoint, pas un contournement.

### Preuves et suite autorisée

Preuves publiques ciblées dans `security/BUNDLE.json` : commande contrôlée/publique en texte, comparaison littérale, événements expurgés des identifiants utilisateur/machine, état protection et résumé de fenêtre, script d'audit sans exécution. Les journaux bruts complets restent dans le répertoire local security ; ils ne sont pas publiés avec les identifiants personnels. Dossier prêt pour un éventuel signalement officiel Microsoft sans fichier, **aucune soumission effectuée**. Références : [processusMicrosoft](https://learn.microsoft.com/en-us/defender-endpoint/defender-endpoint-false-positives-negatives), [ParserSDK](https://learn.microsoft.com/de-de/dotnet/api/system.management.automation.language.parser.parseinput?view=powershellsdk-7.5.0).

Le gate physique court reste acquis, aucune donnée historique n'est altérée. État matériel dernier observé : recorder24aaf06a/run3/stage4/error0/FIFOstoppé/SDquiet/CShaute, témoinCOM5 fermé etGDBdétaché. Aucune nouvelle manipulation physique demandée pour cet incident. Publication/relecture de ces constats **avant toute nouvelle qualification temporelle**, conformément à la note. La seule réexécution interdite concerne le diagnostic détecté ; la suite contractuelle restera sous protections intactes, avec code explicable déjà testé, sans toucher au diagnostic incriminé. StatutIN_PROGRESS à cette publication, heureindépendante/extractionfinale toujoursnonexécutées.
