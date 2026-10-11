---
mission_id: TR2-20261011-M013-D-UART-IRQ-ROOT-CAUSE
status: DONE
base_ref: main
base_sha: null
initial_head: c6d5d2a552b6de2a70789dc7e5f2d1b30f77cc87
mission_sha: 443e6d3eebe18a094ffd09c95c6b087db758784e
result_sha: 09792c8dba98cd3959458f3148d4fae63a9584c9
created_at_utc: 2026-10-11T14:51:02.710944+00:00
updated_at_utc: 2026-10-11T15:58:29.271453+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-D — cause opérationnelle NVIC isolée, instrumentation passive vérifiée

**DONE : critères de diagnostic D satisfaits, code et rapport publiés puis relus depuis GitHub. Modbus bidirectionnel et TX physique NON VALIDÉS.** Aucun nouveau probe HP, aucune requête synthétique sur cible, aucune émission RS485 provoquée, aucune intervention physique. L'essai C unique COM7/1152008E1 sans réponse reste une observation utilisateur ; il n'a pas été répété.

## Référence et préservation

Main initial propre au SHA initial ci-dessus, fetch puis pull fast-forward sûr vers mission443e6d3. AGENTS.md, protocole, journal, rapport C, sources/HAL/CMSIS/startup et chaîne ST qualifiée inspectés. Contrat ChatGPT inchangé ; base_sha absent donc null. Deux stashes005/002 conservés. Archives C créées à partir du HEAD initial, byte-à-byte, sans changer leur verdict historique BLOCKED.

Avant tout reset/reflash : copie vérifiée des **177 fichiers** des deux dossiers C, dumps RAM/logs/ELF/BIN et sauvegardes2MiB compris. Manifestes et toutes preuves D hors Git : `/home/lolo/dev/msm/reports/TR2-mission013-D-20261011T143352Z`. Originaux C conservés. Deux lectures courantes Flash2MiB identiques : SHA256 `be56e0699ea196329c7c3a1bca98743fbdeeb7ce8e7a7a5d370fb0c37c7a83de`. RAM liée complète768KiB, 0x20000000..0x200C0000, conservée avant remplacement : SHA256 `3fd2a5fbeb8e6c53a59a2392ad38f0bc3a571ff3e87fc55832836acb5efa398b`. Deux nouveaux dumps complets précèdent chacun des resets de diagnostic. Voir preservation-C.json, before-manifest.json et diagnostic-manifest.json. Aucune restauration destructive ni suppression de travail.

## Cause opérationnelle démontrée

La **session CubeProgrammer Windows2.23.0 HOTPLUG de lecture sur cette cible/chaîne ST-LINK modifie effectivement le NVIC**. Les snapshots exécutés par le cœur et les lectures SWD concordent : ISER devient0 et ISPR devientFFFFFFFF. Ce n'est donc pas seulement une anomalie de visibilité SWD. Le sous-mécanisme interne connexion/lecture/déconnexion/DLL/ST-LINK n'est pas isolé ; aucune généralisation à toutes versions/cibles.

| Séquence réellement effectuée | Observation CPU et SWD |
|---|---|
| Initialisation passive avant activation | ISER0..2=0 ; ISPR0..2=0 |
| Activation TIM6 puis LPUART1 | ISER1=0x00020000 (IRQ49), ISER2=0x00000004 (IRQ66) |
| Après readback CLI HOTPLUG2MiB | live ISER tous0 ; ISPR tousFFFFFFFF |
| Conservation RAM, GDB monitor reset puis detach | ISER1=20000/ISER2=4 ; ISPR tous0 |
| Deux sessions GDB seules sans reset intermédiaire | État correct conservé |
| Lecture CLI HOTPLUG isolée -r32 0x46002400 32 (CR1..ISR, pas RDR) | Retour au défaut CPU ET SWD : ISER0/ISPRFFFFFFFF |
| Conservation RAM, second reset GDB, puis readback Flash via GDB seulement | État correct conservé, BIN exact |
| Dernier contrôle GDB | ready1, poll_result0, pollerrors0, tick90883, poll_count30924687 ; ISER1=20000/ISER2=4 et ISPR0 |

