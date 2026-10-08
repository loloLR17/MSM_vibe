---
mission_id: TR2-20261008-CODEX-GIT-PUBLISH-001
status: CANCELLED
base_ref: test/echange-chatgpt-codex-20261008
base_sha: adb412d503f5afa9e02f00de9218f27656a83dad
created_at_utc: 2026-10-08
author: ChatGPT
target_branch: publish/tr2-firmware-diagnostics-20261008
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# P2 — ANNULÉE : sauvegarde P1 suffisante

**MISSION ANNULÉE AVANT EXÉCUTION. NE RIEN FAIRE.** L'objectif utilisateur était uniquement de ne pas perdre les travaux locaux. La sauvegarde P1 complète et vérifiée satisfait déjà cet objectif. Aucun commit, push, PR, test ou rapport supplémentaire n'est demandé. Le dépôt principal et le clone préparé restent intacts.

---

## Ancienne mission (historique, non exécutable)


## Objectif
Terminer la passe Git engagée, sans refaire inutilement les audits ni les sauvegardes P0/P1. P1 a vérifié la sauvegarde privée intégrale, préparé le clone firmware et réussi 104/104 tests hôte et cross-build STM32 (BIN/ELF identiques aux précédents). Publier uniquement les 14 fichiers diagnostiques autorisés sur une nouvelle branche distante dédiée, vérifier le push, puis créer une Pull Request vers main pour revue. Ne pas fusionner automatiquement la PR. Ne pas modifier le dépôt principal local.

## Entrées vérifiées P1
- main distant de référence à P1 : 19c39d6a7a9c3d8fef4330a64be427563ed108cb.
- clone préparé : /home/lolo/tr2-private-work/TR2-20261008-CODEX-GIT-PREP-001/firmware
- branche locale préparée : prep/TR2-20261008-CODEX-GIT-PREP-001
- sauvegarde privée vérifiée : /home/lolo/tr2-private-backups/TR2-20261008-CODEX-GIT-PREP-001
- 5 fichiers firmware suivis modifiés, 6 fichiers diagnostics/tests nouveaux, 3 procédures documentaires nouvelles = 14 chemins.

## Périmètre strict des 14 fichiers
- Modbus RTU/00_gouvernance/PROCEDURE_FIRMWARE_F2_DIAG_IIS3DWB_B3.md
- Modbus RTU/00_gouvernance/PROCEDURE_FIRMWARE_F3A_DIAG_MODBUS_RS485.md
- Modbus RTU/00_gouvernance/PROCEDURE_FIRMWARE_F3B_UART_ADM2867E_GDB.md
- Modbus RTU/05_Firmware/CMakeLists.txt
- Modbus RTU/05_Firmware/platform/stm32/CMakeLists.txt
- Modbus RTU/05_Firmware/platform/stm32/iis3dwb_diag_modbus.c
- Modbus RTU/05_Firmware/platform/stm32/iis3dwb_diag_modbus.h
- Modbus RTU/05_Firmware/platform/stm32/iis3dwb_diag_window.c
- Modbus RTU/05_Firmware/platform/stm32/iis3dwb_diag_window.h
- Modbus RTU/05_Firmware/platform/stm32/main.c
- Modbus RTU/05_Firmware/platform/stm32/stm32_serial_transport.c
- Modbus RTU/05_Firmware/platform/stm32/stm32_serial_transport.h
- Modbus RTU/05_Firmware/tests/unit/test_iis3dwb_diag_modbus.c
- Modbus RTU/05_Firmware/tests/unit/test_iis3dwb_diag_window.c

## Exécution
1. Relire AGENTS.md adopté sur main, le protocole et le rapport P1 ; vérifier HEAD/état du clone préparé et ls-remote origin/main. Arrêter si la branche distante a changé, si l'état local ou les 14 empreintes diffèrent du manifeste P1, ou si la branche cible distante existe déjà de manière inattendue.
2. Relire les 14 fichiers pour secrets et données opérationnelles : les procédures mentionnent des identifiants de banc/ST-LINK et chemins locaux. Si une publication de ces informations pose un risque, stopper et rendre BLOCKED sans publier ; ne pas modifier silencieusement les sources historiques. Vérifier que les diagnostics ne sont pas présentés comme firmware de production.
3. Sur le clone isolé, créer la branche locale publish/tr2-firmware-diagnostics-20261008 depuis le main distant vérifié, en préservant les 14 fichiers préparés. Ne pas modifier le dépôt principal ni la sauvegarde. Ne pas indexer de builds, binaires, logs, BOM/PDF, journal ETAT_COURANT_TR2.md ou AGENTS V1 local. Utiliser uniquement git add -- avec les 14 chemins explicites. Examiner diff --cached --check, --stat et noms avant commit.
4. Créer un commit explicite de diagnostic, puis pousser normalement **uniquement** vers origin/publish/tr2-firmware-diagnostics-20261008. Vérifier SHA distant via ls-remote/fetch et contenu publié (14 chemins exacts, empreintes). Aucun force push.
5. Créer une PR vers main, avec description claire : diagnostic uniquement, harness exécuté avant runtime normal, 104/104 tests et cross-build observés dans P1, aucune qualification physique RS-485 bout-en-bout ou H3h-E4, revue humaine requise. Laisser la PR ouverte **non fusionnée**. Si l'outil local de création PR n'est pas disponible, publier le lien de comparaison/branche dans le rapport, sans faire d'actions risquées.
6. Publier le rapport final via le canal Echanges_Codex : DERNIER_RAPPORT.md sur la branche test/echange-chatgpt-codex-20261008, depuis un clone isolé, en respectant les champs du protocole. La branche cible du firmware est celle déclarée dans target_branch ; exception explicite pour le seul rapport sur la branche d'échange, avec commit/push autorisés. Relire le rapport distant après publication.

## Conditions d'arrêt
Divergence main ou fichiers P1, secret/donnée sensible non résolue, conflit inattendu, validation invalidée, absence de permission, besoin de flash/debug ou autre fichier non autorisé : status BLOCKED, aucun push firmware. Ne jamais supprimer/réinitialiser les travaux locaux pour contourner un blocage. Pas de modification de configuration permanente, aucun flash/debug, aucune mutation du dépôt principal, aucune fusion PR, aucun push direct sur main.

## Critères DONE
Branche distante firmware créée et vérifiée, commit limité aux 14 fichiers, PR ouverte ou impossibilité d'ouverture documentée avec lien de comparaison, rapport publié et relu. DONE n'implique ni fusion dans main, ni firmware de production, ni qualification physique.
