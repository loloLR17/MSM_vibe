---
mission_id: TR2-20261011-M014-C-END-TO-END
status: DONE
base_ref: main
base_sha: null
initial_head: 7521ad0390fc323ea53a3aa6fa1497e40a2d4471
mission_sha: 2dfaa45e3ef8f3ba286b61ff484989973b897cac
resume_head: 8299db8f08c00c0ee73604c82a834114d4e3aaf4
result_sha: bcc69c9d07aa02f756f9e2db0d67f62613d1951e
firmware_source_sha: 0bfaa0e0daf3d37f723497b74984cabe1b11ec47
created_at_utc: 2026-10-11T18:03:41Z
updated_at_utc: 2026-10-11T18:17:12Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: true
---

# M014-C-END-TO-END — lecture .NET unique corroborée par le STM32

**Concordance des17deltas MCU avec une seule lecture FC03/1000/1 ; clôture
fonctionnelle limitée avec réserves documentaires. DONE : analyse C terminée,
rapport et livrables publiés puis relus depuis GitHub.**
Aucun nouvel essai Modbus, accès COM7, reset, flash ou CubeProgrammer HOTPLUG.
L'essai HP et sa commande sont consommés : ne pas relancer. Même mission C,
pas de nouvelle mission administrative.

## Références et préparation conservée

HEAD initial de C7521ad0390fc323ea53a3aa6fa1497e40a2d4471 ; mission récupérée
par fast-forward2dfaa45e3ef8f3ba286b61ff484989973b897cac. Préparation et relecture
publiées3b8815b puis8299db8f08c00c0ee73604c82a834114d4e3aaf4. Reprise sur ce
dernier SHA, main propre ; fetch/pull --ff-only : déjà à jour, distant identique.
Deux stashes historiques005/002 inchangés, preuves/backups antérieurs préservés.
AGENTS/protocole/contrat inchangés, journal intégral consulté ; aucune édition
mission ChatGPT. Base_sha absent : null. Extension utilisateur et retour physique
permettent de terminer C et publier, sans nouveau trafic.

Audit et contrôles préparatoires conservés, non recommencés : diagnostic dispatch
avant runtime/config, OFFLINE sans factory/port/journal ; opt-in --read-once exige
paramètres/log. Factory SerialPort/NModbus existante, unité1/8bits/One/FC03/1000/1,
Retries0/WaitToRetry0/SlaveBusyUsesRetryCounttrue. Une connexion/un ReadRegistersAsync,
fermeture/disposal, aucun polling/découverte/B0/B5/écriture/reconnexion. Journal
CreateNew/FileShare.None, intention persistée, decoded_response/result/failure
avec UTC/stade/code ; succès un registre0. Timeout1000ms, attente4000ms,
sans garantie temps réel dure du driver open/dispose. CRC NModbus testé sur flux
mémoire A ; journal PC contient registres décodés/attentes hex, pas capture fil.
Code .NET évalué bcc69c9 inchangé ; firmware E/code09792c8 inchangé.
Résultats conservés A build/publish/502tests réussis et B2tests OFFLINE réussis,
syntaxe bloc PowerShell C réussie ; aucune suite/build relancée sans changement.

Préparation HP unique publiée : modèle HP, EXE SHA13228dfe…, manifeste
SHAeeaf70a3…/370fichiers, PnP FTDI COM7, processus candidats et déclaration opérateur.
Inventaire ne prouve pas port libre, processus non exhaustifs ; dossier mission
fixe/neuf et marqueur CreateNew durable empêchent relance accidentelle, pas
suppression volontaire. Aucun préflight d'ouverture ni changement Windows.
Archives B exactes conservées ; préparation complète dans le rapport précédent
au SHA8299db8 et copie horsGit `report-before-resumption.md`.

## Résultat HP : provenance et réserve explicite

**Rapporté directement par l'utilisateur** : essai physique unique sur HP réussi
le11/10/2026 à18:15:29UTC, UNKNOWN(0), code retour0, journal passedtrue,
aucune répétition. SHA256 annoncé :
`AFDAB5517A13ABED1E9735A4240F0E6B53C5DC110F9CB717BB50410F698A2272`.
Ce résultat est un témoignage utilisateur, pas une lecture directe du HP par Codex.

**Le journal HP intégral n'a pas été fourni à Codex. Son contenu, son hash,
ses chunks/horodatages et sa chronologie ne sont pas vérifiés indépendamment
à partir de ce seul SHA256.** Aucun faux JSONL reconstitué. L'heure est rapportée,
pas une mesure GDB du moment de trafic. ZIP conforme et OFFLINE/code0 étaient
déjà rapportés au contrat ; les sorties détaillées des370fichiers/EXE/PnP,
la revue ChatGPT et l'exclusivité effective ne sont pas documentées directement
par les fichiers disponibles ici. La réussite rapportée ne les transforme pas
en observations Codex. Réserve documentaire maintenue à la clôture bornée.

