---
mission_id: TR2-20261012-M015-D-FIFO-ACTIVE-READBACK
status: BLOCKED
base_ref: main
base_sha: 37b97f7fd8ea70c4ae8e377a3042bb3e2b8c5328
initial_head: 1ba8a5f7816eba37b9c45a4a1f7ff43cc3add178
result_sha: null
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-D — Tranche logicielle prête, diagnostic physique en préparation

Audit et test discriminant dans `../AUDIT_M015_D_FIFO_ACTIVE.md`. Profil isolé borné : lectures brutes simples/groupées pendant remplissage puis entrée coupée/FIFO nonvidée, sept registres volatils sauvegardés/restaurés. Aucun XYZ, DATA_OUT, média ou transport démarré. Code neuf et test ciblé1/1, cross-build défaut/profil réussis ; résultats A/B/C inchangés nonrejoués. Injection initiale du mock mal synchronisée corrigée, sorties d’échec conservées ; pas défaut firmware constaté.

Archives C exactes depuis37b97f7, worktree D isolé/codex/m015-d ; mission active inchangée, anciens worktrees et stashes préservés. Tranche logicielle publiée avant flash ; BLOCKED provisoire en attente de la campagne unique/restauration/publication finale. Permissions commit/push/debug, flash autorisé sous preflight seulement. M015 globale IN PROGRESS.

Preuves locales : /home/lolo/dev/msm/reports/TR2-mission015-D-20261012T135359Z ; logsvalidation-initiaux etfinals, ELF/BIN dédiés. Source finale de la tranche et relecture distante consignées après commit. Aucun résultat physique D déclaré à ce stade.
