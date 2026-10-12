---
mission_id: TR2-20261012-M015-O-ST-OFFICIAL-DEMO-PORT
status: BLOCKED
base_ref: main
base_sha: f14183c81c9ff744ebb1d2c6d5f61537e9c1ddb0
initial_head: 4272a9d9a5a5e71dd1cda67f3cb9e9d176e8d9d1
result_sha: null
created_at_utc: 2026-10-12T21:47:52.664705+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-O — checkpoint du portage réel ST

La fonction `iis3dwb_fifo()` de ST est maintenant compilée et appelée directement,
sans bench_run. Sources originales conservées, commit officiel 1f848c38… relu ;
drivergitlinkd995 inaccessible, sélectionofficiellef83 identique àK/N déclarée.
L'audit du .iocMKI109V3 révèle mode3, contrairement au mode0 K/N. Cela reste
une hypothèse causale à qualifier, pas une preuve de capteur défectueux.

[Audit/équivalence/adaptations](../../../experiments/iis3dwb_st_reference/official/AUDIT.md).
FormatlogST corrigé %ld/castlong sansmasquerWerror,bufferRAM512 sansmodifierWM,
plateformeU5/O2HALSPI/CS/segmentation/timing/consoleRAM/stop/cleanup isolés.
14scénarios mocks exécutantla fonctionST passent, quatrebuildsARM réussis.
Capacité etcleanup réserve12/24/32 vérifiés ;aucun ancien testouessai inchangé
rejoué. Erreurtestd'injection corrigée aprèsinspection de la séquenceréelle,
attentes de refus maintenues. Validation projetindépendant, pasproductionTR2.

AucunaccèsmatérielO encore. Checkpoint avanttémoinofficielmode3,puisvarianteniveau10
etcomparaisonmode0 conditionnelles ;confirmationXYZ512 si intégritéverte.
PublicationST ne prouvepas unessaimatérielST ;sousmoduleinaccessible limite
l'identitédu systèmeoriginal. Aucunautrecapteur/principal/FRAM/SD/Modbus.
ArchivesN exactes,worktrees/stashespréservés. Preuves `/home/lolo/dev/msm/reports/TR2-mission015-O-20261012T214009Z` et
[preuves versionnées](preuves/TR2-20261012-M015-O/). BLOCKED provisoire jusqu'aux
preuves physiques et àla relecture distante,aucunevalidationutilisateurattendue.
