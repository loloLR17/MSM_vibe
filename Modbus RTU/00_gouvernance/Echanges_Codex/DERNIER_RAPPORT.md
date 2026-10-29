---
mission_id: TR2-20261029-R3-COEX-FIFO-FIR-SPI2
status: IN_PROGRESS
base_ref: main
base_sha: c7fb2386f749159cd83b0ac091aad6e05a50e61a
initial_head: 00ebf0ffe5587524585c93ba7e2eee7084ce364f
result_sha: 96c6dfe85c20bfd80be5c8ca60e4cbf454bd9261
created_at_utc: 2026-10-29T19:34:56.655701+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3 — coexistence physique FIFO/FIR/SD obtenue jusqu’à 60 secondes

**C0 à C4 satisfaits dans la portée du banc TR2-002 NON-PRODUCTION.** Capture réelle
SPI3 → propriétaire FIFO unique → datation DEC8 → FIR Q15 causal 107 coefficients /10
inchangé → file bornée → SD SPI2 coopérative. Les trois captures finales 2/10/60 s
ont été arrêtées proprement, puis tous les octets persistés ont été relus depuis
la microSD et comparés au SHA-256 producteur calculé **avant la file et le TX**.
Statut provisoire `IN_PROGRESS` uniquement pour publication et relecture distante
finales à effectuer ; aucun blocage technique restant pour cette portée 60 s.

## Identités, code et opérations

- Cible exclusive TR2-002 : ST-LINK `0032001F3235510C37333439`, UID
  `001B0046/38425018/20333157`, gardes relues avant chaque session ; WHO_AM_I `7B`.
- Code final **`96c6dfe85c20bfd80be5c8ca60e4cbf454bd9261`**, correction réserve après preuve de pauses groupées.
  BIN 43768 octets SHA-256
  **`192db2f087b2bc241a9befad9c763bae6a87e74962553206f61cf7cc0a2af127`** ; ELF
  `be72d0b79bcbfd5d49579d24faa0b1040b844d876798f6c629054d0c7c443c0b`. BIN/ELF/MAP, commandes, sorties et
  readback Flash exact sont dans le manifeste final. Cortex-M33/O2, CubeU5 v1.9.0,
  startup/vector/linker réels, CPU configuré 160 MHz.
- SPI3 mode3 **8 MHz**, SPI2 mode0 initialisation 250 kHz puis **4 MHz**, valeurs
  MBR/CFG relues après STOP. Quanta FIFO 64 mots et SD 128 octets, intervalle
  post-service 250 µs, références **1000/2500/5000 µs inchangées**.
- Montage repris des confirmations utilisateur R1/R2, sans intervention nouvelle :
  capteur PC9/10/11/12 ; SD PB13/CN7-5, PD3/CN9-10, PD4/CN9-8, PF12/CN7-20,
  3V3 CN8-7/GND CN8-11. Anciennes lignes SDMMC retirées, USB natif non utilisé.
- Carte sacrificielle RAW : CID SPI `03534453/44333247/858BF092/3C01927B`,
  CSD `400E0032/5B590000/EDC87F80/0A4040C3`, OCR `C0FF8000`,
  62 333 952 secteurs ×512 =31 914 983 424 octets. CID/capacité/marqueur R2 relus
  avant autorisation d’écriture ; plage strictement bornée [8192,40960).
- **Trois flashs standard R3** avec vérification/readback, uniquement les pages
  nécessaires ; aucun mass erase, Option Bytes, RDP ni reset logiciel supplémentaire
  commandé. GDB hors calibration/capture/écriture/busy, arrêt terminal après STOP
  et SD quiescente. Le reset inclus dans la programmation normale est distingué.
- TR2-001, COM4, service Windows, FRAM et média de production : aucune opération.
  Leur état actuel n’est pas relu et n’est pas présenté comme RUNNING. Anciennes
  images R1/R2 et backup initial réutilisés, aucun cycle de restauration gratuit.

## Résultats finaux mesurés

