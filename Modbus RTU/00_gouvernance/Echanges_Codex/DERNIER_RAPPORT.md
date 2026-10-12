---
mission_id: TR2-20261012-M015-B-FIFO-CADENCE
status: BLOCKED
base_ref: main
base_sha: eab07d49a5f3b86506a97877d0fccf2188fb0a7a
initial_head: 8a51d96dbfc25d2396069210e3cc2b5c45935086
result_sha: 15c3369446a568ff2dd2d7a13453d6cfaa1cdbf0
created_at_utc: 2026-10-12T10:16:04.160618+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-B — FIFO préparée, preuve physique bloquée

**BLOCKED pour la qualification FIFO/cadence : les statuts observés sont incohérents et aucun XYZ n'a été accepté.** L'audit, le prototype, les tests ciblés et l'unique campagne autorisée sont réalisés. La limite de périmètre est respectée : aucune variante SPI supplémentaire, DMA, acquisition continue, stockage, B5, FFT ou Modbus vibration. M015 globale reste IN PROGRESS ; les acquis bornés M015-A ne sont pas rejoués.

**Le firmware permanent M014 est restauré exactement et répond à B0/B1.** Aucun diagnostic ne reste en Flash. Aucun geste physique ou validation intermédiaire n'a été demandé.

## Tranches et résultats

| Tranche | Résultat |
|---|---|
| Reprise | Worktree isolé depuis8a51d96, archives mission/rapport M015-A exactes depuiseab07d4 ; anciens worktrees/stashes préservés |
| Audit ciblé | AN5444/driver officiel ST : modes, watermark, format7octets, tags/parité, ordreBDU et overrun ; budget de bus insuffisant pour un flux continu |
| Prototype | Profil FIFO isolé OFF par défaut, mode3/625kHz fixe,256XYZ au watermark ; statuts bruts, tags et garde de pertes ; restauration7registres |
| Validation | Nouveau test hôte1/1 et cross-build défaut/profil FIFO réussis ; anciens114firmware et539.NET non rejoués |
| Cible | Un flash diagnostic/un trigger ;171polls,11reculs de niveau, flag overrun brut et readback divergent ; refus avant vidange,0XYZ |
| Reprise M014 | Flash2Mio identique au backup initial ; bootgen12 sain ; deuxFC03 B0/B1, deuxréponses, retour0/sansretry |

La configuration prévue est FIFO mode1, STOP_ON_WTM=1, watermark256, BDR_XL=0xA/26667Hz, température/timestamps sensor en FIFO désactivés. Un mode continu32ms devait ensuite vérifier la détection d'overrun et le rejet des mots écrasés. **Ce contrôle négatif n'a pas été atteint**, la séquence nominale étant refusée avant la lecture de données.

À625kHz, un mot avec commande nécessite8octets : plafond9765,6mots/s avant HAL/statuts, inférieur à26667. Le mode FIFO arrêté permettrait une capture finie puis une vidange lente ; il ne résout pas le débit continu. BuffersRAM bornés22716octets. Le driver production et le cœur restent inchangés ; configuration/WHO existants réutilisés, I/O FIFO dans le seul harness. Refus d'une FIFO préexistante/autorité active, pas de média ou UART démarré.

