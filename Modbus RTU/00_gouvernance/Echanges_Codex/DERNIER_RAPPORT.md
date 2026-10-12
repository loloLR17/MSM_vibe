---
mission_id: TR2-20261012-M015-C-FIFO-REGISTER-RELIABILITY
status: BLOCKED
base_ref: main
base_sha: cfb9918dfe8e5ac3148d6ceaa79e50acda6ac53f
initial_head: 834f35e1759b3dd9ac1d42902f1c33cfad63f1c4
result_sha: 89218abff289c8319e03ca3c4525dd9914e7084c
created_at_utc: 2026-10-12T13:26:43.545593+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-C — Lectures au repos cohérentes ; cause FIFO active non isolée

**BLOCKED** pour l’objectif de fiabilité des readbacks pendant le remplissage ou d’isolation précise de la cause M015-B. L’audit, le nouveau probe lectureseule, ses validations, l’unique campagne et la restauration permanente sont exécutés. M015 globale IN PROGRESS ; acquis A/B conservés sans répétition. Aucune qualification FIFO/XYZ/cadence nouvelle.

## Tranche statique et logicielle

Audit et tableau hypothèses/discriminants : `../AUDIT_M015_C_SPI_FIFO.md`. Comparaison octet à octet A/B : R/W bit7, adresse7bits, commande+zéros, CSbas durant HAL duplex, pas dummy additionnel, RX utile après premieroctet. STAT3A→3B/masque03FF cohérent ;0x81 correspond àSTOP_ON_WTM+WTM8. Aucun défaut applicatif de framing démontré, aucune correction heuristique ou import driverST. La copieST conservée enB porte un masque03F anomal dans une API non utilisée ici ; aucun changement hors périmètre.

Nouveaux fichiers `stm32_iis3dwb_register_probe.c/.h` et test/mock dédiés, raccordement CMake/main minimal. OptionOFF/exclusive avant production/médias/UART. Un triggerRAM4D313543 ;512lecturesmaximum etdeadline1s. WHO1/2,CTRL3,FIFO_CTRL2,bloc07..0C,statuts pair/singles ; aucun DATA_OUT/XYZ. OctetsTX/RX commande incluse, timestampsDWT, marqueursGPIOCS, SR/CFG/compteursHAL conservés. Zéro écriture capteur ; STATUS2 peut seulement effacer le latchoverrun à la lecture, traces conservées. Pas acquisition, watermark ou overrun volontaire. Driver/cœur/firmware permanent inchangés.

## Validation réellement exécutée

CMake host puis `cmake --build <preuves>/build-host --target test_stm32_register_probe` et `ctest --test-dir <preuves>/build-host -R '^stm32_register_probe$' --output-on-failure` : **nouveau test1/1 réussi**. Test de l’implémentation surHALsimulée : commandesread, longueurs2/3/7,CS, octetcommande conservé, timeout injecté sans masquage, erreur brute, borne512. Pas preuve du bus ni duHALréel.

`STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only` réussi ; profilDebug Ninja/toolchainARM projet avec `-DTR2_STM32_REGISTER_PROBE=ON` réussi. Text30784/data20/bss42372, BIN30804octets. WarningsNewlib conservés, pas erreur. Symboles média/production/UARTstart/capture absents du profil. Justification : seulharness isolé modifié ; tests114firmware/539.NET etA/Binchangés nonrejoués.

WrapperHP seulement ajouté nom `physical-m015-c-restored`, limitéSteady/2lectures ; refusPrepare/Offline/Physical et syntaxePS vérifiés sansport. Pas rebuild.NET/paquet ni changement permanentWindows/SSH/horloge.

## Observation cible bornée

Code évalué **89218abff289c8319e03ca3c4525dd9914e7084c**. BIN SHA428d0acbb2180ab78f9bbe42493615a065470233b15aa8637ce63fb9b15df404 ; ELF SHA cf9ab01ac4d56502f78fb1d734928aa915e924bf057934b5e1f7efc94da4368e. Une seule campagnelectureseule, **512transactions/128ms**, HCLK160MHz/SPI625kHz/mode3, sizeofrecord76octets.

| Lecture | Répétitions | Attendu / observé utile | Divergences |
|---|---:|---|---:|
|WHO single / WHO+CTRL1|64 chacune|7B /7B00|0|
|CTRL3|64|04|0|
|FIFO_CTRL2|64|00|0|
|BlocFIFO07..0C|64|000000000000|0|
|Statuts3A/3B groupés puis simples|64 chacun|0000 /00 /00|0|

HALstatus/error=0, Rx/TxCount=0,CSmarqueurs1→0→1 pour512transactions. CFG1=70070067/CFG2=87400000. SRavant/après bruts publiés, pas flags internes continus ; RXWNE/RXPLVL/RXP visibles après fermetureHAL ne suffisent pas à une attribution causale. FaultsCFSR/HFSR0. Le bytecommandeRX00/FF est conservé et exclu des données selonprotocole.

