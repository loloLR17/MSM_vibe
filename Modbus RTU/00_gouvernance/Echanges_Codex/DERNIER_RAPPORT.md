---
mission_id: TR2-20261011-M013-C-FIRST-RTU-RESPONSE
status: BLOCKED
base_ref: main
base_sha: null
initial_head: 61e9617ce67f35c9553c6255c82db6388bbb90b2
mission_sha: fe1b9cf274d2fd51b82081e0a20c5e99dc55728c
result_sha: fe1b9cf274d2fd51b82081e0a20c5e99dc55728c
created_at_utc: 2026-10-11T14:13:27.579300+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-C — diagnostic flashé, boot observé ; réponse HP en attente

**GO statique / BUILD RÉUSSI / TESTS3/3 / BACKUP vérifié / FLASH et READBACK exacts / BOOT observé / TX PHYSIQUE NON VÉRIFIÉ.** BLOCKED porte sur l'attente de la capture réelle HP, pas sur RE/RST désormais résolu. Aucune qualification J1 globale ni production.

## Référence, périmètre et preflight

Main initial propre `61e9617ce67f35c9553c6255c82db6388bbb90b2`, fetch/pull fast-forward vers `fe1b9cf274d2fd51b82081e0a20c5e99dc55728c` ; seul contrat changé. MissionC lue, gouvernance V2-B3/protocole/journal intégral/rapportB/preflight/schéma v102 et code diagnostic/transport/main/tests/CMake inspectés. `base_sha` et date absents du contrat : null/références réellement observées, aucune modification de mission ChatGPT. Code firmware inchangé depuis `184b31550e0ce52d28220145967db57ba6fc399b` (diff vide) ; result_sha est le HEAD réel reconstruit. Reprise du travail existant, aucune réécriture firmware. Archives B exactes depuis HEAD initial, sans changer leur conclusion historique. Stashes005/002 préservés.

**Le NO-GO RST M013-B est levé** sur confirmation utilisateur C : RE/RST n°2 totalement libre de toute connexion externe, y compris support/breakout/NUCLEO/autre sortie. Schéma officiel v102 relu depuis copie locale008 : PG4→DE→J2→/RE→J3→RE/RST ; pas d'autre étage de sortie représenté. Avec topologie externe confirmée, PG4 est seul pilote. VIO3,27V/J1=3,3V/J2J3fermés/DE=/RE=0 sont observations utilisateur, sans nouvelle mesure Codex ou demande de répétition. Autres acquis RX011/SB28/câblage conservés ; absence microSD rapportée010, diagnostic sans accès médias. Procédure ST Windows qualifiée relue ; sonde SharedWindows, inventaire CLI positif, aucun OpenOCD/GDB préexistant détecté. GO flash sous permissions explicites C, sans changement USB/montage.

## Validation logicielle effective

CMake/Ninja depuis sources du HEAD réel, SDK `/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0`, toolchain `platform/stm32/cmake/arm-none-eabi-gcc.cmake`, diagnosticON/TXON/unité1, build isolé `--parallel 4` : configure/build retours0. CTest `--test-dir 'Modbus RTU/05_Firmware/build-host-validation' -R '^(stm32_rtu_tx_[01]|stm32_modbus_diagnostic)$' --output-on-failure` : retour0, 3/3 tests (0,01s). Proportionné : sources/tests inchangés, candidat reconstruit et empreintes comparées ; 108/108 de A est preuve antérieure, pas résultat C. Pas de tests .NET. Quatre warnings newlib historiques, aucune erreur de compilation.

BIN48240 octets SHA256 `47d69cc286b0f12771de56eb80f2648cc3c2a7a57b2ce1a8038b34de792b97e8`; ELF168212 SHA256 `762dcf0ba6c3663d9737dd334118a9f7d52a22175dda6d2674e9cb0d7b8e1c4a`, identiques A/B. nm confirme symboles diagnostic, absence des fonctions boot production/médias ; branche diagnostic avant SPI/FRAM/SD. Seule lecture normale FC03/1000/1 expose UNKNOWN0, autres données indisponibles refusées V1, aucun READY/identité/température/runtime fictif. Gate reste OFF par défaut dans sources ; ON explicite pour ce banc uniquement.

## Sauvegarde et flash effectivement réalisés

CLI Windows2.23.0 qualifié, STLINK SN003500463235510B37333439 FWV3J15M7 ; NUCLEO-U575ZI-Q, STM32U575/U585 ID0x482 RevU, CortexM33/2MiB/3,29V. Exécutable `C:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe`, chemins UNC convertis par wslpath.

Commandes réellement exécutées : `-c port=SWD sn=003500463235510B37333439 freq=8000 mode=HOTPLUG -u 0x08000000 2097152 <fichier>` pour deux sauvegardes distinctes ; `-c port=SWD sn=003500463235510B37333439 freq=8000 -w <BIN exact> 0x08000000 -v -rst`; même lecture2MiB après flash. Tous retours CLI0, identité confirmée avant écriture, verify et reset réussis. Effacement standard secteurs0..5 seulement, aucun mass erase/OptionBytes/écritureFRAMSD.

