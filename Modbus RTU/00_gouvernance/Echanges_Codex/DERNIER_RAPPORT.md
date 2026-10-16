---
mission_id: TR2-20261016-R4T-MONOTONIC-JOURNAL-RECOVERY-B3
status: IN_PROGRESS
base_ref: main
base_sha: 1799b97db7d1c5dc0bc3892501569fdcd7a4e0b9
initial_head: d17c5457e35fd99269bdc0312fb5c48770ff0572
result_sha: null
created_at_utc: 2026-10-16T22:05:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-T — checkpoint logiciel avant mutation physique

IN_PROGRESS, poursuite autonome vers maintenance puis B3 si gardes PASS.
[Plan exact et limites de reprise](../../05_Firmware/LIVE_R4T.md).
Solution isolée : retraite du seul slot3 et vrai RESERVED RENEW27 dans la même
transaction, sans publication d'un journal vide. Vrais steps28/29/30, runfloor5
préservé et epoch4→5. Ancien STARTED5 archivé indéterminé, aucun reçu inventé.
Cœur R2, formats, gardes produit, R0/V1 et anciennes assertions inchangés.

Nouveau test ciblé sur copie14fb :568cas de coupure sur4barrières, anti-rejeu,
SHA altérés, monotonie/reboot, INTENT/protégés, prochainrun6/STOP : PASS22,86s.
Même suite O2/ASan/UBSan PASS13,41s.141tests R4-S déjà acquis sur cœur inchangé,
non répétés ; nouveau nombre142enregistrés n'est pas une suite complète relancée.
ARM canonique et probe explicite NON-PRODUCTION réussis. Lancement direct initial
tr2_validate refusé (bit exécutable absent), puis commande documentée via bash
--cross-build-only réussie ; aucun changement de permission/configuration.

Préflight réel22:00:20UTC : UID/SN attendus, capteur arrêté, Flash2Mio c5bc344d…f5b26
et FRAM26214414fb172f…2aa9 exactes aux sauvegardes réutilisées R4-S. Aucun flash,
reset ou writeFRAM R4-T à ce checkpoint ; aucun nouveau run/epoch consommé.
Worktree isolé MSM_vibe-R4T-monotonic-recovery, ancienmain82169af/stashes et étude
86d0f4b préservés. Les archives R4-S existent déjà, à conserver inchangées.

Publication du plan/code/tests/preuves avant toute mutation ; SHA du code exécuté
et image exacte seront consignés au départ du probe, après ce commit. Pas de
qualification physique présumée, R4/R6 ouverts. Voir [preuves](preuves/TR2-20261016-R4T/README.md).
