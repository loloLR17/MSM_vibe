---
mission_id: TR2-20261029-R3-DURABLE-FAT32-10MIN
status: DONE
base_ref: main
base_sha: ffecc3ee372814f9ed5f3ad18d851b62582f7da0
initial_head: 181441dca87403ac1886c62068390584efa9496e
result_sha: 6fdae042333f671629d2c4626d7120b0bd848ae3
created_at_utc: 2026-10-29T22:45:50.679104+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-DURABLE — 10 minutes réelles et fichiers FAT32 intégralement vérifiés sur PC

**D0–D4 satisfaits dans la portée de cette mission expérimentale.** Les trois
fichiers R2N de 2 s, 60 s et 600 s sont réellement présents sur la microSD retirée du banc,
montés dans le lecteur natif du T1000, copiés et validés indépendamment en Python.
Tout le payload de chacun correspond au digest producteur **avant framing/file/TX**,
avec CRC/séquences/comptes/temps/footer vérifiés. L’ancien essai interrompu à 265 s et
son préfixe incomplet restent conservés. **Livraison DONE dans la portée de cette
mission**, après publication et relecture distante complète. La revue indépendante
A-01 par ChatGPT reste distincte.

**Portée bornée** : profil candidat XYZ/DC/FIR Q15 à 107 coefficients, /10 ≈ 2 667 Hz, pas adoption V1-05/2000Hz,
qualification 8 h, FAT tolérante aux coupures, amplitude calibrée, FRAM/RTU ou AC1 Windows.

## Résultats physiques et extraction PC

| Fichier réel | Durée observée µs | XYZ source | SortiesXYZ / payloadB | Records512 / groupes | FIFO/file pic | STOP/drain payload µs |
|---|---:|---:|---|---|---|---|
| R2N0003.TR2 | 2000427 | 52592 | 5260 / 31560 | 68 / 9 | 48 / 5120B | 152 / 9224 |
| R2N0004.TR2 | 60000258 | 1585488 | 158549 / 951294 | 2033 / 255 | 49 / 5120B | 152 / 14025 |
| R2N0005.TR2 | 600000520 | 15856920 | 1585692 / 9514152 | 20330 / 2542 | 49 / 5632B | 151 / 11733 |

Toutes les captures groupées : aucune erreur source/temps/STOP/SD/overflow,
flags FIFO/OVR à 0, rails/clipping0 et 11 sorties WARMUP.1/7/8XYZ en attente sans ancre
future au STOP (2 s / 60 s / 600 s) ne sont pas extrapolés. Un seul propriétaire FIFO, FifoLive,
ancres DEC8 et index validés, même FIR causal107tap Q15/10 que R1 ; aucun signal
nominal fabriqué ni suppression DC. Index de sortie i→source10*i, segment/run explicites.

PC : volume F: FAT32, lecteur RealtekPCIECardReader/MBR, capacité physique31914983424B
(62333952×512), clusters32768B. TR2INFO.TXT (343 octets) lu intégralement avant la première
capture (SHA71e626c7e69644ff7aec1047fbb0417bc9ff80b6ec21a47659d68e70fbd11553),
puis de nouveau à l’extraction finale. **Ce sont des accès à un volume et des
fichiers ordinaires réellement montés**, pas une image simulée ni un header RAW.
Les commandes Codex PC sont de lecture/copie, sans rawdisk/format PC/élévation/SAC.

- **R2N0003.TR2** : fichier entier 131072B copié ; SHA fichier `99f3309864fd4304e6fca88c292c9b388cd6b4395eccac7b320f17644923ae79`. Payload **31560B**, SHA **`eef352a6d5c79da7b995baa84f7b37486ae358505fbe9cc5a739ef51dbd57122`**, tous les octets conformes au contrôle amont.
- **R2N0004.TR2** : fichier entier 2097152B copié ; SHA fichier `95244f0f06f8f21428c25d807af384ec498a1c0a6c4e3da80b2ad369b0f94085`. Payload **951294B**, SHA **`ae0ca9be38047a1448ecb2c90cafbb8500e6ab537d74c7a4584fc5e5526e4d6b`**, tous les octets conformes au contrôle amont.
- **R2N0005.TR2** : fichier entier 12582912B copié ; SHA fichier `2aec9e516f9eec4b838180f880782643bf599d9a606af86cb299a37ab3b4e425`. Payload **9514152B**, SHA **`65d349911b489ec05438bcbd3478f380505863722405c3904cc074bb122a74fc`**, tous les octets conformes au contrôle amont.

