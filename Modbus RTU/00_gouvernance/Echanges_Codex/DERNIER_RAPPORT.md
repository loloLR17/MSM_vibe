---
mission_id: TR2-20261011-M013-F-PHYSICAL-RESPONSE-FORENSICS
status: DONE
base_ref: main
base_sha: null
initial_head: c0e2e4c4c75cb71ef20e03446b59abed90e89847
mission_sha: 36b12d4702f446ff6fe027557da65e3d6220172f
result_sha: 0bfaa0e0daf3d37f723497b74984cabe1b11ec47
created_at_utc: 2026-10-11T16:16:06.403265+00:00
updated_at_utc: 2026-10-11T16:17:09.938785+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: true
---

# M013-F — première réponse HP corroborée par la chaîne STM32

**DONE : analyse F satisfaite, rapport publié et relu depuis GitHub. Premier aller-retour diagnostic fortement corroboré pour cet essai unique ; qualification physique définitive et générale NON ACQUISE.** Zéro nouvelle émission HP/RS485, zéro flash/reset, zéro accès CubeProgrammer HOTPLUG. Aucun geste physique demandé. Essai E consommé, ne pas le répéter.

## Référence et faits utilisateur

Main initial propre c0e2e4c, fetch/pull --ff-only vers36b12d4, seul contrat changé. AGENTS.md/protocole/journal intégral/rapport E DONE/procédure D/sources HAL/transport/observateur/script dépôt inspectés. Contrat ChatGPT inchangé, base_sha absent donc null. Code firmware inchangé depuis09792c8 ; result_sha0bfaa0e est la référence du candidat E réellement sur cible, pas le HEAD documentaire F. Deux stashes005/002 préservés. Archives mission/rapport E byte-à-byte depuis HEAD initial, sans réécriture historique.

**Observation utilisateur transcrite dans le contrat F, pas capture directe Codex du port HP** : après GO E, un seul lancement `& "$HOME\\Send-Tr2RxProbe.ps1" -Transmit`, COM7/1152008E1 ; sortie exacte :

```text
Request: 01 03 03 E8 00 01 04 7A; COM7 115200 8E1; CRC/self-check OK
Observed PC RX (may be echo): 01-03-02-00-00-B8-44
```

## Contrôle indépendant hors ligne et discrimination d'origine

Calcul CRC Modbus indépendant sur les octets rapportés, sans ouvrir de port ni exécuter script HP : CRC du corps réponse `01 03 02 00 00` =0x44B8, ordre sur fil B8 44, résidu de trame complète0. Requête complète résidu0 également. Réponse7octets : unité1, FC03, byte_count2, mot big-endian0=UNKNOWN. Correspond exactement à FC03/1000/quantité1 attendu du diagnostic isolé, sans prétendre READY/production.

- **Écho brut exclu pour la séquence affichée** : la requête a8octets `01 03 03 E8 00 01 04 7A`, la sortie a7octets différents et format réponse correct. Le préfixe unité/FC commun n'en fait pas un écho de requête.
- **Script dépôt** : un seul Write binaire de la requête, puis ReadByte et affichage hex brut, aucune fabrication/transformation de réponse. « may be echo » est un avertissement générique, pas une détection d'écho. La copie exécutée dans HOME sur HP n'a pas été récupérée/hachée ; son identité byte-à-byte avec le dépôt n'est pas démontrée.
- **Origine STM32 fortement corroborée** par réception exacte, requête acceptée, réponse exacte en RAM, TX commencé et callbackTC sur ce MCU entre baseline E et relevés F. Un simple écho local ne suffit pas à expliquer cette chaîne.
- **Écho transformé / autre maître** : un équipement/proxy capable de traiter Modbus ou un autre répondant pourrait théoriquement produire les mêmes octets ; aucun élément observé ne l'indique. Pas de capture bus ni identité authentifiée par ces trames, donc exclusion absolue de toute source concurrente impossible. Aucun nouvel essai pour lever cette limite.

## Préservation et identité de l'image

Preuves F hors Git : `/home/lolo/dev/msm/reports/TR2-mission013-F-20261011T161246Z`. Manifeste preservation-E.json recense **181 fichiers E**, rehashés avant et après lecture, tous inchangés ; originaux E/D et backups conservés. ELF/BIN E et hashes des huit sources comparés, pas de rebuild. Baseline E `healthy-final.json`/`.bin` : tous compteurs trafic0, poll10627242 et tick31167.

