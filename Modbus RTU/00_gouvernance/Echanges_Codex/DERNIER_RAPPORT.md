---
mission_id: TR2-20261012-M015-F-FIFO-BURST-FEASIBILITY
status: BLOCKED
base_ref: main
base_sha: 1c062af564e3d4d45093b1c369128cb4cf6d0575
initial_head: 7e4c75b63395e8937e34b9b923a793ad903ae379
result_sha: 571b0d80d8910500556fc2fcaec4006cb633511c
created_at_utc: 2026-10-12T15:08:16.212301+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-F — Arrêt sûr avant acquisition ; faisabilité non démontrée

**BLOCKED** : l’unique tentative a été refusée par le garde de configuration avant le démarrage FIFO. Aucun DATA_OUT, burst ou XYZ acquis/accepté ; aucun débit ou délai de vidange mesuré. M015 globale reste IN PROGRESS. M014 est restauré à l’identique et sa reprise minimale est vérifiée.

## Méthode préparée et modification

[Audit](../AUDIT_M015_F_FIFO_BURST.md) : SPI3 mode3/2,5MHz candidat ;capture RAM finie256XYZ par deuxbursts128 (897octets chacun, souslimiteSPI3 1024). FIFOmode1/STOP_ON_WTM256,BDR26.667/±4g/BDU ; entrée coupée avant vidange, niveaux256→128→0. Vérification de configurationFIFO enburst07..0A, pas de CTRL2single traité comme vérité. Cela ne corrige pas le défautD ni ne le masque.

Nouveau module `stm32_iis3dwb_burst_capture.c/.h`, test ciblé et branche CMake/main isolée/OFF par défaut. Triggerunique4D313546, septregistresvolatils sauvegardés/restaurés, préexistants refusés. RawRX ycompriscommande conservés parburst, tags/XYZ/niveaux/flags/CS/temps/compteursHAL ; rejets sur ambiguïté. Pas de DMA, production, fluxcontinu, FFT, calibration, B5, accèsFRAM/SD ou Modbusvibration. Driver/cœur/mapping/validator/V1 inchangés.

## Validations logicielles effectives

CMake host, build du seul `test_stm32_burst_capture`, `ctest -R '^stm32_burst_capture$' --output-on-failure` : **1/1 passé**, sixscénarios nominal/deuxbursts/transitioninterburst, gap/parité/timeout/overrun rejetés, préexistants sanswrite, restauration. Première compilation refusée pour deux `-Werror=misleading-indentation` : instructions séparées, aucun flag outest affaibli ;sortie d’échec conservée.

Cross-build défaut : `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only`, réussi. Profil DebugNinja avec toolchainprojet et `-DTR2_STM32_BURST_CAPTURE=ON`, réussi ;text33900/data20/bss7828,BIN33920octets. WarningsNewlib habituels conservés. Symboles media/production/UARTstart/anciennes captures absents du profil. Choixproportionné : harness dédié, core inchangé ; tests/campagnesA-E et114firmware/539.NET nonrejoués. WrapperHP nouveau nom `physical-m015-f-restored` vérifié Steady-only et syntaxePS, sansport.

## Observation de la tentative unique

Code évalué **571b0d80d8910500556fc2fcaec4006cb633511c** ;BIN SHA **966997dd83f417ab85657436461eeff536c96abc7d10d25bc9c9449db31f9fa6**, ELF **56fa8f9b4c2aa885207a43c88ec34154599162e82ccdb7735a0edb74b2ec5818**. HCLK160MHz/SPI2,5MHz,CPOLhaut/CPHA2e/prescaler64 observés. Résultat :stage10/result9(INTERNAL)/restore0/loss_flags4(configuration),HALerrors0, **polls0/bursts0/words0/accepted0**.

Le code localise le refus après les writes de préparation et `controls(actual)`, avant pause100ms/activationFIFO. Il ne conserve pas la valeur divergente : le tableau `configured[]` n’est exporté qu’après freeze, non atteint, et `actual[]` est écrasé par restauration. **Registre exact/attendu/lu NON DISPONIBLES.** Les configured/RX/metadata/polls à0 sont des buffers non renseignés, pas des mesures. Les erreursparité/tags/gaps à0 ne sont pas une validation, puisque aucune donnée lue. BrutsRAM/Flash conservés localement, pas de donnéesXYZ synthétiques publiées.

Aucune conclusion de pannecapteur, d’inadéquation générale2,5MHz ou de fauteCTRL2. Aucun deuxièmeessai, variante, correctif spéculatif ou recherche exhaustive. Les conditions de sûreté pour acquérir ne sont pas réunies ; garde conservé.

## Sauvegarde, restauration et reprise