Header/version/provenance/CRC32, chaque bloc et son ordre/CRC32, compte du footer,
SHA256 complet et identité de session sont vérifiés. Les 128 sorties préfixes du
fichier PC correspondent aux sorties capturées  ; 103 sorties de chaque préfixe sont
indépendamment recalculées par convolution entière Python depuis 1 024 entrées réelles.
La totalité des sources natives à environ 26 kHz n’est pas conservée : **le rejeu FIR numérique
indépendant porte sur le préfixe**, la comparaison de tous les octets persistés sur
le digest producteur séparé en amont de la file/TX. Aucun de ces niveaux n’est
présenté comme l’autre. Les trois anciens RUN0003/4/5 ont été re-hachés sur la carte
et restent byte-identiques aux copies PC du premier cycle, y compris l’incomplet.

## Distributions, marges, mémoire et IRQ sur600s

| Mesure | Moyenne µs | P95 µs | P99 µs | Maximum µs | Référence µs |
|---|---:|---:|---:|---:|---:|
| work | 308.654 | 497 | 511 | 704.331 | 1000 |
| service | 706.817 | 1192 | 1226 | 1396.881 | 2500 |
| gap | 962.799 | 1447 | 1481 | 1653.000 | 5000 |
| sd_step | 51.647 | 265 | 431 | 430.600 | 1000 |
| per_sector_busy | 1187.156 | 2000 | 3000 | 5495 | — |
| FD_stop_busy | 2796.905 | 3000 | 6000 | 20387 | — |
| Groupe write | 82266.044 | 90000 | 90000 | 103280 | — |

623 182 services : sommes et maxima en cycles exacts ; quantiles work/service/gap/
SD step via histogrammes ceil1µs couvrant **toutes** les observations. Busy observé de
20 330 secteurs et 2 542 stops FD : histogrammes ceil1ms, moyenne/max exacts. Durée de
2 542 groupes : moyenne/max exacts, quantiles ceil10ms ; seulement premiers 256 et
derniers 1 792 groupes bruts conservés. La trace bornée n’est ni un plafond de durée ni
un faux journal exhaustif. Les durées groupées incluent les tours FIFO coopératifs,
**pas un blocage du CPU** pendant toute cette durée. Télémétrie RAM version 2 compte
des groupes, pas des secteurs ; les tableaux de busy par secteur et de stop sont séparés.

Références 1 000 / 2 500 / 5 000 µs inchangées ; marges maximales observées work295,669µs,
service1103,119µs etgap3347µs, non sommables et **pas WCET industriel**. Drain mesuré
porte sur les records ; latence de toute l’écriture du footer non instrumentée
séparément. Son acquittement complet et la quiescence sont vérifiés.

600 000 SysTick observés en 600,000520 s ; masques PRIMASK/BASEPRI/FAULTMASK0 au boot.
5 968 contrôles temporels, résidu maximal 42 µs sous la garde de 100 µs. Calibration initiale : 193 brackets /
6 577 ancres / résidu de 30 µs. Temps DWT au CPU configuré à 160 MHz, **pas calibration SI ni UTC**.
Le lecteur distingue la fraction de cadence issue de la calibration initiale du
débit observé entre les extrémités temporelles du fichier ; les temps par échantillon internes à un
bloc sont **interpolés approximativement** entre ses extrémités temporelles DEC8 exactes.
Pour le fichier de dix minutes : fréquence issue de la calibration initiale
2 643,984952 Hz, fréquence observée entre les sorties extrêmes **2 642,868094 Hz**.
Aucune de ces valeurs n’est présentée comme 2 000 Hz normatifs ou 2 666,7 Hz exacts.

Image 6fdae042 : text57428/data20/BSSsize543104B (réserveslinker incluses), file de 64 Kio,
pile de 4 Kio peinte : profondeur empirique2264B/plancher1832B, hors démarrage et garde de 256 octets.
ELF/MAP publiés ; pas addition aveugle des grosbuffers de deux anciens bancs. Source
câblage SPI3 PC9..12, SPI2 PB13SCK/PD3MISO/PD4MOSI/PF12CS,3V3, USB natif débranché.
SPI3mode3/8MHz etSPI2mode0/4MHz relus parCFG/MBR, quanta64motsFIFO/128B SD,
attente de 250 µs inchangés. La fraction du temps dans service ne constitue pas une
mesure exhaustive de chargeCPU, notamment l’attente250µs est active.

## Échec initial, cause et correction conservés

Image initiale fd73fa1d (CMD24/secteur) :2 s et 60 s réussis, puis essai de 600 s interrompu à
265,121081s par **file de 64 Kio pleine**, aucuneOVR niSDerror.700830sorties produites,
8857records confirmés ; trente attentes busy>100ms groupés, busy155539µs/write164448µs. Le
backlog augmente sur la grappe, pas à cause d’un blocageFIFO ; work995,538/service
1704,669/gap1961µs sousréférences. Le dernier bloc est refusé avant écrasement,
STOP150µs, SD quiescente, **aucun footer de succès**. Fichier extrait physiquement sur PC :
INCOMPLETE,690846XYZ/4145076B préfixeCRC/session/index valides.9984sorties restantes
ne sont pas prétendues archivées ; le SHA de ce préfixe **n’est pas** comparé au
SHA de toutesles700830sorties. Contrôles numériques103 et préfixePC128 obtenus.