| Fenêtre | Durée µs | XYZ source acceptés | Sorties /10 | Payload relu B | Pic FIFO mots | Pic file B | STOP / drain µs |
|---|---:|---:|---:|---:|---:|---:|---:|
| Baseline sans writer | 2000194 | 52576 | 5258 | — | 28 | 0 | 152 / 0 |
| Couplé 2 s | 2000673 | 52560 | 5256 | 31536 | 45 | 24880 | 151 / 3023832 |
| Couplé 10 s | 10000213 | 263960 | 26396 | 158376 | 44 | 772 | 151 / 2620 |
| Couplé 60 s | 60000675 | 1585312 | 158532 | 951192 | 45 | 762 | 152 / 2565 |

Toutes les captures finales : FIFO flags/OVR0, source/STOP/SD/queue errors0,
rails/clipping0, WARMUP11 sorties explicitement comptées. Les 2/4/2/3 XYZ en attente
sans ancre future au STOP (baseline/2s/10s/60s) ne sont pas extrapolés. Index sortie i
correspond à la source10×i, origine0, continuité et temps contrôlés par DEC8.
Le débit provient des vrais comptages, pas d’un générateur nominal à16kB/s.

### Distributions 60 s et marges

| Mesure | Moyenne µs | P95 µs | P99 µs | Max µs | Référence µs |
|---|---:|---:|---:|---:|---:|
| work | 319.164 | 476 | 520 | 529.438 | 1000 |
| service | 719.685 | 1161 | 1204 | 1254.956 | 2500 |
| gap | 975.765 | 1417 | 1460 | 1511.000 | 5000 |
| sd_step | 51.090 | 265 | 489 | 489.306 | 1000 |
| sd_write_us | 10957.468 | 11110 | 13528 | 14585 | — |
| sd_busy_us | 2121.947 | 2180 | 4699 | 5705 | — |

FIFO/work/service/gap : 61490 services, sommes/maxima en cycles exacts,
quantiles histogrammes arrondis au µs supérieur ; premières/dernières256 traces
couplées conservées, pas un faux journal brut intégral. SD : 1858
paires brutes toutes conservées, quantiles exacts au µs ; les durées d’écriture
incluent les tours FIFO coopératifs, **ne sont pas du blocage CPU**. Statistiques
SD incluent le dernier secteur de vidange, hors capture après STOP.
Les maxima restent sous les références historiques ; pas une certification WCET.
Occupation mesurée du service complet ≈73.75%
de la fenêtre, callback consommateur ≈32.71%
(inclus dans le service), pas une mesure CPU industrielle exhaustive.

60 000 SysTick observés en60,000675s ; PRIMASK/BASEPRI/FAULTMASK0. Pas d’IRQ UART,
DMA ni FRAM qualifiée ici. Calibration sensor/MCU2s :193brackets/6573ancres,
résidu23µs ; suivi60s598contrôles/résidu max19µs, garde100µs conservée.
Les temps sont référencés au DWT/horloge configurée160MHz ; pas une calibration SI
indépendante. CTRL1_XL `AC` (±8g dans ce profil) relu par les contrôles capteur ;
pas une calibration physique d’amplitude ni une qualification AC1.

Mémoire image finale : `size` text43748/data20/bss587000B (cette dernière valeur
inclut la réservation heap/stack du linker). `_ebss=0x2008eb04`, sommet
`0x200c0000`, espace201980B entre eux. File réelle64KiB, pas cumul aveugle des
sondes R1/R2. Pile4KiB peinte : profondeur observée1592B, plancher intact2504B ;
mesure empirique excluant startup et garde de256B, pas une borne universelle.

## Intégrité persistante et limites de l’oracle

- c3-2s-queue64 : **31536 octets** relus, 208 octets FF de padding contrôlés ; SHA-256 **`054866b60ac83bde3038fccc44a741f214ae5d4d38eb3853765b4f19e026c6bc`**. Header LBA28672, payload 28673..28734.
- c3-10s-queue64 : **158376 octets** relus, 344 octets FF de padding contrôlés ; SHA-256 **`d7643a14fb37cce6f76f2691ac44f504a942cbae023c856ffe5befc630ec5f2d`**. Header LBA20480, payload 20481..20790.
- c4-60s-queue64 : **951192 octets** relus, 104 octets FF de padding contrôlés ; SHA-256 **`cf6a7f870080057ceb36959b38a597d09a399da0eb11b3211a8961204fbf0853`**. Header LBA32768, payload 32769..34626.

