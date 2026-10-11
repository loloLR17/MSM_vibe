---
mission_id: TR2-20261011-M014-C-END-TO-END
status: BLOCKED
base_ref: main
base_sha: null
initial_head: 7521ad0390fc323ea53a3aa6fa1497e40a2d4471
mission_sha: 2dfaa45e3ef8f3ba286b61ff484989973b897cac
result_sha: bcc69c9d07aa02f756f9e2db0d67f62613d1951e
created_at_utc: 2026-10-11T18:03:41Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: true
---

# M014-C-END-TO-END — préparation publiée, attente du seul essai HP

**Préparation logicielle et baseline STM32 réalisées ; NO-GO inconditionnel tant
que revue ChatGPT et prérequis HP ne sont pas établis. GO conditionnel pour une
seule lecture par l'utilisateur selon la procédure C. BLOCKED pour l'exécution
HP et l'analyse après essai, pas un échec logiciel.** La mission étendue reste
ouverte ; compléter ce même rapport après réception du résultat, sans nouvelle
mission administrative ni répétition de la campagne Python M013-I.

## Contrat et état réel

Main initial propre7521ad0 ; fetch puis pull --ff-only sûr vers2dfaa45, seul
contrat changé. Deux stashes historiques005/002 identiques et conservés, preuves
A/B/E/J/backups préservées. AGENTS.md, protocole, journal intégral, rapports
J/A/B, procédure B et code diagnostic réel consultés. Base_sha/date contractuelle
absents : null, pas de valeur inventée. Mission active ChatGPT inchangée.
L'extension utilisateur prévaut sur l'ancien objectif readiness : préparation,
contrôles HP, unique lecture puis analyse indépendante dans cette même C.
Elle impose expressément « l'utilisateur exécute sur HP après revue de la commande
par ChatGPT ». Aucun accès distant HP supposé ; pas d'essai exécuté par Codex.

**Faits utilisateur consignés au contrat** : ZIP SHA256 conforme et diagnostic
OFFLINE COM7/115200/8E1 avec message aucune ouverture/transmission/journal, code0.
EXE extrait,370 fichiers et identité PnP ne sont pas encore confirmés sur HP.
Les résultats A/B sur T1000 ne remplacent pas ces contrôles HP.

## Audit .NET conservé

Code évalué bcc69c9, sources diagnostic/dispatch/politique retry comparées
byte-à-byte à ce commit : inchangées. Pas de modification firmware/V1/gels,
transport partagé, production ni configuration permanente. Dispatch diagnostic
avant composition runtime/config ; sans --read-once, retour OFFLINE avant factory,
port et journal. Activation exige paramètres explicites/log ; unité1/8bits/One,
FC03/1000/1 fixes, retries0/WaitToRetry0/SlaveBusyUsesRetryCounttrue.
Une connexion, un ReadRegistersAsync, disposal ; pas polling/découverte/B0/B5,
écriture ou reconnexion. Journal CreateNew/exclusif, intention persistée avant
ouverture/lecture, UTC et decoded_response/result ou failure/stage/kind/code.
Succès seulement un registre0, UNKNOWN0 ; DeviceId indisponible/production non évaluée.

Profil HP timeout série1000ms/deadline attente4000ms, absence retry. Driver
open/dispose n'a pas de garantie temps réel dure ; cancellation ne retire pas
une émission déjà possible. CRC NModbus réel testé en A, journal CLI contient
registres décodés et attentes hex, aucune capture brute du fil supposée.
Résultats conservés vérifiables : build/publish A et TRX502/502, tests framing
mémoire exact/badCRC/silence/exceptions etc ; B2/2 absence ouverture sans activation.
Pas de rerun/build/cross-build inutile : code intact, tranche documentaire/outillage.

## Observations fraîches STM32, sans reset ni flash

Preuves `/home/lolo/dev/msm/reports/TR2-mission014-C-20261011T180341Z`.
Inventaire processus Windows corrigé : aucun GDB/server/CubeProgrammer/maître
TR2 actif avant collecte, aucun après. Chaîne Windows qualifiée et procédure
GDB seule D/J réutilisées ; ST-LINK SN003500463235510B37333439, cible connue
NUCLEO-U575ZI-Q. Helpers J copiés dans nouveau dossier C, pas anciennes preuves
écrasées. Trois sessions before/image-ram/baseline-final, retours GDB/server0 :
`ST-LINK_gdbserver.exe -d -g -i <SN> --frequency 8000 -p 61234 -cp <bin qualifié>`
et `arm-none-eabi-gdb.exe -nx -q -batch -x <script> <ELF E>`.
Lecture/dumps puis detach/quit ; attach suspend brièvement hors trafic. Aucun
monitor reset/load, écriture mémoire/NVIC/GPIO/RDR/ICR, CLI HOTPLUG ni transmission.
Le répertoire -cp n'est pas un appel CubeProgrammer CLI.

Flash49200octets0x08000000..0x0800C030 identique byte-à-byte au BIN E, SHA256
`dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540`.
ELF E168512octets SHA256 `4c496149b7b3af593269dbff7c92368d575cebe824f62ffa67ed57e522d3adc2`,
source firmware0bfaa0e/code09792c8 ; diagnostic/observer/TXON unité1 inchangés.
CPUID410fd214/VTOR08000000 cohérents. Pas nouveau relevé identité silicium
DBGMCU/UID : identité cible par sonde connue/image exacte/contexte qualifié.
Observer992octets et transport692octets décodés avec nm/layout E conservés.
RAM768KiB conservée. Baseline finale SHA256
`28d7bff75da0eb1ed7c7fc108c69da3a17d50b744712a5b21341a909d07d0cfb`.

