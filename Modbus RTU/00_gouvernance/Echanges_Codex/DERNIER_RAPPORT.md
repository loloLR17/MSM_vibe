---
mission_id: TR2-20261031-R3-8H-FASTSTART-PC-MACHINE
status: BLOCKED
base_ref: main
base_sha: f5850ad2de81a6b7639d947d54751305cde3e75b
initial_head: d5af0828e370780320bda67fac5d537bf3fd5016
result_sha: 98457b5aa5c82dc44fe2305225790234654ea2c1
created_at_utc: 2026-10-31T18:24:20.286731+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# 3,3V rapportés et comparaisons SPI — tous refusés, capteur de référence proposé

L'utilisateur rapporte **VDD=3,3V/VDDIO=3,3V** au multimètre. Mesurestatiqueutilisateur, pasqualificationdesfrontsSPI. Investigation ciblée surtracesdéjàsauvées etcode : précédentR4-Udebitshiftapparent avecvoieHALduplexexistante. Ajoutminimum **R3_SPI_DUPLEX** opt-in OFFpardéfaut, limitéauprofil8H ; aucun nouveaupilote, algorithme/gain/FS/ODR/FIR/v3/importeur/contrôleschangés. Codeévalué **98457b5aa5c82dc44fe2305225790234654ea2c1** ; buildsARMciblésréussis, configurationprécommitpuisrebuildSHAcommitépourimageexacte, aucunancienPASSrépété. Validationcanoniqueancienne réutilisée car cibleinchangée ; physique requise avanttouteadoptiondevariante.

[Traces, images et résultats distincts](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/duplex-differential/README.md), [synthèse](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/duplex-differential/SUMMARY.json).

| Comparaison | Observation réelle | Issue |
|---|---|---|
| Duplex8MHz, HIGH, nouveauN8D | BIN53032B/SHA889c551f625b9d5d5594b637a535446ed71455fbf46722b469d71cfdd6e43cfe ; flashstandardunique/readbackexact.6625ancresFIFO/197brackets pendant2s, aucuneerreurFIFO, mais timestampsdirectsincohérents/résidu1842816µs | error7, garde100µsconservée, calibrationrefusée |
| Mêmeimage8MHz, SCKMEDIUMvolatile | Aprèsunresetlogicieljustifié, PC10OSPEEDR seulHIGH→MEDIUM ; calrésidu214061379µs | error7, garde100µsconservée, aucunbénéficeétabli |
| Mêmeimageduplex4MHzvolatile/HIGH | Aprèsunsecondresetlogicieljustifié, SPI3désactivéauWAIT/prescalerMBR1(HSI16/4), ODRinchangé | error1/FIFOconfig6 avantcalibration, refus |

AucunSTARTlong ni gateSD ni nouvellecréationfichierN8D pendantcescomparaisons, calibrationscommand1/seulementlecturesSD ; contrôleabsencedesnomsnonatteint. Aucunefaussequalification. Le duplexnepeutpasêtreprésentécommecorrectionphysiquementvalidée. Lesrésultats n'isolentpasune causeexclusivecapteur/nappe/électrique/transport ; pasdepreuveCEM. Touslesrefus etbrutspréservés, pasdebitcorrigé/mesureignorée pourforcerPASS.

**Étatfinalréel : imageDduplex conservée, stage99/run1/error1, FIFOoff/writeroff/SDCShaute, card.failedfalse/clockfault0/GDBdétaché.** Overridesvolatilesrestaurésàl'image (SPI3=8MHz/HIGH) sansreset/flashsupplémentaire ; bootWAITsansautostart. Unflashstandard+deuxresetslogiciels danscettetranche ; pasmass-erase/OptionBytes/format/FRAM/COM4/TR2-001logiciel. READYfalse, BLOCKEDjustifié : aucuneconfigurationtestée n'admetcalibrationetcapturedepréparation.

