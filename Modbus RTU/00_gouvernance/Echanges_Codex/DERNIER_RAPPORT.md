---
mission_id: TR2-20261011-M013-C-FIRST-RTU-RESPONSE
status: BLOCKED
base_ref: main
base_sha: null
initial_head: 61e9617ce67f35c9553c6255c82db6388bbb90b2
mission_sha: fe1b9cf274d2fd51b82081e0a20c5e99dc55728c
result_sha: fe1b9cf274d2fd51b82081e0a20c5e99dc55728c
created_at_utc: 2026-10-11T14:13:27.579300+00:00
updated_at_utc: 2026-10-11T14:28:30.408594+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-C — essai HP sans réponse ; réception UART non traitée observée

**GO statique / BUILD RÉUSSI / TESTS3/3 / BACKUP vérifié / FLASH et READBACK exacts / BOOT observé / TX PHYSIQUE NON VÉRIFIÉ.** BLOCKED porte désormais sur l'absence de réponse au probe unique et l'anomalie de réception constatée, pas sur RE/RST désormais résolu. Aucune qualification J1 globale ni production.

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

## Essai unique rapporté par l'utilisateur et diagnostic APRÈS, 11 octobre2026

**Observation utilisateur, pas capture instrumentée Codex** : un seul essai HP, COM7 1152008E1, requête `01 03 03 E8 00 01 04 7A`, CRC/self-checkOK, résultat « No response in ~1 s ». Aucun nouvel essai effectué. La requête n'est plus à lancer : instruction précédente consommée. L'utilisateur demande expressément le diagnostic STM32 sans nouvelle transmission HP ; aucun envoi Modbus, rejeu logiciel, commande TX, reset ou reflash n'a été effectué pendant cette reprise. Aucun geste physique/contrôle statique acquis redemandé.

HEAD de reprise propre `fbfc70de70532afc5e1d2bc07d37ed5344081039`, fetch confirme origin/main identique et contrat C toujours actif. Permissions debug/commit/push C applicables à cette continuation ; deux stashes conservés. Cible/SN/3,29V identiques aux constats précédents. Flash relue **48240 octets courants**, retourCLI0, comparaison exacte BIN diagnostic et SHA256 `47d69cc286b0f12771de56eb80f2648cc3c2a7a57b2ce1a8038b34de792b97e8`. ELF exact rehashé `762dcf0ba6c3663d9737dd334118a9f7d52a22175dda6d2674e9cb0d7b8e1c4a`. Pas de nouvelle lecture2MiB prétendue ici ; sauvegarde2MiB et readback initial restent preuves historiques de cette même mission.

Relevé GDB APRÈS avec ELF exact, dump RAM `0x20000000..0x20000674`, trois sessions lecture/halt/detach réussies (serveurs sortis0). Deux objets de diagnostic DWARF construits **hors Git**, avec commandes ARM exactes du build initial et ajout `-g3`, pour obtenir offsets/types de transport et runtime ; jamais liés dans firmware ou flashés. Layouts recoupés avec désassemblage ELF et symboles existants, aucune adresse de champ supposée. `layout.log`, `diagnostic-layout.log`, `ram.bin`, `decoded-ram.json`, scripts/logs/Flash dans `/home/lolo/dev/msm/reports/TR2-mission013-C-after-20261011T142134Z`.

