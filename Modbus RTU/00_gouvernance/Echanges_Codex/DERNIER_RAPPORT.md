---
mission_id: TR2-20261011-M013-G-RTU-TIMING-ROBUSTNESS-PLAN
status: BLOCKED
base_ref: main
base_sha: null
initial_head: 0cccb89c19c9060e185c2d369dfe55ec78ddb4b9
mission_sha: 15dcb82f492c121746da4ca3cd34f83b93ed89c1
result_sha: 158feddfa99dd5fff71aec4902cd59e3095124f0
created_at_utc: 2026-10-11T16:30:53.691497+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-G — préparation de qualification temporelle et robustesse limitée

**READY outillage logiciel HP hors bus ; WAITING instrumentation physique identifiée. Aucun GO de transmission en G.** Critères de préparation G satisfaits ; BLOCKED provisoire uniquement pour publication/relecture distante. Aucune mesure temporelle ou campagne physique réalisée, aucune qualification générale revendiquée.

## Référence, reprise et état conservé

Main initial propre0cccb89, deux stashes005/002 conservés. Fetch puis pull --ff-only vers15dcb82, seul contrat changé. AGENTS.md/protocole/journal/rapport F DONE/archives D-E/procédure D et sources main/diagnostic/transport/HAL/script historique inspectés. Archives F créées depuis HEAD initial, exactes et immuables ; F était le rapport actif, pas une archive déjà existante. Contrat ChatGPT inchangé, base_sha absent donc null.

F a corroboré l'essai unique E : réponseHP0103020000B844, ADUrequête exacte8octets, CRCvalid/accepted1, réponseRAM7octets/TXstarted/callbackTC1, sans erreur. Ces preuves restent historiques ; aucun rejeu ni nouvelle mesure G. Manifeste des fichiers E et F réalisé et rehashé, fichiers/backups originaux inchangés ; SHA des artefacts et observations dans les rapports E/F archivés. Preuves G hors Git : `/home/lolo/dev/msm/reports/TR2-mission013-G-20261011T162139Z`, preserved-E.json/preserved-F.json et tool-manifest.json, logs de tests et JSONLmock.

Image existante E diagnosticON/observerON/TXON/unité1, référence source0bfaa0e0daf3d37f723497b74984cabe1b11ec47/code09792c8. BIN49200 SHA256 dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540 ; ELF4c496149b7b3af593269dbff7c92368d575cebe824f62ffa67ed57e522d3adc2. Identité Flash/MCU/SN établie F via GDB seul, pas nouvelle lecture en G. Dernier état F : RXarmé/TXidle/PG4bas/NVICsain/MCUrunning ; état instantané G NON RÉOBSERVÉ, à vérifier sous futur contrat avant campagne. Aucun motif de reset/reflash/debug pour un plan et outil hors bus.

## Livrables et choix technique

Plan `00_gouvernance/QUALIFICATION_TEMPORELLE_RTU_M013_G.md` : inventaire factuel, voies et masses/isolement, ordre des acquisitions, définition d'incertitude, critères temporels mesurables, outillage/commandes exacts, maximum d'essais et arrêts, deltas attendus et limites. La recherche documentaire ne trouve aucun instrument de mesure identifié : **disponibilité NON ÉTABLIE**, pas preuve d'absence. Capture DE/TXD/RXD/A–B reste WAITING modèle/sondes/canaux/isolement ; aucun branchement proposé comme immédiatement exécutable ni manipulation demandée G.

Lecture du transport : T1.5=750µs +1000µs pour T3.5, TX après consommationT3.5/filevide/conditionsRX, PG4haut puis HALTransmit_IT, watchdog50ms ; HALcallbackTC libèreDE/réarmeRX et lance garde1750µs. Délais de code et compteurs ne sont pas chronogrammes physiques. Observateur existant suffit à corréler la future campagne ; ses absences de timestamps/frontTC haute résolution sont documentées. Instrumentationfirmware supplémentaire non indispensable aux captures physiques proposées, donc **aucune modification firmware**. FC03/1000/1 UNKNOWN0 et V1 inchangés, aucune production simulée.

