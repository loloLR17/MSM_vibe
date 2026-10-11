---
mission_id: TR2-20261011-M013-I-HP-CAMPAIGN-GO
status: DONE
base_ref: main
base_sha: null
initial_head: 31fd364c56ddb8cd3591198059c3df6e4e9aa3e9
mission_sha: 826bf93f9c4acf68ed7ef5404ea66f542f8e99fb
result_sha: ef9e4e64a4a93a5bffdb81daeee2049fc651dbde
firmware_source_sha: 0bfaa0e0daf3d37f723497b74984cabe1b11ec47
created_at_utc: 2026-10-11T16:55:19.237960+00:00
updated_at_utc: 2026-10-11T16:56:11.452675+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-I — GO technique conditionnel, campagne préparée et non exécutée

**DONE : préparation I satisfaite, rapport et livrables publiés puis relus depuis GitHub.** **Zéro accès COM7, zéro émission RS485, aucun flash/reset/CubeProgrammer HOTPLUG en I.** Aucun GO inconditionnel sur une topologie non observée, aucune qualification physique générale.

## Verdict et commande future exacte

**GO technique conditionnel pour une seule campagne HP, 12 écritures maximum.** Aucun lancement avant contrôle indépendant du présent rapport par ChatGPT et exclusivité du maître HP assurée. Si plusieurs maîtres/répondants sont présents, si l'exclusivité est incertaine ou si l'état cible a été perturbé depuis la baseline : **NO-GO d'exécution**, ne pas lancer. La topologie exclusive n'est pas déduite des accès Codex et aucun autre équipement n'est supposé présent ou absent.

Observation rapportée par l'utilisateur dans le contrat I, pas mesure Codex directe : HP Laptop15-db1xxx, COM7 disponible, Python3.13.3/pySerial3.5, préflightH environment_ready=true, OFFLINE/mock réussis, fichiers hachés conformes, zéro transmission. Le bloc ci-dessous réutilise exactement le dossier fourni ; pas de préflight, téléchargement ou installation répété. SHA256 du script figé `ef9e4e64a4a93a5bffdb81daeee2049fc651dbde` contrôlé avant toute ouverture ; journal `M013-I-physical.jsonl` créé exclusivement par Python, jamais écrasé.

Avant le lancement futur : aucune autre application/maître actif sur COM7/bus ; seule cible diagnostic unité1 et aucun répondant unité2 pour le cas négatif. Aucun recâblage/contrôle J1/J2/J3/RE-RST/VIO/SB28 redemandé. Aucun CubeProgrammer HOTPLUG, reset/flash/coupure ou halt GDB entre baseline saine et campagne ; si intervention connue, état à actualiser par Codex sous autorisation avant lancement. Aucun debugger/halt pendant les échanges. Exécuter ce bloc **une seule fois sur HP après revue GO ChatGPT**, puis communiquer toute sortie et fichierJSONL même en cas d'échec. Ne pas relancer, renommer ou supprimer le journal pour contourner l'arrêt ; ne pas répéter l'ancien probeE.

```powershell
& {
  $ErrorActionPreference = 'Stop'
  $tr2Dir = 'C:\Users\Lolo\AppData\Local\Temp\TR2-M013-H-696f8b55f09242cfaa38ff397de54256'
  $tr2Script = Join-Path $tr2Dir 'rtu_campaign.py'
  $tr2Log = Join-Path $tr2Dir 'M013-I-physical.jsonl'
  if (-not (Test-Path -LiteralPath $tr2Script -PathType Leaf)) { throw 'Script absent : arret sans ouvrir COM7.' }
  if ((Get-FileHash -Algorithm SHA256 -LiteralPath $tr2Script).Hash -ne '5efd7e37769f84adfe1a439225814e7ead481a0fdb48614331d8ffe4b649d957') { throw 'Script modifie : NO-GO.' }
  if (Test-Path -LiteralPath $tr2Log) { throw 'Journal deja present : ne pas relancer, renommer ou supprimer.' }
  $tr2Python = Get-Command py.exe,python.exe -ErrorAction SilentlyContinue | Where-Object { $_.Source -notlike '*\WindowsApps\*' } | Select-Object -First 1
  if (-not $tr2Python) { throw 'Python indisponible : arret sans ouvrir COM7.' }
  $tr2Args = @()
  if ($tr2Python.Name -eq 'py.exe') { $tr2Args += '-3' }
  $tr2Args += @($tr2Script, '--transmit', '--authorized-mission', 'TR2-20261011-M013-I-HP-CAMPAIGN-GO', '--log', $tr2Log)
  & $tr2Python.Source @tr2Args
  $tr2Exit = $LASTEXITCODE
  Write-Output ('Exit=' + $tr2Exit + '; journal=' + $tr2Log + '; ne pas relancer')
  if (Test-Path -LiteralPath $tr2Log) { Get-Content -LiteralPath $tr2Log }
}
```

