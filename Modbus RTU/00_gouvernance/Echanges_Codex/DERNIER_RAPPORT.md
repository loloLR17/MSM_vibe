---
mission_id: TR2-20261012-M015-I-GROUPED-FIFO-SOLUTION
status: IN_PROGRESS
base_ref: main
base_sha: c0d4f7b9786921db64f9e031f5458a662ba90332
initial_head: 1c1f9ae6a241d65efbc04e1c5f44ef19fc7e0d9f
result_sha: null
created_at_utc: 2026-10-12T16:27:24.655352+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-I — Logiciel validé, campagne physique reportée

**IN_PROGRESS : tranche logicielle prête ; voie FIFO réelle NON VÉRIFIÉE.** Campagne reportée après renouvellement pour préserver la réserve de publication/restauration avec12% de quota annoncé. Aucun flash/debug/reset/HP ou accès média en I ; **M014-H n’a pas été remplacé**, aucun besoin de restauration créé cette session. Dernier état cible H historique : Flash2Mio byte-identique, FRAMgen12/configUNAVAILABLE/SDOFF/MCUrunning, B0/B1passés ; pas observation matérielle nouvelle I.

Checkpoint initial **c84de86168c5d8495ff38e633cb54a899a9c7d79**, publié puis relu avant code. Archives mission/rapport H exactes,12 artefacts H rehashés, dix worktrees antérieurs/deux stashes intacts. Source et référence de mission réelles vérifiées ; corps entier du journal déjà lu réutilisé après comparaison byte/diff du delta H, aucune information ancienne supposée actuelle.

Modification minimale de `stm32_iis3dwb_burst_capture.c` et son test : readbacks groupés07..0A et0F..14, WHO/IF_INC/CTRL4 cohérents, audit initial groupé ; aucune lecture simple sur le chemin. Les sept attendus restent stricts, CTRL1=A8 obligatoire en actif, gardes FIFO et intégrité inchangés. HAL_OK seul ne suffit plus pour tout transfert : ErrorCode/compteurs/CS doivent être conformes ; validités/firstref préservés, aucunvote/fallback. Un groupe incohérent refuse immédiatement avant admission, pas de recherche générale de0x50. La lecture groupée reste **candidate**, pas réputée fiable par principe.

VALIDÉ logiciel : CMakehost/build du seultest puis `ctest --test-dir …/build-host -R '^stm32_burst_capture$' --output-on-failure`, **1/1 passé,26scénarios** ; nominaldeuxbursts128, gaps/parité/timeout/overrun/préexistants,septreadbacks/écritures enéchec, WHO/CTRL4/IF_INC incohérents, HAL_OK ambigu/error ou Rxleft. Assertions gardées/adaptées à la nouvelle méthode ; aucun test affaibli. Cross-build défaut par `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only`, profil DebugNinja/burstON : réussis,text35112/data20/bss16788/BIN35132. WarningsNewlib usuels ; aucunmedia/production/UARTstart/Hprobe dansprofil. Commandes exactes etsorties0..5 publiques. A-H inchangés nonrejoués ;26cas nécessaires carle harness FIFO réel change. Cœur/V1 inchangés ; validation proportionnéeharness seulement.

BIN SHA **760aab56cf32aa458c861bf5f22e2801dd595f3bd73ad6a98602c1cff0cdd258**, ELF **7893fda8b3e43e215cbab0a1bcc515d51b0a6986cdf9ff742974e53b8cbb076e** ; bruts horsGit `/home/lolo/dev/msm/reports/TR2-mission015-I-20261012T162708Z`. Aucun résultatphysique/tag/XYZ/débit/spectre inventé, pasFIFOcontinue/V1qualifiés. ODR26667/fenêtres4096..32768/continuité/store-and-forward inchangés. Les différences de stratégie rendent nécessaire une nouvelle preuve physique bornée ; les acquis H ne la remplacent pas.

[Point de reprise complet](../REPRISE_M015_I_FIFO_GROUPEE.md) : à la reprise vérifiermain/mission/SHA, réutiliserbuilds/tests siinchangés ; préparerhelpercampagneG adaptéI etnomHPguardéI, preflightUID/image +nouvelle sauvegardeFlash2Mio/RAM. Une activationmaximum trigger4D313546, tracessauvegardées avantrestaurationfinally, NORMAL sansHOTPLUG ; permanentM014-H BINb462c29a restauré byte-identique,2lectures derepriseB0/B1sansGDB puisjournalcomplet etrapport. **Ne pas lancer l’ancien helper G ni flasher avant marge de restauration garantie.** Pas de demandevalidation supplémentaire des opérations autorisées ; intervention humaine seulement si physique indispensable/risque/décisionstructurante.

Journal actualisé etroadmap M015 INPROGRESS ; mission I active immuable, worktree isolé codex/m015-i. Permissionscommit/push exercées uniquement ; flash/debug autorisés NONexercés. Archives/brutsH/FRAM/SD/partition/stashespréservés. Diff/check/index inspectés ; pasmerge/rebase/reset/forcepush/configuration permanente. Checkpointlogiciel/relecture finale etSHA à consigner après publication, aucunstatusDONEprematuré.
