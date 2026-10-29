---
mission_id: TR2-20261029-R3-DURABLE-FAT32-10MIN
status: IN_PROGRESS
base_ref: main
base_sha: ffecc3ee372814f9ed5f3ad18d851b62582f7da0
initial_head: 181441dca87403ac1886c62068390584efa9496e
result_sha: 6fdae042333f671629d2c4626d7120b0bd848ae3
created_at_utc: 2026-10-29T20:25:12.340680+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-DURABLE — checkpoint FAT32 : fichiers PC courts validés, arrêt long sur grappe SD

**IN_PROGRESS — 600secondes réelles réussies sur imagegroupée6fdae042.**
15856920XYZ sources →1585692sorties/9514152B utiles,20330records512/2542groupes,
zéroOVR/SD/file/source/temps/rail/clip. Capture600,000520s, STOP151µs etdrainpayload
11733µs, footercompletacknowledged/SDquiescente/debugdétaché.
Work704,331/service1396,881/gap1653µs sous1000/2500/5000, filepeak5632B/FIFO49,
pile2264B/plancher1832B,600000SysTick/5968contrôleshorloge/résidu42µs.
**La vraie extractionPC finale desR2N reste à effectuer avantDONE.** Retrait vers
lecteurPC aprèsarrêtsûr demandé àl’utilisateur ; attendre sa confirmation.
Aucune répétition du600s réussi, aucunformat/reset/reflash supplémentaire.
[Preuves600s](preuves/TR2-20261029-R3-DURABLE-FAT32-10MIN/checkpoint-group-600s/BUNDLE.json).

## Correction minimale et résultats intermédiaires nouveaux

ACMD23+CMD25 par1..8secteurs, quanta128B, busy coopératif, stopFD+CMD13,
charge/emprunt du groupe entier jusqu’àACK. **64KiB inchangés** ; aucuneinflation de
file pour cacher lesgrappes. NouvelleCRC32 nibble identique àzlib réduit lecoûtdu
callback ; FIR/protocoleV1 inchangés. FichiersR2N0003/4/5 distincts, pasreformatage
ni effacement desRUN0003/4/5. UnepositionCMD25 incertaine exige arrêt et powercycle,
aucun retry/commande devinée.80cas STOP(8tailles×10états) et modèlesframing/busy
150/500ms,19×151054µs(peak47616B),19×500ms(refus65536B) PASS,29caslecteur dont
28négatifs+CSV/reconstructionPASS, ARM/canoniquecrossPASS ; ce sont des modèles.
BSS543104B incluant télémétrie accrue, pas empilement de bancs.

Calibrationnouvelle30µs/193brackets/6577ancres, UID/CID/mount/Flash vérifiésaprès
réinsertionconfirméeutilisateur, aucunformat. Baseline2s saine.

| Nouveau fichier | SourceXYZ | Sorties | PayloadB | Groupes / secteurs512 | FIFO/file pic | Maxwork/service/gap µs |
|---|---:|---:|---:|---|---|---|
| group-file-2s | 52592 | 5260 | 31560 | 9 / 68 | 48 / 5120B | 661.812 / 1362.787 / 1619.000 |
| group-file-60s | 1585488 | 158549 | 951294 | 255 / 2033 | 49 / 5120B | 704.256 / 1387.606 / 1644.000 |

ZéroflagsOVR/SD/source/temps/queuefaute,103sortiespréfixe indépendantes exactes.
2s/60s footercomplet et relectureFatFsdufichierprécédent avantpalierlong.
Sur60s busyparsecteur max5462µs/FDstop5384µs, STOP152µs/drainpayload14025µs.
Les statistiquesSD RAM version2 comptent desGROUPES ; leshistogrammesbusy réels
parsecteur etstop sont séparés. Ne pas comparer abusivement leurs sommes aux
anciennes latences unitairesCMD24. Références1000/2500/5000µs inchangées.

Le600s a terminé normalement aprèsune fenêtre sansdebughalt. Encasd’interruptionfuture,
vérifier logs/processus avant toutattachement ; pas de répétitioncodeinchangé.
Retraitphysiquesûr demandé pourcopierles3R2N*.TR2
depuislecteurPC et confronter touslesoctets auSHAproducteur. Lesanciensfichiers
seront égalementhachés pourvérifier leurpréservation.
[Preuves groupées](preuves/TR2-20261029-R3-DURABLE-FAT32-10MIN/checkpoint-group-short/BUNDLE.json).

Les sections suivantes conservent l’historique initial fd73 et sonnégatif :

## Acquisition physique courte acquise / échec long conservé

