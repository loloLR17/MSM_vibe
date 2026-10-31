---
mission_id: TR2-20261031-R3-8H-FASTSTART-PC-MACHINE
status: IN_PROGRESS
base_ref: main
base_sha: f5850ad2de81a6b7639d947d54751305cde3e75b
initial_head: d5af0828e370780320bda67fac5d537bf3fd5016
result_sha: d5af0828e370780320bda67fac5d537bf3fd5016
created_at_utc: 2026-10-31T18:24:20.286731+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

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