Dernier état : UARTISR0x006000D0 (PE/FE/NE/ORE/RXNE0), PG4ODR0, CFSR/HFSR0, masques globaux0. Compteurs IRQ/RX/ERROR/trame/TX tous0, cohérents avec absence d'entrée nouvelle ; ils ne prouvent pas un service RX réel. Cible laissée en exécution après detach, serveurs terminés normalement0, aucun GDB/OpenOCD restant. **Aucun nouvel accès CubeProgrammer MCU après ce contrôle sain.**

Lecture seule des options via `-ob displ` : TZEN0, RDP0xAA, NSBOOTADD0=0x08000000. Aucune Option Byte modifiée ni protection changée. TrustZone désactivé, banques secure/nonsecure ne constituent pas l'explication du défaut observé. Startup/vecteurs/VTOR, mapping49/66 et écritures EnableIRQ sont corrects ; les lectures CPU d'initialisation le confirment. Aucun correctif firmware de réactivation NVIC en boucle n'est justifié.

**Déduction pour C :** IRQ externes désactivées expliquent RXNE/ORE non traités et absence de callbacks/file/RTU, alors que SysTick et polling progressent. Les accès CLI précédant le probe C sont une cause opérationnelle reproduite en D. Cela ne démontre ni les huit octets effectivement reçus en C ni une réponse physique future ; aucun nouveau trafic n'a été capturé.

## Correction et logiciel livré

Correction de procédure : conserver RAM avant reset ; terminer d'abord les opérations CLI de sauvegarde/flash/verify/readback ; effectuer ensuite le reset logiciel qualifié via GDB, detach, contrôler NVIC depuis le cœur et SWD en utilisant uniquement GDB. Le readback GDB est ici vérifié sans altération du NVIC. Ne pas rouvrir CubeProgrammer sur le MCU entre contrôle sain et futur essai. Voir DIAGNOSTIC_UART_IRQ_M013_D.md.

Commit code `09792c8dba98cd3959458f3148d4fae63a9584c9`, message `Add passive STM32 IRQ and RTU observer with both TX gates tested`. Huit fichiers dans 05_Firmware : nouveau platform/stm32/stm32_irq_observer.h ; transport, diagnostic .c/.h, main, CMake STM32/hôte et test_stm32_rtu_tx.c. Option TR2_STM32_IRQ_DIAGNOSTICS par défautOFF, autorisée seulement en diagnostic isolé. Observer RAM992octets : snapshots cœur/NVIC/UART en lecture seule, compteurs IRQ/HAL/callback/file/RTU/CRC/adressage/préparationTX, dernières trames/réponses. Compteurs modulo2^32 sur un boot. Aucun RDR/ICR consommé par observateur, aucune IRQ forcée, aucun log UART ou requête ajoutée. Chemins normaux et gateTX conservés ; cœur portable src/include et contrat V1 inchangés. FC03/1000 reste UNKNOWN0, aucune fausse autorité production.

## Validation effective et image sur cible

Validation proportionnée : uniquement harness/diagnostic STM32, pas de modification du cœur portable ; tests ciblés des callbacks et gates plus cross-build. Pas de campagne hôte complète ni .NET inutilement répétée.

