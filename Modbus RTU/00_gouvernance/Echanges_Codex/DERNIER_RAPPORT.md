---
mission_id: TR2-20261029-R1-ARM-DSP-FIFO-BENCH
status: IN_PROGRESS
base_ref: main
base_sha: 8ab7aeccf3a623cefc9fe92a3d88c44c5e6d1ea8
initial_head: c4c0d1820487f3394a98a46960375ede6feb79cd
result_sha: 0c4b86997829496768bd31bf0fd9bf7b27dbdc0a
created_at_utc: 2026-10-29T15:44:57.451929+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R1-ARM — checkpoint physique micro-benchmark acquis, FIFO à poursuivre

**Mesures DWT réelles acquises sur TR2-002**, données synthétiques enRAM. Une image identique compare sans/avec FIR,8cas appariés, un seul déclenchement. **Ce n’est pas encore une qualification FIFO**. La préparation de la dérivation source/temps se poursuit ; le raccordement physique du kit capteur est demandé à l’utilisateur.

## Identification, sécurité et image

InitialHEADc4c0d182 récupéré depuisGitHub ; gouvernance/journal/revue indépendanteR1-DSP-A lus. Nouveau travail surbranche `codex/r1-arm-dsp-fifo-bench`, sans resetGit/merge/rebase/nettoyage. ArchiveR1-DSP précédente byte-identique au rapport avant remplacement. Pointretoura3f5ac33 et deuxstashes conservés.

TR2-002 d’abord absent, puis ST-LINK **0032001F3235510C37333439** détecté après confirmation utilisateur «c’est bon» à la demande de banc nu, sans microSD/module/USBnatif. **UID001B0046/38425018/20333157 effectivement relu avantflash**, PC/SP inspectés sans attribuer à l’ancien firmware les symboles du nouveau. Aucun debugger ou flash ne cibleTR2-001.

Sauvegarde initialeFlash2MiB achevée, vecteurs cohérents, SHA256 **dd1a2b44d155abbfc094dacf498d61f973688f83cf6c7685ecb265bc444fb1f1**. Préfixe21860octets **fdb7fa151bd69051c839cf11a1f1e3eb134d6a821a07ab0b827eef1d91bdf829**, identique au probeSDMMC-B historique. Copie locale conservée/récupérable, pas de restauration inutile.

Code banc **0c4b86997829496768bd31bf0fd9bf7b27dbdc0a**, FIR portable0ad417d6 inchangé. Un flash standard qualifié surla seule sonde002 à0x08000000, pagesnécessaires/verify/reset intégré ; aucun masserase/OB/RDP/FRAM/SD/RENEW. BIN **9ffe22d574053bea653ba21e86dc92344f874ccbd824ba531ee86400cf5507e6**,13260octets ; ELF **1b796822b1cf80aabbbd30add7272df55d0a4eb230e1e83024ecdf17bdd62846**. Readback exact auWAIT2/0erreur/160MHz avantdéclenchement. Vraie table de vecteurs/startupCubeU5, pas l’ancienELF link-only.

`artifact.json` des preuves est l’inventaire **avant programmation** ; ses champs «compiled_not_flashed/physical_results_obtained» décrivent ce stade initial. Les tracesflash/readback/RUN/capture qui suivent établissent les opérations réellement effectuées.

## Méthode et résultats physiques

Foreground seul, GPIO/SPI/UART/FRAM/SD noninitialisés. Horloge160MHz configurée par la routineST déjà présente, IRQ/SysTick laissés actifs ; PRIMASK/BASEPRI/FAULTMASK0. Comparaison1/8/60/128sources parbloc,256blocs parcas ; ordre alterné des deux modes selonlespaires. Inputsidentiques vérifiés parindex/count/hash, temps **synthétiques** explicitementdistincts d’une autoritécapteur. Préparationhorsfenêtre ; validation/dispatch/FIR/qualité/publication inclus danslescyclesbloc. Aucunprintf ni arrêtGDB pendantlesfenêtres.

DWTread+barrières :28cycles mesurés,non soustraits. Capture aprèsDONE4/0erreur/STOPDSP réel, breakpointmatérieltemporaire à`r1_bench_finished`. Statistiques recomputées indépendamment depuislescapturesbrutes, comptes/paires/qualitésvérifiés. TouslescasavecFIR :11WARMUP,0clip/0rail, étatarrêté àlafin.

