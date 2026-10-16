---
mission_id: TR2-20261016-R4S-SERVICE-BUDGET-SAFE-RECOVERY
status: IN_PROGRESS
base_ref: main
base_sha: a57cc5bcc84e593a4a99f4100600346b6d990eb4
initial_head: 20c8089514b40a75ed59d7c6616b75838482e0c8
result_sha: 452ab6cc3a5b871b19648448740a9fb08ce12f9b
created_at_utc: 2026-10-16T21:32:09.743860+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-S — phase 1 PASS, reprise transactionnelle en préparation

**IN_PROGRESS**, poursuite autonome versphase2. Préflight UID/SN/FlashM016/FRAM14fb
conforme, archiveR4-R déjàprésente vérifiéeexacte ; worktreeisolé/stashes préservés.
Aucune admission ni écriture R2/FRAM durant cette phase.

Trois essais distincts conservés :

1. Affine fixe :31787XYZ,1,206563s ; budgets335/2322/2457us conformes, arrêt sur
 écart109>100us.4096XYZ diagnostiques vérifiésparoracle (RMS960mg/peak997mg global).
2. Suivi/instrumentation :746XYZ, arrêt service2511>2500us ; traceappariée prouve
 poll1991us dont métrologie556/date102, RTU513. Feuilles de calcul encore-O0 ;
 aucune nouvellewindow. Une plageGDBvide a nécessité collecteFRAM terminale séparée,
 sans rejouer l'essai ; snapshotsintacts. Ne pas déclarer ce probePASS.
3. Feuilles statistiques/calculateur-O2 dansR4S uniquement : **10,001045s,
265669XYZ/33209TS,5fenêtres diagnostiques et808RTU**. Work333/service1804/gap1902us,
FIFOmax58/0erreur ; STARTvolatil911us/STOP232us.99contrôles de conversion capteur→DWT,
maximum10us, seuil100us conservé. Traceappariée maximale1801us (poll1248 dont
consommation351, date64/métrologie201 ; clock30/RTU513). Ce sont des coûts inclus,
non additionnables àleurs parents. Oracle4096 bruts : RMSglobal/X/Y/Z959/953/102/39mg,
crêtes998/992/138/82mg exactes. FRAM entière14fb byte-identique,0tentative d'écriture.

**Pas B3 LIVE autoritatif** : identité d'acquisition diagnostique volatile, autorité
métrologiefalse/V0/F0 enRTU. ÉtatR2 reste STARTED epoch4/tx5/run5. Pas UTC/SI ni
qualificationR4/R6, UART/RS485/SD/PC. Le provider corrige la conversion entrehorloges
àpartir de brackets réellement observées ; pivotcontinu, pas de grilleODR ni
redatation. Les141tests complets60,50s/ARM etASan/UBSan-O2/provider+448oracles passent.
V1 etR2/B5 inchangés ; validation complète justifiée par ajout du suivi temporel.

Image troisième probe etpreuves exactes BIN/ELF dans phase1c-build.json et
[preuvesR4S](preuves/TR2-20261016-R4S/README.md). Détails [LIVE_R4S](../../05_Firmware/LIVE_R4S.md).
Carte au terminal arrêté, GDBdétaché, M016backup récupérable ; restauration finale
prévue après lesprobes suivants, pas de flash intermédiaire inutile.

**Prochaine tranche** : preuve d'applicabilité du retraitisolé R4-M/R4-P au vrai
snapshot14fb, tests coupures/contre-tests/anti-rejeu ; seules transitions R2 réelles
seront admises après toutespréconditions. Aucun isolement/RENEW/START physique
encore effectué. Run5 resteconsommé ; toute décision hors procédure sera escaladée.

Checkpointprécédent c071d05f861ec20698ae61776a45b4441bd78dec relu depuisGitHub,
69fichiers/58preuves vérifiés. Publication de ce nouveaucheckpoint àvérifier après
push ; le statut n'est pas une fin de mission. Journal actualisé.
