---
mission_id: TR2-20261011-M013-E-INSTRUMENTED-RTU-RETRY
status: BLOCKED
base_ref: main
base_sha: null
initial_head: d2f4bb5862ac64292c882e3a73d85b5e499fa98a
mission_sha: 0bfaa0e0daf3d37f723497b74984cabe1b11ec47
result_sha: 0bfaa0e0daf3d37f723497b74984cabe1b11ec47
created_at_utc: 2026-10-11T16:06:34.631427+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-E — GO préparation du probe RTU unique, NVIC sain et TXON

**GO technique de préparation. BLOCKED provisoire uniquement pour publication/relecture distante. Aucun essai HP effectué en E ; Modbus bidirectionnel et TX physique NON VALIDÉS.** Le firmware actuel est diagnosticON/observerON/**TXON**/unité1. Le probe C consommé n'a pas été relancé. Un nouvel essai unique E appartient à l'utilisateur, après lecture de ce rapport GO par ChatGPT.

## Référence et périmètre

Main initial propre d2f4bb5, deux stashes historiques005/002 conservés. Fetch puis pull --ff-only vers 0bfaa0e : seul contrat changé. AGENTS.md, protocole, journal intégral, rapport D DONE, procédure corrigée D, diagnostic/transport/main/CMake/tests et preuves D relus. Mission ChatGPT inchangée, base_sha absent donc null. Code firmware inchangé depuis09792c8 ; result_sha est le HEAD réellement utilisé pour construire E. Hashes des huit fichiers D comparés au code courant ; BIN/ELF D rehashés. Aucune réécriture de code, refactorisation, modification V1 ou configuration permanente. Archives mission/rapport D exactes depuis HEAD initial, sans modifier leur conclusion.

Acquis physiques VIO/J1/J2/J3/RE-RST libre/SB28/câblage/RX011 conservés, aucun contrôle répété ni geste physique demandé. Branche diagnostic avant SPI/capteur/FRAM/SD/production ; absence microSD acquise. FC03/registre1000/quantité1 expose seulement UNKNOWN0 (initialisation incomplète), données indisponibles refusées V1, aucun READY/identité/température fabriqués. TX activé explicitement dans build E, gateOFF par défaut dans sources. Réponse seulement après requête acceptée et T3.5, sans émission sollicitée par Codex.

## Preuves et sauvegarde courante

Dossier hors Git : `/home/lolo/dev/msm/reports/TR2-mission013-E-20261011T160141Z` ; preuves D originales intactes dans `/home/lolo/dev/msm/reports/TR2-mission013-D-20261011T143352Z`. Manifestes source-manifest.json, before-manifest.json, artifacts.json, readback-manifest.json et evidence-manifest.json ; logs/scripts/dumps/images conservés hors dépôt public.

Inventaire CLI -l st-link-only : ST-LINK SN003500463235510B37333439 FWV3J15M7, NUCLEO-U575ZI-Q. Connexion sauvegarde : STM32U575/U585 ID0x482 RevU/Cortex-M33/2MiB, tension3,29V, fréquenceSWD8000kHz. Identité recontrôlée au flash. Chaîne ST Windows qualifiée et mêmes exécutables sous C:\ST\STM32CubeCLT_1.22.0.

Avant flash : session GDB avec ELF D exact, dump RAM liée entière768KiB 0x20000000..0x200C0000, detach ; NVIC déjà sain ISER1=20000/ISER2=4, ISPR0. Deux sauvegardes Flash courantes2MiB via CLI HOTPLUG -u0x08000000 2097152, retours0, byte-à-byte identiques SHA256 `f9894c46b429ee87a48d34f9e3caaa7a600fa47d8a4fb179fe7eab7234c0dc1b`. Premiers47812octets identiques image D TXOFF ; backup courant identifié, pas simple copie d'ancienne preuve. RAM initiale SHA256 `d20069d96b014339e2c336a85b1aede5ab7a7df7035b87a8aa09b24c1941be04`. Nouveau dump complet ram-preflash.bin avant remplacement ; dump ram-before-final-reset.bin avant reset final. Les accès CLI sont considérés perturbants conformément à D, sans nouvelle investigation de cause.