**Dernièreinstructionutilisateur : emprunterlecapteurdeTR2-001 pourcomparer.** Décisionexpliciteconsignéecommeexceptionlimitéeauprêtphysiquedumodule ; aucuneautorisationdéduitepourflash/debug/FRAMdeTR2-001. Interventiondemandée : deuxbancshorstensionavantretrait, mêmeréférence/connecteursconfirmés, moduleuniquement surTR2-002/nappe/brochage/3,3Vinchangés ; sicompatibilitéincertaine, préciserréférencesavantrecâblage/retoursoustension. ReconnecterSTLINK002seul/CN15off/SDenplace puisconfirmation. Pasdenouvelessaid'ici-là. Reprendresansbuild/flashsiimageDintacte, mêmecondition8MHz/HIGH pourcomparaisonnechangeantquelecapteur. Toutchangementcapteur/pose seratracédansprovenance, pasdéclaréidentiqueaux6h.

Code98457b5a etpreuves05b01525 publiés, 139fichiers relusintégralement/132artefactsbrutsconformes aprèsdécompression siapplicable ; preuve remote-readback.json. Bilanpublication final àvérifierpendantlapréparationphysiqueutilisateur. MissionBLOCKEDsurinterventioncomparative, pasDONE niSTART8h. Rubriquessuivanteshistoriquespréservées.

# Après réenfichage utilisateur — défaut reproduit, mesures électriques attendues

**Retour utilisateur : nappe etSTLINKdébranchés/rebranchés, contrôlevisuel sansanomalie.** Le firmwareN8C est intact : relectureintégrale54556B identiqueauBIN0844bc7a…2009a, UID002 etWAITinitial corrects. Réutilisation de l'image/ELFexistants, **aucunbuild/flash/resetCodex** lorsdecettevérification.