Réinsertion confirmée utilisateur ; UID/imageFD73 de nouveau contrôlés, aucunflash
oureset logiciel supplémentaire. MountFAT32 réel au reboot, pasnouveauformat ;
calibration63µs/193brackets/6576ancres puisbaseline2s saine.

| Capture réelle | XYZ sources | Sorties | Payload B | Records512B | FIFO/file pic | Max work/service/gap µs | STOP/drain payload µs |
|---|---:|---:|---:|---:|---|---|---|
| d3-file-2s | 52592 | 5260 | 31560 | 68 | 57 / 512B | 841.013 / 1646.894 / 1903.000 | 151 / 3042 |
| d3-file-60s | 1585704 | 158571 | 951426 | 2033 | 57 / 1024B | 841.850 / 1681.744 / 1938.000 | 152 / 3040 |

ZéroOVR/flags/source/temps/SD/overflow/rail/clip, footer completacknowledged aprèsSTOP,
103sorties de chaquepréfixe recalculées indépendamment. SD60s write/busy max32711/23186µs ;
pileobservée2264B/plancher1832B. Le digest et tous les records du2s ont été relus
parFatFs avant60s ; même garde relit le60s avant600s. Ces relectures cible ne sont
pas présentées comme les extractionsPC finales. Le drain mesuré porte sur les
records, pas sur la latence de toute l’écriture dufooter (noninstrumentée séparément).
[Preuves courtes](preuves/TR2-20261029-R3-DURABLE-FAT32-10MIN/checkpoint-short/BUNDLE.json).

## Acquis et architecture

Projet isolé `experiments/r3_durable_fat32` : FatFs officiel ST R0.16/ST4.1.0,
commit upstream dd9126e957ae841a13ccbe2115d39a1fb7414126, fichiers/licences/hashes
conservés dans PROVENANCE.json. Aucun FAT/SDMMC/USB de l’ancien PoC copié comme
pilote. Nouveau diskio SPI2 utilise le writer coopératif R3 et ses contrôles CRC,
IO incertaine/no retry. Opt-in `R3_DURABLE_CARD_PROFILE` pour la seule carte test,
préprocesseur par défaut SdWriter byte-identique18438B ; production/FIR inchangés.

Préallocation **contiguë**, vérifiée via FatFs, f_sync/f_close et header/footer
initialisés **avant le FIFO**. Aucune opération FAT pendant l’acquisition ; écriture
directe dans les seuls secteurs du fichier réservé. File64KiB inchangée ; bloc512
contient78vraisXYZ/468B plus identifiant session, séquence, index, timestamps et CRC.
Header versionné avec UID/CID/GitSHA/coeffSHA/config±8g/244µg par compte/cadence
rationnelle réelle/UTC inconnu. Footer complet uniquement après STOP/drain/succès,
avec digest producteur calculé avant framing/file/TX. Données XYZ/DC conservées.
Format **candidat NON-PRODUCTION**, aucune adoption implicite du /10 en V1-05 2000Hz.

Lecteur Python portable indépendant : versions inconnues, CRC, index, compte,
temps, digest final, complète/incomplète/corrompue. Les temps internes au bloc sont
une **interpolation approximative** des endpoints DEC8 exacts, pas une datation
exacte revendiquée pour chaque sample ni une UTC. Le préfixe récupérable correspond
aux blocs CRC/séquence/session valides ; un bloc de queue déchiré sans footer ne
fabrique pas un succès. La survie physique après perte d’alimentation et la perte
maximale restent **inconnues** (FTL/cache/FAT non journalisée), même après CMD13/sync.
La préparation FAT interrompue peut ne laisser aucun fichier ou une allocation
orpheline ; aucune source n’est capturée pendant cette phase.

Fichiers préparés par commandes RAM explicites : RUN0003.TR2 128KiB/2s,
RUN0004.TR2 2MiB/60s, RUN0005.TR2 12MiB/600s. Noms existants refusés, pas d’autorun.
Avant60s puis600s : relecture complète du fichier précédent via FatFs, CRC/index/
comptes/digest confrontés au producteur, FIFO arrêté. Après le600s, retrait sûr et
**relecture PC indépendante des trois fichiers** obligatoire pour DONE.

## Résultats logiciels réellement obtenus

- Deux tests ciblés C/Python PASS. Le modèle C utilise le vrai FatFs et vrai writer
  sur un fichier disque sparse32Go **hôte, pas un disque physique PC** : format,
  mount, expansion contiguë, sync/close, remount et lecture standard du fichier.