ELF exact168512octets SHA256 `4c496149b7b3af593269dbff7c92368d575cebe824f62ffa67ed57e522d3adc2` ; Flash courante lue via **GDB seul** sur49200octets à0x08000000, byte-à-byte identique BIN E SHA256 `dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540`. Aucune nouvelle sauvegardeFlash2MiB prétendue : celles E restent conservées et vérifiées par manifeste. DiagnosticON/observerON/TXON/unité1, aucune autorité média/production bootée. RAM liée complète768KiB conservée (ram-after.bin), SHA256 `541c1a8aba716cfcbca6b9b1c16d5333f5596a49c437e4bc24534d285ceffd7e` ; observateur brut final992octets conservé. Symboles/adresses issus du même ELF ; layout transport/HAL E -g3 conservé et sources inchangées, g_serial692octets/observateur992octets recoupés par nm actuel. Manifestes offline-response-check.json, decoded-after.json, evidence-manifest.json, symbols.txt.

## Lectures STM32 réellement effectuées

Deux sessions qualifiées ST-LINK/GDB, sonde SN003500463235510B37333439/NUCLEO-U575ZI-Q, mêmes exécutables Windows ST-LINK_gdbserver et arm-none-eabi-gdb sous C:\ST\STM32CubeCLT_1.22.0. Serveur `-d -g -i <SN> --frequency 8000 -p 61234 -cp <CubeProgrammer/bin>` ; GDB `-nx -q -batch -x <script> <ELF exact>`, target extended-remote localhost:61234. Première session : dump RAM/Flash, globals, NVIC/UARTISR/PG4/IO2SV/faultregs. Seconde : observer992octets et mêmes états passifs pour vérifier reprise/progression. Aucune commande monitor reset/load, breakpoint/watchpoint, RDR/ICR ou écriture NVIC/UART/GPIO ; aucun CubeProgrammer CLI lancé. Répertoire -cp est celui requis du serveur qualifié, pas un accès CLI HOTPLUG.

Attach suspend brièvement le cœur pour les lectures, puis detach/quit immédiat après collecte ; pas de halt d'attente ou pendant échange HP, déjà terminé. Cette observation après coup n'est pas une capture continue non intrusive. Toutes commandes GDB réussies, serveurs sortis normalement0, inventaire Windows final sans GDB/OpenOCD, cible laissée running. Poll187193205→202036798, tick548991→592523 ; compteurs trafic stables, NVIC reste sain après lectures. Aucun effet perturbant NVIC/RX/TX détecté par ces relevés, sans garantie d'absence de tout effet temporel du halt.

## Deltas AVANT E → APRÈS F et portée

Baseline compteurs0 ; valeurs ci-dessous = deltas observés sur ce boot. Aucun reset effectué par Codex depuis clôture E ; progression tick/poll et buffers/counters cohérents. Pas de compteur de boot persistant pour exclure indépendamment tout reset externe inconnu ; aucun indice de celui-ci dans les preuves.

| Chaîne | Delta / contenu observé | Conclusion limitée à l'essai |
|---|---|---|
| IRQ | UART17 ; TIM6 IRQ/callback3 | Les handlers exécutent réellement ; pas partition chronologique de chaque IRQ |
| RX/HAL | RXcallback8, bytes_polled8, ERRORcallback/errors0 | Huit octets remis au transport ; pas de défaut UART tracé |
| File/timers | poussés10/pollés10, dropped0 ; T1.5=1/T3.5=1 ; head=tail10/overflow0 | Octets et fins de trame consommés, file vide ; pas de mesure des durées physiques |
| RTU | frame1, CRCvalid1, CRCbad/invalid0, foreign/broadcast0, accepted1/available_status1 | Requête locale acceptée, sourceUNKNOWN diagnostic rendue disponible |
| Dernière ADU | length8, codec0 ; `01 03 03 E8 00 01 04 7A` | Exactement requête rapportée HP, CRC/unité/FC/PDU concordants |
| Réponse | last_tx_length7, observateur et bufferTX : `01 03 02 00 00 B8 44` | Exactement sortieHP et réponse prévue, pas simple copie de requête |
| TX | attempt1/started1, callbackTX1, failsafe0 ; TxXferSize7/Count0 | HAL TX IT lancé, traitement fin de transmission effectué ; pas capture analogique bus |
| Retour final | gState0x20READY, RxState0x22BUSY_RX/count1, phaseIDLE, tx_busy/fault0 ; PG4ODR0 | RX réarmé, direction libérée et timer revenu IDLE ; aptitude à une seconde requête non testée |

