---
mission_id: TR2-20261011-M013-J-POST-CAMPAIGN-AUDIT
status: DONE
base_ref: main
base_sha: null
initial_head: cfc8e271db5fe65c887834749f6447f621db2dae
mission_sha: bc20ef26d7d31aaa883cc537f0ab8dbc50f0996e
result_sha: 0bfaa0e0daf3d37f723497b74984cabe1b11ec47
tool_sha: ef9e4e64a4a93a5bffdb81daeee2049fc651dbde
created_at_utc: 2026-10-11T17:08:53.706743+00:00
updated_at_utc: 2026-10-11T17:10:02.383538+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-J — campagne HP corroborée par les deltas STM32

**Concordance complète avec les deltas attendus ; clôture fonctionnelle limitée de M013 proposée, avec réserves documentaires et temporelles.** **DONE : audit J réalisé, rapport et livrables publiés puis relus depuis GitHub.** Aucun nouvel essai HP, accès COM7, émission RS485, reset, flash ou CubeProgrammer HOTPLUG. La commande I est consommée, ne pas la relancer.

## Référence et provenance

Main initial propre `cfc8e271db5fe65c887834749f6447f621db2dae`, deux stashes005/002 conservés. Fetch puis pull --ff-only vers `bc20ef26d7d31aaa883cc537f0ab8dbc50f0996e`, seul contrat changé. AGENTS.md, protocole, journal intégral, rapport I DONE, baseline I, scripts de lecture et firmware réel consultés. Mission ChatGPT inchangée ; base_sha absent donc null. Archives I exactes depuis HEAD initial, sans réécriture historique. Image E évaluée, source `0bfaa0e0daf3d37f723497b74984cabe1b11ec47`, code firmware inchangé depuis09792c8 ; huit sources comparées au manifeste E. Outil HP figéef9e4e6 inchangé.

**Observation utilisateur transcrite dans le contrat J** : HP seul maître, une campagne physique COM7, 12 écritures de8octets, dix réponses exactes `01 03 02 00 00 B8 44`, mauvaisCRC et unité2 sans réponse, complete/passedtrue, durée13,3657s. SHA256 annoncé `23f463fb241c7378c46a79af0a43bce122511799cd1066836a787687c629bed4`. Le journal est joint à la conversation ChatGPT, **non accessible directement à Codex ici** : ni recalcul du hash ni analyse indépendante des chunks/horodatages/chronologie exacte du JSONL ne sont prétendus. Recherche ciblée dans rapports locaux et TempT1000 sans fichier correspondant ; un ancien dossier temporaire Windows sans rapport est inaccessible (permission), recherche non exhaustive sur ce dossier, aucun contournement. Ne pas recréer un faux journal à partir du résumé et ne pas demander un nouvel essai.

## Identité et collecte GDB seule

Chaîne Windows qualifiée, ST-LINK SN003500463235510B37333439/NUCLEO-U575ZI-Q. Trois sessions : observateur/état APRÈS, RAM+Flash exactes, dernier observateur/état après detach pour progression. Serveur `ST-LINK_gdbserver.exe -d -g -i <SN> --frequency 8000 -p 61234 -cp <répertoire CubeProgrammer/bin qualifié>` ; `arm-none-eabi-gdb.exe -nx -q -batch -x <script> <ELF E exact>`, target extended-remote localhost:61234, lectures/dumps puis detach/quit. Le répertoire -cp n'est pas un appel CLI HOTPLUG. Tous GDB et serveurs retours0 ; MCU laissé en exécution. Attach suspend brièvement pendant collecte après campagne terminée, sans attente halt ni trafic ; aucun monitorreset/load, écriture NVIC/GPIO/RDR/ICR ou changement permanent.

Flash0x08000000..0x0800C030, **49200 octets byte-à-byte identiques BIN E**, SHA256 `dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540`. ELF168512 SHA256 `4c496149b7b3af593269dbff7c92368d575cebe824f62ffa67ed57e522d3adc2`. DiagnosticON/observerON/TXON/unité1, FC03/1000/1 UNKNOWN0 ; pas production/médias bootés. Adresses/tailles observer992octets et g_serial692octets recoupées par nm ; layouts E conservés et sources inchangées pour décoder RAM, pas de rebuild.

