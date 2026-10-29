---
mission_id: TR2-20261029-R3-DURABLE-FAT32-10MIN
status: IN_PROGRESS
base_ref: main
base_sha: ffecc3ee372814f9ed5f3ad18d851b62582f7da0
initial_head: 181441dca87403ac1886c62068390584efa9496e
result_sha: fd73fa1dc74c29093e3f094cca0fff2fd319a9b6
created_at_utc: 2026-10-29T20:25:12.340680+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-DURABLE — checkpoint logiciel + première lecture FAT32 réelle sur PC

**IN_PROGRESS, pas DONE.** D0/D1 préparés et validés sur les portes ci-dessous ;
D2 tests ciblés réussis ; première preuve PC standard acquise. Les acquisitions2s et60s ont maintenant réussi ; la fenêtre600s est en cours, sans
debugger interrompant la capture. **Extraction PC des campagnes encore à faire**.
La première lecturePC du fichier témoin ne qualifie pas seule les trois campagnes.


## Acquisition physique courte acquise / 600 s en cours

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

Dernière confirmation physique : carte réinsérée TR2-002/ST-LINK connecté/USBnatif
débranché. Contrôles frais passés ; commande600s soumise après60s sain. Aucune
lecture/halt supplémentaire pendantcapture. Attendre le terminal STOP/SDquiescente
et analyser les bruts, puis demander le retrait sûr verslecteurPC et copier/importer
RUN0003/4/5.TR2. Aucune répétition des courtes captures, aucunformat/reflash gratuit.
Le helperGDB600s a timeout750s ; preuve/local log dans d3-file-600s.log.
Si interruption : vérifier processus/logs avant toute action ; ne jamais attacher
un nouveau debug pendant une capture encore active. Images/recovery conservés.

600s et intégritéPC intégrale des campagnes restent NONQUALIFIÉS tant que non
observés. SDlong : quantiles sur histogrammes complets, pas sur seulement2048traces
(premières256/dernières1792). Ne pas confondre BSS de banc et futur budgetproduction.
Pas de retraitàchaud/coupureprovoquée, pasqualification8h/FRAM/RTU/Windows/AC1/amplitude.

Sources/worktree : `/home/lolo/dev/msm/projects/MSM_vibe-R3-durable`, branche
`codex/r3-durable-fat32-10min`, pushnormal explicite surmain. Dépôthistorique82169af,
anciensworktrees etstashes6251b988/f3b829e6préservés ; aucunmerge/rebase/reset/clean.
Preuveslocales `/home/lolo/dev/msm/reports/TR2-r3-durable-fat32-10min`.
[Format et workflow](../../../experiments/r3_durable_fat32/README.md).
[Manifeste des preuves](preuves/TR2-20261029-R3-DURABLE-FAT32-10MIN/checkpoint-d0-pc/BUNDLE.json) : 103artefacts
zlib, tailles/SHAoriginaux etstockés ; l’image sparse32Go du modèle n’estpas publiée.
Journal technique actualisé. Publication et relecture distantes du checkpoint à
faire ; mission continue, aucun DONE physique anticipé.