| Point | Observation APRÈS | Portée |
|---|---|---|
| Exécution | ready1, dernierpoll0, compteurpollerrors0 ; PC boucle main/RTU ; uwTick737253→851024→913436 | MCU en fonctionnement, pas un échec de boot ; poll_result0 ne prouve pas réception valide |
| UART matériel | CR1=0x152D, CR2=0, CR3=1, BRR=0x56CE4 ; ISR=**0x006000F8**, au lieu de0x006000D0 avant essai | **RXNE=1 et ORE=1**, IDLE/TC/TXE présents, PE/FE/NE=0 ; données en attente et overrun, requête exacte non lisible sans consommer RDR |
| HAL UART | RxXferSize1/RxXferCount1, RxState0x22(BUSY_RX), RxISR0x080071C9(UART_RxISR_8BIT) ; ErrorCode0 ; TxXferSize/Count0, gState0x20(READY) | Réception IT armée selon HAL mais non complétée ; ErrorCode0 n'exclut pas l'erreur matérielle ORE non traitée |
| File transport | head=tail=0, overflow0 ; tous64 slots encore0, rx_byte0 | Aucune trace de callbackRX/ERROR dans la file conservée ; zéro n'est pas un compteur monotone |
| RTU | unitélocale1, étatIDLE/longueur0, **tout bufferRX256 octets0**, réponsePDU253 et ADU256 octets0 ; sourceB1 non activée | Aucun traitement réussi de la requête ni réponse préparée observable ; pas d'ADU exacte/CRC/adresse acceptés prouvés |
| Direction/TX/timer | PG4ODR/entrée bas, tx_busy/fault/bus_silent0, toutbufferTX256 octets0 ; timerphaseIDLE, TIM6CR1/DIER/SR0, PSC159/ARR749 | Pas de tentativeTX tracée ; timer de fin de trame non déclenché observable ; état final sûr au niveau digitalMCU, pas chronologieDE/TC |
| Initialisation/fautes | IO2SV0x20000000, PG7/8AF8 ; CFSR/HFSR0 ; PRIMASK/BASEPRI/FAULTMASK0 | Absence de ces défauts/masks au relevé, pas qualification électrique globale |
| Vecteurs | VTOR0x08000000 ; vecteurIRQ66=0x08001A81(LPUART1_IRQHandler), IRQ49=0x08001A8D(TIM6_IRQHandler) | Bons handlers présents dans image exacte ; pas preuve qu'ils ont été exécutés |
| NVIC via SWD | ISER0..2 à0xE000E100 : tous0 ; ISPR et ICPR0..2 : tousFFFFFFFF ; IABR et ITNS tous0, priorités lues0 | **Lecture anormale**, incompatible avec activation attendue. CubeProgrammer HOTPLUG `-r32 <adresse> 12` confirme ISER0 et ISPRFFFFFFFF indépendamment du clientGDB ; les deux utilisent toutefois ST-LINK/SWD |

Alias nonsecureSCS0xE002E100/200/300 et SCB0xE002ED00 lus0 ; SCB principalCPUID0x410FD214, ICTR3. DHCSR0x00030003 au halt. Ces valeurs ne prouvent ni la configuration de sécurité ni une cause unique : aucune OptionByte lue/modifiée, aucune protection changée. Le motif ISPR=FFFFFFFF pour tous les mots impose de distinguer état réel et visibilité/accès debug avant de conclure « toutes interruptions pendantes ». Ne pas transformer ISER0 en diagnostic définitif d'une désactivation volontaire du firmware.

### Lecture du code, déduction et limites