Chaque fenêtre utilise de nouvelles lectures CMD17 du média, après STOP, par
tranches64KiB ; header512 relu byte-à-byte, CRC32/identité/comptages/index/temps
contrôlés et comparaison de **tout** le payload au digest amont indépendant des
buffers file/TX. Pour60s, quinze tranches récupèrent951296octets dont104FF.
En complément, 1024 entrées réelles datées sont contrôlées contre le préfixe brut ;
**103 sorties sont recalculées numériquement** par convolution entière Python
indépendante et128 sorties capturées sont identiques au début du média.
Les quelque1,58M sources natives complètes ne sont pas archivées : le rejeu FIR
numérique indépendant ne porte donc pas sur chaque sortie de la capture longue.
Cette distinction est maintenue dans les preuves et le verdict.

## Deux échecs conservés et corrections étayées

1. Image `ebd433e3`, SPI3=4MHz : baseline saine puis couplage arrêté1,067492s sur
   **vrai FIFO_OVR** (pic510/flagsA1/error7), queue864B/overflow0/SDerror0.
   Maxservice2368/work838/gap2624µs sous références, mais drainage soutenu insuffisant.
   Correction `932f9e95` : SPI3=8MHz dans la limite constructeur10MHz, quanta et
   limites inchangés. Correction du comptage cleanup du seul secteur en vol et
   gel du digest en faute. Ancienne capture comptait31writes avantcleanup contre32
   carte aprèscleanup ; son digest encore hérité de baseline **n’est pas valide**.
   Aucun header de succès, données8193..8224 et image négative conservées.
2. Image8MHz/file32KiB : 2s/10s sains et entièrement relus, puis60s interrompu à
   **15,284988s sur refus atomique de file**, queue32764B/overflow1, FIFOOVR0/pic45,
   SDerror0. 19writes>100ms dont17busy>100ms groupés ; busy142090/write151054µs.
   40363sorties/242178B produits incluent le dernier paquet6B explicitement refusé ;
   seulement209920B/410secteurs confirmés, pas de header succès ni de prétention
   d’intégrité complète. Zone16385..16794 conservée.
   Correction `96c6dfe8` : réserve64KiB après modèle conservateur19attentes
   consécutives151054µs à16kB/s. Le même writer/file déborde32KiB à32768B et
   restitue exactement64000B avec64KiB (pic38538B), sans changer les gardes.

La nouvelle capture2s a réellement subi une grappe SD (write154566/busy149217µs,
picfile24880B, drain3,023832s), absorbée sans perte. Les60s finales n’ont pas rencontré
cette même grappe (busymax5705µs/picfile762B) : leur succès ne démontre pas une
réserve suffisante pour des pauses arbitrairement longues ou répétées indéfiniment.
La correction vise une tolérance transitoire bornée ; toute future file pleine
reste un défaut explicite et ferme la capture, jamais une perte silencieuse.

Zones antérieures préservées par séparation des écritures, sans nouveau scan
physique inutile : R2 secteur0 et2048..3922 ; R3 négatif8193..8224 ; ancien10s
12288..12598 ; négatif16385..16794 ; ancien2s24576..24638. Les nouveaux headers
28672/20480/32768 et leurs payloads sont disjoints. Ce layout RAW versionné est
**un format de test**, aucune FAT/partition montable Windows, aucun format V1 produit.

## Tests, validation et reproductibilité

- C1 : vrais parser/DEC8/FIR Q15 réutilisés, busy injecté150/500ms,31998B exacts,
  source invalide→invalidation, CID/zone/refus/IO/timeout, file pleine/emprunt,
  vecteur SHAabc et streaming vs one-shot. Ajout grappe conservatrice dans deux
  exécutables32/64KiB, **2/2 tests ciblés PASS**, oracles positifs non affaiblis.
