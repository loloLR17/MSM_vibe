---
mission_id: TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL
status: DONE
base_ref: main
base_sha: 85e7a1ce38fb7d79d10dcb779275ec5c05561e7b
initial_head: 5045bc7862b02e76b3ca0b0c4c54cc369ad88acc
result_sha: 811a3eda214ffb9c6c3dfbacd2af2884d87143c4
host_tools_sha: 73c3d252ccc534576dc72cba1414d4a0312e7dd4
verified_remote_sha: 2eefbb7e0cff1f1c8c72aa3e38dd495b1d6eddf0
updated_at_utc: 2026-11-18T20:36:38.354462+00:00
created_at_utc: 2026-11-18T20:29:33.335884+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# RS485 Click sur shield — RX, TX et Modbus physiquement démontrés

**Les trois critères techniques ciblés sont satisfaits sur TR2-001.** Le code, les résultats et les preuves ont été publiés puis relus intégralement depuis GitHub à la référence 2eefbb7e0cff1f1c8c72aa3e38dd495b1d6eddf0 ; la mission ciblée est DONE, sous réserve de la contre-lecture indépendante de ChatGPT. Aucun nouvel essai n'est nécessaire pour ces résultats. Le défaut SPI3 J8 et le **NO-GO global E-01 restent inchangés** ; aucune qualification complète du shield ni décision de refabrication.

## Résultats et preuves

Dossier [preuves de mission](preuves/TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL/), notamment [comparaison indépendante hôte/RAM](preuves/TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL/independent-wire-RAM-verification.json), [trames RAW](preuves/TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL/native-RAW-01.jsonl), [trames Modbus](preuves/TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL/native-MODBUS-01.jsonl) et [manifest](preuves/TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL/MANIFEST.json).

| Tranche réellement exécutée | Résultat observé |
|---|---|
| RX seul, 20:19:20 UTC | 1 challenge de 14 octets, reçu exactement en RAM par le STM32 ; zéro TX, zéro erreur UART |
| UART brut bidirectionnel, 20:21:15–20 UTC | 8 challenges avec séquences 0..7, 8 réponses de 26 octets exactes, CRC valides ; nonce et UID de TR2-001 présents, réponse distincte de la requête : écho local exclu |
| Modbus RTU, 20:24:05–10 UTC | 8 requêtes FC03 adresse 1, registre 1000, quantité 1 ; 8 réponses exactes, 8 CRC valides/8 requêtes acceptées côté STM32 |
| DE et retour RX | 16 paires haut/bas enregistrées, IDR/ODR cohérents ; DE bas, RX réarmé et transport au repos après chaque série |
| Bilan | 17 requêtes, 190 octets RX comparés entièrement aux journaux hôte, 16 réponses et 16 callbacks TX ; aucune erreur UART, perte d'événement, faute TX ou faute Cortex observée |

Modbus : request `01 03 03 E8 00 01 04 7A`, response `01 03 02 00 00 B8 44`. **UNKNOWN(0)** provient du diagnostic existant B1, initialisation incomplète ; il ne simule aucune mesure capteur ni identité métier. Aucun FC10, START/STOP, accès média ou écriture persistante. Seul FC03 est transmis au serveur PDU dans ce banc expérimental.

Le [vérificateur hors ligne](../../../experiments/shield_rs485_click/tools/verify_evidence.py) relit indépendamment tous les octets hôte, les trois snapshots RAM, CRC, nonce/séquences/UID, compteurs, événements et fronts DE. Exécution : `python3 experiments/shield_rs485_click/tools/verify_evidence.py "Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL"`. Ce contrôle ne transmet rien et ne rejoue aucun essai.

## Montage et gates constatés

