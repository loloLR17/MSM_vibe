---
mission_id: TR2-20261017-R4V-RTU-START-STOP-LATENCY
status: IN_PROGRESS
base_ref: main
base_sha: 1a9cf6a34bd36a695fb0410ead4a65f38b948a1f
initial_head: 8010d0101d70170a13731ca928833ce8c95774c3
result_sha: 8db2f673ac46c3e4d2bb64f0bbd39ca16bd39a4e
created_at_utc: 2026-10-17T10:54:34.530677+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-V — diagnostic causal et correction qualifiés dans la portée bornée

**Résultat technique acquis ; IN_PROGRESS uniquement pour publication et relecture distante de ce bilan.** La reprise a trouvé le candidat déjà terminé, COM4 fermé, GDB détaché ; aucun essai physique, build ou test réussi n'a été rejoué. Aucun obstacle technique actif. [Chronologie, sûreté et limites](../../05_Firmware/LIVE_R4V.md).

## Résultats avant/après

Référence exécutée `f8bf7ca80a7dad385dd84d00ffecea7e3dea125b` ; candidat exécuté `8db2f673ac46c3e4d2bb64f0bbd39ca16bd39a4e`. Instrumentation identique, vraies commandes COM4 et vrai store, nouvelles identités monotones.

| Mesure | Référence | Candidat |
|---|---:|---:|
| Capture MCU → effet START |7,678087s|7,816831s|
| Capture MCU → terminal START |64,280557s|40,809834s|
| Capture MCU → effet STOP |56,628081s|33,028351s|
| Capture MCU → terminal STOP |64,318544s|40,858764s|
| Callback STOP seul |205µs|205µs|
| Fin callback → HELD publié |1242µs|429µs|
| ACKSTOP → premierHELD maître |56,354535s|32,866055s|
| Work/service/gapFIFO max |815/1624/1852µs|814/2063/2274µs|
| FIFOmax/erreurs |55/0|68/0|
| Réponses COM4 CRC exactes |1655|1075|
| B3 V-F/HELD |740/50|447/51|
| XYZ/TS |3084000/385500|1833115/229140|

Réduction STOP terminalMCU36,47%, ACK→HELD maître41,68%. **La latence restante est importante : aucun arrêt industriel rapide ni GO R4global/R5/R6 annoncé.** Horloges MCU DWT160MHz nominal et maître UTC/Stopwatch distinctes ; points de départ distincts, pas de synchronisation SI. CaptureMCU vue après traitementRTU, pas arrivée électrique ; derniers XYZ consommés, pas dernier produit garanti. B1 non supporté : aucune publication fictive.

## Cause et correctif minimal

Le service FIFO/SPI coûteux à chaque checkpoint CRC/copie/store occupe82,14% du temps STOP de référence ; quatre barrières sont conservées, I/O physique6,980s/commande. Pacing minimal250µs après **fin** de drain réduit la part service à71,41%. STOP candidat : spans40,859791s = service29,178062s +I/O6,979826s +résiduCPU/instrumentation4,701903s. Les deux barrières avant l'effet prennent33,0s ; le callback matériel prend205µs. Donc ni «FRAM seule63s» ni «callbackSTOP33s». Tables des admissions/STARTED/effets/observations/reçus/terminaux et bruts dans LIVE_R4V et summaries.

API `live_runtime_set_drain_period` arrêtée seulement, bornée work_limit/4 ; défaut0 préserve le comportement antérieur. UART8événements/deadlines à chaque checkpoint, aucune relaxation1000/2500/5000µs. Publication immédiate sur transitionrunning/error évite un cacheFRESH pendant10ms après STOP ; erreurforeground/service utilise le faultR2 existant puis publie invalide sans reçu fictif. Moteur R2, formats, barrières et anti-rejeu inchangés ; pas de RTOS/refonte. Fichiers principaux : `live_runtime.c/.h`, harnessR4Uparamétré/R4Vdédié, CMake et nouveau testpacing145. Probe explicitement NON-PRODUCTION opt-in ; V1/protocole1 inchangés.

## Validations effectives et incidents

- `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 ./tr2_validate.sh` :145tests complets et cross-build finalPASS, pertinent car runtime partagé modifié.144anciens tests conservés, dont1561coupures/rebootR2 ; contre-tests retry/collision/budgetSTOP, fauteFRAM/service, nonréentrée et invalidationB3. Logs final et intermédiaires conservés.
- Quatre suitesO2/ASan/UBSan ciblées PASS8,69s ; ARMbaseline/candidatPASS. BIN canonique `85b1be6044d76349a715ea0627b442bd3301e63daa606399b3d83bd5b9bbc0f5` inchangé et non flashé.
- Rouge/vert significatifs : faute service arrêtait le FIFO sans retirer le B3retenu, corrigée ; STOP avantpublication10ms laissait FRESH, corrigé. Deux erreurs initiales d'oracle corrigées explicitement (lecturehorloge modèle4µs, projectionB3enfaute retourneINVALID_STATE) ; aucun test supprimé/affaibli. Première variantepacingdepuisdébut jamaisflashée conservée distincte. Décodage horsligne d'un log GDB nonUTF8 repris avec remplacement des caractères d'affichage, pas de changement des octets bruts.
- Préflight10:18:02UTC UID/SN/M016/FRAM entière/WHO7B exacts. COM4FTDI0403:6001/BG04KSSW identifié, utilisateur avait confirmé raccordement ; COM3STLINK exclu, maître natif exclusif1152008E1/T1000, pasHP. Flashs normaux qualifiés et vérifiés, GDB seulement aux checkpoints/fin, pas de pause pendant acquisition.
- Référence START5/5/run8/STOP5/6 ; candidat START5/7/run9/STOP5/9 terminaux4/0. Retry exact sans effet, collision27 ; STOP5/8budgetrefus5/7/I/O0/journal58→58/root71→71/runningtrue. Refus négatif volontaire, aucune admission fictive ; STOPneuf9 sur budgets réels ensuite.
- Oracle indépendant4096XYZ réel : RMS958/955/75/13mg, peaks998/994/110/50mg exacts. UART0drop/0erreur, FIFO0erreur,25événements/11spans sans overflow. Pas de calibration vibratoire. Nsupérieurs non collectés.

