---
mission_id: TR2-20261013-M015-S-SPI-FREQUENCY-SWEEP
status: BLOCKED
base_ref: main
base_sha: 4c0785bf11dfedabe93e996fefecdb58971d0177
initial_head: 7e28d919974f7c24356b88c3ab72f1f5803fb513
result_sha: null
created_at_utc: 2026-10-12T23:19:00Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-S — checkpoint préparation comparative

Deux profils4/8MHz préparés, CPU16 et seuldiviseurSPI3 changé, acquisition4096 etguards Q inchangés. TroisARM réussis, référence2MHz byte-identiqueQ, testdiviseurhorspérimètre refusé ; aucune validationantérieure rejouée. Pasencore d'accèscible. Protocole et limitesconstructeur dans [QUALIFICATION_M015_S](../../../experiments/iis3dwb_st_reference/QUALIFICATION_M015_S.md) :max10MHzcapteur, conditionsVDD3,3V utilisateur conservées, signalintégrité nonmesurée. Unecapture parfréquence ;stopdéfautcritique,8conditionné.

AuditFRAM lecturedesources :SPI1/PA5-7/PD14, capteurSPI3/PC9-12 distincts.2,5MHzFRAMhistorique qualifié, maxcomposant40MHz àVDD2,7..3,6V nequalifiepaslebusTR2. AucunchangementFRAMprimaire, archivesRexactes/stashes/worktreespréservés. BLOCKEDprovisoire pourphysique/publication àfaire ;4096nonvalidé,512/répétabilitéQconservés. Preuves locales : /home/lolo/dev/msm/reports/TR2-mission015-S-20261012T231900Z ; [preuves](preuves/TR2-20261013-M015-S/).
