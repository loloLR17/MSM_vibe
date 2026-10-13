---
mission_id: TR2-20261013-M015-U-INTEGRATION-TR2-001
status: BLOCKED
base_ref: main
base_sha: null
initial_head: c27621f
result_sha: null
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-U — Checkpoint logiciel avant qualification TR2-001

Contrat courant sans base_sha ; référence main récupérée c27621f. Dépôt principal historique82169af propre, worktrees/stashes préservés. Worktree codex/m015-u isolé. Contrat/rapport T-reprise archivés byte-exacts depuis51dff58.

TR2-001 reconnecté rapporté par utilisateur, seule sonde003500463235510B37333439 observée, UID004A003D3842501820333157 lu. M014-H présent, stage2/result0, FRAM recovery VALID/gen12, configUNAVAILABLE, NVIC49/66 actifs, faults0. Flash2MiB double identique et prefix BINhistorique b462c29a exact, RAM sauvegardée. Aucun reset/flash à ce checkpoint.

Profil RAM ST4096 intégré au main principal, OFF par défaut, choixCPU160/HSI16/SPI4/mode3. Driver/exemple ST, parser et instrumentation T réutilisés, admission READY/INVALID distincte deB1. SPI1 protégé readonly et SD/UART inaccessibles au profil ; pas activation V1 ni stockage.

Validation complète123/123 et cross-build défaut réussis, ARMprofil/production réussis, quatre refus de profils concurrents ; forbidden symbols SD/FRAMwrite/UART absents ELFisolé. Buildproduction nouveau nonidentique M014-H : compilation seulement, retour prévu à l’artefact historique qualifié. Premier lancement direct script126 (nonexécutable) remplacé bash ; déclarationbench_result manquante puis collisionmacro corrigées ; omissionnoreturn détectée par contrôle symboles et corrigée avant accès physique. Logs conservés, guards nonaffaiblis.

[Audit/matrice/plan](../INTEGRATION_FIFO_M015_U.md), [preuves](preuves/TR2-20261013-M015-U/). BLOCKED provisoire : qualification physique et publication finale encore à réaliser. M015 IN PROGRESS, aucune réussite physique U prétendue. Firmware permanent toujours installé.
