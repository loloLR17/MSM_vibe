---
mission_id: TR2-20261008-CODEX-GIT-PREP-001
status: BLOCKED
base_ref: test/echange-chatgpt-codex-20261008
base_sha: 43e72c0c87c1f5b38e8565fd3f3cb876eef40135
initial_head: 836746ff9168e6abc08d5cfdd120a67ea0d48b84
result_sha: null
created_at_utc: 2026-10-08T19:12:35.844136+00:00
author: Codex
target_branch: test/echange-chatgpt-codex-20261008
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# P1 — Sauvegarde privée vérifiée et préparation isolée

Sauvegarde intégrale vérifiée et clone diagnostique préparé. Statut provisoire BLOCKED uniquement dans l’attente de publication et relecture distante du rapport. Aucun commit firmware, aucune publication de la branche de préparation, aucune synchronisation du dépôt principal. DONE ne signifiera que la réalisation de P1.

## Références et périmètre

- Dépôt principal : branche main, HEAD `18c9871481e714e05c93ee60d982c7293f488ba3` ; six suivis modifiés, 2621 non suivis observés ; fichiers générés/ignorés inclus dans la sauvegarde complète.
- Main distant revérifié avant clone de travail : `19c39d6a7a9c3d8fef4330a64be427563ed108cb`, ls-remote code 0.
- Branche de publication : `test/echange-chatgpt-codex-20261008`, HEAD initial `836746ff9168e6abc08d5cfdd120a67ea0d48b84`.
- Base contractuelle `43e72c0c87c1f5b38e8565fd3f3cb876eef40135`, ancêtre du HEAD expérimental vérifié (code 0).
- Clone de publication : `/tmp/TR2-20261008-CODEX-GIT-PREP-001-publication`, initialement propre ; seule modification autorisée sur GitHub : ce DERNIER_RAPPORT.md.

Mission active, AGENTS local et journal local consultés ; AGENTS adopté distant identique à la version intégralement lue lors de l’audit (SHA256 `090f0b43fafa9217581abbca5c4d048e0e49905062f6d34ca4c1857a5e1b3ba0`). Protocole relu. Aucun pull/fetch ni autre mutation dans le dépôt principal. Aucune attribution non prouvée des modifications locales.

## Sauvegarde intégrale — VALIDÉE par comparaison de fichiers

Destination exacte : `/home/lolo/tr2-private-backups/TR2-20261008-CODEX-GIT-PREP-001` ; copie complète dans son sous-répertoire `repository`.

Préconditions de confidentialité/espace vérifiées avant copie : `/home/lolo` réel sans lien symbolique, mode 700, propriétaire lolo ; montage Linux ext4 local `/dev/sdd`, hors des volumes Windows et répertoires synchronisés/publics. Parent dédié et destination mode 700 ; création exclusive échouant si destination existante, aucun écrasement. Environ 937 Go disponibles contre 193 Mo occupés par la source ; contrôle programmatique de plus de 1 Gio libre. Confidentialité locale par permissions, sans revendication de chiffrement ni de protection contre l’administrateur du système.

Copie Python `shutil.copytree` avec copy2, aucune exclusion : .git, tous les fichiers suivis, non suivis et ignorés, documents, BIN/ELF et builds. Les données, noms et métadonnées prises en charge par copy2 sont préservés ; les ACL/flux Windows spécifiques ne sont pas qualifiés comme sauvegardés par cette copie Linux. Aucun lien symbolique ou objet spécial dans l’inventaire. Aucun fichier supprimé.

Inventaire complet et SHA256 de chaque fichier : **8314 fichiers**, **1675 répertoires**, **0 lien**, **189906654 octets** de contenu. Trois manifestes en mémoire comparés : source avant copie = copie = source relue après copie. Égalité complète des chemins/types, tailles et SHA256 ; source stable pendant la sauvegarde. Les fichiers générés et .git sont couverts, contrairement à un simple diff Git.

Fichiers de preuve privés, dans la destination :

- `manifest-source.json` et `manifest-copy.json` : inventaires complets avec empreintes individuelles ; SHA256 identique `3c855be2bdfa31c9b5026f97b2c50eb90e6dba6ddbf0016e8f809a27ccd38900`.
- `git-state.json` : HEAD et empreintes de status NUL complet, refs, inventaire index et diff HEAD binaire.
- `tracked.diff` : copie du diff suivi, complément à la sauvegarde entière.
- `verification.json` : résultats et décompte.
- `recovery-check/` : **7 récupérations distinctes vérifiées par SHA256** : .git/HEAD, AGENTS, main.c, BIN, ELF, journal local et BOM XLSX. Ce contrôle porte sur la récupération de fichiers, pas une qualification de restauration complète de l’environnement Windows.

Sauvegarde jamais indexée ni poussée ; elle n’est pas un clone de publication. Les manifestes détaillés et contenus privés ne sont pas publiés dans ce rapport.

## Préparation locale — PRÉPARÉE, logiciel vérifié

Clone de travail créé **après** sauvegarde vérifiée et nouvelle lecture de main distant :

