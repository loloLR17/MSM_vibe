---
mission_id: TR2-20261012-M015-G-CONFIG-READBACK
status: BLOCKED
base_ref: main
base_sha: e11088772987a50ce729eb4ca1148d657b2cc673
initial_head: fc60d98dd2d1dc64f377f613dbf6ce0ec65ff11a
result_sha: null
created_at_utc: 2026-10-12T15:28:10.201893+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-G — Tranche logicielle préparée, essai physique en attente

Statut provisoire BLOCKED : acquisition et relecture distante finales à effectuer. Aucun essai G effectué à cette publication.

L’audit `../AUDIT_M015_G_CONFIG_READBACK.md` explique la perte de télémétrie F. Instrumentation minimale du harness existant : premier refus immuable, registre/attendu/lu, validités explicites, 96 échanges bornés TX/RX/HAL/SPI/CS/DWT. Attendus et gardes conservés ; aucune substitution ni nouvelle transaction SPI. Cœur/driver/V1 inchangés. Wrapper HP étendu au seul nom G, toujours deux lectures Steady au maximum.

VALIDÉ logiciel : test ciblé 1/1 avec21 scénarios ; cross-build profil dédié et `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only` réussis. Pas de répétition des campagnes A-F ni des validations acquises. BSS16788/text34936/data20 ; warnings Newlib habituels. Commandes et sorties conservées sous `preuves/TR2-20261012-M015-G/`.

Préflight G observé : UID004A003D3842501820333157/STLINK003500463235510B37333439, image permanente reconnue ; sauvegarde Flash2Mio SHA556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0 et RAM768K conservées. GDB seul, sans reset/flash ; stage2/result0/FRAMgen12/configUNAVAILABLE/IRQ49-66/faults0. M014-H encore en exécution. Aucun accès SD/FRAM d’écriture.

Prochaine tranche autorisée : un flash NORMAL diagnostique, readback/reset qualifié, un trigger unique ; rejet immédiat si configuration différente, sinon deux bursts128 maximum. Restauration M014-H byte-identique et contrôle minimal HP B0/B1. Aucun débit continu, FFT, fenêtre V1 ou acquisition de production n’est validé.

Archives F exactes depuis e110887 ; worktree G isolé, original et sept worktrees antérieurs/stashes préservés. Permissions commit/push et debug en lecture exercées ; flash pas encore exercé. Preuves complètes : /home/lolo/dev/msm/reports/TR2-mission015-G-20261012T152038Z. Journal actualisé ; M015 globale IN PROGRESS. SHA final et état Git seront vérifiés après publication.
