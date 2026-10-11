---
mission_id: TR2-20261011-M013-B-PHYSICAL-TX
status: BLOCKED
base_ref: main
base_sha: null
initial_head: d8e893364ca469463fdba1006a57df0a9ecb0335
mission_sha: 70843b31a1fb1db631d1852a9384aa76136063cf
result_sha: 70843b31a1fb1db631d1852a9384aa76136063cf
created_at_utc: 2026-10-11T13:59:07.211015+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-B — mesures statiques acquises ; route externe RE/RST à confirmer

**BUILD RÉUSSI / TESTS CIBLÉS 3/3 / FLASH NON EFFECTUÉ / EXÉCUTION CANDIDAT NON VÉRIFIÉE / TX PHYSIQUE NON VÉRIFIÉ. Verdict : NO-GO activation**, en attente de la seule clarification de topologie externe prescrite au contrat§2. Rapport provisoire également en attente de publication/relecture distante ; cette publication ne lèvera pas le blocage matériel.

## État réel, inspection et preuves

HEAD initial propre main `d8e893364ca469463fdba1006a57df0a9ecb0335`, fetch/pull fast-forward vers mission `70843b31a1fb1db631d1852a9384aa76136063cf`. Seul fichier de mission changé. AGENTS.md/protocole V2-B3/journal intégral/rapport et preflight M013-A, schéma Clickv102 graphique intégral, source diagnostic/transport/main/init GPIO et tests lus ; procédure ST qualifiée T1000 relue. Champs base_sha/date absents du contrat : null/références observées, sans modification du fichier ChatGPT. Target main et quatre permissions true, sous conditions matérielles explicites.

`git diff 184b31550e0ce52d28220145967db57ba6fc399b HEAD -- 'Modbus RTU/05_Firmware'` vide : code/test M013-A strictement inchangés. `result_sha` désigne le HEAD réellement inspecté et utilisé pour reconstruire le candidat ; le commit firmware184b315 est son ancêtre. Archives M013-A mission/rapport conservées exactement depuis d8e8933, sans transformer le précédent BLOCKED en DONE. Deux stashes005/002 conservés. Aucun code, freeze, mapping V1, configuration production, E4, paramètre global ou câblage modifié.

## Preflight actualisé et point de décision

Observations **utilisateur**, nouvelles dans contrat M013-B, pas mesures Codex : MIKROE-3863 rev1.02 ADM2867E, J1 position3,3V, J2/J3 fermés, VIO3,27V contre masse logique, DE0V et /RE0V au repos. Lacunes d'identification/révision, rail VIO et direction statique levées à ce niveau de preuve. SB28/SB29, J2 soudé et RX011 conservés ; aucune répétition demandée. Transitions/turnaround/reset restent non mesurés.