17 comparaisons trafic J→C toutes delta0, compteurs stables entre relevés C :
RX/bytes104,frames13,CRCvalid12,bad1,foreign1,accepted/available/TXattempt/started/TC11,
erreurs/drop/failsafe0. UARTIRQ203/TIM637. Dernière TX RAM0103020000B844 historique,
pas résultat .NET nouveau. Ready1/poll_result0/pollerrors0 ; tick7285499→7294644,
poll2484185902→2487304112 : exécution progresse après detach. NVIC ISER0=0,
ISER1=20000/ISER2=4, ISPR0..2=0, masques CPU0 ; UARTISR6200D0/CR1152D/CR31,
RxState22/count1/ErrorCode0, PG4ODR0, TXbusy/fault/bus_silent0, phaseIDLE,
head/tail2/2,overflow0,CFSR/HFSR0. État ponctuel sain, aucun défaut HOTPLUG D observé.
Aucun reset depuis J par Codex ; pas compteur boot persistant excluant un reset
externe inconnu. La baseline n'est pas une surveillance continue.

## Commande HP et critères

[Procédure complète](../ESSAI_HP_DOTNET_M014_C.md) : un seul bloc interactif
PowerShell5.1x64, utilisant le dossier M014-A déjà extrait. Vérifie modèle HP,
SHA manifeste/370 fichiers/EXE, présence/PnP FTDI COM7, absence processus candidats,
et déclaration opérateur convertisseur/exclusivité/horsGDB/revue ChatGPT.
PnP ne prouve pas port libre ni convertisseur physique ; inventaire processus
non exhaustif, aucun Open de disponibilité. Pas besoin de rejouer l'OFFLINE acquis.

Dossier fixe LOCALAPPDATA/TR2/M014-C-END-TO-END neuf et marqueur CreateNew/Flush(true)
avant invocation, arrêt si dossier existant : protection contre relance accidentelle,
pas contre suppression volontaire ; ne jamais le supprimer pour retenter.
Une invocation --read-once, COM7/115200/Even/1000ms/logCreateNew. Capture sortie,
code,JSONL/hash et transcript sans écraser. Toute anomalie : conserver et arrêter,
zéro relance. Requête attendue010303E80001047A/réponse0103020000B844 ; PCsuccess
code0+un resultpassedtrue/register0/connexionfermée. Qualification end-to-end
attend ensuite journal exact/hash recalculé et compteurs MCU indépendants.
Deltas nominaux attendus RX+8/frame+1/CRCvalid+1/accepted/available/TX/TC+1,
aucune erreur ; documentés dans procédure, pas présentés comme observés.

## Validation de préparation et limites

Parser::ParseFile du bloc HP sur Windows Dell : SYNTAX_OK, retour0 ; **bloc non
exécuté**. Inspection statique : exactement une invocation EXE, pas boucle autour,
retry/config/Python/Open de disponibilité ; marqueur durable avant invocation,
contrôles avant émission, refus machine Dell. Aucune installation/politique Windows.
Hashes source/Flash/ELF,17deltas et progression contrôlés réellement. Diff/index,
archives exactes B depuis initial7521ad0 et périmètre examinés ; diff --check réussi.
Erreurs d'outillage : première commande inventaire PowerShell avait $_ développé
par Bash ; corrigée avec quoting littéral, inventaire réussi, aucun accès cible
lors de cet échec. Recherches de chemins candidats absents résolues par rg,
sans fabriquer de fichiers. Assertion de contrôleur statique initiale cherchait
.Open au lieu de ::Open pour le marqueur fichier ; corrigée, contrôle réussi,
script HP inchangé. Aucun échec test/build masqué.

NON VÉRIFIÉ : prérequis complémentaires HP, port réellement disponible, branche
SerialPort/driverFT232R/CRC/fermeture physiques, essai .NET et analyse après.
La transmission demeure conditionnelle à la revue/confirmation, sans nouveau
flash/reset. UNKNOWN0 ne signifie ni READY ni DeviceId/B0/B1complet/production.
Limites M013 inchangées : DE/TC/dernierstopbit/t1.5/t3.5/retourRX réel,
robustesse/endurance/reset/watchdog IRQbloquées/J1global/E4 ; aucune qualification
bidirectionnelle générale. Après tout échec, analyser les preuves disponibles
sans nouveau trafic et compléter C. Aucun acquis électrique à répéter.

## Livrables, permissions et publication

Procédure C ajoutée, B reliée à C en conservant ses états historiques, journal
actualisé (OFFLINE utilisateur/santéMCU observée/préparation/attentes), archives B
immuables. Mission active/AGENTS/protocole/code/.NET/firmware/V1/gels inchangés.
Preuves horsGit : scripts/logs GDB/server, before/final.bin/json, Flash/RAM,
comparison-J-C.json, decoded-transport.json, audit.json, hp-command.ps1,
syntax-check.log, stashes-initial.txt, manifeste et relectures publication.
Permissions exercées : debug lecture/attach/detach, documentation, commits/push
normaux main ; allow_flashfalse respecté, zéro reset/COM7/RS485/HOTPLUG/USB/média,
OptionBytes/effacement ou changement configuration. Préparation sera publiée puis
relue depuis référence GitHub ; SHA final fourni après publication. BLOCKED reste
approprié jusqu'aux preuves physiques HP et analyse MCU, même après relecture.