Sources et HAL CubeU5v1.9.0 locales inspectées : initialisation appelle SetPriority/EnableIRQ(TIM6_IRQn49 et LPUART1_IRQn66). Désassemblage `__NVIC_EnableIRQ` écrit bien ISER0xE000E100 aux banques/bits attendus ; vecteurs corrects. HAL_UART_IRQHandler doit lireRDR et traiterORE lorsque RXNEIE/PEIE/EIE sont actifs (CR1/CR3 l'indiquent). CallbackRX pousseBYTE et démarreTIM6 ; callbackERROR pousseERROR et réarmeRX. Aucun appel initialisation répété ou effacement de tous ces buffers dans la boucle normale. Ces faits logiciels contrastent avec l'absence de traces et l'ORE/RXNE persistant.

**Déduction prioritaire : le problème observé se situe avant le décodage RTU/PDU et avant le TX, dans la prise en charge de RX par interruptions**, ou sa visibilité/configuration effective. L'UARThardware a reçu de l'activité, mais aucune ADU exacte n'a été constatée. La concordance temporelle avec l'unique probe et l'état avant rendent le probe source probable ; **les huit octets exacts ne sont pas démontrés**, et ORE peut aussi provenir d'activité non caractérisée. Aucune trame CRCincorrecte ou mauvaise adresse identifiée, aucun rejetTX ou défautDE démontré. Ce résultat ne remet pas à lui seul en cause VIO/J2/J3/RE-RST confirmés ni acquisRX011 sous autreimage.

Lectures RDR/ICR et écrituresNVIC/UART volontairement absentes pour préserver l'état ; pas de correction invasive basée sur lectureNVIC ambiguë. Aucun reset après probe, aucun breakpoint/watchpoint pendant échange. Absence de compteurs dédiés diagnosticRX/IRQ/CRC/TX et capture temporelle dans imageC limite la reconstitution. ORE/RXNE confirmé encore lors des deux relevés ultérieurs et viaCLI. Sessions détachées, aucun serveurGDB/OpenOCD restant, cible laissée en exécution ; cette reprise a préservé l'anomalie, sans réarmer artificiellement ni relancer TX.

### Prochaines actions, sans nouvelle transmission HP

1. Revue ChatGPT des preuves et de l'écart **EnableIRQ attendu / tracesRX absentes / lecturesNVIC anormales**. Établir l'état effectif d'activation/attribution et la visibilitéSCS/SWD avant d'imputer une panne électrique ou modifier la direction.
2. Sous tranche explicitement cadrée, préparer instrumentation **passive** du diagnostic (compteurs entréeIRQ UART/TIM6, callbacksRX/ERROR, derniersISR/HALerrors, progressionRX/RTU/CRC/adresse/TX), lecture NVIC depuis le cœur et historique d'activation, sans requête synthétique/émission ni faux runtime. Un éventuel reflash/reset effacerait ces preuves RAM : conserver dump actuel et backup avant cette étape.
3. Aucun nouvel essai Modbus HP demandé ni autorisé dans cette reprise. Après isolation et correction démontrée par tests/build et diagnosticlocal non émetteur, une **mission suivante** décidera d'un futur essai instrumenté si nécessaire ; ne pas recycler automatiquement le probe unique.

**Premier dialogue diagnostic non réussi : absence de réponse rapportée, TX PHYSIQUE NON VÉRIFIÉ**, réceptionADU complète non démontrée sous imageC. J1 global, timingsDE/TC/retourRX/reset/watchdogIRQ, production et E4 restent non qualifiés. Aucun code de production/test/configuration/matériel modifié. Validation de cette reprise : identitéFlash/ELF, sortiesdebug/CLI réelles, lecturecode et diffdocumentaire ; aucune recompilation firmware/campagneCTests répétée, car changement documentaire et lecture de l'état physique, les objetsDWARF sont uniquement outillage horscible.

## Publication

Modifications limitées au preflight, journal, rapport et deux archives B ; code/spécifications V1/configuration inchangés. Permissions exercées : build/tests, backupFlash, flashstandard/reset, debuglecture/halt/detach, commit/push main selon protocole. Premier commit `bd891105440a645f298461417b9fe5f4ab7f4b67`, message `Record M013-C verified diagnostic flash and boot pending HP response`. Push normal explicite `HEAD:refs/heads/main` réussi, fetch : HEAD=origin/main à ce SHA. Relecture intégrale des cinq livrables et contrat C depuis cette référence distante, UTF8 et comparaison byte-à-byte locale réussis ; manifeste `remote-reread.json` hors Git. Arbre propre après première publication, deux stashes inchangés. Cette mise à jour de preuve sera publiée puis relue ; SHA final communiqué séparément. Première publication historique préparatoire ; statut BLOCKED désormais conservé pour probe sans réponse et anomalieRX. Reprise diagnostic publiée au commit `a974a7b30c7e8b874d75c2a6fee4c476a964d6d9` (`Document M013-C single-probe failure and passive STM32 RX diagnostics`), push normal explicite main réussi puis fetch HEAD=origin/main. Relecture intégrale des trois documents modifiés et contratC depuis référence distante, UTF8/comparaison byte-à-byte réussies ; preuve `remote-reread.json` horsGit dans dossierAPRÈS. Arbre propre et deux stashes inchangés après publication. Cette mise à jour de traçabilité sera publiée et relue à son tour ; SHA final communiqué séparément. BLOCKED technique reste maintenu, sans nouvelle demande HP.