- Backup courant vérifié2MiB : `/home/lolo/dev/msm/reports/TR2-mission013-C-20261011T140835Z/flash-before-verified.bin`, deuxième lecture `flash-before-check.bin` identique ; SHA256 `67d505e6f03d52a868d2f87b8e0a6f3a9b209cd858f06dec8ee3087b8de31740`. Premiers157944 octets identiques BIN008 SHA02ed2cf6… ; dernière identité008 reconfirmée avant remplacement.
- Readback2MiB : `/home/lolo/dev/msm/reports/TR2-mission013-C-20261011T140835Z/flash-after.bin`, SHA256 `be56e0699ea196329c7c3a1bca98743fbdeeb7ce8e7a7a5d370fb0c37c7a83de`. Premiers48240 octets exactement BIN candidat ; padding jusqu'à0x0800C000 = FF ; toute Flash restante identique backup. Aucune ancienne sauvegarde utilisée comme preuve actuelle.

## Boot et observations via debugger

Chaîne STWindows qualifiée : serveur `-d -g -i <SN> --frequency 8000 -p 61234 -cp <répertoireCubeProgrammer>` ; GDB `-nx -q -batch -x <script> <ELF exact>` sur localhost61234, casts explicites/adresses vérifiées par nm et désassemblage. Deux relevés réussis `boot-2.log`/`boot-3.log` : ready1, poll_result0, poll_error_count0, uwTick71285→75800 ; PC dans pollingtransport puis runtimeRTU, IO2SV0x20000000 ; PG4 output/pulldown/ODR et entrée bas, PG7/8AF8 ; UART CR1=0x152d/CR2=0/BRR0x56ce4/ISR0x006000d0, bitsPE/FE/NE/ORE0, REACK/TEACK présents. TimerphaseIDLE, tx_busy/fault/bus_silent0, bufferTX0, indicesfile0. CFSR/HFSR0. Adresses g_serial et offsets proviennent de cet ELF, sans types TR2 DWARF complets.

**Aucun TX spontané observé** dans snapshots, bufferTX non utilisé, chemin logiciel sans réponse sans requête. Aucune capture continue DE/bus : absence instrumentée de tout transitoire boot non démontrée. Pas de halt pendant échange (aucun échange ici). Chaque session réussie détachée, serveurs sortis0 ; inventaire final sans GDB/OpenOCD, MCU laissé en exécution.

Deux erreurs d'outillage corrigées dans le périmètre : décodage UTF8 de sortie CLI Windows refusé (progression contient octets nonUTF8), conservation binaire/décodagecp1252 puis sauvegardes vérifiées ; première macro GDB faisait une arithmétique sur symbole sans typeDWARF, correction vers adresses/offsets vérifiés puis deux relevés avec detach. Première session sortie non0, serveur terminé normalement ; relevés réussis prouvent reprise/progression ultérieure. Aucun échec masqué ou test affaibli.

## Action HP unique et limites

COM7 FT232R sur HP inaccessible T1000 ; aucun envoi HP simulé ni ouverture COM3. Une instruction utilisateur transmise : seul maître HP prévu actif, autres applications série fermées ; dans dossier du script existant, une seule fois :

```powershell
.\Send-Tr2RxProbe.ps1 -Transmit
```

COM7 1152008E1, requête `01 03 03 E8 00 01 04 7A`, réponse attendue `01 03 02 00 00 B8 44`. Copier toute sortie, **sans retry**. CRC calculé indépendamment hors ligne : résidu0 pour chaque trame complète, réponse0x44B8 little-endian B8 44. Une capture reçue sera comparée octet par octet, unité1/FC03/bytecount2/UNKNOWN0/CRC ; éventuel écho ne compte pas comme réponse. L'ancien texte RX-only du script n'est plus pertinent pour l'imageC. Si instrumentation déjà accessible sans modifier le montage, conserver DE/TXD/bus ; DE haut jusqu'au dernier stopbit/TC puis bas, retourRX/garde à observer. Un snapshot final pourra montrer buffer/erreurs/état, pas chronologie des fronts.

**TX PHYSIQUE NON VÉRIFIÉ : aucune réponse réellement capturée à ce stade.** Attente utilisateur prévue au contrat§4. J1 global, timing/reset pré-init/watchdogIRQ/endurance/production/E4 restent non qualifiés ; le premier dialogue diagnostic ne les qualifiera pas automatiquement. Aucun contrôle statique/RX déjà acquis redemandé. Journal actualisé avec levéeRST, flash/readback/boot et actionHP ; transmettre ces faits et limites à ChatGPT.

## Publication

Modifications limitées au preflight, journal, rapport et deux archives B ; code/spécifications V1/configuration inchangés. Permissions exercées : build/tests, backupFlash, flashstandard/reset, debuglecture/halt/detach, commit/push main selon protocole. Premier commit `bd891105440a645f298461417b9fe5f4ab7f4b67`, message `Record M013-C verified diagnostic flash and boot pending HP response`. Push normal explicite `HEAD:refs/heads/main` réussi, fetch : HEAD=origin/main à ce SHA. Relecture intégrale des cinq livrables et contrat C depuis cette référence distante, UTF8 et comparaison byte-à-byte locale réussis ; manifeste `remote-reread.json` hors Git. Arbre propre après première publication, deux stashes inchangés. Cette mise à jour de preuve sera publiée puis relue ; SHA final communiqué séparément. Statut BLOCKED conservé pour preuveHP manquante.