Les seuils normatifs haut débit sont distingués des critères de banc proposés : [spécification Modbus Organization V1.02 §2.5.1.1, copie ANL](https://millenia.cars.aps.anl.gov/software/epics/Modbus_over_serial_line_V1_02.pdf), section lue. Calcul 1152008E1 : bit8,6806µs/caractère95,4861µs ; aucune valeur mesurée. Plan : intertrame>=1750µs, continuité sans silence>=750µs, margeDEavantTX proposée1µs, maintien jusqu'à dernierstopbit, libérationproposée[0,100µs], retourRX et tolérancebaudbanc±2%, budgetincertitude<=0,5µs pour marges courtes. Critères de banc à confirmer par mission/instrumentréel, pas modificationV1 ou qualification actuelle. HorodatageHP via USB/OS ne valide aucun de ces fronts.

Outil dédié `05_Firmware/platform/host/rs485_rx_probe/rtu_campaign.py` et `test_rtu_campaign.py`, Python>=3.9 : OFFLINE par défaut, mock entièrement mémoire et horloge simulée, pySerial importé seulement pour mode physique explicite Windows. Script E Send-Tr2RxProbe.ps1 byte-à-byte inchangé. Dépendance proposée pySerial3.5, API vérifiée depuis [documentation officielle](https://pyserial.readthedocs.io/en/latest/pyserial_api.html) ; Python/pySerial sur HP et voie physique de cet outil NON TESTÉS. READY signifie outil hors bus testé, pas environnementHP confirmé ou autorisation d'envoi.

Campagne fixe12écritures maximum/10nominales/1CRCincorrect/1unitéétrangère2, suivies de nominales pour récupération. Observation1s +pause100ms par cas, garde100ms initiale, refus nouvelle écriture si20s dépassées, RXmax256octets, écrituretimeout1s. Aucun retry, fonctionV1d'écriture/broadcast ou boucle infinie. PortCOM7 fixé1152008E1, handshake/DTR/RTS désactivés avant ouverture ; l'éventuel transitoire de driver à l'ouverture reste limite non qualifiée. Condition future : seul maîtreHP et seule cibleunité1, aucun autre répondantunité2.

JSONL brut UTC+tempsmonotone, chunksRX/réponses/CRC/timeout, intentécriture et nombre réellement écrit, premierRXhôte, marqueurmission/simulated. Créationexclusive sans écraser logs, flush avant écriture. Écho brut/écho+réponse jamais retiré ; bruit, réponsepartielle/inattendue, timeoutnominal, RXtardif/préexistant, overflow, écriturepartielle, I/O/logdéfaillant→arrêt/fermeture, sans retry ni purge silencieuse. Silence attendu négatif ne prouve que la fenêtre1s ; captureanalogique/retourRXminimal/watchdogpanne/endurance restent séparés.

## Validation réellement exécutée et justification

Depuis dossier rs485_rx_probe :

```text
python3 -m unittest -v test_rtu_campaign.py
python3 rtu_campaign.py
python3 rtu_campaign.py --mock --log <dossier-preuves>/mock-campaign.jsonl
```

Retours0 ; **10/10 tests réussis** (0,007s) sur Python3.13 Linux. Tests : CRCgolden/planborné, accumulationchunks/récupération aprèsrejets, timeout/écho/écho+réponse/CRCpartiel/exception/overflow→stop, réponseauxnégatifs→stop, RXpréexistant/tardif, écriturepartielle, écheclogfermeture, defaultOFFLINE/mockavecopenréelinterdit, refus d'écraserjournal, paramètres du serialadapter via bibliothèque simulée. Aucun pySerial installé/importé réellement ni port ouvert. Exécutionmockintégrale : **78 événements JSONL**, simulatedtrue/MOCK_ONLY,12écritures virtuelles, dixnominales exactes et deuxsilences attendus, fermetureportmock ; contenu journal contrôlé. validation-0/1/2.log conservés hors Git.

Choix proportionné : nouveau outillage hôte autonome et plan documentaire seulement, ni cœurportable ni harnessSTM32 ni interfaces partagées changés. Aucune compilation/testfirmware/cross-build/flash répété ne pourrait établir les mesures recherchées ; pas de campagne .NET. Les résultats E/F sont conservés, sans les présenter comme nouveaux tests G. Erreur documentaire/outillage : URLmodbus.org/docs renvoie404, sourceprimaireauteur conservée via miroirANL et sectionweb lue ; pdftotext absent, aucun logiciel/politique installé pour contourner, lectureweb utilisée. Aucun échec de test masqué ou affaibli.

Commit tranche outillage/plan `158feddfa99dd5fff71aec4902cd59e3095124f0`, message `Prepare bounded offline-tested HP RTU campaign and physical timing qualification plan`. Sources testées correspondant à ce SHA, manifeste hors Git. Firmware sur carte inchangé.

## Actions futures minimales et limites

Après revue de ce rapport par ChatGPT : contrôler hors port environnementPython/pySerialHP ; identifier seulement les moyens réellement disponibles et leurs masses/isolement, décider mission future limitée logicielHP et/ou chronogrammes. Pas de répétitionRE-RST/VIO/SB28/câblage, pas de branchement ni lancement maintenant. Commandes OFFLINE/mock et commandephysique réservée à contratfutur sont dans le plan ; flag authorized-mission ne constitue aucune autorisation autonome.

Prérequisfutur : étatNVIC/ready/RX/PG4 sain et imageexacte, baselineRAM avant campagne, GDB seul avant/après sans haltpendantéchange. Dernier accèsCubeProgrammer éventuel devra être suivi de préservationRAM/resetGDB/contrôle comme D, sous autorisationsdistinctes ; aucune utilisation actuelle. Campagnecomplète attenddelta96octets/12trames/11CRCvalides/1CRCincorrect/1foreign/10accepted/TXstarted/TCcallbacks, sanserreur/drop/failsafe, puisRXarmé/TXidle. Campagne interrompue se juge surpréfixe réellement envoyé, pas sur12théoriques. Compteursmodulo32bits/reboot et dernièrestrames seuls ont des limites.

**NON VALIDÉ** : timingsphysiques/robustesse réelle/répétabilité générale/endurance/IRQbloquées/reset/J1global/production/E4. Ce plan et ses mocks ne qualifient pas RS485. Aucun nouvel essaiHP, requête synthétique surbus, reset/flash/debug/CLI MCU, média/OptionBytes/configuration/USB/interventionphysique effectués en G. allow_flash/allow_debugtrue accordés mais **non exercés**.

## Publication

Journal mis à jour avec outil READY horsbus, instrumentationWAITING, absence de nouvelle qualification et prochaines décisions ; preflight marque Gpréparation sansnouvelleémission. ArchivesF exactes. Commit/pushmain autorisés et utilisés seulement dans périmètre G ; diff/index/publication/relecture finale en attente. Deux stashes et tous backups préservés. BLOCKED provisoire porte uniquement sur preuve distante ; attente instruments n'empêche pas clôture de cette mission de préparation. RevueindépendanteChatGPT non démontrée par Codex.
