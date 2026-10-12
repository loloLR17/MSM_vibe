---
mission_id: TR2-20261013-M015-Q-FIFO-REPEATABILITY-WINDOWS
status: BLOCKED
base_ref: main
base_sha: 6ea0468b43aa966a98a4037db5b741a0ec98178f
initial_head: 0e20ade0bba44db1206f4af47ca0c30dfca70bce
result_sha: b79fe8d5ba495b14ec15379b111ccb4a948faabf
created_at_utc: 2026-10-12T22:41:11Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-Q — checkpoint répétabilité acquise

Trois captures uniques du BIN M015-P inchangé (ed19310485d51af22b18420ea8af3ff3acba36531b3e9741bc78e91792f282f5), sans flash ni manipulation : chacune 263 XYZ distincts +33 timestamps, 296 mots. MCU reset GDB avant chaque trigger ; le programme ST fait ensuite son propre reset logiciel capteur. Aucun power cycle manuel/mesuré. Seconde SN0032001F3235510C37333439/UID001B00463842501820333157 vérifiée et Flash byte-identique avant chaque reset ; primaire strictement exclu.

Parité/discontinuités/saturation/HAL/SPI/overflow/refus journal zéro ; chaque capture reconstruite byte-identique depuis traces SPI et buffer officiel. Cleanup0/0C-80-00, faults0/detach/serveurs0. Débit payload DWT : environ246,5..246,8ko/s pour896octets, environ3,63ms ; pas mesure indépendante de SCK/ODR ni preuve globale perte zéro.

Erreur du nouvel évaluateur hors ligne conservée : il comparait tout le dump, dont adresse18 réservée (E0 actuel contre E2 historique), à une empreinte de configuration. Aucun garde-fou cible n'a refusé. Correction documentée d'après DS12569Rev8/table8 et header ST : les registres définis restent contrôlés strictement, l'octet réservé reste brut. Analyse originale et correction publiées, aucune quatrième capture.

Archives P exactes depuis6ea0468 ; worktree Q isolé, ancien dépôt82169af et stashes préservés. Les tests/builds P inchangés ne sont pas rejoués. Statut BLOCKED provisoire : fenêtres512/4096 non encore préparées/testées. Suite : définir capacité, débit, timeout, overflow, tags et réserves, puis nouveaux tests ciblés/ARM avant extension progressive. M014-H/FRAM/SD/Modbus/HP non accédés ; cause O/P toujours non établie.

Preuves locales : /home/lolo/dev/msm/reports/TR2-mission015-Q-20261012T224111Z ; [preuves versionnées](preuves/TR2-20261013-M015-Q/). Publication/relecture distante de cette tranche à consigner après push.

## Checkpoint fenêtres préparées

[Critères et différences](../../../experiments/iis3dwb_st_reference/QUALIFICATION_M015_Q.md) fixés avant augmentation. Nouveau service segmenté borné, initialisation ST/transport inchangés, mots FIFO et XYZ distincts, budgets/overflow/config/tags/cleanup stricts. CTest ciblé et14scénarios intégration passent ;3ARM passent, contrôle byte-identiqueP. Images512/4096 préparées, pas encore flashées. Premier test synthétique refusé pour compte attendu incluant un TS non reçu corrigé selon séquence brute ; logs initiaux conservés. Répétabilité publiée06038823d0e80963d8d6f85a85a774a9fe4ff141/126fichiers byte-identiques relus.