## Collecte indépendante GDB après essai terminé

Preuves nouvelles : `/home/lolo/dev/msm/reports/TR2-mission014-C-after-20261011T181712Z`.
Baseline originale conservée : dossierC20261011T180341Z,
`baseline-final.bin`992octets SHA256
`28d7bff75da0eb1ed7c7fc108c69da3a17d50b744712a5b21341a909d07d0cfb`,
empreinte recontrôlée et copie préservée. Helpers lecture C réutilisés sans
écraser les anciennes preuves ; observer992/transport692/layout E/nm et code
réel recoupés. Une recherche de chemin candidat absent a été résolue par rg,
sans fichier substitut ni incident matériel.

Inventaires processus Windows avant/après : aucun GDB/server/CubeProgrammer/maître
TR2 actif. ST-LINK SN003500463235510B37333439, NUCLEO-U575ZI-Q connue,
chaîne Windows qualifiée et voie GDB seule D/J/C. Trois sessions : after-before,
image-ram, after-final ; GDB et serveur retours0. Serveur
`ST-LINK_gdbserver.exe -d -g -i <SN> --frequency 8000 -p 61234 -cp <bin qualifié>` ;
client `arm-none-eabi-gdb.exe -nx -q -batch -x <script> <ELF E>` ; target
extended-remote localhost61234, lectures/dumps puis detach/quit. Suspension
brève à l'attach après trafic terminé, MCU laissé en exécution. Aucun monitorreset,
load, écriture NVIC/GPIO/RDR/ICR/OptionBytes ; -cp n'est pas un appel CLI HOTPLUG.

Flash49200octets0x08000000..0x0800C030 identique byte-à-byte au BIN E,
SHA256 `dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540`.
ELF168512octets SHA256 `4c496149b7b3af593269dbff7c92368d575cebe824f62ffa67ed57e522d3adc2`.
Image diagnostic/observer/TXON unité1, firmware source0bfaa0e confirmé par image
exacte ; sources observer/transport comparées sans diff. Pas nouveau relevé
silicium UID/DBGMCU : identité via sonde connue/image/contexte qualifié.
Observer final SHA256 `687519dcda998d25e37cd10a930cbeccf43b2fadaf24bd2bdfd1dfd532c511dc` ;
RAM768KiB SHA256 `2aebd0623b1037a482c79ec4209ab7c6d71700ec87496432bdfbf6798d9970da`.

## Baseline C → après essai : toutes comparaisons conformes

Deltas modulo2^32, même image. Compteurs de trafic stables entre les deux lectures
après ; aucune transmission Codex ni reset. Pas compteur boot persistant pour
exclure toute intervention externe inconnue ; progression et compteurs cohérents.

| Compteur | Baseline C | Après | Delta | Attendu |
|---|---:|---:|---:|---:|
| rx_callback | 104 | 112 | 8 | 8 |
| bytes_polled | 104 | 112 | 8 | 8 |
| frame_count | 13 | 14 | 1 | 1 |
| crc_valid | 12 | 13 | 1 | 1 |
| crc_bad | 1 | 1 | 0 | 0 |
| foreign_unit | 1 | 1 | 0 | 0 |
| accepted_request | 11 | 12 | 1 | 1 |
| available_status | 11 | 12 | 1 | 1 |
| tx_attempt | 11 | 12 | 1 | 1 |
| tx_started | 11 | 12 | 1 | 1 |
| tx_callback | 11 | 12 | 1 | 1 |
| error_callback | 0 | 0 | 0 | 0 |
| events_dropped | 0 | 0 | 0 | 0 |
| errors_polled | 0 | 0 | 0 | 0 |
| frame_invalid | 0 | 0 | 0 | 0 |
| broadcast | 0 | 0 | 0 | 0 |
| tx_fail_safe | 0 | 0 | 0 | 0 |

Compléments : events_pushed/polled130→140 (+10 =8octets+T1.5+T3.5), T1.5/T3.5
13→14 chacun ; UARTIRQ203→220 (+17), TIM6IRQ/callback37→40 (+3).
Ces nombres ne mesurent pas les durées physiques. File head/tail12/12=140mod64,
overflow0. Dernière ADU RAM8octets exacte `010303E80001047A`, dernière réponse
RAM7octets exacte `0103020000B844`, identique buffer transport. Les mêmes bytes
existaient pour d'anciens essais ; c'est leur combinaison avec les deltas nouveaux
qui corroborent cette lecture, pas le seul dernier buffer.