Baseline I finale992octets SHA256 `fadf0042a2a6946586815a6b9c01c192499cf8ed764bf4349784d0fc7a034feb` préservée. Observer J initial SHA256 `e760ed13916d58c9647c0dc9108d4c9b3fce4f444fbf789c3170c28a3cebc059`, final SHA256 `1c7644405cf9c0e10d016881bb2916b84c72a39e845036946fcfe4c0fdfba146`. RAM768KiB SHA256 `b848fe920e4fe5501c53a3fa5e12e93d6d4969c0a6c627b6dc515ef84d30ac24`. Ces snapshots contiennent des preuves STM32 réelles, distinctes du journal PC annoncé.

## Comparaison baseline I → APRÈS J

Calcul des deltas modulo2^32, même image et progression cohérente. Aucun reset depuis I effectué par Codex ; pas de compteur de boot persistant pour exclure tout reset externe inconnu, aucun indice de celui-ci. Toutes valeurs de trafic stables entre les deux observations J.

| Compteur | I avant | J après | Delta observé | Attendu | Verdict |
|---|---:|---:|---:|---:|---|
| rx_callback | 8 | 104 | 96 | 96 | OK |
| bytes_polled | 8 | 104 | 96 | 96 | OK |
| frame_count | 1 | 13 | 12 | 12 | OK |
| crc_valid | 1 | 12 | 11 | 11 | OK |
| crc_bad | 0 | 1 | 1 | 1 | OK |
| foreign_unit | 0 | 1 | 1 | 1 | OK |
| accepted_request | 1 | 11 | 10 | 10 | OK |
| available_status | 1 | 11 | 10 | 10 | OK |
| tx_attempt | 1 | 11 | 10 | 10 | OK |
| tx_started | 1 | 11 | 10 | 10 | OK |
| tx_callback | 1 | 11 | 10 | 10 | OK |
| error_callback | 0 | 0 | 0 | 0 | OK |
| events_dropped | 0 | 0 | 0 | 0 | OK |
| errors_polled | 0 | 0 | 0 | 0 | OK |
| frame_invalid | 0 | 0 | 0 | 0 | OK |
| broadcast | 0 | 0 | 0 | 0 | OK |
| tx_fail_safe | 0 | 0 | 0 | 0 | OK |

Compléments cohérents : events_pushed/polled10→130 (+120 =96octets+24événementsT1.5/T3.5), T1.5/T3.5 chacun1→13 (+12), UARTIRQ17→203 (+186), TIM6IRQ/callback3→37 (+34). Aucun nombre IRQ imposé avant campagne ; ces valeurs ne sont pas des durées. File64slots head/tail2/2 =130modulo64, overflow0 : la valeur2 n'est pas une réinitialisation supposée. DernièreADU8octets exacte `010303E80001047A`, dernièreTX7octets exacte `0103020000B844` ; elles ne remplacent pas l'historique des12cas.

**Déduction étayée** : les 96 octets/12trames, un rejet CRC et une unité étrangère, dix acceptations et dix TX/callbacks concordent exactement avec la campagne HP rapportée, sans anomalie tracée. Répétabilité limitée à dix nominales et récupération après deux rejets corroborées par résumé HP et compteurs ; l'ordre cas5/cas8 et chaque délai/silence ne peuvent être reconstitués depuis compteurs agrégés seuls. Le callbackTX appartient au traitementTC réel du HAL selon code transport/HAL déjà inspecté ; preuve logicielle de finTX, pas mesure du dernierstopbit ni frontDE sur fil.

## État final observé

Ready1, poll_result0/pollerrors0 ; tick3704607→3713609, poll1263184426→1266253855. IRQ49/66 actives CPU/SWD : ISER0=0/ISER1=0x20000/ISER2=4, ISPR0..2=0, masquesPRIMASK/BASEPRI/FAULTMASK0. UARTISR0x006200D0 (RXNE/ORE/PE/FE/NE0), CR1=152D/CR2=0/CR3=1/BRR56CE4 ; RxState0x22BUSY_RX/sizecount1, ErrorCode0. PG4ODR0, TXbusy/fault/bus_silent0, phaseIDLE, filevide ; CFSR/HFSR0. Aucun signe du défautHOTPLUG D. État ponctuel sain, pas observation continue des fronts ou garantie sous toutes pannes.

## Verdict M013 et proposition M014

