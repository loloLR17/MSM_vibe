---
mission_id: TR2-20261028-ACCEL-FS8G-VIBRATION-AC
status: IN_PROGRESS
base_ref: main
base_sha: e7f17f2cd2b8470244a99f7c9e73e9d0f43b3dea
initial_head: afa7d707b50d56b771c3dcb7bf719ab66aa9cbb6
result_sha: ae6e2d5e9cd2d161abeda1659d99296a0c274ab8
created_at_utc: 2026-10-28T22:30:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Checkpoint logiciel — AC1 dynamique LIVE et configuration nominale ±8g

**Mission en cours, non clôturée.** Addendum36fd068 récupéré/relu avant poursuite, puis décisionutilisateur explicite autorisant AC1v1 readonly8000–8063 exclusivementopérateurNON-PRODUCTION. La suppression effective deDC enLIVE et±8g ont la même priorité. Cecheckpoint ne prétend pas qualifier les deux fonctionnalités physiquement.

Le lot logiciel est publié au72a7b573 : traitementdynamiquecontinu paraxe, passe-haut2Hznominal/Q16micro-g, préparation26667échantillonsauSTART, fenêtresN/Psurgrille deSTART, RMS/crêtesXYZ/global calculés sur le signal filtré. B3absolu/registre/modèlenormatif inchangés ; métadonnées et données AC1distinctes, pas de reconstruction àpartirB3. AC1estdésactivépar défautet activé uniquementdanslacompositionopérateur ; sourceCSlit lecanal seulementavec`--ac1-experimental`, archiveunobjetoptionneldistinctsansrewritedesancienspayloads, UI/CSVnommentexplicitementlesgrandeurs etleursprovenances.

Le cheminB4préexistant sert à préparer/copierl’active avecFS=2etnouvelID/CRC, relirelestaging puis soumettrelevraiAPPLY1avecleledgeractuel/handofftokenisé/portexclusif. AucunwriteFRAMbrut. Le contrôlebootn’imposeplus lesseulsconfig3/gen3, toutenconservantautoritéactive/CRC/identitéetR2terminalnonindéterminé. La calibrationtemporelleduprofilopérateurreprend également leFSconfiguré ; elle neréintroduitpas±2g aprèsrebootd’uneconfiguration±8g.

**Validationsréellement acquises :**148testsC+cross-buildcanoniquePASS ; imageopérateurARMcompilée,150484text/492data/659176bss. ASan/UBSanO2 :3suitesPASS, dont448oraclesB3inchangés. TransfertAC1conformeauxattendusanalytiques20/50/1000Hz500mg et100Hz6500mg(rejeulogiCiel, **pas**excitationphysique±6g), DCconstantrejeté, quatreFS/inversionssignées/rails/STOP/restart/faute. Blocopt-in/readonly/borne/absencecollisiontestés, modèlecanonique8000..8063resteillégal. .NET305autrestestsPASS etfinalsprotocole52/Service252PASS ; JSAC1etnonrégressionshistoriquesPASS. Échecsinitiauxdefixtures/typepréservés/corrigés, attendusnonaffaiblis.

**État matériel avant toute mutation :**à22:27:29UTC, serviceWindowsinitialCOM4connecté, B3arrêté/nonFRESH, ledger5/25. Aucunflash/reset/debug/commandeécrituredanscettemissionencore. Imagec36/paquetd677et378fichiersinitiauxmanifestés/préservés. Vérificationmatériellefraîche/Flash/FRAMsûreàfaireavantflash ; nepasprésumerleshashesphysiquesantérieursactuels. WorktreeisoléMSM_vibe-accel-fs8g-vibration-ac, ancienmain/worktrees/stashespréservés. Gouvernance/journalinchangésdepuisledébut, lus/référencésàpartirdelaversionréelle ; archiveprécédentepréservée.

**Prochaine tranche autorisée, sans demande intermédiaire :** paquetWindowscomplet/gateCOM99 avant modificationdesversionsfonctionnelles ; préflightSTOPetbackupvérifiéFlash/FRAMhorsscope ; qualificationbornéeAC1/FS, vraiAPPLY±8gdurable/resetetreadbackCTRL1_XL ; START/STOPphysiques, chaînemesuredynamiqueCOM4→SQLite/API→Edge etconservationdeshistoriques. B3absolurestedistinct. Aucunessaimachine±6ginventé. SiWindowsrefuse le nouveaupaquet, conserverl’ancienetnecontourneraucuneprotection ; rendrecompteavecsolution/livraisonpartiellefranche. StatutIN_PROGRESS, pasBLOCKEDenl’absenced’obstacle.

[ExtensionAC1v1etdécision](../../Supervision%20TR2/operator-demo/AC1_V1.md), [preuveslogicielles](preuves/TR2-20261028-ACCEL-FS8G-AC1/software/README.md). Preuveslocales`/home/lolo/dev/msm/reports/TR2-accel-fs8g-ac/`. LesSHA/étatsdepublicationserontvérifiésaprèschaquepush ; unecompilationoucecheckpointneconstituepaslaqualificationLIVErequiseparl’addendum.


## Reprise du 29 octobre 2026 — checkpoint intermédiaire

Le worktree précédent et ses deux fichiers non committés sont préservés. Main a avancé par trois documents R0 métier uniquement ; le correctif est réappliqué dans un nouveau worktree `MSM_vibe-accel-fs8g-ac-resume` basé sur bdaa3335, sans merge/rebase ni abandon de travail. Le commit ae6e2d5e autorise uniquement la lecture exacte AC1 8000/64 lorsque l’option expérimentale est présente. Test rouge/vert conservé, 253 tests Service réussis ; aucun besoin de répéter les tests C sur code inchangé. Push normal et SHA distant ae6e2d5e vérifiés.

**Rectification de l’état matériel décrit plus haut :** la tranche précédente a effectivement sauvegardé deux lectures Flash complètes identiques et la FRAM, puis flashé une fois l’image e1846c87017fd7667daf1b79cb440af1024346c5deae8d5065f8525c00aa7b80. Un contrôle GDB trop précoce a interrompu la calibration au premier démarrage, conservé comme échec. Après diagnostic FIFO arrêté/aucune acquisition active, un reset logiciel de récupération a été effectué, sans reflash ni écriture FRAM brute. B0/B3 répondent ensuite. Le refus AC1 provient de la liste de lectures autorisées du transport PC, corrigée ci-dessus. Ne pas attribuer ce refus local au STM32.

À la reprise, le 29/10 à 06:14:59 UTC, l’API reçoit réellement B3 STOP, flags4097/seq0, puis refuse AC1 localement. Ledger relu : epoch5/transaction25 ; aucune nouvelle commande B5 soumise par cette mission à ce point. Le paquet Windows R1 est préservé ; publication complète R2 séparée en cours, gate de confiance requis avant usage COM4. Plage ±8g persistante, mesures dynamiques physiques et chaîne Edge restent **non qualifiées**, mission IN_PROGRESS. Aucun nouveau flash/reset à la reprise. [Preuves de reprise](preuves/TR2-20261028-ACCEL-FS8G-AC1/resume/BUNDLE.json).
