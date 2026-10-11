---
mission_id: TR2-20261011-M013-H-HP-CAMPAIGN-PREFLIGHT
status: BLOCKED
base_ref: main
base_sha: null
initial_head: fe96dcac9e8610aa5e444da859d1c24d0622e8bc
mission_sha: 562972fa2c735470747975eb56dd58f6747dd57f
result_sha: ef9e4e64a4a93a5bffdb81daeee2049fc651dbde
created_at_utc: 2026-10-11T16:43:50.808937+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-H — NO-GO campagne HP, état STM32 sain et préflight hors port prêt

**NO-GO pour la campagne physique : environnement HP non confirmé.** Préparation H réalisée, verdict documenté ; statut provisoire BLOCKED uniquement en attente de publication/relecture distante. Le contrat accepte un NO-GO documenté lorsque HP inaccessible : sa clôture DONE concernera cette préparation, jamais une autorisation de transmission ou qualification physique. Aucun accès COM7, aucune émission RS485, aucun flash/reset/HOTPLUG en H.

## Références et reprise

Main initial propre fe96dcac, deux stashes historiques005/002 conservés. Fetch et pull --ff-only sûr vers562972fa (seul contrat changé), local/distant vérifiés. Mission ChatGPT figée, base_sha/created_at absents du contrat : référence réelle indiquée, base_sha null sans inventer. AGENTS.md/protocole/journal intégral, rapports E/F/G, plan G, outil/tests et preuves existantes relus. Archive G créée depuis HEAD initial byte-à-byte et immuable. Aucun travail ancien refait, V1/firmware/production/médias/paramètres système inchangés.

Code outillage effectivement testé : `ef9e4e64a4a93a5bffdb81daeee2049fc651dbde`, commit `Add nonintrusive HP environment preflight and explicit offline campaign mode`. Référence source de l'image E sur cible `0bfaa0e0daf3d37f723497b74984cabe1b11ec47` (firmware inchangé depuis09792c8) ; les huit fichiers du source-manifest E correspondent encore à main. Les preuves historiques E181 fichiers et F22 fichiers rehashées et comparées aux manifestes G inchangées ; G12 fichiers conservés. Backups et stashes non supprimés/écrasés.

## Accès réel HP et verdict

L'hôte Windows effectivement accessible est **Dell Inc. Latitude7320 / T1000**, obtenu par PowerShell `Get-CimInstance Win32_ComputerSystem`. `Get-Command py,python -ErrorAction SilentlyContinue` n'y renvoie aucune commande (commande globale exit1 sur cette absence) ; cela n'exclut pas un Python installé ailleurs. Linux Debian13/Python3.13.5 disponible. Aucun répertoire SSH local/configuration d'accès HP établi, aucun outil terminal HP disponible, pas de nouveau canal distant créé ni scan réseau. **HP réel inaccessible**, ses versionsPython/pySerial/compatibilitépiloteCOM7 restent NON VÉRIFIÉES. Le T1000 ne remplace pas le HP pour conclure.

Livrable `00_gouvernance/PREFLIGHT_CAMPAGNE_HP_M013_H.md` : **une seule action ultérieure hors port**, bloc PowerShell à coller sur HP après revue ChatGPT, recherche Python existant sans aliasStore, deux sources téléchargées à SHAfigé et hachées dans nouveau dossier, inventaire WMI machine/COM7 sans ouverture, préflightJSON enregistré et affiché. Aucune installation, aucun changement de politique PowerShell, aucune commande physique. Si erreur : communiquer sortie et conserver NO-GO. Commande complète analysée syntaxiquement sous Windows T1000, **non exécutée sur HP** ; dépendances/réseau/driver non présumés disponibles.

`preflight_hp.py` contrôle Windows/Python>=3.9/pySerial3.5 et API Serial sans construire de port, imprime versions/chemins/hashes, teste CLI --offline et --mock avec fonction d'ouverture interdite. `environment_ready=true` ne prouve que compatibilité logicielle du poste d'exécution ; **campaign_go=false toujours**, identité HP/COM7 et topologie exclusive doivent encore être contrôlées puis revueGO indépendante et mission distincte. L'import standard pySerial n'est ni ouverture ni réservation COM7 ; fonctionnement série/DTRRTS/écho non validés. Un COM7 absent dans inventaire laisse NO-GO. Aucun accès utilisateur demandé pendant l'exécution autonome H.