Sources : [ST AN5444 Rev3, §6](https://www.st.com/resource/en/application_note/an5444-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf) et [driver ST](https://github.com/STMicroelectronics/iis3dwb-pid), consultés12/10. Nouvel accès datasheet DS12569 expiré ; sa configuration SPI acquise enA est reprise, règles FIFO établies par AN5444/sourceST, aucun contenu non lu inventé. Audit et critères quantitatifs dans `AUDIT_M015_B_FIFO.md`. Aucun test de norme1g ou d'immobilité : les mouvements du voilier ne sont pas un motif de rejet.

## Verrou physique et limites

Code réellement évalué **15c3369446a568ff2dd2d7a13453d6cfaa1cdbf0**, BIN36532octets SHA **09a6ded74b4a254792c7ab9f1c5a38352166bfafb09537e710760e4cb19ad4fb**, ELF SHA **8d0f06e24147414e970dcef30dd3f24a6350e01bc1f557b6596bb639476217b8**. Flash vérifié et relu identique. MCU : WHO0x7B, SPI3 mode3/prescaler256,625kHz,HCLK160MHz, CFG1=0x70070067/CFG2=0x87400000, fautes0.

| Observation brute | Portée |
|---|---|
|171statuts FIFO avant toute vidange | Tous conservés dansCSV, avec bornesDWT |
|11reculs du niveau, notamment127→0 | Incompatible avec le remplissage attendu sans consommateur ; cause inconnue |
|Poll169 :0x40FE, niveau254 | BitOVR_IA observé ; dans des statuts incohérents, ne prouve pas seul un écrasement physique |
|Poll170 :0xA100, niveau256 | Watermark/full observés, mais fiabilité insuffisante pour qualifier la séquence |
|FIFO_CTRL2 demandé/vérifié0x81, puis relu0x80 | Readback ultérieur divergent ; transport/configuration à établir |
|bus_errors=0 | HAL_OK ne garantit pas l'intégrité des octets retournés |
|result=INVALID_STATE,count=0 | Refus conservateur avant vidange ; aucune donnée XYZ ni faux échantillon publié |

loss_flags=0x21 :0x01 correspond à l'overrun observé ;0x20 est ajouté par le garde global quand une perte existe. Ce dernier bit ne prouve pas un mauvais niveau final : celui-ci vaut256. Aucun correctif ne masque l'erreur ou modifie artificiellement les attendus.

La cause précise n'est pas isolée. Les lectures SPI/framing/longueurs, l'ordreBDU ou le comportement capteur restent à distinguer ; aucune de ces hypothèses n'est déclarée cause certaine. Le tag modulo4 ne pourrait pas détecter seul des pertes multiples de4, et son observation physique n'a pas été atteinte. Ni consécutivité, ni absence de perte, ni cadence fiable ne sont démontrées.

Les bornes d'activation/transition watermark donnent une **estimation non qualifiée26244..26864Hz**, dépendante de statuts incohérents. DWT date les lectures MCU, pas les conversions capteur ni l'UTC. Cette estimation n'est pas une validation de l'ODR. La série de256XYZ et le contrôle négatif restent seulement testés sur hôte.

Conformément à la mission courte, aucune deuxième campagne ou nouvelle variante de vitesse/mode n'est lancée. La prochaine mission doit d'abord établir la fiabilité des lectures des registres FIFO avant de reprendre une qualification de séquence.

## Préservation, flash et contrôle M014

Avant flash : ST-LINK003500463235510B37333439/NUCLEO-U575ZI-Q/STM32U575-U5850x482RevU/3,29V, UID96 **004A003D3842501820333157** ; BIN/ELF permanents rehashés, préfixeFlash reconnu ; backups Flash2Mio etRAM768K conservés. Flash standard NORMAL `-c port=SWD sn=<SN> freq=8000 -w <BIN> 0x08000000 -v -rst`, secteurs0..4 pour diagnostic et0..17 pour restauration. Deux flashs seulement, chacun suivi de relectureGDB/reset qualifié aprèsCLI ; aucun HOTPLUG/mass erase/Option Bytes/RDP. Un triggerRAM4D313542 ; debugger détaché pendant capture.

Résultats/RAM conservés avant restauration. Les sept registres du capteur sont relus restaurés :0/0/0/0/0/4/0, restore_result=0, avec la réserve générale de fiabilité des lectures FIFO. Les **2Mio de Flash sont entièrement identiques octet par octet au backup initial**, SHA **556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0**.

Firmware actuellement laissé en exécution : production M014-H/source **0b2a901e185c9cbf8c02410bf2a29a0a3a75491e**, BIN143108octets SHA **b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611**, ELF SHA **c37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de**. Pas nouvelle acquisition activée en production.

Profil isolé sans symboles d'accès FRAM/SD ou démarrage UART. Aucune écriture/provisioning FRAM, lecture/allocation/écriture microSD ; partition LBA65536 préservée par absence d'accès. Boot permanent : FRAMV2/génération12/identité1, configuration UNAVAILABLE/READY0/stockage0, pas campagne B5. Les sauvegardes et acquis M014/A restent conservés.

Contrôle de reprise seulement : paquet HP H existant,371fichiers hashés, nouveau nom `physical-m015-b-restored`, modeSteady limité à2FC03/sansretry. Garde fraîcheur0..30s avec calibrationSSH de l'heureHP ; baselineGDB sansreset, serveurs fermés avant trafic. Wrapper scope/syntaxe vérifiés sans port. Pas rebuild.NET, installation ou changementPC/SSH/horloge.

HP12/10/2026 à10:06:21..22UTC, COM7/1152008E1/unité1 : B0/21 puisB1/20, deuxréponses/retour0/connexionfermée. B0 intégral conforme à l'identité persistée, B1 uptime215s/status1/flags0/stockage0/acquisition0. Journal complet récupéré parSCP, hash comparé au distant **8B31E409B02580B5D109D281C506D69B9F470C56A7C9B5812A4BDBF14831278F**, septévénements publiés/analyse complète, pas conclusion du seulSHA. GDB après sansreset : stage2/result0/pollerrors0,gen12/configUNAVAILABLE, UARTerror0/TXidle/filehead=tail20/IRQ49&66/CFSR-HFSR0 ; détaché enexécution/serveurterminé. Pas répétition du polling/perte M014, aucune autre émission HP.

## Validation, publication et suite

Commandes réellement réussies : CMake host puis build du seul `test_stm32_iis3dwb_fifo`, `ctest -R '^stm32_iis3dwb_fifo$'` **1/1** ; depuisracine `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only` ; profilDebug dédié avec toolchainprojet et `-DTR2_STM32_FIFO_CAPTURE=ON`. WarningsNewlib/nosys conservés, aucuneerreurbuild/test. Choixproportionné : nouveauharness isolé, cœur/driver inchangés ;114firmware/539.NET historiques nonrejoués.

Le test hôte vérifie capture256, framing8octets/tag, niveau/parité/gaps, overrun/rejet, timeout et restauration, refusFIFOpréexistante. Il ne simule pas la cause des incohérences physiques et ne remplace pas la preuve cible. Replay offline `python3 'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261012-M015-B/verify_fifo.py'` réussi :171polls/11reculs/hashbrut et rejet reproduits, contrôleHP passé. Aucun XYZ synthétique.

Code et archives publiés au15c3369 puis relus intégralement depuisGitHub,11livrables byte-identiques. La présente publication ajoute CSV/analyses/manifest/statuts/résultats/rapport et modifications minimales du wrapper de reprise. Journal actualisé pour verrouFIFO/restauration/suite ; roadmap conserve M015 IN PROGRESS et M015-B BLOCKED. La mission active reste inchangée. Permissions exercées : commit/pushnormaux surmain, flash/debugstandard et2lecturesB0/B1 prévues. Aucun geste physique ou configurationpermanente.

Dépôtinitial82169af, worktreeH762c459, worktreeAeab07d4 etdeuxstashes historiques préservés ; worktreeB isolé `codex/m015-b`, destinationexplicitement main. Aucun merge/rebase/reset destructif/force push. Diff etindex à inspecter avantpublication finale ; SHA distant et relecture seront consignés ensuite. BLOCKED restera pour la preuveFIFO manquante, même après publication.

Preuves publiques : `preuves/TR2-20261012-M015-B/`. Preuvescomplèteslocales : **/home/lolo/dev/msm/reports/TR2-mission015-B-20261012T095010Z**, avec BIN/ELF/backups/commandes/CLI/GDB/logsvalidation/sourceST. Les dumpsmémoire etbinairesfirmware ne sont pas publiés.

**Suite à transmettre à ChatGPT** : contrat ciblé de fiabilité des registres FIFO (readbacks stables, ordreBDU/framing/longueurs ; observation SCK/MOSI/MISO si nécessaire), avant toute nouvelle capture. Aucun besoin de DMA ou stockage pour résoudre ce verrou ; aucune hypothèse d'immobilité. M015-A garde sa portée, M015globale n'est pas clôturée.