Une seule nouvelleinitialisation/calibration aprèsintervention : WHO7B/carteinitialisée/montageFAT32, mais de nouveau stage99/error2/FIFO9/**reason6 tagnonreconnu**. Quinze brackets avantéchec, aucun résultatdecalibrationvalide, **aucun gateSD ni STARTlong ni nouvelleécritureSD** (writes0). La récidive intervientpendantcalibration, sanswriteractif ; elle ne démontre pasunproblèmedechargeSD. Causeexacteinconnue, défautSPI/banc à départager, contrôlevisuel nequalifiepasl'électrique. [Preuves](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/after-user-replug/README.md).

Étatfinal vérifié : FIFOoff/writeroff/card.failedfalse/clockfault0/CShaute, GDBdétaché/serveurfermé. READYopérationnelfalse, statutBLOCKED conservé, aucunlong. L'utilisateur dispose d'unmultimètre et prépare lesmesuresVDD/GND etVDDIO/GND au modulecapteur (attendu≈3,3V chacun), bancalimentémaisarrêté, sansdéplacerlesfils. Attendrecesvaleurs avanttouteopérationdépendante. Aucune calibration/acquisition ne sera relancée pendantmesures. Lesréserves, échecs et fichiersprécédents restentpréservés.

Checkpointdocumentaire àpublier/relectureensuite, sansattendrelesmesures pourpréservercesconstats. Lesrubriquessuivantes décriventlesétapeshistoriques.

# Réarmement demandé — BLOCKED sur défaut de tags FIFO, intervention physique attendue

**Aucun long lancé. La préparation n'est pas rétablie.** À la demande explicite de réarmement, deuximages à nouveauxpréfixes ont été construites/flashées par procédurestandard et relues intégralement conformes ; aucune garde, fréquenceSPI, FIR/gain/FS/format/importeur modifiés. Images/source75ea886c, sourcefonctionnelleidentiqueauprofilacquis. Deuxflashsstandardsavecresetdeprogrammation danscettetranche, aucunresetsupplémentaire. N8A/précédentgate/donnéeshistoriques intacts ; pasdesuppression/formatage.

- **N8B** : BIN91b191228bf23b35bfad3aa41ae8863f45deefca69f269188f4bafc622a225c3,54556B. UID/SN/WHO7B/CID/FAT32 conformes, calibration40µs, nomsN8B0003/5absents vérifiés/libresuffisant. Un seulgatephysique tenté, arrêté0,205253s : error5/FIFO9/parserreason1 (paritétag),5168natifs/517sorties,0rails/clips/FULL-OVR, SDerror0 mais0groupeacquitté. STOP153µs, FIFOoff/SDquiescente. FichierN8B0003préalloué/header/invalidfooter àpréserver, pasCOMPLETE ; aucunevalidationcourte.
- La rafale fautive a été extraite enlectureseule : tagsvalides au début puis tagsincohérents avecdécalageapparent, causeexactenonisolée. Pas de correctionspéculative/filtrage des erreurs.
- **N8C**, uniquereprisebornéejustifiéeparéchecB : BIN0844bc7abdaffc163010e59526a92294ce492e31a805b0ddba4ca937a732009a,54556B. WHO7B/carteinitialisée, mais nouvelleerreur dès calibration : error2/FIFO9/parserreason6 (tag nonreconnu),0sortie/0écriturecapture. PasdegateC niSTARTlong ; absence desnomsCnonvérifiée (contrôlenonatteint).

[Preuves séparées, manifestes, builds, images, GDB et dumps](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/rearm-after-reboot/README.md). PremièreinvocationhelperB stoppéeavanttrigger pournative-host.jsonmanquant : lecturehôte effectuée ensuite ; ce n'estpasunpremieressaiphysique caché. Leséchecs sont conservés, pastransformés enPASS. Validationsantérieuresréussies nonrejouées ; contrôledecode parlecture etcompilationciblée dechaquenouveauprofil uniquement, sansnouvellecampagneannexe.

**ÉtatfinalN8C vérifié : stage99/run1/error2, FIFOarrêté/writerinactif/card.failedfalse/clockfault0/CShaute, GDBdétaché/serveurfermé.** READYlocauxfalse, aucunverrouSTARTlong. Défaut de liaison/lectureSPI suspecté, pasdecausalitématérielleaffirmée. Leserreurs ne viennentpasd'unclip ou d'unSDerror observé ; ilestinterdit delesignorerpourpartir.

**Interventiondemandée selonAGENTS.md§9 :** utilisateurcoupe uniquementSTLINKTR2-002, vérifie/réenfichelesconnecteurs de lanappecapteur auxdeuxextrémités etcontacts3,3V/GND, sanschangerbroches/pose ; SDresteenplace/USBnatifdefbranché, puisSTLINKreconnectéetretourutilisateur. Arrêtsûrconfirméavantcetteaction. Aucun nouvelessai avantconfirmation. Aprèsretour, vérifierétat/imageCconservée, réutilisersansflashsiconforme, relirecarte/noms puisvalidationcourteadaptée ; pasderelanceautomatique du longni de nouveauflashaveugle.

Rapport/journal/preuves sontpubliés commecheckpointBLOCKEDréel surinterventionphysique, pasDONE niREADY fictif ; checkpoint8446c221 publié/relu intégralement (145fichiers/141artefacts conformes aprèsdécompression siapplicable), preuve remote-readback.json ; bilanfinalvérifiéetSHAfinaldansréponse. AucunTR2-001/COM4/FRAM/serviceWindows/OptionBytes ni donnéespersistantes anciennes modifiés. Lesbilanssuivants sonthistorique et ne décriventpasunétatprêtactuel.

# Actualisation après installation — 31/10/2026 21:16 UTC

**L'état PRÊT antérieur est invalidé.** À la demande de vérification RAM de l'utilisateur, observation GDB sur UID002 conforme : WAIT initial stage2/run0/error0, compteurs0, FIFOoff/writeroff/file0, montageFATnoninitialisé, digestproducteurRAM32octets nuls. Un redémarrage a donc eu lieu depuis la préparation ; sa cause n'est pas démontrée (pas d'attribution certaine à une coupure). Le firmwareN8A préparé a été relu entièrement :54556octets strictement identiques auBINc31904fe…3ddc17. Aucun reset/flash/START/commandeSD exécuté pendantcecontrôle ; debuggerdétaché.

Le READY.json opérationnel local est passé à prepared=false, ancienne version conservée ; le lanceur refuse donc un départ prématuré. Le dossierpreuves historique et le gateSD ne sont pas réécrits ; la carte et ses fichiers n'ont pas été relus lors du contrôleRAM. [Observation et traces](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/post-install/POST_INSTALL_RAM.json). MissionIN_PROGRESS, pasblocageextérieur identifié. **Àfaire avantLANCE8H : réarmer une voie de départ compatible avec le reboot et le gateN8A0003 déjàcréé, sans l'écraser ni forcer run3 enRAM.** L'estimation antérieure20–30s suppose un étatpréparé conservé et n'est plus applicable àl'étatactuel. Cette tranche répond seulement au contrôleRAM demandé, aucunecapture nouvelle.

Le bilan ci-dessous décrit la préparation historique avant déplacement ; ses mentions PRÊT ne décrivent plus l'état courant. Actualisation àpublier puis vérifier surmain, SHAfinal dansréponseutilisateur.

# Phase A préparée — attente du signal LANCE 8H

**Préparation matérielle et logicielle acquise ; aucun long START ni auto-start.** La mission reste IN_PROGRESS : phaseB attend uniquement le signal explicite `LANCE 8H`, puis phaseC différée. Aucun blocage technique identifié. [Note PRÊT_8H et commande exacte](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/PRET_8H.md).

Image8h/source **d5af0828e370780320bda67fac5d537bf3fd5016**, BIN54556B/SHA **`c31904feca48fd06d6102eee40b54b373bbfe5e9896d0ddc007434cf253ddc17`**, ELF/SHA`45831eccc8744bce99c5f2c51b5d79564ee9c6416c6986d7a47f6e733376fbcf`. Trois options8H/QPC/1H actives, préfixeN8A/garde30600000000µsMCU/640MiB ; v3/int16/FIR107Q15/décimation10/gain/±8g logique/importeur/gardes unchanged. Sources de la chaîne comparées parGit àc3cd40ce : identiques. Aucune modification firmware fonctionnelle ni nouveau calcul scientifique.

## Faits et validations

1. Git réel vérifié : dépôtprincipalhistorique82169af propre, anciens worktrees/deuxstashes6251b988/f3b829e6 conservés. Worktreeisolé MSM_vibe-R3-8h-faststart créé surorigin/main d5af0828. Gouvernance/journal/précédentrapport et chaînes6h/8h consultés, archivesPDF préservées ; pas de requalificationR5.
2. ST-LINK0032001F3235510C37333439 et UID001B0046/38425018/20333157 réels, ancienBIN6h b704…5330 relu intégralement conforme/WAITinitial sûr. Utilisateur a confirméSTLINKbranché etSDinsérée ; pas de gestephysique supplémentaire demandé.
3. BuildARMciblé CMake du profilN8A réussi, code0, BIN/ELF/MAP hachés. Tests/cross-build canonique déjàPASS surcodeinchangé réutilisés, logs inspectés et comparaisonsGit publiées ; pas de suitehôte/ARMcanonique relancée. Le profil/date/nom/image nouveaux justifient le gate physiqueunique. Warningsnosys _read/_write linker conservés dans sortie ; buildréussi.
4. **Un flashstandard qualifié** à0x08000000 avec verification/reset normal ; zéro mass erase/OptionBytes/RDP, zéro resetsupplémentaire. ImageFlash54556B intégralement relue identique/WAITstage2, puis initialisation explicite et calibration horloge autorisées, WHO7B. Ce succès ne vaut pas8h.
5. CID0353445344333247858bf0923c01927b/CSD/capacité62333952secteurs/FAT32/csize64 exacts. N8A0003 et5 réellement absents avantwrite ; libre927219clusters/long20480requis. Aucun format/réparation/suppression ; seulnouveaufilegateN8A0003 créé. Anciennesarchives non ciblées, leurs empreintesSD n'ont pas été intégralement refaites danscettepréparation.
6. **Gatecourt unique**2,000357sMCU,52936XYZnatifs/5294sorties/31764B, session6755700148813124 ; QPCintérieur1,0422059s. ZéroFIFO/SD/queue/rails/clips, FIFO46/file5120B ; work684.281/service1262.013/gap1521.000µs sous1000/2500/5000, STOP152µs/drain9277µs. Oracle103sortiespréfixe exact. **RelectureintégraleFatFs du gate** CRC/index/session/compte/footer/SHA vsdigestproducteurRAM :PASS. Pas une extractioncartePC ni preuve8h.
7. TémoinWindowsnatifpassif QPC/COM5 lié auSN002 et générationnativeCIM démontrés surgate ; COM5exclusif reprislibre ensuite. Secteurtrue, veilleAC0/hibernationAC0 observés, aucunepolitiqueWindows modifiée. GDBetserveurs fermés, aucun moniteurCodex/WSL laissé.
8. Lanceur `launch_8h.py --check-only` réellement exécuté sansSTART : UID/image/idle/CID **relectureCMD10**/FAT32/libre/nomlongabsent/secteur/veille/COM5exclusifPASS. Gardesd'entréehorslignePASS : signalabsent/erroné etétatnonprêtrefusés, verrou exclusif conservé aprèséchec, deuxièmeinvocationrefusée avantmatériel. Aucun STARTlong simulé présentécommephysique.