- CMake/Ninja ARM frais, CubeU5v1.9.0 Linux, toolchain platform/stm32/cmake/arm-none-eabi-gcc.cmake, diagnosticON/observerON/unité1/**TXOFF**, configure/build0. Quatre warnings newlib historiques (_close/_lseek/_read/_write), aucune erreur.
- CMake hôte et build des cinq cibles, puis `ctest --test-dir 'Modbus RTU/05_Firmware/build-host-validation' -R '^(stm32_rtu_tx_[01]|stm32_irq_observer_[01]|stm32_modbus_diagnostic)$' --output-on-failure` : **5/5 réussis**, retour0. Matrice observerOFF/ON × TXOFF/ON, tests historiques conservés ; CRC erroné/adresse étrangère et erreurs HAL testés. Trames synthétiques seulement dans mocks hôte, jamais sur cible/bus.
- `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only` : retour0, default-cross-build.log. Ce build par défaut n'est pas l'image flashée.
- Configuration négative observerON/diagnosticOFF : rejet attendu « IRQ observer requires isolated diagnostic mode », preuve conservée ; ce rejet confirme le garde-fou.

Image réellement programmée : BIN47812octets SHA256 `f9efbdd7dce624616402f4c957878651aa971df0d87f797df4689f9f3e7f1f1e` ; ELF168232 SHA256 `f047aa6df2cd02c0ef63bafb7e5707caa2d65859c5db87d4abb84f8907054833`. Sources firmware comparées au commit result_sha ; ajustement ultérieur matrice de tests hôte sans changer l'image ARM. **Le firmware actuel est TXOFF**, contrairement au candidat C TXON.

Chaîne qualifiée : NUCLEO-U575ZI-Q/STM32U575/U585 ID0x482 RevU/2MiB, ST-LINK SN003500463235510B37333439 FWV3J15M7, 3,29V. CLI Windows2.23.0 sous C:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin. Sauvegardes `-c port=SWD sn=<SN> freq=8000 mode=HOTPLUG -u 0x08000000 2097152 <fichier>` ; flash standard `-c port=SWD sn=<SN> freq=8000 -w <BIN exact> 0x08000000 -v -rst`, retour0, secteurs0..5 uniquement, verify/reset réussis. Readback2MiB SHA256 `f9894c46b429ee87a48d34f9e3caaa7a600fa47d8a4fb179fe7eab7234c0dc1b` : BIN exact, paddingFF jusqu'à0x0800C000, reste Flash identique backup. Readback final GDB des47812octets exact, sans changement NVIC. Aucun mass erase, modification OptionBytes, FRAM/SD, montage ou configuration permanente. Deux resets logiciels de diagnostic, chacun précédé d'un dump RAM complet.

Serveur qualifié `-d -g -i <SN> --frequency 8000 -p 61234 -cp <CubeProgrammer/bin>` ; GDB `-nx -q -batch -x <script> <ELF exact>`, target extended-remote localhost:61234, lectures/snapshots puis detach/quit. Adresses/offsets vérifiés par ELF/nm. Logs individuels, scripts et images hors Git ; aucune preuve de compilation assimilée à validation physique.

## Limites et prochaines actions à transmettre à ChatGPT

Aucune preuve de réception réelle complète avec l'image D, d'exécution IRQ sur trafic, de réponse RS485, DE jusqu'à TC/dernier stopbit, retourRX ou dialogue bidirectionnel. J1 global/production/E4 restent non qualifiés. Aucun motif pour répéter VIO/RE-RST/SB28/câblage acquis ou modifier matériel.

Pour une **mission future explicitement autorisée**, préparer candidat TXON/observerON séparé, vérifier image/gates/sauvegarde et terminer la chaîne d'accès par GDB comme ci-dessus. Prérequis objectifs : ready1, polling progressant sans erreur, ISER IRQ49/66 actifs par CPU et SWD, ISPR cohérents, RX armé sans RXNE/ORE résiduel, PG4 bas. Ensuite seulement un probe HP explicitement autorisé, sans retry et sans halt pendant échange ; conserver réponse exacte/CRC et compteurs IRQ→callback→8octets→CRC/unité→UNKNOWN0→TX/TC. ChronologieDE et retourRX exigent capture physique adaptée ; snapshot seul insuffisant. **Aucune transmission n'est demandée dans D.**

## Publication

Journal actualisé avec cause reproduite, procédure corrigée, TXOFF courant, limites et prochaine mission ; preflight historique précédé du verdict D pour ne pas relancer l'ancien probe. Archives C immuables. Permissions exercées : commit/push main, flash standard et debug qualifié/lectures/reset ; aucun élargissement d'accès. Commit documentaire `1b4b7fd01ee2d206b96428e5ef5ff78c26cc6a73`, message `Document M013-D reproduced HOTPLUG NVIC failure and corrected access procedure`, publié par push normal explicite `HEAD:refs/heads/main`. Fetch : HEAD=origin/main. Relecture intégrale UTF8 et comparaison byte-à-byte de 15 fichiers (six livrables documentaires, huit sources, contrat D) depuis cette référence distante réussies ; manifeste remote-reread.json hors Git. État Git propre après publication, deux stashes historiques conservés. La présente clôture DONE sera publiée puis relue à son tour ; son SHA final est communiqué dans la réponse Codex, sans autoréférence dans le commit. Contrôle indépendant ChatGPT non établi par Codex.