Correction minimale : ACMD23/CMD25 coopératif par1..8secteurs préalloués, tokenFC,
4quanta128B/secteur, busy par unbyte/tour, tokenFD/stopbusy puis CMD13. Groupe entier
emprunté/chargé jusqu’àACK ; STOP du capteur termine seulement le groupe déjà emprunté.
PositionCMD25 ambiguë surI/O : sourcearrêtée/powercycle exigé, **pas de retry ni
commande devinée**. CRC32 nibble exactementidentique à IEEE/zlib réduit le coût du
callback. Aucunagrandissement de file ni assouplissement des contrôles. Les nouveaux
nomsR2N préservent tous les anciens fichiers et le formatversion1 reste indépendant
du transport. Pas de reformatage pendant cette correction.

La nouvelleméthode est démontrée jusqu’à 600 s sur ce média et ces régions. Son succès
ne prouve ni une borne universelle des pausesSD ni un effet isolé de chaque
modification (histoireFTL et zones différentes). Une grappe arbitrairement longue
peut encore saturer64KiB et doit rester un défaut explicite, jamais une perte masquée.

## D0/D1/D2/D4, format et récupération

FatFs officiel ST R0.16/ST4.1.0, upstream dd9126e957ae841a13ccbe2115d39a1fb7414126,
licences originales et empreintes PROVENANCE conservés. SDKCubeU5v1.9.0 sans middleware FatFs peuplé,
pas de reprise du driverSDMMC/USB communautaire. FAT32 deuxFATs/32KiBclusters : une
seule initialisation autorisée sur carte test exacte, après lectureCID/capacité/
ancienmarqueur et protection des preuvesRAW GitHub. TR2-001 jamais sollicité.

f_expand réserve une chaîne contiguë, chaque cluster vérifié, f_sync/f_close
persistent métadonnées et taille **avant capture**. Aucune opération FAT pendant le FIFO,
seuls secteurs du fichier fermé écrit via writercoop ; footer réservé horspayload.
Header magic/version/UID/CID/GitSHA/coeffSHA/FS±8g logique/244µg parcompte/fraction
de cadence/session/UTCunknown. Blocs78XYZint16/DC avecCRC/index/temps/qualité ;
footer complet contient compte/digest producteur/raisonSTOP et n’est écrit qu’après
succès. File taille préallouée128KiB/2MiB/12MiB **ne signifie pas donnéesvalides**.
FlagsWARMUP/rail/clip/temps et pendingSTOP explicités. FATdatesfixes2025 ne sont pas
une vraie UTC. Sémantique B0–B7/FRAM/LIVE R0 inchangée.

Tests hôte réels FatFs+writer sur un **fichier disque modèle**, pas disquePC :
format/mount/expand/sync/close/remount/f_read, C→Python indépendant,28négatifs+
reconstruction/CSV ;80STOP(8tailles×10états), I/O potentiellement acceptée sansretry,
framing20000XYZ etbusy150/500ms,19×151054µs (pic47616B) ;19×500ms déborde64KiB
explicitement. Allocation FAT effectivement pleine, nomdéjàprésent, CIDerroné
refusés. Métadonnées/header/footer tronqués et pertes simulées ne créent pas de
faussecomplète. Les défauts source/temps gardent les chemins DEC8/FIR failclosed.
Les erreurs de compilation/setup intermédiaires ont été corrigées d’après les
sorties et leurs logs gardés ; seuls résultats finauxréussis sont retenus.

ARM dédié etcanonique cross-buildPASS ; ciblésproportionnés à ce harness isolé.
Cœur scientifique inchangé/anciens150tests non rejoués ; brancheSdWriter par défaut
préprocesseuridentique18438B, aucunevalidation affaiblie. Lecteur Python portable,
CRC/indices/compte/hash/version/corruption/troncature et préfixes, extractionCSV
optionnelle surPC avec temps interpolés et valeurs g nominales explicites ; pas de nouvelle DLL .NET.

Récupération garantie dans le modèle de lecture : préfixe de blocs CRC/séquence/
session valides ; footer absent →INCOMPLETE, une complète corrompue n’est pas
artificiellement tronquée pourréussir. **L’arrêtcontrôlé265s fournit une preuve
physique de préfixe**, pas de panne d’énergie en écriture. Une préparation FAT
interrompue peut laisseraucunfichier ou allocationorpheline. CMD13/sync/CRC ne
qualifient pas le cacheFTL : survie après disparition d’énergie/perte maximale
physique restent inconnues. Aucun retrait à chaud ou coupure volontaire en écriture.