[Preflight actualisé](../PREFLIGHT_RTU_TX_M013_A.md) : J2 ferme DE–/RE, J3 ferme /RE–contact RST mikroBUS. [Pinout fabricant](https://www.mikroe.com/rs485-isolator-2-click) effectivement consulté : **RST mikroBUS n°2 correspond à RE sur le Click**, voisin de DE/CS n°3. Le schéma interne v102 ne représente aucune seconde sortie active sur ce net. RST ici n'est pas la demande de recâbler NRST STM32. L'utilisateur rapporte aucune liaison RST réalisée vers NUCLEO, mais le point n'a pas été identifié physiquement : absence de fil NUCLEO acquise, absence de liaison implicite par support/breadboard non établie. Deux sorties maintenues à0V au repos ne démontrent pas l'absence de conflit quand PG4 passe haut.

**Une seule question posée via l'interface, réponse en attente :** « La broche RE du Click — position RST mikroBUS n°2, voisine de DE/CS — est-elle libre de toute liaison externe, y compris via une rangée de breadboard ou un support ? » Attendu : aucune autre liaison externe/sortie. Pas de nouvelle mesure, coupure, dessoudage, déplacement J3 ou recâblage demandé. Si route inconnue/concurrente, conserver NO-GO. Motif : contrat§2 exige de contrôler cette route implicite ; AGENTS.md§9 réserve l'observation physique à l'utilisateur. Ce n'est pas une nouvelle permission de flash à solliciter : la permission est présente, sa condition de sûreté reste à établir.

## Build et tests réels

Reconstruction isolée depuis main courant, aucune activation matérielle : CMake/Ninja, source `Modbus RTU/05_Firmware/platform/stm32`, toolchain ARM `cmake/arm-none-eabi-gcc.cmake`, SDK `/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0`, options `TR2_STM32_MODBUS_DIAGNOSTIC=ON`, `TR2_STM32_RS485_TX_QUALIFIED=ON`, `TR2_STM32_DIAGNOSTIC_UNIT_ID=1`, build `--parallel 4` ; configure/build retours0.

```sh
ctest --test-dir 'Modbus RTU/05_Firmware/build-host-validation'   -R '^(stm32_rtu_tx_[01]|stm32_modbus_diagnostic)$' --output-on-failure
```

Retour0, **3/3 tests** (0,01s). Choix proportionné : code/test inchangés, comparaison Git et empreintes confirmées ; cas pertinents diagnostic/RTU/transport rejoués et candidat ARM reconstruit. Validation complète108/108 et builds défaut/OFF/ON de M013-A sont des preuves antérieures, **pas une nouvelle campagne108 tests M013-B**. Aucun test affaibli et aucun problème corrigé silencieusement. Quatre warnings newlib historiques `_close/_lseek/_read/_write`, sans erreur de build.

Candidat BIN48240 octets, SHA256 `47d69cc286b0f12771de56eb80f2648cc3c2a7a57b2ce1a8038b34de792b97e8` ; ELF SHA256 `762dcf0ba6c3663d9737dd334118a9f7d52a22175dda6d2674e9cb0d7b8e1c4a`, identiques M013-A. text48216/data24/bss4200. `arm-none-eabi-nm` confirme fonctions FRAM/SD/boot production absentes. Diagnostic seulement FC03/1000/1→UNKNOWN0, autres données/autorités non disponibles refusées V1 ; pas un statut industriel provisionné. Fichiers/logs/manifestes hors Git `/home/lolo/dev/msm/reports/TR2-mission013-B-20261011T135539Z`.

## Matériel et prochain jalon

Aucun accès cible, nouvelle sauvegardeFlash, flash, GDB, reset, changement USB ou émission dans M013-B. Aucune utilisation ELF013 sur image008. Dernière identité Flash sur carte établie011 : BIN008157944 octets SHA256 `02ed2cf64d4d78225a4781b823c9d27e96a8ea693679d8a834d230568383593b`, aucun reflash Codex depuis ; absence d'accès013-B ne constitue pas une nouvelle lecture de la carte. Chaîne ST Windows qualifiée et SN attendu003500463235510B37333439 préparés, pas d'inventaire matériel prétendu effectué aujourd'hui.

Après confirmation de topologie et porte GO : identifier cible/SN/ST-LINK exclusif, sauvegarder et vérifier **Flash courant2MiB**, puis flashstandard candidat exact à0x08000000/verify/readback, debug avec ELF correspondant. Pas de reprise d'une ancienne sauvegarde comme preuve courante. Avant essai, contrôler que seul le maître HP prévu est connecté et qu'aucune contention n'est présente. COM7 HP inaccessible à Codex, aucune émission depuis COM3 ni envoi automatique.

Essai unique préparé, **ne pas lancer actuellement** : HP COM7 1152008E1, requête `01 03 03 E8 00 01 04 7A`, réponse attendue `01 03 02 00 00 B8 44`, CRC et UNKNOWN0. Script existant `platform/host/rs485_rx_probe/Send-Tr2RxProbe.ps1` émet une seule trame avec switch explicite, sans retry ; son affichage peut inclure un écho et ne valide pas le timing. Après firmware/GO établis, instructions HP minimales et retour réel requis, pas nouvel essai RX. DE haut jusqu'au dernier stopbit/TC et retour RX devront être observés ; lecture de l'état final et tests HAL ne prouvent pas ces transitions physiques. Limites reset pré-init/watchdogIRQ et dette E4 inchangées.

## Modifications documentaires et publication

Journal actualisé : nouvelles mesures utilisateur, nommage RE/RST, unique lacune de topologie, candidat/test réussis et opérations matérielles non effectuées. Preflight M013-A enrichi d'une section M013-B, ancienne matrice explicitement historique ; archives M013-A exactes. À ChatGPT : vérifier publication, distinguer preuve statique et dynamique, résoudre cette seule route externe puis cadrer la suite physique.

État initial propre, modifications limitées à journal/preflight/archives/rapport. Diff et index à contrôler avant commit ; push normal sur main et relecture distante encore à réaliser. BLOCKED reste matériel même après publication. SHA final du rapport communiqué séparément après vérification, sans auto-référence circulaire.