## Build et tests effectivement exécutés

CMake/Ninja frais depuis HEAD réel, SDK Linux CubeU5v1.9.0 et toolchain platform/stm32/cmake/arm-none-eabi-gcc.cmake, build-diagnostic-observer-tx-on hors Git : TR2_STM32_MODBUS_DIAGNOSTIC=ON, TR2_STM32_IRQ_DIAGNOSTICS=ON, TR2_STM32_RS485_TX_QUALIFIED=ON, TR2_STM32_DIAGNOSTIC_UNIT_ID=1. Configure/build --parallel4 retours0. text49176/data24/bss5192, quatre avertissements newlib historiques (_close/_lseek/_read/_write), aucune erreur. Symboles diagnostic/observateur présents, absence de boot production/FramSpi_Init/Iis3dwbSpi_Init/Sdmmc2_Bringup dans image liée vérifiée par nm.

BIN49200octets SHA256 `dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540` ; ELF168512 SHA256 `4c496149b7b3af593269dbff7c92368d575cebe824f62ffa67ed57e522d3adc2`.

CTest réellement relancé sur exécutables hôte existants du même code inchangé : `ctest --test-dir 'Modbus RTU/05_Firmware/build-host-validation' -R '^(stm32_rtu_tx_[01]|stm32_irq_observer_[01]|stm32_modbus_diagnostic)$' --output-on-failure`, retour0, **5/5** (0,02s). Matrix observerOFF/ON × TXOFF/ON plus garde-fous diagnostic. Validation proportionnée : aucune source/interface modifiée, nouveau cross-build exact du candidat TXON et tests ciblés ; pas de campagne complète/.NET ni buildARM défaut répétitif qui n'évaluerait pas ce candidat. Le cross-build-only du script principal reste une preuve D, pas annoncé comme relancé E. Aucune simulation de trafic sur la cible.

## Flash, readback et procédure D appliquée

CLI qualifié `-c port=SWD sn=003500463235510B37333439 freq=8000 -w <BIN exact> 0x08000000 -v -rst`, retour0 ; effacement standard secteurs0..6 seulement (candidat dépasse0x0800C000), verify et reset réussis. Readback CLI HOTPLUG2MiB retour0, SHA256 `0ebb31d951a24bd0b609d38f8180ebe1e2976072a7f853a3d116799a48916a7b` ; premiers49200octets exactement candidat, paddingFF jusqu'à0x0800E000, toute Flash restante identique backup. Aucun mass erase, aucune lecture/modification OptionBytes, aucun changement de protection, média ou câblage.

**Dernier accès CLI MCU = readback ci-dessus**, suivi de conservation RAM768KiB puis `monitor reset` GDB qualifié et detach. Ensuite uniquement GDB : premier relevé sain, dump transport et relectureFlash exacte49200octets, dernier relevé sain. Aucun CLI MCU après ce point ; aucun reset supplémentaire. Exécutables Windows ST-LINK_gdbserver/arm-none-eabi-gdb qualifiés, serveur -d -g -i<SN> --frequency8000 -p61234 -cp<CubeProgrammer/bin>, GDB -nx -q -batch -x<script> <ELF exact>, target extended-remote localhost:61234. Toutes sessions réussies détachées, serveurs sortis normalement0 ; inventaire final Windows/Linux sans GDB/OpenOCD. MCU laissé en exécution.

## État observé et verdict GO