| Burstsource | Moyenneémission sans/avecFIR,cycles | AvecFIR P95/P99/max,cycles | Maxbloc sans/avecFIR,µs àHCLKconfiguré |
|---:|---:|---:|---:|
|1|182 /9371,38|9406 /9427 /9427|5,156 /62,669|
|8|182,29 /9363,68|9406 /9434 /9445|12,244 /84,863|
|60|182,12 /9364,75|9414 /9440 /9482|81,738 /543,081|
|128|182,09 /9364,40|9414 /9445 /9492|174,125 /1167,544|

Charge **déduite des cycles instrumentés au débit nominal**, pas mesurée comme utilisation d’un vraiFIFO : pourburst60,3,616% sansFIR /24,064% avecFIR, delta20,448points ; burst128delta20,448points également. L’émission FIR est d’environ58,53µs enmoyenne, max59,325µs. IRQ/instrumentation sontinclus ; pasdeWCETindustriel ni decalibrationSI.

**Contrainte démontrée :128sources dépassent la référencework1000µs enmicro**. Ne pas qualifier cequantum comme conforme ni relever laseuil. À60sources, maxbloc543µs danscebanc, mais lemapping/SPI/service réel restentàmesurer. La dérivation FIFO devra êtrebornée et seswork/service/gap observés séparément, avec références historiques1000/2500/5000µs, sans addition abusivede maxima deprofilsdifférents. Pasd’explorationQ31 gratuiteàcestade.

Marqueur pile :464octets de profondeur observée,1584octets auplancher intacts ; couverture aprèspeinture incluantWAIT/bench/statistiques/IRQ, excluantstartup etguard256B deframes déjàactives. Ce n’est pas une borneexhaustive. Image :text13240/data20/bss164232, réserveheap/pilelinker comprise ; ABIsoft/MLA réels, commandes/MAP/ELF fournis.

## Erreurs de contrôle et collecte corrigées

- Test hôte rouge/vert : effacement du marqueurcomplet au début d’une tentative, afinqu’un échec ultérieur ne conservepasl’ancien succès. Testscontrôle hôte etgardesoutils PASS ; faussecadencehôte jamaisprésentéecommeDWTphysique.
- AprèsreadbackWAIT, une erreurPython de posttraitement`st_size()` a été corrigée **horscible** ; bytesdéjàlus comparés etmanifesteachevé, sansretryGDB/flash.
- AprèsleRUNphysique réussi, GDB a refusé lenom dedumpentreguillemets. Générateurcorrigé ettesté. **Captureseule effectuée auDONE existant**, pasde nouveauRUN/reset/flash. Tracesdupremieréchec etdelacollectecorrigée toutespréservées. Aucunrésultatrecréé parsimulation.

## État actuel et poursuite

TR2-002 laissé aveccefirmware, **DONE4/0erreur**, aucunpériphériqueinitialisé ; mémoirederésultatscapturée, debuggerdétaché. Unflash, unRAMtrigger, zéroresetlogicielsupplémentairecommandé. FIFO/FRAM/RTU/SD : **NONEXÉCUTÉS** ; aucunecompensationdecetteabsenceparunrésultathôte.

Lorsdupassageausecondbanc, APIprimaireHTTP200 mais communicationModbusperdue, constatéeavantdebug/flash002. **Utilisateurconfirme qu’il nepeut pasalimenterTR2-001 etTR2-002 simultanément actuellement.** Alimentationprimaireoff acceptée pourcettecampagne ; serviceWindows inchangé poursuit ses pertes explicites, aucunSTART/STOPouautrecommandeCodex. PasdeFlash/FRAMprimaire relus/prétendusvalidés. Sonretour pourraêtre vérifié aprèslesessais, sans imposer lesdeuxalimentations.

Le kitcapteurhistorique etlesraccordements3,3V/SPI3 sontdemandés pourlaprochainephase. Aucunflashcapteur ni essaiWHO/FIFO avantconfirmationmatérielle. Préparationlogicielle indépendante en cours ; mêmesourceFIFO/validation/tempsµs, sanssecondlecteur, aucuneécritureSD/FRAM ouémissionRTU. CanoniqueARM toujourscompatible, cross-build-only PASS ;148/149testsnumeriques inchangés nonrejoués.

[Projet etséquencedesécurité](../../../experiments/r1_dsp_arm_bench/README.md), [preuvesmicro physiques](preuves/TR2-20261029-R1-ARM-DSP-FIFO-BENCH/micro/README.md). Local`/home/lolo/dev/msm/reports/TR2-r1-arm-dsp-fifo-bench/`. Journalactualisé. Rapportintermédiaire àpublier/relire avantpoursuite, statutIN_PROGRESS ; aucuneclôtureFIFO/production.