[Format, procédures, références ST/SDA](../../../experiments/r3_durable_fat32/README.md).
[FatFs officiel et provenance](https://github.com/STMicroelectronics/stm32-mw-fatfs/tree/dd9126e957ae841a13ccbe2115d39a1fb7414126).

## Traçabilité, état final et publication

Code réellement flashé/évalué : **6fdae042333f671629d2c4626d7120b0bd848ae3**. BIN57448B SHA256
**27f5d59fc58803d6a7fd66714014ca7d24da60023736386b3375b9272fbf9ef6** ; ELF
a00c0b0ae76b23e5fc5719b1d3163791d1882772663014ec3fa674ecf5ac9b50. Tests tailles/STOP ajoutés e9cccacb,
lecteur/extraction améliorés séparément ; aucunemodificationfirmware après ces
qualifications. Deuxflashsstandards mission, verify/readbackexacts, zéroresetlogiciel
supplémentaire ; pas de mass erase/OptionBytes/RDP/FRAM, imagesR1/R2/R3 etbackupFlash
initial conservés, pas de sauvegardes/restaurations répétitives.

CibleTR2-002 exclusivement, SN0032001F3235510C37333439/UID001B0046-38425018-20333157,
WHO7B/CTRL1AC dans le profil±8g ; CIDSPI03534453-44333247-858BF092-3C01927B, CSD et
capacité relus aux transitions. Aucun accès à TR2-001, COM4, serviceWindows ou média
production ; leur état actuel n’est pas supposé RUNNING. Interventions physiques
par l’utilisateur, à chaquefois après STOP du capteur/SDquiescence etdebugdétaché.

Dernier état logique avant retrait : run5/stage4/error0, FIFOarrêté/SDwriterdone,
CShaute, aucun démarrage ni écriture automatiques. L’utilisateur a ensuite confirmé la carte au lecteur
PC après la demande de retrait hors alimentation ; l’alimentation du banc n’a pas
été recontrôlée après ce geste. Image groupée conservée enFlash ; ne pas réinsérer
àchaud ou relancer les campagnes acquises. Aucun flash ou restauration supplémentaire
nécessaire. Carte FAT32 enétat test, anciens fichiers conservés, pas média production.

Worktree `MSM_vibe-R3-durable`, branche `codex/r3-durable-fat32-10min`, publication
normale `HEAD:refs/heads/main` avec contrôle distant. Main historique 82169af et autres
worktrees/stashes6251b988/f3b829e6 préservés ; aucunreset/clean/merge/rebase/forcepush.
Journal technique actualisé : fichiersPC, échec265s, correctiongroupée,600s etlimites.
Les preuves sont en `preuves/TR2-20261029-R3-DURABLE-FAT32-10MIN` : checkpoint-d0-pc,checkpoint-short,
checkpoint-negative-265s,checkpoint-group-short,checkpoint-group-600s et
[final-PC](preuves/TR2-20261029-R3-DURABLE-FAT32-10MIN/final-pc/BUNDLE.json). Chaquezlib porte taille/SHAoriginal et
stocké ; vraisfichiersPC complets, imagesELF/BIN/MAP, bruts et outils inclus. Preuves
locales `/home/lolo/dev/msm/reports/TR2-r3-durable-fat32-10min/`.

Suite : revue indépendante A-01 ChatGPT, puis décision sur l’endurance progressive.
Aucune qualification 1 h / 8 h / coupure / FAT journalisée /
FRAM/RTU/AC1 Windows, calibration amplitude±8g ni gelnormatif2000Hz revendiqués.

## Publication et relecture distante obtenues

Le bilan **`f4236b40d98cc7778e8824cd5270dfe3aef4f3d8`** a été récupéré depuis GitHub
et relu intégralement à **2026-10-29T22:48:55.782868+00:00** : **489 fichiers
byte-identiques, 453 artefacts décompressés et hachés**, y compris les fichiersPC
entiers, images et traces ; empreintes du middleware ST original conformes. Les
conclusions d’intégrité PC reposent sur les lectures effectives décrites plus haut,
et ces mêmes octets ont été vérifiés après publication. Aucun nouvel essai, flash,
reset ou validation déjà acquise n’a été rejoué pendant cette clôture documentaire.

[Preuve de relecture distante](preuves/TR2-20261029-R3-DURABLE-FAT32-10MIN/REMOTE_REREAD_f4236b40.json).
Archives MISSION/RAPPORT créées à l’identique sans écrasement d’une archive.
Worktree propre après le bilan provisoire ; le commit de clôture porte uniquement
statut, journal, preuve de relecture et archives. Son SHA final sera fourni après
push et dernière relecture, distinct du SHA firmware évalué **6fdae042**.