## Revue de l'engine et modifications minimales

12cas inchangés : nominal4, mauvaisCRC1, nominal2, unité2/CRCcorrect1, nominal4. Dix réponses exactes0103020000B844 et deux silences, nominales après chaque négatif pour récupération. Pas de broadcast/écritureV1/retry/purge. Fenêtre1s/pause100ms/gardeinitiale100ms, avant nouvelle écriture refus si20s atteintes ; RXlimite256/timeoutwrite1s, journal exclusif/flush avant intent. Écho brut/écho+réponse/extra/partiel/CRCincorrect/exception/timeoutnominal/erreurI/O/log→stop et fermeture, sans extraction opportuniste. RX tardif/préexistant conserve brut et stop avant envoi suivant, dernièrepause contrôlée. Limites : race check→write/latencesOS, octets après fermeture invisibles ; aucune exclusion absolue de réponse infiniment tardive et20s n'est pas garantie tempsréel. Compteurs finaux et journal sur préfixe réel, jamais extrapolation après arrêt.

Lacune avérée : CLI G ne reconnaissait pas l'option **--offline** citée au préflight H (horsligne implicite seulement). Ajout alias explicite mutuellement exclusif aux modes actifs, comportement défaut inchangé. Nouveau préflight et trois tests couvrant environnement Windows simulé avec Serial interdit, paquet/versionerronés et dépendance/postemanquants ; aucun correctif fonctionnel injustifié au moteur. Script historique E inchangé, ancienne instruction E consommée.

## Santé STM32 effectivement réobservée via GDB seul

Sonde qualifiée SN003500463235510B37333439 / NUCLEO-U575ZI-Q, chaîne ST Windows existante. Trois sessions GDB -nx -q -batch -x script avec ELF E exact, serveur -d -g -iSN --frequency8000 -p61234 -cpC:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin. Le chemin -cp requis du serveur n'est pas une invocation CLI HOTPLUG. Scripts : target extended-remote localhost:61234, lectures/dumps seulement, detach/quit ; toutes sessions et serveurs retours0. Aucun monitorreset/load/breakpoint/watchpoint, aucune écritureRDR/ICR/NVIC/GPIO. Attach suspend brièvement pour collecte puis detach ; aucun halt d'attente ou pendant trafic, aucun trafic nouveau. Cible laissée en exécution.

Flash lue0x08000000..0x0800C030, **49200 octets byte-à-byte identiques BIN E**, SHA256 `dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540` ; ELF168512 SHA256 `4c496149b7b3af593269dbff7c92368d575cebe824f62ffa67ed57e522d3adc2`. RAM liée768KiB conservée SHA256 `9a4f5b1535b6d548dfea2546fe91167c71cdae8d902c629ec7b51981d160ff2e`. Observer992octets/g_serial692octets confirmés par nm ELF ; layouts E relus et sources hachées inchangées avant décodage.

| Contrôle | Observation H |
|---|---|
| Ready/progression | ready1/poll_result0/pollerrors0 ; tick2121087→2130079, poll723241806→726307644 |
| NVIC CPU/SWD | ISER1=0x20000 IRQ49 ; ISER2=4 IRQ66 ; ISER0/ISPR0..2/PRIMASK/BASEPRI/FAULTMASK0 |
| UART/RX | ISR0x006200D0 (RXNE/ORE/PE/FE/NE0), CR1=152D/CR2=0/CR3=1/BRR56CE4 ; RxState0x22BUSY_RX/size-count1/ErrorCode0 |
| Direction/transport | PG4ODR0, phaseIDLE, tx_busy/fault/bus_silent0 ; eventhead=tail10/overflow0 ; CFSR/HFSR0 |
| Baseline trafic | Stable depuis F : UARTIRQ17/TIM6IRQcallback3, RXcallback/octet8, eventspushed-polled10, T1.5/T3.5=1, frame/CRCvalid/accepted/available1 ; TXattempt/started/TCcallback1 |
| Anomalies | CRCbad/invalid/foreign/broadcast/errors/drop/failsafe0 ; dernièreADU8/dernièreTX7 identiques essai E |