HAL CubeU5v1.9.0 UART_IRQHandler n'appelle UART_EndTransmit_IT que sur ISR_TC et CR1_TCIE ; ce dernier appelle HAL_UART_TxCpltCallback. Le callback transport compte l'appel, met PG4bas, réarmeRX et démarre gardeT3.5. Le compteur callback1 est donc **preuve logicielle indirecte de traitement TC sur cible**, pas une capture du dernier stopbit ou de l'instant DEbas. Watchdog non déclenché observable (failsafe0), sa robustesse sous IRQ bloquées non qualifiée.

Derniers états CPU/SWD concordants : ISER1=0x00020000/ISER2=4, ISPR0..2=0, PRIMASK/BASEPRI/FAULTMASK0 ; UART CR1=0x152D/CR2=0/CR3=1/BRR=0x56CE4, ISR0x006200D0 (RXNE/ORE/PE/FE/NE0), HAL ErrorCode0 ; IO2SV0x20000000, CFSR/HFSR0. Aucun signe du défaut HOTPLUG D. Zéro nouvelle requête/émission Codex, aucune répétition de contrôle physique acquis.

## Verdict, limites et suite proposée

**Faits établis** : la sortie utilisateur a format/CRC/valeur attendus ; le MCU a reçu la requête exacte, traité son CRC/adresse, préparé la réponse exacte, démarré TX et achevé son traitementTC avec RX réarmé. **Déduction fortement étayée** : les sept octets HP proviennent de la réponse du STM32 pour cet essai unique. **Non démontré directement** : provenance bus exclusive, chronologieDE/dernierstopbit/TC/retourRX, timings/latence/marge électrique, absence de tout transitoire, répétabilité/endurance/erreurs/timeout/IRQbloquées/reset. Pas de qualification définitive TX/bidirectionnelle générale, J1global/production/E4 inchangés et NON QUALIFIÉS.

À ChatGPT : proposer une **mission distincte** de qualification temporelle avec capture DE/TXD/bus et retourRX, suivie de robustesse ciblée (requêtes successives espacées, CRC/adresse erronés, panneTX/timeout) sous permissions et instrumentation adaptées. Ne pas lancer ces essais maintenant ni demander une nouvelle manipulation utilisateur dans F. Conserver chaîne d'accès GDB seule ; tout futur accès CLI nécessite procédure D sous contrat approprié. L'instruction E -Transmit est consommée.

## Validation et publication

Validation F proportionnée : vérificationCRC indépendante hors ligne, identityFlash/ELF et sources, dumpRAM/relecturecodeHAL, cohérence des deltas et état après detach ; aucune source firmware modifiée, donc aucune recompilation/campagnetests qui n'apporterait pas de preuve de cet essai. Tests5/5/build/flash E sont historiques explicitement, pas résultats F. Aucun échec de diagnostic à masquer ou corriger.

Journal actualisé avec réponse utilisateur, chaîne MCU/deltas, verdict nuancé et prochaine tranche ; preflight marqué essai E consommé. Modifications limitées à ces trois documents et deux archives E. Permissions exercées : lecturedebug/halt d'attach/detach, commit/push main autorisés ; allow_flashfalse respecté, zéro reset/flash/OptionBytes/média/USB/configuration/émission. Diff et contenu indexé vérifiés. Commit `393ed0d19eb48f0e1dbf8b1f3e42df2f2e6d053a`, message `Record M013-F matching HP response and passive STM32 RX RTU TX TC evidence`, push normal explicite HEAD:refs/heads/main réussi. Fetch HEAD=origin/main ; relecture intégrale UTF8 et comparaison byte-à-byte des 14 fichiers (cinq livrables documentaires, huit sources, contrat F) depuis cette référence distante réussies, remote-reread.json hors Git. Arbre propre, deux stashes historiques conservés. La présente clôture DONE sera publiée et relue à son tour ; son SHA final est communiqué dans la réponse Codex. DONE concerne le diagnostic de cet essai, pas une qualification physique générale. Contrôle indépendant ChatGPT à effectuer après publication.