Bloc analysé syntaxiquement sur Windows T1000, retour0 ; **non exécuté** par Codex. Il résout le même launcher py.exe/python.exe existant que le préflight H, évite l'aliasStore ; un autre environnement/PATH que celui validé laisserait le lancement non couvert. Le marqueur --authorized-mission identifie I, ne remplace pas la revue humaine. L'ouverture exclusive du journal précède l'ouverture série ; erreur de fichier, hash, script, launcher ou journalpréexistant empêche tout envoi. Journal créé puis échec d'ouverture = essai arrêté sans retry.

Campagne fixe : cas1..4 nominaux, cas5 CRCincorrect `010303E80001047B`, cas6..7 nominaux, cas8 unité2/CRCcorrect `020303E800010449`, cas9..12 nominaux. Nominal : requête `010303E80001047A`, réponse exacte7octets `0103020000B844` (UNKNOWN0/CRC0) ; négatifs : silence1s puis nominalsuivant réussi. COM7/1152008E1, timeoutlecture0/écriture1s, handshake/DTRRTSdésactivés. Fenêtre1s+pause100ms parcas,100msinitiaux ; barrière20s avant nouvelle écriture, RXmax256. Tout écho/écho+réponse/bruit/CRCpartiel/extra/exception/timeoutnominal/RXtardif/overflow/écriturepartielle/I/O/logdéfaillant→arrêt/fermeture sans retry ni purge ; aucun écho supprimé. Journaux UTC/tempsmonotone/chunksbruts/intent/nbécrits/verdict, simulated=false pour branchephysique. Retour0 seulement campagne complète ; retournon0/traceback toujours arrêter et préserver preuves.

Limites : première branche COM7 physique de cet outil encore non testée ; le préflight et les doubles de Serial ne qualifient pas le pilote ni un transitoireDTRRTS/écho à l'ouverture. 20s est une barrière logicielle avant écriture, pas garantieOS. Course RXcheck→write et octets après fermeture restent nonexclus ; aucun succès temporel général déduit. Acquisition DE/TC/dernierstopbit/retourRX WAITING instrument identifié. La campagne ne constitue pas qualification temporelle, endurance, production, J1global ou E4.

## Références, préservation et modifications

Main initial propre31fd364, fetch/pull --ff-only sûr vers826bf93, seul contrat changé. AGENTS.md réel relu intégralement (incluant décisions permanentes et architectureT1000), protocole/journal intégral/rapportH DONE/planG/script/tests/procédureGDB qualifiée inspectés. Contrat ChatGPT inchangé ; base_sha absent donc null. Deux stashes historiques005/002 conservés ; archive H mission/rapport depuis HEAD initial byte-à-byte et immuable. Preuves E181/F22/G12/H34 fichiers manifestées et conservées horsGit. Aucune source firmware/outil/test modifiée : mêmes SHA de script et huit sources firmwareE, mêmes tests historiques. Ne pas refaire préflightHP réussi.

Livrables documentaires : GO_CAMPAGNE_HP_M013_I.md (commande/cas/arrêts/deltas), DERNIER_RAPPORT.md, journal actualisé et verdictI précédant sections historiques du preflightTX, deux archivesH. PlanG/préflightH restent documents historiques de leurs dates, leurs anciens NO-GO ne remplacent pas la décisionI fondée sur le retour utilisateur nouveau. Firmware sur cible E, référence0bfaa0e/code09792c8 ; script ef9e4e6 avec --offline explicite ajoutéH, SHA2565efd7e37769f84adfe1a439225814e7ead481a0fdb48614331d8ffe4b649d957. Aucun refactor/correctif horspérimètre/V1/configuration/média.

## Observations GDB I, image exacte et baseline

Trois sessions qualifiées : ST-LINK SN003500463235510B37333439 / NUCLEO-U575ZI-Q, serveurST Windows -d -g -iSN --frequency8000 -p61234 -cpC:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin ; GDB -nx -q -batch -x script avec ELF E exact. target extended-remote localhost:61234, lectures/dumps, detach/quit ; GDB/serveurs retours0. -cp ne lance pas CLI HOTPLUG. Attach suspend brièvement pour lecture puis detach, pas d'attentehalt/traffic ; aucune écriture NVIC/GPIO/RDR/ICR, breakpoint/load/monitorreset. Cible laissée running, mêmes IRQ après sessions. Flash lue0x08000000..0x0800C03049200octets exactement BIN E, SHA256 `dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540`. ELF168512 SHA256 `4c496149b7b3af593269dbff7c92368d575cebe824f62ffa67ed57e522d3adc2`. Huit sources E inchangées, nm/taillesobserver992/g_serial692 confirmés ; layoutsE inchangés repris sans rebuild.