- Défaut CRC incertain corrigé dès `ebd433e3` : latch placé avant transfert CRC,
  contre-test rouge/vert conservé. Pas de retry après acceptation potentielle.
- ARM banc final PASS et `STM32CUBE_U5_ROOT=... bash tr2_validate.sh
  --cross-build-only` PASS. Validation proportionnée au harness isolé ; cœur/FIR,
  interfaces de production et150tests précédents inchangés, non rejoués sans raison.
  L’observateurSPI2 opt-in du fichier partagé conserve le préprocesseur par défaut
  byte-identique135631B (preuve conservée). Aucun RTOS/DMA/refactoring général.
- Durant la reprise, une erreur de génération CMake du second target hôte a été
  corrigée (chemin source remplacé à tort par le nom du target). Le premier vieux
  ctest, lancé après cette erreur, **n’est pas une validation du nouveau code** ;
  seuls configure/build/ctest `*-v2.log` réussis sont retenus. Logs d’échec conservés.
- Les fichiers `.zlib` sont marqués binaires localement : aucune altération des
  preuves pour éliminer de faux avertissements de whitespace sur octets compressés.

[Projet et protocole RAM](../../../experiments/r3_coex_fifo_fir_sd/README.md).
[Manifestes finaux et images](preuves/TR2-20261029-R3-COEX-FIFO-FIR-SPI2/final-queue64/BUNDLE.json).
[Premier négatif](preuves/TR2-20261029-R3-COEX-FIFO-FIR-SPI2/checkpoint-4mhz/BUNDLE.json),
[acquis8MHz](preuves/TR2-20261029-R3-COEX-FIFO-FIR-SPI2/checkpoint-8mhz/BUNDLE.json),
[négatiffile32KiB](preuves/TR2-20261029-R3-COEX-FIFO-FIR-SPI2/checkpoint-queue32/BUNDLE.json).
Chaque pièce compressée porte taille/SHA256 original et SHA256 stocké ; outils de
capture/décodage/rejeu/relecture, commandes, bruts, distributions et résultats inclus.
Preuves locales : `/home/lolo/dev/msm/reports/TR2-r3-coex-fifo-fir-spi2/`.

## État final, Git et suite

TR2-002 conserve l’image de banc évaluée, run5/stage4/error0, propriétaire FIFO
arrêté, writer inactif, CS SD haute, SD quiescente, débogueur détaché après la dernière
relecture. Aucun autostart/autowrite au reboot ; images de retour R1/R2 et backup
initial conservés. Pas de restauration non nécessaire, TR2-001 non touché.
Deux stashes historiques `6251b988` et `f3b829e6` conservés ; aucun reset/clean/rebase/
merge/force push. Worktree de mission `MSM_vibe-accel-fs8g-ac-resume`, branche
`codex/r3-coex-fifo-fir-spi2` publiée par push normal explicite `HEAD:refs/heads/main`.
Dépôt historique main82169af préservé, divergent du main distant et non mis à jour
par écrasement. Modifications limitées au banc, hook opt-in et livrables R3.

La fenêtre10min facultative **n’est pas exécutée** : priorité à la publication de
la porte obligatoire60s ; queues de latence longues/stockage récupérable/endurance
restent à traiter séparément. Aucun GO8h/coupure/FAT/FRAM/RTU/AC1 Windows/industrie.
Prochaine étape : revue indépendante A-01 ChatGPT de ces preuves, puis mission de
persistance récupérable/formats et endurance progressive. Journal technique
actualisé avec les résultats observés, les échecs et leurs limites ; aucun gel
V1/B3/B4/B5 ou décision normative modifié implicitement.

Les checkpoints antérieurs et le code ont été relus depuis la référence GitHub
récupérée `96c6dfe8` (208fichiers exacts et artefacts BUNDLE décompressés/hachés).
La relecture de cette publication finale est encore à faire ; son SHA et le statut
final seront consignés après vérification distante, sans SHA autoréférent fictif.