- Chemin : `/home/lolo/tr2-private-work/TR2-20261008-CODEX-GIT-PREP-001/firmware`.
- Branche locale : `prep/TR2-20261008-CODEX-GIT-PREP-001`.
- HEAD initial et final : `19c39d6a7a9c3d8fef4330a64be427563ed108cb` ; aucun commit ni push firmware.
- Parent privé mode 700 ; clone indépendant du dépôt principal et de la sauvegarde.

Les **14 fichiers autorisés** ont été copiés depuis la sauvegarde vérifiée, dont les octets étaient identiques à la source stable. Empreintes importées comparées au manifeste source puis aux originaux : identiques. Aucun changement de code supplémentaire.

- `Modbus RTU/00_gouvernance/PROCEDURE_FIRMWARE_F2_DIAG_IIS3DWB_B3.md`.
- `Modbus RTU/00_gouvernance/PROCEDURE_FIRMWARE_F3A_DIAG_MODBUS_RS485.md`.
- `Modbus RTU/00_gouvernance/PROCEDURE_FIRMWARE_F3B_UART_ADM2867E_GDB.md`.
- `Modbus RTU/05_Firmware/CMakeLists.txt`.
- `Modbus RTU/05_Firmware/platform/stm32/CMakeLists.txt`.
- `Modbus RTU/05_Firmware/platform/stm32/iis3dwb_diag_modbus.c`.
- `Modbus RTU/05_Firmware/platform/stm32/iis3dwb_diag_modbus.h`.
- `Modbus RTU/05_Firmware/platform/stm32/iis3dwb_diag_window.c`.
- `Modbus RTU/05_Firmware/platform/stm32/iis3dwb_diag_window.h`.
- `Modbus RTU/05_Firmware/platform/stm32/main.c`.
- `Modbus RTU/05_Firmware/platform/stm32/stm32_serial_transport.c`.
- `Modbus RTU/05_Firmware/platform/stm32/stm32_serial_transport.h`.
- `Modbus RTU/05_Firmware/tests/unit/test_iis3dwb_diag_modbus.c`.
- `Modbus RTU/05_Firmware/tests/unit/test_iis3dwb_diag_window.c`.

État Git du clone : **5 suivis modifiés**, **9 fichiers non suivis**, index sans modification ; diff suivi **294 insertions / 7 suppressions**. Les ajouts sont les quatre sources/headers, deux tests et trois procédures. AGENTS adopté de main inchangé ; AGENTS V1 local, journal, PDF/XLSX et anciens builds exclus. Aucun add global ni indexation firmware. `import-manifest.json` privé dans le parent du clone : SHA256 `594243d37d7e76658bb0d7ae4648f82a06adc6d5be1856a9101dae0a5d8fee28`.

Lecture des diffs CMake/main/UART et des modules/tests : dépendances diagnostiques présentes, deux tests CMake ajoutés avec sources/includes, deux sources dans la cible STM32. Vérification des imports .c/.h et tests, et des références SDK avant compilation. Diff --check code 0 ; contrôle d’exclusions et d’égalité des 14 contenus réussi.

Contrôle de sensibilité avant import : lecture programmatique intégrale des 14 contenus et recherche de motifs de clés privées et tokens connus, **0 résultat**. Les procédures ont aussi été examinées pour leur portée/historique. Cela n’est pas une certification exhaustive d’absence de secrets : elles contiennent des chemins locaux et identifiants opérationnels de banc/ST-LINK à revoir avant une éventuelle publication firmware. Aucun secret affiché ou publié.

## Validation logicielle réelle

Niveau choisi : build/tests hôte complets et cross-build STM32, car CMake, tests et transport sont concernés. Les scripts `tr2_validate.sh` et `tr2_build_stm32.sh` ont été inspectés : ils contiennent rm -rf. La mission interdit toute suppression ; ils n’ont donc **pas été exécutés**. Leurs commandes de configuration/build/test ont été exécutées directement, dans des répertoires neufs **hors du clone**, sans nettoyage ni modification de script. Pas de claim de réussite de tr2_validate.sh lui-même.

Prérequis : CMake 3.25.1, Ninja 1.11.1, GCC ARM 12.2.1 ; STM32CubeU5 `/mnt/c/Users/Lolo/Desktop/STM32/STM32CubeU5`, tag **v1.9.0**, HEAD `d88042df24f16799957c24f00c2b234c9e306188`. Device CMSIS `624374fa1e21ca195d6f2102ac0caaa50d0ea4c8` (v1.4.3), HAL `2552682ee5a0d6f826a6435750fc9267be65868b` (v1.6.3), submodules présents aux références du SDK ; les 16 chemins requis par la boucle CMake sont présents. Le SDK est uniquement lu ; pas de mise à jour ni qualification exhaustive de sa propreté.

Commandes réellement exécutées, toutes **code 0**, logs conservés dans le parent privé du clone :

Depuis `firmware` :