**M013 peut être clôturée pour le diagnostic fonctionnel borné FC03/1000/1, avec réserves explicites.** Les preuves directes STM32 concordent, le dialogue PC rapporté est corroboré pour cette campagne. Aucune qualification générale de communication bidirectionnelle/électrique/temporisation/production n'est annoncée. Journal HP intégral/hash à conserver côté utilisateur/ChatGPT, son authenticité et chronologie ne sont pas recalculées ici ; cette réserve ne masque aucune divergence dans les17comparaisons MCU.

Limites non levées : DE/TC/dernierstopbit, t1.5/t3.5 réels, retourRX minimal, latence physique (13,3657s n'est que durée hôte rapportée), watchdog sous panne/IRQ bloquées/reset, endurance/J1global/production/E4. Aucun instrument temporel identifié, aucune mesure ou intervention physique nouvelle. Aucun acquis J1/J2/J3/RE-RST/VIO/SB28/câblage redemandé.

Proposition `00_gouvernance/CLOTURE_M013_ET_PROPOSITION_M014.md` : M014-A supervisionPC horsport, réutiliser transport .NET/NModbus/factory/paramètres existants, retry0 et accès diagnostic unitaire borné, avant éventuel essai unique explicitement autorisé. Inspection réelle de factory/settings/safety/client/B0SessionFactory/B1Reader/B1SystemState/gelS4. **Obstacle établi** : B1Reader demande20registres ; la cible diagnostic ne fournit normalement que1000/1. B0/DeviceId/READY production ne sont pas disponibles et ne doivent pas être inventés pour faire démarrer la supervision complète. Prévoir mode diagnostic explicite isolé de découverte/polling/B5, sans modifierV1/gels ; la transition firmwareproduction requiert un contrat distinct. Proposition seulement, aucune implémentation M014 ni autorisation de trafic hérité.

## Validation, preuves et publication

Validation J réellement effectuée : hashes source/ELF/BIN et identité Flash, troislectures GDB/dumps, décodageobserver/transport sur layouts exacts, comparaison automatique17deltas tousconformes et cohérence des états. Aucun test/cross-build/.NET relancé : code inchangé, mission d'observation/documentation, ces builds ne prouveraient pas la campagne. Résultats tests antérieurs conservés sans être présentés comme nouveaux. Aucun échec de test masqué/affaibli. Problème de recherche TempWindows limité ci-dessus, sans impact sur lectureMCU ; journalHP nonlocal déjà prévu aucontrat.

Preuves horsGit : `/home/lolo/dev/msm/reports/TR2-mission013-J-20261011T170518Z` : after-before/final.bin/.json/.gdb/.log, image-ram.gdb/.log, logsserver, flash-current.bin/ram-current.bin, comparison.json/decoded-transport.json/symbols.txt, hp-report-provenance.json, preserved-E/I.json et evidence-manifest.json. E181fichiers/I32fichiers conservés ; backups originaux/stashes préservés. Modifications uniquement rapport/journal/clôtureM013-propositionM014, marquage commandesI consommées et archivesI exactes. Mission/AGENTS/V1/code/configuration inchangés.

Journal actualisé : campagne rapportée utilisateur, deltasconcordants/santéMCU observés, clôturelimitée/réservehashHP/limites temporelles et suiteM014. Permissions exercées : debuglecture/haltattach/detach, commit/push normalmain ; flashtrue nonexercé et reset explicitement interdit respecté, zéroCOM7/RS485/OptionBytes/masserase/média/USB/gestephysique/configuration. Diff/index, périmètre sans secrets, branche main/destination origin et distant contrôlés. Commit `979cd12975f0812335edc67d8177b82039ce0b28`, message `Record M013-J matching campaign deltas and limited closure with M014 PC proposal`, push normal explicite HEAD:refs/heads/main réussi ; fetch HEAD=origin/main. Relecture intégrale UTF8 et comparaison byte-à-byte des **16 fichiers** (sept livrables dont archivesI, contratJ et huit sources firmware E) depuis cette référence distante réussies, remote-reread.json horsGit. Arbre propre après première publication, deuxstashes conservés. La présente clôture DONE sera publiée normalement puis relue ; SHA final dans réponse Codex. DONE qualifie auditJ, pas revue indépendante ChatGPT ou qualification temporelle supposée ; réserve journalHP/hash nonrecalculé maintenue.