- Nucleo réellement identifiée TR2-001 : ST-LINK `003500463235510B37333439`, UID `004A003D/38425018/20333157`. Autre banc hors tension selon utilisateur ; aucun accès à TR2-002.
- Shield V2.3 et seul Click enfiché : FRAM, microSD et MKIGIBV2 retirés, inspection à froid et stabilité sans contact/contrainte confirmées par utilisateur. Révision Click fraîche inaccessible ; il confirme le même Click historiquement utilisé avec pont J2, sans démontage imposé. Référence attendue MIKROE-3863/v102, pas de nouvelle identification de sérigraphie revendiquée.
- Mesures utilisateur à froid : TP1↔TP2 = 0 V puis 5,18 kΩ stabilisés ; J2-11↔TP2 = 0,2 Ω ; CN12-69/PG4↔CS/DE Click = 0,3 Ω. J5 femelle 2 × 3 confirmé monté. Continuité mesurée en bout de chaîne, pas six contacts mesurés individuellement.
- Sous ST-LINK seul, CN15 débranché : TP1↔TP2 = 3,3 V ; puis TX Click/J9-13↔TP2 = 3,3 V et DE/J9-3↔TP2 = 0 V avant trafic. Le CLI relève 3,29 V côté cible, mesure distincte.
- Configuration utilisateur confirmée inchangée : VCCSEL 3V3, J2 DE–/RE, J7 A/Y et J8 B/Z half-duplex ; aucune liaison ajoutée GND2↔GND logique. Blanc Data+ → A, vert Data− → B, noir → GND du bornier isolé. Aucune inversion expérimentale ni modification des straps.
- Adaptateur fraîchement retrouvé : FTDI `0403:6001`, série `BG04KSSW`, instance `FTDIBUS\VID_0403+PID_6001+BG04KSSWA\0000`, COM4 ; COM ST-LINK exclu. Trois ouvertures exclusives par le client, fermeture explicite dans chaque journal, aucun service arrêté ni double maître observé. TERM/BIAS conservés ; positions individuelles non fraîchement relevées, pas de qualification de topologie longue déduite.

GPIOG4/7/8 et domaine VDDIO2 : configuration IOSV/AF8 observée, TX haut mesuré 3,3 V, DE bas mesuré 0 V, niveaux logiques et échanges fonctionnels cohérents. **Pas de mesure directe du rail VDDIO2 ni de son ondulation/marges.** Source primaire : netlist ST C05 déjà archivée dans l'audit E-01 et [UM2861 ST, §7.4.5/table9](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf). Schéma Click v102 empreinte 204f5c4b…b716c consignée dans l’audit E-01 ; DE est CS mikroBUS 3, pas PWM.

## Logiciel, flash et récupérabilité

Nouveau [banc isolé](../../../experiments/shield_rs485_click/README.md), NON-PRODUCTION, zéro driver SPI/SD/FRAM/IIS3DWB lié. Cœur, transport et diagnostic partagés inchangés ; LPUART1 PG7/8, DE PG4, 115200/8E1, horloge CPU 160 MHz / LPUART HSI 16 MHz. Boot RX seul, armement RAM `0xC118A55A` distinct pour RAW puis Modbus, 16 crédits maximum par armement, arrêt sur erreur et aucun retry automatique. Le flag TX est limité à cette cible de qualification, aucun changement du profil permanent/V1.

Code firmware évalué **811a3eda214ffb9c6c3dfbacd2af2884d87143c4** ; client natif/vérificateur **73c3d252ccc534576dc72cba1414d4a0312e7dd4**. BIN de 34 380 octets SHA256 **72e5ab65456a9743e3f0bcfd5d33d751b9daa05792a47be46197e277682f4563**, ELF 6932a60c…52b12. Sources/hashes, BIN et client natif sont publiés dans les preuves ; ELF/map complets restent dans le dossier local de mission.

Flash initiale de 2 MiB sauvegardée fraîchement et vérifiée SHA256 `374d5153992585ef2cbf57dccdac83a8b8dcdd0775af7ed908713fd0e6fc7547` ; son préfixe correspond exactement à l'ancien banc FRAM 250 kHz / BIN 019569f9…795fd. Un seul flash standard qualifié NORMAL/SWD/SN explicite/freq8000/adresse08000000/verify/reset, secteurs 0..4 seulement. Readback de 2 MiB : image byteexact et tout le suffixe après 40 960 octets inchangé. SHA du readback `21b324f9dff4aee6317683a5450e955f37257dec95f0d07ae12e5fd6f5f53c18`. Reset logiciel GDB après dernier CLI selon RCA NVIC historique ; aucun HOTPLUG, mass erase, OptionBytes/RDP ou changement de boot/sécurité.

Sauvegardes intégrales privées et vérifiées : `/home/lolo/dev/msm/reports/TR2-shield-rs485-click-20261118/001-before-flash-2MiB.bin` et `001-click-readback-2MiB.bin`. Original CN0 et acquis de la mission du 17/11 restent conservés séparément. Aucune restauration superflue d'un firmware accédant à des modules désormais absents.

## Validations et incidents

