---
mission_id: TR2-20261012-M015-M-SPI-LONG-TRANSFER
status: BLOCKED
base_ref: main
base_sha: f8e782a41e2cca94ab12ff3f180a2dc6461611aa
initial_head: 3e147109cb21c30a4b4525a664504bdee9eeefa7
result_sha: a7a2ebc0ff400fb1bb39a4d43acb4ae4279bd725
created_at_utc: 2026-10-12T20:58:22.674062+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-M — transfert long corrigé, qualification FIFO encore bloquée

**Cause opérationnelle du refus SPI reproduite et correction ciblée observée :
HAL SPI Debug O0 à CPU16MHz ne soutient pas la réception longue à SPI2MHz.
L'optimisation du seul module HAL SPI en O2 permet deux lectures complètes de
896octets sans OVR ni timeout.** Une nouvelle limite du harness interrompt la
fenêtre :1024traces atteintes après256entrées, toutes timestamps, aucunXYZ.
La qualification FIFO512 reste **BLOCKED**, pas DONE ni qualification V1.
Les acquisitions sont arrêtées conformément au STOP sur anomalie nouvelle.

## Tranches et preuves

1. **Audit et logiciel validés** : HAL réel CubeU5v1.9.0, sourceSPI SHA
   `a1668346fe2847e5ed1fb1c7812158e3e39391a62cfd6487c4973e66308c22a3` ;
   CPU MSI16MHz, SPI3PCLK3/div8, mode0, FIFOseuil1, réception bloquante 8bits.
   CloseTransfer détecteOVR puis remet les compteurs à0. Ajout de snapshots
   bornés **avant** ce nettoyage, hors boucle de service RX ; offsets réellement
   consommés, tailles/restants/SR/config/CS/tick/DWT. Lectures incomplètes refusées.
   Copie HAL de build instrumentée, sourceoriginale inchangée ; SHA copie
   `85fa97b8d658c69e7846c13ed5975d13c01af61a2917877899e20f97aad2fdfa`.
   Pilote/exemples ST inchangés, vérifiés contre vendor/SOURCES.json.
2. **Validation logicielle** :14scénariosC +2testsPython/2CTest réussis,
   deux cross-builds FIFO STATUS10 O0/O2 réussis. Les builds ne diffèrent que
   par `-O0`/`-O2` sur HALSPI ; CPU/SPI/délais20ms/réglages capteur constants.
   Les tests portent sur offsets/acceptation complète et adaptation HAL exacte,
   sans affaiblir les tests existants. Aucun ancien essaiWHO/polling/test41C/5Python
   ni buildTR2 inchangé rejoué. Validation ciblée du projet indépendant,
   pas besoin de tr2_validate.sh sur le firmware principal non modifié.
3. **Checkpoint publié** : `a7a2ebc0ff400fb1bb39a4d43acb4ae4279bd725`,
   message «M015-M: observe SPI before HAL cleanup and isolate service optimization» ;
   push normal main puis fetch/relecture intégrale de23fichiers byte-identiques.
4. **Diagnostic O0 unique** : résultatIO1/cleanup0/count0,597traces,
   1194snapshots, aucune saturation de stockage. Trace590, registre78,
   taille896, HAL_TIMEOUT3/error104. Avantcleanup :SR`104A` inclutOVR40/EOT8,
   RxSize896/restant685/offset211, CSbas, erreurHAL encore0 avant CloseTransfer.
   Durée DWT306138cycles=19,134ms, ticks4654→4674. Bufferpartiel **rejeté**,
   aucune mesure acceptée. Aprèscleanup HALcompteurs0 ne prouve pas896 reçus.
5. **Candidat O2 unique** : deux bursts complets, traces688/954, HAL_OK0/error0,
   snapshotsSR`100A` sansOVR, RxSize896/restant0/offset896/CSbas.
   DWT58115/58121cycles=3,6322/3,6326ms par896octets. CR1/CR2/CFG1/CFG2
   des bursts identiques entreO0/O2. Aucun nouveau mode, timeout, réglage ou
   fallback. Readbacks deconfiguration après chacun des deuxbursts conformes.
6. **Nouvelle anomalie et STOP** : resultIO1/cleanupIO1,256entrées,
   1024traces/2048snapshots exactement. Aucun transfert enregistré invalide ;
   les suivants sont refusés **avant** enregistrement par le garde de capacité.
   Le même garde empêche les six transactions du cleanup original.
   Aucun troisième essaiFIFO ni agrandissement opportuniste du stockage.
7. **Mise au repos nécessaire** : image dédiée `BENCH_CLEANUP_ONLY=ON`,
   cross-build réussi, identité et image contrôlées, flash normal/verify/readback.
   Unique trigger de sécurité, **aucune acquisition** : setters ST XLoff,
   FIFO bypass, timestampoff puis readbacks ;9transactions/18snapshots valides,
   résultat0/cleanup0. CTRL1_XL=`0C` (xl_en0), FIFO_CTRL4=`80` (mode0),
   CTRL10_C=`00` (timestampoff). Cette action ne transforme pas le cleanup
   O2 initialement échoué en réussite ; les preuves de cet échec sont intactes.

## Cause démontrée, déductions et limites

