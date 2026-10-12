---
mission_id: TR2-20261012-M015-C-FIFO-REGISTER-RELIABILITY
status: BLOCKED
base_ref: main
base_sha: cfb9918dfe8e5ac3148d6ceaa79e50acda6ac53f
initial_head: 834f35e1759b3dd9ac1d42902f1c33cfad63f1c4
result_sha: null
created_at_utc: 2026-10-12T13:20:30.315264+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-C — Tranche logicielle publiée, diagnostic cible en préparation

Audit statique et tableau hypothèses/discriminants dans `../AUDIT_M015_C_SPI_FIFO.md`. Archives B mission/rapport copiées exactement depuis cfb9918 sans écrasement ; mission C active inchangée. Worktree isolé MSM_vibe-M015-C/codex/m015-c ; originalmain82169af divergé préservé, aucun pull/rebase/merge/reset, deux stashes intacts.

Probe lectureseule512transactions bornées, RX bruts/CS/temps/SR/CFG/HAL enregistrés ; aucune écriture capteur, FRAM ou SD, aucun FIFO/XYZ/UART démarré. Production et driver inchangés. Test ciblé neuf1/1, cross-build défaut et dédié réussis, warningsNewlib conservés. Commandes et sorties effectives dans **/home/lolo/dev/msm/reports/TR2-mission015-C-20261012T131552Z** (`validation-0..4.log`, `default-cross.log`, `register-symbols.txt`). Justification : harness isolé seulement ; pas répétition des tests A/B/.NET.

Sauvegarde GDB seule effectuée :UID004A003D3842501820333157, permanentBIN SHA b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611 vérifié contre Flash, backupFlash2Mio/RAM768K. Bootproductionstage2/result0/FRAMgen12/configUNAVAILABLE, IRQ49/66/faults0, détaché running. Aucun flash C à ce stade. Permissions commit/push/debug exercées ; flash autorisé mais pas encore exercé.

BLOCKED provisoire en attente de l’unique diagnostic cible et de relecture distante. Pas de fiabilité ni cause précise déclarée. Firmware permanent M014 en place ; aucune requalification M014/A/B. Publication de cette tranche validée avant poursuite ; rapport complété après acquisition/restauration.