## Persistance et fermeture matérielle

FRAM complète262144octets initiale `95dab7909dbbab0a85abf1a3d9f8292bafb48dc98de73f026ce2786251b9af41` ; aprèsbaseline `fce8e1f9330989e02f7139d10cb066383bca1c58447a00a9852a8ca6c4b4a682` ; finale **`70e273972a83e61283a6e572db3187af002fd97ba8de63f8c709d3e30af73587`**. Deux snapshots finaux exacts aux modèles utilisant le vrai store/R2 ;419408octetsécrits/8commits parphase, uniquementLIVE. Journal46→54→62, root59→67→75, epoch5/run9/config3gen3, STOP5/9COMPLETED/effect39phase3. Historique/tail exacts. Aucune clé admise recyclée, retraite ou epochréinitialisé.

**M016 CommissioningReadOnly WAIT restauré à10:52:16UTC**,19pagesnormales155648octets ; relecture intégraleFlash2097152octets identique àl'initiale **`c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`**. FRAMentière relue aprèsrestauration identique70e finale, aucunrollback ; capteurWHO7B/ODRoff/FIFObypass/TSoff constaté ensuite, COM4fermé/GDBdétaché. Aucun SD/HP/câblage/OptionBytes/RDP/masserase. Les permissions commit/push/flash/debug ont été exercées uniquement dans ce périmètre.

## Preuves et traçabilité

[Référence physique](preuves/TR2-20261017-R4V/baseline-summary.json), [candidat physique](preuves/TR2-20261017-R4V/paced-summary.json), [refus et comparaison](preuves/TR2-20261017-R4V/comparison-final.json), [auditFRAM](preuves/TR2-20261017-R4V/final-fram-audit.json), [restauration](preuves/TR2-20261017-R4V/hardware/restoration-summary.json), [bundle bruts/empreintes](preuves/TR2-20261017-R4V/BUNDLE.json). Captures COM4 complètes, traces GDB/commandes, modèles, sources du maître, BIN/ELF et backups complets conservés souspreuves ; zlib standard avec SHA brut et stocké. Copieopérationnelle `/home/lolo/dev/msm/reports/TR2-r4v`.

BINbaseline150664octets SHA e6e744dbd414014a34af86577987cf6fb82b242499687e9053e79bce029db676 ; BINcandidat150768octets SHA a4c253386ba488cf9cd54373ac9cc3ca8b3a9d096260693c8a334d19c6663e1f. ELFbaseline25c3eea7077700acbbc0ea465131bfdd7f43a4c3e7e65196de68d236523e6e20 ; ELF candidat698f97902dd70e9e18c9ed3fc41c21fd38451e0e2cde77cdaf5ff28e60c76dc8.

Worktree isolé `MSM_vibe-R4V-rtu-latency`, branche `codex/r4v-rtu-latency` versmain parpushnormalHEAD:refs/heads/main. Ancienmain82169af propre préservé,62autresworktrees,2stashes6251b988…/f3b829e6… et6873fichiers protégés vérifiés intacts. ÉtudeR4S86d0f4b et archivesR4U inchangées. Commits publiés/revus f8bf7ca instrumentation, bad1041baseline,8db2f67correctif final ; HEADlocal=origin/main8db2f67propre avantce bilan. Pas de merge/rebase/reset/forcepush.

Journal actualisé : nouveau bilanR4V, état matérielfinal/journal62/run9, diagnostic et latencerésiduelle ; roadmapR2/R3/R4 factualisée sans qualificationindépendante. Missionactive et registredettesA04préservés. Ce rapport attend son premierpush/relecture ; statutdepublication distinct du résultattechnique.

## Limites et recommandations pour ChatGPT

Diagnostic et correction minimale prouvés dans ce périmètre, aucune garantie industrielle. Les maxima mesurés ne sont pas un WCET ; fautesFRAM/service et coupures/reboot couverts surhôte, aucun défautélectrique ni coupurephysiqueinjecté ici. Pas de bornageIRQ/HALisolé/pile/endurance/dériveSI/thermique. Pas de produitLIVEcomplet, B1/B2/B7absents/B6UNAVAILABLE/capLIVE16absente ; R5PC/SQLite/Web etR6bout-en-bout nonqualifiés.

L'optimisation seule ne répond pas à un éventuel besoin d'arrêt rapide. Faire décider explicitement le besoin de sécurité et la sémantique deSTOP ; envisager contratd'optimisationbornée CPU/CRC/service sanschangerbarrières, ou décisiond'architecture surreprésentationdurable/voied'arrêtindépendante. Aucun déplacementdebarrière/changementnormatif appliqué. Contrôle indépendantChatGPT requis, pas de tag ni GO R4global/R5/R6 parCodex.