UID **004A003D3842501820333157**, STLINK003500463235510B37333439/V3J15M7/NUCLEO-U575ZI-Q,U575/U5850x482RevU/3,29V vérifiés ;imagepermanente reconnue avantflash, backupsFlash2Mio/RAM768K hashés. FlashstandardNORMAL08000000/verify/reset, secteurs0..4 diagnostic puis0..17 restauration ;aucunHOTPLUG/masserase/OptionBytes/RDP/câblage. Chaque dernierCLI suivi de readback/resetGDB qualifié ; ensuiteGDBseul. Triggerunique, debugger détaché pendant tentative ;résultats conservés avantrestaurationenfinally.

Septcontrôles relus restaurés0/0/0/0/0/4/0,restore_result0. **Flash2Mio entière byte-identique à l’initiale**, SHA556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0. ImageM014-H/source0b2a901e185c9cbf8c02410bf2a29a0a3a75491e :BIN143108 SHA **b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611**, ELF c37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de. ProfilF sansmedia/transport ;aucune écritureFRAM ni accèsSD dansle diagnostic,partitionLBA65536 protégée parabsenceaccès.

ContrôleminimalHP aprèsrestauration :371fichiersdupaquetH rehashés,2FC03 B0/21puisB1/20,COM7/1152008E1/unit1,sansretry,retour0/connexionfermée. Garde fraîcheur0..30s calibréeSSH readonly, serveursGDB fermés pendanttrafic. Journal7événements intégralement récupéré etlu, hash comparéaudistant **c8ecaba10918e2b23f5fe6264f1b96aefef6d7f5dd6c2c524548f0d33ef17aea**, **2026-10-12T15:04:33.8457156+00:00→2026-10-12T15:04:34.6092702+00:00** ;B0completconforme,B1uptime186s/status1/flags0/stockage0/acq0. Pas preuve du seulSHA. GDBaprès sansreset :stage2/result0/pollerrors0/FRAMV2gen12/configUNAVAILABLE/IRQ49-66/UART0/TXidle/filehead=tail20/CFSRHFSR0 ; détaché enexécution/serveurexit0. QualificationM014 nonrejouée.

## Exigences V1 et suite minimale

V1 conserve ODR26667Hz, fenêtres4096..32768, continuité etstockage store-and-forward. N256 prévu (nonacquis) représente9,59988ms/6,25%du minimum4096 et0,78125%de32768 ;ce n’est pas une fenêtreV1. GrilleDFT hypothétique104,168Hz contre6,5105/0,813873Hz aux extrêmesV1 ;aucuneFFT/spectre/bande6kHz/ODRmesurée/calibration qualifiés. MémoireXYZraw24576..196608octets pourV1,3int32 :49152..393216 ;latences/traitement/ressourcesstockage restent ouverts. READY0/configUNAVAILABLE/SDOFF deM014 conservés.

Pour un **prochain contrat distinct**, ajouter la télémétrie de configuration au point de rejet, puis une tentative bornée choisie sansaffaisser le garde. Alternative conservatrice à considérer :mode3/625kHz pourFIFOfigée,11,4816ms idéales parburst128 ;cela peut permettre une preuveRAM lente, **pas un flux V1** (occupationidéale240%àODR avecstatut). AucunrejeuF lancé,aucuninstrument absent exigé. Le choix2,5MHz reste nonqualifié ; réduireSPI ne change pas silencieusementODR ouexigencesV1.

## Git et preuves

Tranchelogicielle publiée au571b0d8 puis11livrables relus intégralement depuisGitHub,byte-identiques (`software-readback.json`). ArchivesE exactes depuis1c062af ;missionactive inchangée. Journal actualisé pourrefus/restauration/limites etroadmapM015INPROGRESS. WorktreeFisolé/codex/m015-f, pushnormal explicite versmain ;original82169af/H762c459/Aeab07d4/Bcfb9918/C37b97f7/Ddbfcec4/E1c062af etdeuxstashes préservés. Aucunmerge/rebase/reset/forcepush ouparamètrepermanent. Permissionscommit/push/flash/debug standard exercées ;2lecturesdereprise prévues.

Preuvespubliques `preuves/TR2-20261012-M015-F/` :info/rejet/analysesV1/logsGDB/HPcomplet/artifacts/validations/restauration/hashes. Preuvescomplètes **/home/lolo/dev/msm/reports/TR2-mission015-F-20261012T145521Z** :ELF/BIN/Flash/RAM/CLI/GDB ;dumps etbuffers nonacquis ne sont pas publiés commeXYZ. SHAfinal etrelecture sont donnés aprèspush ;BLOCKED reste technique, publication ne vautpas faisabilité.

Publication des résultats vérifiée au **0843c62782457780d85af9b87b3404d196a1101f** :31livrables intégralement relus depuis la référence récupérée de GitHub, UTF8 et identiques octet par octet. Preuve `publication-readback-0843c62.json`. WorktreeF propre, sept autres worktrees propres/HEAD préservés, deux stashes intacts, archivesE exactes et mission active inchangée. BLOCKED demeure technique ;publication réalisée. Le complément final est contrôlé après son push et son SHA fourni dans la réponse Codex.
