---
mission_id: TR2-20261012-M015-H-CTRL1-DIFFERENTIAL
status: BLOCKED
base_ref: main
base_sha: 6357b667a45b0979e5edaf394cb273595295414d
initial_head: 168ddbd444d0d8d6aa3ceabe115cc315d2ad5452
result_sha: null
created_at_utc: 2026-10-12T15:59:24.326529+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-H — Diagnostic différentiel préparé

BLOCKED provisoire en attente de la campagne unique et de sa publication/relecture. [Protocole](../DIAGNOSTIC_M015_H_CTRL1_DIFFERENTIEL.md) : même SPI mode3/2,5MHz, baseline puis writeA8, lecture simple/groupée contenant10 à ordre fixe S G G S, immédiatement puis après20ms, enfin retour arrêté. Zéro FIFO/XYZ ; divergence latched, aucune admission par vote/fallback. Erreur de sûreté arrête vers restauration.

VALIDÉ logiciel : nouveau harness OFF/exclusif et test ciblé1/1 avec53scénarios ; cross-build défaut/profil réussis (text33280/data20/bss9852, BIN33300). Rejet de profil concurrent, wrapper HP Steady-only/deuxFC03 et parsing PS contrôlés horsport. Commandes et sorties conservées, warnings Newlib habituels. Cœur/mapping/validator/V1 inchangés, tests et campagnes A-G nonrejoués. Choixproportionné : harness et branche isolés uniquement.

Préflight observé : STLINK003500463235510B37333439/UID004A003D3842501820333157, permanent M014-H reconnu ; Flash2Mio etRAM768K sauvegardées/hachées, stage2/result0/FRAMgen12/configUNAVAILABLE/IRQ49-66/faults0. MCU encore M014-H ; aucun flash/reset H à ce point. Les IRQ externes attendues du profil H sont absentes par conception, distinctes de la production ; snapshots NVIC CPU ajoutés. Précédent HOTPLUG M013-C/D relu, procédure corrigée maintenue.

Archives G exactes depuis6357b66, mission active immuable, worktree H isolé, original/huit worktrees antérieurs/stashes conservés. Journal etroadmap actualisés pour préparation. Commit/push/debug lecture autorisés exercés ; flash ensuite après publication. Preuves complètes : /home/lolo/dev/msm/reports/TR2-mission015-H-20261012T155415Z. Aucun instrument/recâblage/remplacement demandé, aucuneFRAM/SD écrite ni partition accédée.