```sh
cmake -S 'Modbus RTU/05_Firmware' -B ../build-host-validation
cmake -G Ninja -S 'Modbus RTU/05_Firmware/platform/stm32' -B ../build-stm32-p11c -DCMAKE_TOOLCHAIN_FILE='/home/lolo/tr2-private-work/TR2-20261008-CODEX-GIT-PREP-001/firmware/Modbus RTU/05_Firmware/platform/stm32/cmake/arm-none-eabi-gcc.cmake' -DSTM32CUBE_U5_ROOT=/mnt/c/Users/Lolo/Desktop/STM32/STM32CubeU5
```

Depuis `/home/lolo/tr2-private-work/TR2-20261008-CODEX-GIT-PREP-001` :

```sh
cmake --build build-host-validation -j 4
cmake --build build-stm32-p11c -j 4
ctest --test-dir build-host-validation --output-on-failure
```

Sorties redirigées vers host-configure.log, host-build.log, cross-configure.log, cross-build.log et host-tests.log. Configure hôte/ARM réussi ; build hôte réussi ; **104/104 tests hôte PASS**, 43.07 secondes, dont les deux diagnostics ; cross-build réussi, 102 étapes Ninja. Taille ELF rapportée : text 94472, data 144, bss 71656 octets. Aucun échec de compilation/test nécessitant correction, aucun test affaibli, aucune validation matérielle.

Artefacts neufs dans `build-stm32-p11c/` privé :

- BIN SHA256 `65b734933d1f3947b30f82c3371eb10e557a34d8d893202de23ef5be82d56321`.
- ELF SHA256 `a8e4040c35548a1e8a0cb962082c2e2582d7943e09b2596c611481317a3c9810`.

Ces empreintes sont **identiques aux fichiers originaux sauvegardés** : reproduction observée pour ces sources, SDK et toolchain. Le HEAD seul ne décrit toujours pas la tranche, qui demeure non committée. Les anciens builds ne sont ni importés ni réutilisés.

Empreintes de logs privés :

- `host-configure.log` : `b35d67fd18d28236e90b16a0c400fb89de35ab48127a74280417c3e14dd34e6d`.
- `host-build.log` : `19c7177d11d137fa21e5f0731ca4fb4ffb23d3d880c65a365bdb5cc6f3c6711a`.
- `host-tests.log` : `e8e560becbbff432aacea4bd9af0486e778aa7c831a34c033642ea6d130207a6`.
- `cross-configure.log` : `7cc772de66b25d2cc28cb1b7e5dbf301d9a872be93e24c8d4a3c244dfb986a2a`.
- `cross-build.log` : `2764744b938fba22e5d9b3fe7eaf37af59853eb8a45294a7d8f63605e86f12f0`.

## Portée diagnostique, limites et prochaines étapes

**DIAGNOSTIC UNIQUEMENT** : main appelle Iis3dwbDiag_Run après init SPI et avant FramSpi_Init/Sdmmc2_Bringup/runtime normal. La boucle acquiert une fenêtre puis sert B0/B3, ou reste arrêtée sur erreur ; elle court-circuite le runtime normal. Pas de qualification P8, persistance, recovery E4, cadence soutenue ou endpoint production complet. Sonde B0 non sollicitée opt-in RAM, aucune activation matérielle dans cette mission.

La procédure F3-B contient encore une mention « aucune observation physique » historique, alors que le journal local rapporte des observations ultérieures. Écart documentaire signalé, documents conservés à l’identique ; aucune consolidation de preuves physiques ni correction silencieuse. Les SHA de départ des procédures restent des références historiques, distinctes du HEAD du clone préparé. La provenance exacte des changements locaux n’est pas déduite.

Préservation finale : empreintes de HEAD/status complet/refs/index/diff du dépôt principal identiques au relevé privé initial ; 14 imports identiques aux originaux, AGENTS et journal locaux identiques à la sauvegarde. Sauvegarde complète a déjà fait l’objet d’une relecture de tous les fichiers source après copie ; ces contrôles finaux ciblés ne prouvent pas l’absence de toute activité externe ultérieure sur chaque fichier. Aucune suppression, aucun flash/debug, aucune écriture sur main, aucune configuration permanente Codex, aucune archive/mission modifiée. Journal lu, NON actualisé : le contrat interdit sa modification et son importation.

Avant publication éventuelle : mission distincte autorisant branche, fichiers et commit/push firmware ; revue de la portée diagnostique et de son isolation, revue de sensibilité des procédures, décision séparée sur journal/BOM/PDF et harmonisation de l’historique documentaire. Indexer explicitement les 14 chemins autorisés, exclure toute sauvegarde/build/log privé, examiner le diff indexé et revérifier main distant. Stop si divergence, périmètre supplémentaire nécessaire, incohérence normative, besoin matériel non autorisé ou données sensibles non résolues. Rien dans P1 n’autorise une promotion en production ou une synchronisation du dépôt principal.

## Publication du rapport

Seul DERNIER_RAPPORT.md sera committé/poussé normalement sur la branche expérimentale. Première publication et relecture restent à effectuer ; la clôture consigne la preuve distante avant DONE. Le commit final de rapport sera indiqué dans la réponse finale sans autoréférence.