Les valeurs CPU et SWD concordent, lectures seules préservent IRQ. Aucun besoin technique de reset/flash : non exercés malgré permissions. Les compteurs stables ne sont pas une capture continue du bus ; image/counters ne qualifient pas chronologieDE/dernierstopbit/TC/retourRX. Baseline avant campagne **conservée**, mais son état doit être actualisé sous future mission avantGO si délai/intervention entretemps ; jamais halt pendant échange.

## Validation proportionnée et preuves

Commandes réellement exécutées depuis dossier outil : `python3 -m unittest -v test_rtu_campaign.py test_preflight_hp.py` → **13/13, retour0**, 0,014s ; `python3 preflight_hp.py` → **retour1 attendu** sur Linux (Windows absent et No module named serial), OFFLINE/mock réussis10nominales/2silences, aucune ouverture. Analyse `[System.Management.Automation.Language.Parser]::ParseInput` du bloc futur HP sur Windows → retour0/syntaxOK. Ces simulations ne sont pas une exécution Windows HP. Aucun test affaibli/désactivé, aucune dépendance/politique installée ou contournée. Validation choisie : outil hôte seulement ; pas de firmware/cœur/interface changé, donc aucun rebuild/cross-build/testsfirmware/.NET répété qui n'apporterait preuve du préflight. Résultats E/G historiques conservés, non annoncés comme relancés H.

Preuves horsGit : `/home/lolo/dev/msm/reports/TR2-mission013-H-20261011T163855Z`. `baseline-before/final.gdb/.log/.bin/.json`, troislogsserver, `image-ram.gdb/.log`, Flash/RAMbrutes, `decoded-transport.json`, `symbols.txt`, `tool-manifest.json`, `tests.log`, `preflight-linux.log`, `checks.json`, `hp-preflight-command.txt`, `preserved-E/F/G.json`. Manifestes/hashes publics essentiels ci-dessus, logs/binaries/backups volumineux hors dépôt. Aucune preuve HP exécutée fabriquée. Défaut NVIC CubeProgrammer D conservé comme cause reproduite historique, aucune nouvelle reproduction perturbante H.

## Limites et prochaines actions

**NO-GO campagne** tant que HP réel Python/pySerial et inventaireCOM7 non confirmés. Après contrôle ChatGPT du présent rapport, une seule action minimale = bloc de préflight HP hors port du livrable ; transmettre sortie, sans --transmit. Une réponse positive devra être revue avec exclusivitémaîtreHP/cibleunité1/absenceautrerépondantunité2 ; nouvelle mission/GO explicite et baselineactualisée avant **une seule campagne maximum12écritures**, journalnouveau/arrêtimmédiat/pasretry. Aucune commande physique immédiate livrée comme autorisée.

Mesures temporelles toujours WAITING instrument identifié, pas de nouvel essai ou manipulation. AcquisJ1/J2/J3/RE-RST/VIO/SB28/câblage non redemandés. Répétabilité/robustesse/endurance/watchdogIRQ/resetpréinit/J1global/production/E4 NON QUALIFIÉS. Aller-retour E corroboré F reste unique, communication Modbus bidirectionnelle générale non validée.

## Journal, permissions et publication

Journal actualisé : état ponctuel MCU sain/imageexacte, préflight préparé/testé, HP non accessible et NO-GO campagne, limites et prochaine revue. Preflight historique précédé du verdict H, archive G exacte ; contrat ChatGPT/AGENTS/V1 intacts. Permissions exercées : commit/push normal main et debuglecture/haltattach/detach ; allow_flashtrue non exercé, aucun reset/OptionBytes/média/configuration/USB/physique/COM7/émission. Diff/index et branche/destination seront vérifiés avant push explicite HEAD:refs/heads/main ; pasforce/merge/rebase. Publication/relecture distante encore à obtenir pour ce rapport provisoire, deux stashes conservés.