Commandes/sorties exactes : [preuves](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/README.md), build.log, launcher-guards.log, prepared-launch-check.log, GDBtraces et analysesdugate. Helpers locaux `/home/lolo/dev/msm/reports/TR2-r3-8h-faststart-20261031/`, reproduits danspreuves/helpers. Les nouveaux gardes complètent le lanceur existant ; aucune refonte du système ni normeV1 modifiée.

## État laissé et départ

**TR2-002 idle stage4/run3/error0, FIFOarrêté/writeroff/filevide/CShaute, GDBdétaché, imageN8A conservée.** N8A0005.TR2 absent, aucun verroulong ni témoinlong lancé. Le gate et les contrôles sont terminés ; au signal, pas de nouveau build/flash/campagne/Git/revueR5 avantSTART. Préflightbref seulement, puis commande5unique et confirmationACTIVE/aprèspremierSDgroupACK. Lancementetlatence ne sont pas encore démontrés ; mesures à consigner enphaseB.

**Garder cette session alimentée sansreset et sanschangerlacarte**. Un redémarrage perd le gateRAM/état3 et sera refusé, pas réparé automatiquement au moment du signal. `LANCE 8H` confirmera fixationplancherPCmachine/configurationidentique etalimentationmaintenue seloncontrat. Aucune validationintermédiaire supplémentaire sollicitée. Ne pascouperpendantcapture.