Faits : O0 produit un OVR matériel STM32 et perd la réception complète ; O2,
seul facteur de compilation changé dans les deux images instrumentées, reçoit
intégralement deux bursts sous les mêmes registres/horloges/temps limites.
Lecture HAL : branche8bits RXP consomme un octet par itération ; son budget
théorique à2MHz est4µs/octet,64cycles CPU. Le désassemblage O0 montre de
nombreux accès de pile/handle par octet, réduits enO2. Déduction étayée :
le service RX HAL non optimisé est insuffisant dans cette configuration,
provoquantOVR, puis EOT avec des octets manquants ; la boucle attend le compte
restant jusqu'au timeout. Un simple allongement de timeout ne restituerait
pas les octets perdus. Correction minimale retenue : optimisation deHALSPI seul.

Limites : pas analyseur logique ni mesure indépendante deSCK, pas chronométrage
cycle-par-cycle de la boucle. Le hook est placé à la fermeture, pas entre
les octets ;O0 instrumenté reproduit le refus historique L sans hook.
Deux bursts conformes qualifient ce transfert borné, pas sa robustesse générale
ni l'origine des anomalies TR2 I/J. OVR_SPI n'est pas OVR FIFOcapteur.

Intégrité hors ligne : bruts256×7 reconstruits byte-identiques entreRAMwords
et les deux payloads ;256tags4/timestamp,0tag2/XYZ, parité correcte256/256,
compteurs modulo4 tous0. Timestamps753→6873,255pas de24ticks, évolution
strictement croissante. Aucune continuitéXYZ ni absence globale de pertes
ne peut être établie par ces compteurs constants. Données exportées avec
`accepted=false` : fenêtre incomplète et cleanupinitial échoué. Derniers
statutsprérefus niveau171/flags00 ; absence d'OVR dans les lectures observées
ne prouve pas absence ultérieure pendant la mise au repos.
L'absence deXYZ est un fait distinct encore inexpliqué ; aucune modification
heuristique du BDR constructeur ni attribution au module/piloteST.

## Cible, Flash et préservation

Seconde NUCLEO seulement : SN`0032001F3235510C37333439`,
UID`001B00463842501820333157`, ID482/RevU, tensionCLI3,28V.
Identité relue **avant chacun des trois flashs** ; image/BIN/ELFhashes contrôlés,
adresse08000000, NORMAL/SWD/verify/reset ; chaque readback byte-identique.
Après dernierCLI, resetGDB puisGDBseul ;bootready1/core16MHz/UIDexact/faults0,
detach après trigger et capture, tous GDB/serverexit0.

Nouvelle règle appliquée : **aucune sauvegardeFlash2MiB ni restauration initiale**.
L'identité et la Flash initiale ont seulement été observées via32octets de
vecteurs, cohérents avec l'état L antérieur ; pas nouvelle relecture intégrale.
Aucun masserase/OptionBytes/RDP/changementboot. La seconde carte reste en
exécution sur l'image cleanup inactive, capteur arrêté ; son firmware initial
n'a pas été restauré. TR2principal/M014-H jamais connecté, aucunHP/COM7,
aucunFRAM/microSD/Modbus. Worktrees historiques/HEADs propres et deuxstashes
vérifiés inchangés. ArchivesL repriseessai02 ajoutées exactes depuisf8e782a,
premieréchecWHO00 etpreuvesessai1/essai2 conservés séparément.

| Image réellement évaluée | Taille BIN | SHA256 BIN |
|---|---:|---|
| FIFO O0 / codea7a2ebc | 32444 | `a88be10a08bad22230487648d6737e53baf9269a65bd2c324c8361e52db24bd7` |
| FIFO O2 / codea7a2ebc | 30852 | `98ec786c70a8fe104eb4dc6a8f5773ae525554f5a7d4b61f408beac7d90ef40a` |
| Cleanup seul / sourcesa7a2ebc+ajoutsécurité | 26616 | `0f40f3b2ecac8b8131a86677e0b0d23f911d1d3515d2c884844e0d8966c9c004` |

Les sources cleanup/CMake exactes sont publiées avec ce rapport ; SHA256
cleanup_main.c `e53efc68c2bcc2275cf3809a2ed869b2ac69f083fc5cd2359419f1e875d4acf7`.
Son build a précédé son commit ; aucune équivalence erronée à l'image a7a2ebc.

## Publication et prochaine action

[Preuves brutes et index](preuves/TR2-20261012-M015-M/README.md),
originaux `/home/lolo/dev/msm/reports/TR2-mission015-M-20261012T204419Z`. Logs/scripts/commandes, capturesresult/traces/payload/preclose,
CSV/timestamps, binaires enhex et manifestes conservés. Les traces restent
observationsMCU ; pas preuvewire. Journal et roadmap actualisés.

Le prochain contrat doit réserver la capacité du cleanup indépendamment des
lectures répétitives et réduire les polls de niveau bornés, **sans affaiblir
les gardes** ; qualifier cette correction horsligne. Puis campagneO2 distincte,
explicitement autorisée après STOP, pour expliquer l'absenceXYZ avec réglages
ST inchangés et capturesdeconfiguration/tags. Pas de nouvel essai nécessaire
à la présente publication. ComparaisonTR2 ultérieure à un seul facteur,
avec sauvegarde/restaurationM014 propres au principal sousmission dédiée.

Git : worktree`codex/m015-m` basé surmain réel3e14710 ; commits propres vers
`HEAD:refs/heads/main`, aucune fusion/rebase/reset/force. Rapport final et
preuves préparés pour push normal ; leur relecture distante reste à consigner.
BLOCKED est **technique**, fenêtreFIFO512/XYZ non qualifiée ; aucune validation
intermédiaire utilisateur attendue pour terminer la publication.