- Test hôte nouveau `click_policy` **1/1 PASS**, cross-build ARM001 **PASS**, sans refaire les validations anciennes ni la campagne complète du cœur inchangé. Sources C partagées non modifiées ; choix proportionné au seul nouveau banc. Sorties CMake/ctest/build et commandes exactes publiées. Avertissements nosys `_read/_write/_lseek` conservés ; aucune fonction filesystem utilisée. Aucun symbole HAL_SPI/HAL_SD/FRAM/f_mount/f_write trouvé dans l'ELF lié.
- Script PowerShell : syntaxe/oracles CRC hors bus PASS ; exécution `.ps1` ensuite refusée avant ouverture COM. Refus conservé, aucun Bypass/déblocage/changement de politique ni chargement détourné du script. Anciens clients métier inspectés mais non lancés.
- Client natif C# compilé par le csc .NET Framework déjà installé, selftest sans ouverture COM : PASS (CRC, vecteurs connus, UID, nonce, rejet d’écho). Application non signée acceptée par Windows ; SAC=1 et zéro 3033/3077 corrélé dans les fenêtres effectivement relues. Politiques PowerShell inchangées. Ce résultat ne qualifie aucun canal durable de signature.
- Erreur d'observation GDB : membre `tr2_irq_observer.init` inexistant, corrigé vers le vrai `after_uart_enable`. Journaux conservés ; nouvelle lecture seule sans rejeu de flash/reset/trafic. Incident d’encodage d'inventaire Windows et outils PDF absents également tracés dans [INCIDENTS_OUTILS.md](preuves/TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL/INCIDENTS_OUTILS.md).

## Timing et limites

DWT à 160 MHz nominaux : dernier RX → DE haut **1,782–1,799 ms**, DE haut **2,495 ms** pour 26 octets RAW et **0,675 ms** pour 7 octets Modbus, cohérents avec 115200/8E1 et T3.5 = 1750 µs du transport. Le wrapper enregistre l'écriture DE et lit IDR/ODR au MCU ; les deux écritures de boot antérieures à l'initialisation DWT sont exclues de ces calculs. Temps USB/QPC hôte incluent l’ordonnancement et les latences et ne sont pas une mesure directe de la réponse sur paire A/B.

**Preuve fonctionnelle bornée du chemin PG4 → J5 → Click**, par continuité froide, mesure de DE bas, transitions MCU et vraies réponses distinctes. Pas de capture oscilloscope au Click, de certification SI/isolement galvanique, de marge électrique/EMC/endurance/longueur de câble ou de qualification du second shield. Pas de tests FRAM/SD/capteur rejoués. SPI3 J8 reste FAIL, ancienne mission globale BLOCKED et E-01 NO-GO préservés.

## État final et publication

STM32 TR2-001 laissé avec ce seul banc RX : **stage1 / mode1 / error0 / crédits0 / commande0 / arm0**, DE bas, TX inactif/non fautif, timer IDLE, GDB détaché. COM4 fermé, client et GDB server terminés. Aucun bouton ou geste physique supplémentaire requis. Les trois autres supports restent dépeuplés ; aucun module à réinsérer avant mission corrective.

Départ réel main 5045bc78 ; worktree isolé `MSM_vibe-SHIELD-RS485-CLICK-PHYSICAL`, branche `codex/shield-rs485-click-physical`, publication vers main par push normal. Commits logiciels publiés 811a3eda et 73c3d252 ; racine historique 82169af7, autres worktrees et deux stashes 6251b988/f3b829e6 préservés. Pas de merge/rebase/reset/clean/forcepush. Rapport E-01 précédent [archivé byteexact](archives/TR2-20261117-SHIELD-DUAL-BRINGUP/RAPPORT_E01_AVANT_RS485.md), mission humaine inchangée.

Journal `ETAT_COURANT_TR2.md` actualisé avec résultats bornés, configuration, état final et limites. **À transmettre à ChatGPT : contre-lecture indépendante des preuves RX/TX/Modbus ; aucune levée du NO-GO global ou décision PCB déduite.** Publication normale **2eefbb7e0cff1f1c8c72aa3e38dd495b1d6eddf0**, confirmée par `ls-remote` puis récupération fraîche de `origin/main`. **97 fichiers modifiés relus intégralement et identiques aux fichiers publiés**, à 2026-11-18T20:35:08.268322+00:00. Détail dans [publication-first-readback.json](preuves/TR2-20261118-SHIELD-RS485-CLICK-PHYSICAL/publication-first-readback.json). Le manifest des preuves et les liens du rapport ont également été contrôlés. Le présent commit documentaire de clôture sera vérifié à son tour ; son SHA final est communiqué dans la réponse utilisateur pour éviter une référence circulaire. Aucun essai matériel effectué après les observations finales.