Duréefinale à démontrer parQPC≥28800sintérieures etfichiersPC ; marge30600sMCU inchangée. Pas decollecteurautomatiqueduSHAproducteur déjàqualifié identifié : témoinQPC ne contient pasceSHA, donc revenirversCodex aprèsSTOP pourdumpRAM avantcoupure si possible. Unecoupure aprèsquiescence préservepotentiellementleFATmaisperdcettedémonstrationindépendante, comme6h. Clips/rails restent qualitédégradée/error26, jamaisfauxCOMPLETE.

Permissions exercées : debug/lecture/init/gatecourt/flashstandardunique etédition/commit/pushmain ; aucunTR2-001/COM4/FRAM/serviceWindows/SDhistorique/format/OptionBytes. Prochainesétapes : signalutilisateur, STARTunique, libérerCodex ; puisrécupérationdifférée etqualificationindépendanteChatGPT. AucunDONE8h/préqualificationSI/V1/production.

## Publication de préparation

Checkpoint léger PDF/anciennedonnées inchangés ; imageBIN etpreuvescompactesseulement. **Checkpoint `60030c5f902773247b61f817d35d81958036f141` publié par pushnormal surmain et relu intégralement après `git fetch origin main` : 56fichiers/327333octets identiques, 17sortiesbrutes décompressées/hachées conformes**, manifeste complet vérifié. [Preuve](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/remote-prepared.json). Le présent bilan et cette preuve sont publiés dans un dernier commit vérifié séparément ; SHAfinal dans la réponse utilisateur. **IN_PROGRESS reste le statut exact : préparationPRÊTE, phaseBnonlancée.** Journalactualisé avec image/étatprêt/gate/attentesignal/risqueperteRAM. Branches/stashes historiques préservés, aucun merge/rebase/forcepush/nettoyage.