**Déduction étayée** : une nouvelle requête nominale CRC/adresse acceptée et une
réponse STM32 terminée TC concordent exactement avec le succès .NET rapporté.
Chaîne fonctionnelle diagnostic HP .NET→STM32→HP corroborée pour ce seul échange,
avec réserve journal PC non analysé intégralement. Aucune anomalie MCU tracée.
Compteurs agrégés ne démontrent pas à eux seuls client émetteur, exclusion de
source concurrente ni heure exacte ; application/code0/passedtrue reposent sur
retour utilisateur. Pas de capture bus ni qualification électrique générale.

## État final et portée

Ready1/poll_result0/pollerrors0 ; tick8062742→8071595,
poll2749207528→2752226463 : progression après detach. NVIC ISER0=0,
ISER1=0x20000/ISER2=4 (IRQ49/66 actives), ISPR0..2=0, masquesCPU0,
CPUID410fd214/VTOR08000000. UARTISR6200D0 (pas RXNE/ORE/PE/FE/NE),
CR1152D/CR31/BRR56CE4, RXBUSY_RX22/sizecount1/ErrorCode0 ; PG4ODRbas,
TXbusy/fault/bus_silent0, phaseIDLE/filevide, CFSR/HFSR0. Pas défautHOTPLUG observé.
État ponctuel sain, pas surveillance continue ni mesure dernierstopbit/frontDE.

**Qualification fonctionnelle limitée acquise avec réserves de provenance HP**
pour cette lecture .NET diagnostic FC03/1000/1 UNKNOWN0. Ni READY, ni DeviceId,
B0/B1complet/production. Pas de qualification générale bidirectionnelle,
DE/TC/dernierstopbit/t1.5/t3.5/retourRX, latence physique, robustesse/endurance,
reset/watchdogIRQbloquées/J1global/E4. Le succès nominal ne valide pas fermeture
Windows sous panne. Ces limites restent ouvertes ; aucun acquis électrique à refaire.

Suite : conserver JSONL/transcript/marqueur HP ; si le fichier exact devient
disponible, recalculer le hash et analyser la chronologie sans nouvelle émission.
ChatGPT doit contrôler indépendamment le rapport distant et choisir le prochain
jalon (instrumentation temporelle identifiée ou intégration production sous contrat).
Étude accès Codex HP différée, aucun accès/service/compte configuré ici.

## Validation, livrables et publication

Réellement exécuté : état Git/fetch/pull sûr, hash baseline/ELF, trois GDB lectures
et dumps avec retours0, Flash byte-à-byte,17comparaisons toutes réussies,
décodage RAM/layout réel, progression/santé et absence processus résiduels.
Pas tests/build/cross-build supplémentaires : logiciel intact, analyse de preuves
matérielles/documentation ; résultats A/B réutilisés, pas présentés comme nouveaux.
Aucun échec test masqué/affaibli. Preuves : scripts/logsGDB/server, observerbin/json,
Flash/RAM, comparison.json/decoded-transport.json, provenanceHP explicite,
baseline copiée/rapport précédent/stashes et manifestes/relectures horsGit.

Modifications : ce même rapport final, journal actualisé (résultat utilisateur,
deltas/santéobservés, réserves), procédureC et renvoiB marqués essaiC consommé.
Mission ChatGPT/AGENTS/protocole/V1/gels/code/firmware/config inchangés, archivesB
immuables. Permissions exercées : debuglecture/attach/detach et commits/push
normaux main ; allow_flashfalse, zéro nouveau COM7/RS485/reset/flash/HOTPLUG/
OptionBytes/effacement/USB/média/gestephysique/configurationpermanente.
Diff/index/périmètre contrôlés, diff --check réussi. Commit `8725f243f3fbd77a2227a03146629467aa44ca16`,
message `Corroborate unique M014-C .NET HP read with STM32 counter deltas`,
push normal explicite HEAD:refs/heads/main réussi ; fetch et relecture intégrale
UTF8/comparaison byte-à-byte des10fichiers livrés/gouvernance/sources inchangées
depuis GitHub réussies (`remote-reread-1.json`). HEAD=origin/main, arbre propre,
deuxstashes conservés. Cette clôture DONE sera également publiée et relue,
SHA final dans réponse Codex. DONE qualifie la mission C et la corroboration
fonctionnelle bornée avec réserves ; ne valide pas journalHP intégral, revue
indépendante ChatGPT ni communication bidirectionnelle générale.