| Contrôle | Observation I |
|---|---|
| Ready et progression | ready1/poll_result0/pollerrors0 ; tick2901075→2910233 ; poll989199439→992322297 |
| NVIC CPU/SWD | ISER1=0x20000 (TIM6IRQ49), ISER2=4 (LPUART1IRQ66), ISER0/ISPR0..2/masks0 |
| UART et RX | ISR0x006200D0, RXNE/ORE/PE/FE/NE0 ; CR1=152D/CR2=0/CR3=1/BRR56CE4 ; RxState0x22BUSY_RX/sizecount1/ErrorCode0 |
| Transport | PG4ODR0, phaseIDLE/tx_busy/fault/bus_silent0 ; head=tail10/overflow0, CFSR/HFSR0 |
| Baseline stableF/H | UARTIRQ17/TIM6IRQcallback3 ; RXcallback/octet8 ; eventspushedpolled10/T1.5-T3.5=1 ; frameCRCvalidacceptedavailable1 ; TXattemptstartedTC1 |
| Défauts | CRCbad/invalid/foreign/broadcast/errors/drop/failsafe0 ; dernièresADU8/TX7 exactes requête/réponseE |

RAM768KiB SHA256 `aa8fdc4d4e266675772f6f4d3fff697224108ae1b03699466987d2b57dc934ce`, baseline-before992octets SHA256 `c307e763066d6ce63246faf457d3e3534809a837dc38ca0c08e453c04da942b9`, baseline-final992octets SHA256 `fadf0042a2a6946586815a6b9c01c192499cf8ed764bf4349784d0fc7a034feb`. Compteurs non remis à zéro, aucune nouvelle requête démontrée : état ponctuel sain, pas capturecontinue. Aucun besoin de reset/reflash, permissions non exercées.

## Validation réellement faite et proportion

LecturesGDB/snapshots/imagebyte-à-byte/ELF et sourcehashes/structlayout/cohérencecompteurs effectuésI, logs bruts conservés. Bloc PowerShell futur analysé `[System.Management.Automation.Language.Parser]::ParseInput` sous WindowsT1000 → retour0/syntaxOK, nonexécuté. Comparaison du bloc publié à l'entrée analysée et hash outil. TestsH13/13 et mockG/horsportHP réussi rapporté danscontrat sont **preuves conservées**, pas nouvelles campagnesI. Documentation seule/sourceinchangée : pas de testsfirmware/cross-build/.NET ni testsPython/préflight répétés sans nécessité. Aucun échec de test masqué/affaibli, aucun logiciel ou politique installé.

PreuvesI : `/home/lolo/dev/msm/reports/TR2-mission013-I-20261011T165146Z` ; scripts/logs GDB et serveurs, Flash/RAM, baseline-before/final.bin/.json, decoded-transport.json/symbols.txt, preserved-E/F/G/H.json/tool-manifest.json, hp-reported-preflight.json (provenancecontrat/observationutilisateur, pas captureHP), hp-campaign-command.txt/powershell-syntax.log. Aucun log de campagnephysiqueI fabriqué, preuvesvolumineuses/backups horsGit.

## Suite unique et portée

Après revueGO indépendante ChatGPT et conditions exclusives réunies, l'utilisateur exécutera une seule fois la commande ci-dessus, renverra sortie etJSONL, sansretry. Cette instruction futureI ne signifie pas que Codex a ouvertCOM7 enI. Aucun autrelancement/ancienprobeE. RelevéAPRÈS sous mission appropriée parGDBseul avanttoutreset/flash ; comparer préfixe réel si arrêt. Douze cas complets : deltaRX96/frame12/CRCvalid11/CRCbad1/foreign1/acceptedavailable10/TXattemptstartedTC10, erreurs/drop/failsafe0/RXarmé/TXidle ; absolus sur bootcourant104/13/12/1/1/11/11 respectivement. Deltasmodulo32bits/rebootincertain/dernièretrame seule limites documentées ; IRQsansnombreimposé.

Production/J1global/E4/DEdernierstopbitTCretourRX temporels/robustesseendurancewatchdogIRQ/resetpréinit **NON QUALIFIÉS**. Seul aller-retourE historique corroboréF, ne pas déclarer communicationbidirectionnellegénérale validée. Aucun instrumentphysique nouveau identifié ; aucun contrôlephysiqueacquis redemandé.

## Journal, autorisations et publication

Journal actualisé : préflightHP réussi rapporté utilisateur levant blocageH logiciel, COM7disponible rapporté, nouveaux états/baselineI et GOconditionnel/commandeunique/limites. AutorisationsI : commit/push normalmain et debuglecture/haltattach/detach exercés ; flashtrue nonexercé, zéroreset/masserase/OptionBytes/protection/USB/configuration/média/physique/COM7/RS485. Diff/index, périmètre sans secrets, branche main/destination origin et état distant contrôlés ; commit `c7074e7d04b399968633951f8a68335a6d13261c`, message `Prepare M013-I conditional GO with healthy MCU baseline and single HP campaign command`, push normal explicite HEAD:refs/heads/main réussi. Fetch HEAD=origin/main ; relecture intégrale UTF8 et comparaison byte-à-byte des **9 fichiers** (six livrables dont archivesH, contratI, planG et scriptcampagne) depuis cette référence distante réussies, remote-reread.json horsGit. Arbre propre après première publication, deux stashes conservés. La présente clôture DONE sera publiée normalement puis relue ; SHA final dans réponse Codex. DONE qualifie préparationGOconditionnelI, jamais campagne effectuée ou revue indépendante ChatGPT présumée.