- Fixture produite en C,78XYZ vérifiés séparément en Python,28cas négatifs de
  troncature/header/footer/CRC/version/index/temps/empreinte. Complète et préfixe
  incomplet distingués. Ce n’est pas une coupure électrique réelle.
- STOP aux6états writer, CRC potentiellement accepté→latchincertain/refus sansretry.
  Modèle encadré78XYZ par bloc,20000sorties exactes : busy150ms pic3072B,
  busy500ms pic9216B ;19×151054µs pic42496B sansperte ;19×500ms file65536B/refus
  explicite, jamais un PASS d’acquisition sans perte. Ce sont des simulations.
- Allocation FAT réellement saturée dans le modèle, puis refus de préallocation ;
  doublon de nom et mauvaisCID refusés avantcapture. Défauts source/temps restent
  failclosed par les propriétaires DEC8/FIR R3 réutilisés ; anciennes preuves R3
  ne deviennent pas des essais physiques du nouveau format.
- ARM dédié +canonique `tr2_validate.sh --cross-build-only` PASS. Validation ciblée
  justifiée : banc isolé, cœur scientifique/protocoleinchangés ; anciens150tests
  et captures R3 non rejoués. Image text55352/data20/BSSsize527312B,
  `_ebss=0x200801dc`, pile peinte4KiB ; pas de garantie WCET à ce stade.
- Erreurs d’assemblage hôte initiales (objets cooperative_work/queue manquants),
  puis préparation du test remount invalide ont été corrigées d’après les sorties ;
  seuls logs finaux réussis sont retenus. Whitespace upstream conservé via attribut
  vendor local, empreintes originales contrôlées ; pas d’erreur de test masquée.

## Observations physiques D0 et véritable PC

TR2-002 uniquement : SN0032001F3235510C37333439 et UID001B0046/38425018/20333157
relus avantflash/debug. Préfixe43768B de l’ancienne image R3 exact192db2…f127,
retours R1/R2/R3 et backup initial préservés. **Unflash standard** de
`fd73fa1dc74c29093e3f094cca0fff2fd319a9b6`, verify puis readback55372B exact :
BIN **cf47853f6b4fcf7e43641fda72c8e578ddfeaac431974f2438f7c3c51982d697**,
ELF **d56fd3bc813edc9381da6b4b2b026341e56d88ebaaf6a0a150580dfc9c764437**.
Aucun mass erase/OB/RDP/reset supplémentaire ni accès TR2-001/FRAM/COM4/serviceWindows.

WHO7B, CID03534453/44333247/858BF092/3C01927B, CSD400E0032/5B590000/EDC87F80/0A4040C3,
capacité62333952secteurs512 et ancienmarqueurR2 réellement relus. Nouvellecalibration
2s sanswriter :193brackets/6576ancres/résidu48µs<100. Aucun GDB pendantcalibration.
Formatage autorisé effectué : **FAT32, deuxFATs, clusters64secteurs/32768B**,
TR2INFO.TXT créé,15294écritures de secteur comptées, error/IO/incertain0 ; FIFOarrêté,
SDquiescente/CS haute, GDBdétaché. Les anciens secteursRAW ne sont plus considérés
comme protégés surcarte ; **toutes leurs preuves GitHub R3 sont conservées**.

L’utilisateur a confirmé la carte dans le lecteur PC et le banc hors tension.
**Windows monte réellement F: en FAT32** (Healthy, allocationunit32768B), disque1
RealtekPCIECardReader/MBR, taillephysique **31914983424B** exacte. Le fichier
TR2INFO.TXT est lu intégralement par File.ReadAllBytes Windows, **343B**
identiques au textefirmware ; SHA256 **71e626c7e69644ff7aec1047fbb0417bc9ff80b6ec21a47659d68e70fbd11553**.
Pas seulement une image FAT simulée ni une lecture de secteurs GDB. Opérations PC
lectureseule, aucunformat/écriture/élévation/SAC modifié. FATdatesfixes2025 ne sontpas
présentées comme une vraie horlogeUTC.

## Point de reprise et critères non acquis

Dernière confirmation physique : carte dans le lecteurPC, TR2-002 hors tension,
après arrêt logique source/SDquiescence/sessiondebugfermée. Imagefd73 conservée,
aucun nouveauflash/reset depuis le préflight. Ne pas relancer les captures courtes
validées sur codeinchangé et ne pas reformater ; conserver RUN0003/4/5.TR2.

## Première anomalie longue et intégrité PC obtenue