**Limites** :64répétitions par type au repos ;FIFO vide etCTRL2=0 n’éprouvent ni niveaubit7/8/9 ni readback81 actif. CTRL3=04/BDUOFF préservé, aucune conversion. Les171statuts/11reculs/overrun ambiguB restent une preuve historique rejetée, nonrefaite/noncorrigée. Mouvement du bateau noninvoqué ; causecapteur/HAL/électrique inconnue. EstimationODR nonqualifiéeBinchangée. ErrataCRCsize condition absente (valeur7), troncatureEOT pointàmesurer, aucunworkaround spéculatif.

## Préservation et contrôle minimal M014

Préflight GDB :UID004A003D3842501820333157, Flashpermanente reconnue, bootstage2/result0/gen12/configUNAVAILABLE/IRQ49-66/faults0 ; sauvegardesFlash2Mio/RAM768K ethashes. InventaireSTLINK attendu003500463235510B37333439/V3J15M7/NUCLEO-U575ZI-Q ; flashCLI identifie U575/U5850x482RevU/3,29V. FlashstandardNORMAL à08000000/verify/reset, secteurs0..3 diagnostic puis0..17 restauration ; aucunHOTPLUG/masserase/OptionBytes/RDP. Chaque dernierCLI suivi de readback/resetGDB qualifié ; ensuiteGDBseul sansreset. Triggerunique, debugger détaché pendantprobe ; dumps acquis avantrestauration, restauration enfinally.

**Flash complète2Mio byte-identique àl’initiale**, SHA556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0. PermanentM014-H BIN143108 SHA b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611, ELF c37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de/source0b2a901e185c9cbf8c02410bf2a29a0a3a75491e. Pas nouvelleimageproduction, pas configurationcapteurC àrestaurer puisque aucuneécriture.

FRAMV2/gen12/identité1/configUNAVAILABLE/READY0/stockage0 retrouvés. Aucun provisioning/écritureFRAM ou accèsSD dansleprobe ; partitionLBA65536 protégéeparabsenceaccès. Pas DMA/B5/calibration/FFT/Modbusvibration.

HP :contrôle minimal de reprise seulement, paquetH371fichiershashés réutilisé ;2FC03 B0/21+B1/20,COM7/1152008E1/unit1,sansretry,retour0/connexionfermée. Journal **2026-10-12T13:23:17.9051947+00:00→2026-10-12T13:23:19.1522393+00:00** intégralement récupéré/lu,7événements, SHA **17c2a7205bf87eba0a91e823992c0c92273253192f9f986cc8b557788e2a314d** comparéaudistant ;B0complet21registresconforme,B1uptime=80s/status1/flags0/stockage0/acq0. Pas conclusion duseulhash. Garde fraîcheur0..30s etcalibrationSSHreadonly, serveursfermés avanttrafic. AprèsGDB sansreset :stage2/result0/pollerrors0/UARTerror0/TXidle/filehead=tail20/IRQ49-66/CFSRHFSR0 ; MCU détaché running/serveurexit0. CampagnequalificationM014 nonrejouée.

## Publication et prochaine action

Tranchecode/audit/archives publiée au89218ab, **12livrables relus intégralement depuisorigin/main et byte-identiques**, preuve `software-readback.json`. ArchivesB exactesdepuiscfb9918 ; missionCactiveinchangée. Journal actualisé pour observationsbornées/restauration/verrou ; roadmapM015INPROGRESS conservée.

WorktreeCisolé codex/m015-c, pushnormal explicite versmain ; original82169af/H762c459/Aeab07d4/Bcfb9918 etdeuxstashespréservés. Diff/indexinspections, aucunmerge/rebase/reset/forcepush. Permissions exercéescommit/push/flash/debugstandard ;2lecturesB0/B1uniquement aprèsrestauration. Aucun gestephysique ni paramètrepermanent.

Preuvespubliques `preuves/TR2-20261012-M015-C/` :CSV512TX/RX/flags, analysesattendu-observé, logsGDBbornés/HPintégral, hashes/backups/restauration/validations. Preuvescomplètes **/home/lolo/dev/msm/reports/TR2-mission015-C-20261012T131552Z** :commandes/CLI/GDB/BIN/ELF/Flash/RAM/sourcepreuves. Binaires/dumpsmémoire nonpubliés. SHAfinal fourni aprèspush/relecture danslaréponseCodex, nepasinscrire ici lehashducommitcontenant cetexte.

**Suite à ChatGPT** : budgetunecampagne consommé ;nepasrépéterC ouB. Voir protocole NONEXÉCUTÉ dansaudit pour capturelogiqueCS/SCK/MOSI/MISO avecinstrument3,3V/GNDlogique, comparaisonwire↔RAM ettimingpremier/dernierbit. Gestephysique etnouvelleexécution souscontratdistinct, aprèsidentificationinstrument/sécurité. Aucun branchement demandé comme actionimmédiate ni mesure prétendue. M015-C BLOCKED etM015globaleINPROGRESS ; aucunecorrection sansdéfaut démontré.

Contrôle Git whitespace : CSV généré en CRLF signalé, puis normalisé en LF avant commit ; contenu des transactions inchangé, original conservé localement et manifeste actualisé. Aucun test matériel ou logiciel relancé pour cette normalisation documentaire.
