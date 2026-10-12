---
mission_id: TR2-20261012-M015-J-CONTINUOUS-FIFO
status: BLOCKED
base_ref: main
base_sha: d25aa42a3d2c4850f4cedeeaca545da63aad0c29
initial_head: de173b229d4a2cbaf54aa9f9f208cd186443f73d
result_sha: null
created_at_utc: 2026-10-12T18:50:02.647108+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-J — Logiciel et préflight validés, campagne en attente

BLOCKED provisoire pour essai physique et relecture finale. Nouveau profil exclusif/OFF :FIFO mode6/STOP_ON_WTM0, fenêtre4096XYZ RAM,32bursts128 maximum,192statuts/384traces/250ms, entréeA8 activependantvidange. Contrôlesgroupés I etguardstransfert stricts, niveaux/flags/tags/parité/compteurs/timingconservation etownership/validitéwindow bornés. Aucunfallback/lecture simple/DMA/média/coremappingV1 modifié.

[Protocole/resources/limites](../QUALIFICATION_M015_J_FIFO_CONTINUE.md) :104100BSS/20data GNUsize, RAM768KiB et réservepile2048, buffersownedstatiques ; rétentiondiagnostique complète×8 ne tientpas pour32768, propositionsegmentée future sanschangeV1/média. Cadencecohérence ±5%/2mots déclarée, pas mesure indépendanteODR ni garantie absoluepertes.

VALIDÉ logiciel :18scénarios ciblés/CTest1/1 etcrossdéfaut-profil réussis, warningsNewlibusuels,text37248/BIN37268. Anciennesvalidations nonrépétées. Profilconcurrentrefusé etwrapperHPJ Steady-only/deuxFC03/parserPS contrôlés sansport. Commandes/logs/resourcebudgetpubliés. Aucunflash/campagneJ effectué àcecheckpoint.

Préflightfresh :UID004A003D3842501820333157/STLINK003500463235510B37333439/M014-H reconnu, sauvegardeFlash2Mio/RAM768K hashées ;stage2/result0/FRAMgen12/configUNAVAILABLE/IRQ49-66/faults0. GDBdétaché enexécution. Restoreenfinally prêt, imagepermanenteBINb462c29a/ELFc37ad837 ;aucuneFRAM/SDwrite oupartitiontouchée.

Archives I-physical exactes depuisd25aa42, worktreeJ isolé, autresworktrees/stashes préservés. Journal etroadmap actualisés ;permissionscommit/push/debuglecture exercées, flashàsuivre aprèscheckpoint etreadback. Preuves /home/lolo/dev/msm/reports/TR2-mission015-J-20261012T184145Z et `preuves/TR2-20261012-M015-J/`. M015globaleINPROGRESS, aucunequalificationphysiquecontinueannoncée àcepoint.