| Point contrôlé | Observation réelle |
|---|---|
| Ready/polling | ready1, dernierpoll0, erreurs0 ; uwTick4389→31167 ; poll_count1496571→10627242 |
| NVIC CPU et SWD | ISER0=0, ISER1=0x00020000 (TIM6 IRQ49), ISER2=0x00000004 (LPUART1 IRQ66) ; ISPR0..2=0, masques PRIMASK/BASEPRI/FAULTMASK0 |
| UART1152008E1 | CR1=0x152D/CR2=0/CR3=1/BRR=0x56CE4, ISR=0x006000D0 ; RXNE/ORE/PE/FE/NE0 |
| HAL réception | RxState0x22 BUSY_RX, RxXferSize/Count1, RxISR0x08007589 ; ErrorCode0 ; gState0x20 READY, TxXferSize/Count0 |
| Direction/transport | PG4output/ODR0 ; phaseTIM6IDLE, tx_busy/fault/bus_silent0 ; queuehead/tail0, overflow0, bufferTX256octets0 |
| AlimentationGPIO/fautes | IO2SV0x20000000, PG8AF8 ; CFSR/HFSR0 |
| Observateur | IRQ/RX/ERROR/trames/CRC/TX_attempt/started/failsafe/callbackTC tous0 ; aucun trafic nouveau, référence AVANT conservée |

Layouts transport/HAL obtenus depuis objet DWARF -g3 compilé avec commandes exactes du build E, hors image et jamais flashé ; nm actuel confirme g_serial692octets et observateur992octets, adresses issues de cet ELF. Petite erreur d'outillage hors cible : filtre initial de commande Ninja supposait fin de ligne sans guillemet ; aucun match, corrigé par découpage shlex puis compilation/layout réussis. Aucun résultat de test affaibli ou échec matériel masqué.

**GO pour préparer l'unique essai E**, grâce à image exacte/TXON, polling, IRQ et réception armée sains. Compteurs0 et PG4bas sont observations ponctuelles et cohérence logicielle ; aucune capture continue de tous transitoires boot/DE, aucun service IRQ sur trafic nouveau démontré. Le mot QUALIFIED de l'option de build n'est pas une déclaration de qualification physique.

## Action HP après lecture du GO par ChatGPT — une seule fois

Fermer les autres applications série ; seul maître prévu HP actif, sans accès CubeProgrammer au MCU entre état sain et échange. Dans le dossier HP du script existant, lancer **une seule fois** `./Send-Tr2RxProbe.ps1 -Transmit` (PowerShell accepte `.\Send-Tr2RxProbe.ps1 -Transmit`). COM7,1152008E1 ; requête `01 03 03 E8 00 01 04 7A` ; réponse attendue `01 03 02 00 00 B8 44` (UNKNOWN0). Pas de retry, ni nouvel essai RX préalable, ni halt/debug pendant l'échange. Transmettre toute sortie réelle, succès ou absence de réponse. Le texte historique « RX-only » du script en cas de timeout n'est pas le verdict attendu du candidat TXON E.

Cette instruction est **nouvelle tranche E**, pas reprise du probe C clos. Codex n'a ouvert ni COM7 ni COM3, n'a émis ni requête synthétique ni commandeTX. Le contrôle indépendant ChatGPT du GO n'est pas démontré par sa publication ; il doit précéder le lancement utilisateur.

Une mission séparée analysera ensuite réponse/CRC/écho éventuel, deltas IRQ→HAL→file→8octets→CRC/adresse→UNKNOWN0→TX→callbackTC et état final. Sans captureDE/dernierstopbit/TC/retourRX, pas de qualification temporelle de la ligne ; snapshots seuls insuffisants. Modbus bidirectionnel/TXphysique/J1global/production/E4/resetpré-init/watchdogIRQ restent NON QUALIFIÉS.

## Publication

Modifications documentaires seulement : rapport E, journal actualisé TXON/GO et prochaine action, preflight précédé du verdict E ; archives D immuables et référence au rapport D corrigée dans sa procédure historique. Contrat/AGENTS/code/V1/configuration inchangés, deux stashes conservés. Permissions E exercées : backupFlash/RAM, build/tests, flashstandard/reset, debuglecture/halt/detach et reset logiciel de clôture après RAM, commit/push main autorisés. Diff documentaire à vérifier, publication/relecture distante en attente ; BLOCKED porte uniquement sur cette preuve. Aucun succès physique anticipé.