À265,121081s, queue65536B/overflow1, FIFOerror10(refusconsommateur), FIFOOVR0/flags0/
pic58, SDerror0, STOP150µs, writer terminé sansincertitude ; pas de footercomplet.
7008291XYZ reçus/700830sorties/4204980B produits ;8857secteurs512 confirmés,
4534784B encadrés. Le dernier bloc refusé demeure explicitement nonpersisté.
Trente busy>100ms dans la trace conservée sont groupés en fin, LBA29880..29913 ;
busymax155539µs/write164448µs. La file augmente par pas de≈2048B entre ceswrites.
MoyenneSDglobale ne suffit pas à expliquer cette grappe ; leshistogrammes couvrent
les8857writes, la tracebrute conserve seulementpremiers256/derniers1792.
Workmax995,538µs/service1704,669/gap1961 sousréférences1000/2500/5000, margework
faible mais nonfranchie. La cause d’arrêt est le backlogSD, pas un timeoutFIFO.

L’utilisateur a transféré la carte aulecteurPC après misehors tension. Depuis le
volumeFAT32 F: réel, les fichiers entiers128KiB/2MiB/12MiB ont été copiés en lecture
seule, parsés parPython et tous leurs blocs valides contrôlés.

| Fichier PC réel | Statut | XYZ validés | Payload B | SHA256 payload |
|---|---|---:|---:|---|
| RUN0003.TR2 | COMPLETE | 5260 | 31560 | dc885198cadde6f264fb4a4ea1e98b5026d2512efef7d9fce4365807c61ae7fe |
| RUN0004.TR2 | COMPLETE | 158571 | 951426 | 6286345f03e82fb9a56d1b875e20465af57bd0402ac059c0f683ca36ed8dc4b9 |
| RUN0005.TR2 | INCOMPLETE | 690846 | 4145076 | 29b367b99f39fd35a462c75cc3d2d10adf41f5cbbd7e64dfc64b3766b4514900 |

Les2s/60s correspondent intégralement au digest producteur amont capturé avantTX ;
103sorties indépendamment recalculées et128sorties préfixes réelles identiques au
fichier pourchacun des3runs. Pourlepréfixe incomplet seulement CRC/session/index et
cescontrôlespréfixes sont prouvés : son SHA est celui du préfixe effectivementrelu,
**pas le digest de l’intégralité des700830sorties**. Les9984sorties restantes ne
sont pas présentées commearchivées. Cela prouve la récupération après cetarrêt
contrôlé surbackpressure, pas la survie à une coupureélectrique.

Poursuite : examiner une méthode d’écriture groupée préallouée/cooperative avec
hints SD (CMD25/ACMD23) plutôt qu’augmenter sansborne les buffers ; conserver
64KiB, lescontrôles et leslimites. Aucun correctifphysique nouveau encoreévalué,
aucune garantie future de file bornée face à despauses arbitraires.
[Preuves du négatif et fichiers PC](preuves/TR2-20261029-R3-DURABLE-FAT32-10MIN/checkpoint-negative-265s/BUNDLE.json).

Sources/worktree : `/home/lolo/dev/msm/projects/MSM_vibe-R3-durable`, branche
`codex/r3-durable-fat32-10min`, pushnormal explicite surmain. Dépôthistorique82169af,
anciensworktrees etstashes6251b988/f3b829e6préservés ; aucunmerge/rebase/reset/clean.
Preuveslocales `/home/lolo/dev/msm/reports/TR2-r3-durable-fat32-10min`.
[Format et workflow](../../../experiments/r3_durable_fat32/README.md).
[Manifeste des preuves](preuves/TR2-20261029-R3-DURABLE-FAT32-10MIN/checkpoint-d0-pc/BUNDLE.json) : 103artefacts
zlib, tailles/SHAoriginaux etstockés ; l’image sparse32Go du modèle n’estpas publiée.
Journal technique actualisé. Publication et relecture distantes du checkpoint à
faire ; mission continue, aucun DONE physique anticipé.

## Relecture du checkpoint et reprise finale

Le checkpoint9e5cfc5d8ee45a8e52d7bbc27640fba8f78419b7 a été récupéré et relu
intégralement à2026-10-29T22:40:23UTC :466fichiers exacts,433artefacts décompressés
et hachés, empreintesvendorpristines conformes. Aucune qualificationPCfinale
déduite de cette publication. L’unique dépendance restante est le transfert
physique de la carte verslecteurPC, demandé àl’utilisateur aprèsquiescence.
[Point de reprise et outils](preuves/TR2-20261029-R3-DURABLE-FAT32-10MIN/await-final-pc/REPRISE.md).
Les scripts de finalisation **ne sont pas encore exécutés** et exigent les preuves
PC des troisR2N. Ne pas rejouer les validations/acquisitions acquises.
