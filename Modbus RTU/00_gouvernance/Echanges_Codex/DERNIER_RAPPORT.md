---
mission_id: TR2-20261029-R2-SPI2-MICROSD-ISOLATED
status: IN_PROGRESS
base_ref: main
base_sha: bc090a048fc75968d9869b5d7a49f694dee0f7e2
initial_head: 8aeccd5d48fc84b8582d9ff6d0b2c095f212d0ff
result_sha: 7c2f07a51d112f7d034dab422bb60a26af832ac9
created_at_utc: 2026-10-29T17:14:25.693514+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R2 — qualification physique bornée de la microSD en SPI2 isolé

**P0–P3 réussis sur TR2-002 dans cette portée : lecture seule, écritures contrôlées,
charge synthétique à16 000B/s pendant10s puis60s, chaque octet relu et confirmé sur PC.**
Le statut IN_PROGRESS porte uniquement sur la publication/relecture de ce bilan.
Aucune qualification de coexistence FIFO/SD, filesystem, endurance ou production.

## P0 — sources, montage et récupérabilité

La mission/revue R1 ont été relues, pas rejouées. Nouveau projet isolé
`experiments/spi2_microsd_isolated` ; aucun changement de FIR, propriétaire FIFO,
coeur fonctionnel, profil de production ou contrats B3/B4/B5. Code effectivement
flashé et exécuté : **7c2f07a51d112f7d034dab422bb60a26af832ac9**,
`feat(bench): add isolated guarded SPI2 SDHC qualification on accessible Zio pins`.

Cible vérifiée réellement : ST-LINK **0032001F3235510C37333439**, UID
**001B0046 /38425018 /20333157**, NUCLEO-U575ZI-Q/TR2-002. Préflight GDB sans
acquisition active et comparaison des34192octets de Flash au BIN FIFO **d30c8b…f868d**.
Ce BIN et l'ELF R1 restent conservés et vérifiés ; backup initial2MiB **dd1a2b…fb1f1**
conservé, aucune nouvelle sauvegarde complète inutile. Référence distante de retour
`archive/tr2/point-retour-pre-spi2-20261029` toujours **a3f5ac33**.

L'utilisateur a demandé un raccordement sans soudure, puis confirmé le **montage et
la tension vérifiée**. Révision de montage locale, même SPI2, fonctions vérifiées
ST DS13737Rev10/table27 et UM2861Rev10/tables15–19 :

| Adafruit4682 | MCU /fonction | Connexion réalisée selon utilisateur |
|---|---|---|
| CLK | PB13 /SPI2_SCK AF5 | CN7-5 |
| SO /D0 | **PD3 /SPI2_MISO AF5** | **CN9-10** |
| SI /CMD | PD4 /SPI2_MOSI AF5 | CN9-8 |
| CS /D3 | **PF12 /GPIO** | **CN7-20** |
| 3V /GND | 3,3V exclusivement /GND | CN8-7 /CN8-11 |

Ce montage remplace le **candidat MORPHO** PB14/PB4 de l'étude ; PF12 évite les ponts
PB4. Aucune soudure ni changement d'architecture normative. Anciennes lignes SDMMC
retirées àla demande, SPI3 capteur préservé. Le firmware met les anciennes PB3/4/14/15
en INPUT, PD6/7 en ANALOG, trace désactivée, PC9/CS capteur haut sans lancer SPI3.
Les tensions sont **rapportées par l'utilisateur**, pas mesurées par Codex. Les
registres du capteur n'ont pas été relus pendant R2 ; aucun START/acquisition capteur.

Référence constructeur la plus proche examinée : BSP **ST Adafruit802 SPI SD**,
STM32CubeG4 commit **1d9f03564da407b0a70bfa53e54681c8dc923bc2** ; pas d'exemple exact
U575+4682 trouvé dans le SDK installé. Implémentation de transport minimale indépendante,
SDHC seulement, séquence ST conservée mais délais/CRC/identité fermés ; **pas de
HAL_SD SDMMC porté en SPI**, pas de code BSP copié. Provenance/SHA/adaptations dans
[SOURCES.json](../../../experiments/spi2_microsd_isolated/SOURCES.json). La licence
component référencée par ces fichiers ST n'a pas été trouvée dans leur répertoire :
ils ne sont pas redistribués. HAL/startup/clock réutilisés sous leurs licences ST
existantes. Les PDF ST ont été consultés via l'outil Web après échec des téléchargements
directs ; aucune prétention de les avoir archivés localement.

## P1 — carte réellement identifiée, lectures seules

Un flash standard au SN explicite, pages nécessaires seulement, à0x08000000,
`-v -rst`, puis WAIT et readback exact. **Aucun reset logiciel supplémentaire commandé.**
Démarrage inactif avant RAM trigger. SPI2 mode0, HSI16/64=250kHz pour init, puis
HSI16/4=4MHz ; HCLK160MHz configuré, pas étalonné par instrument externe.
CMD0/CMD8/ACMD41(HCS)/CMD58, CMD59 active le CRC ; réponses contrôlées, CRC7 des
commandes et CRC16 des données vérifiés. Première initialisation unique, sans essai
physique SDMMC ni campagne annexe.

- **CID SPI :03534453/44333247/858BF092/3C01927B**. Historique natif :dernier mot
  3C01927A ; seule différence du bit de fin SPI1, absent du R2 natif. Quinze premiers
  octets et sept bits CRC identiques ; CRC7 et bit de fin du CID et CSD recalculés
  indépendamment et conformes. Ce n'est pas un autre numéro de série.
- **CSD :400E0032/5B590000/EDC87F80/0A4040C3**, CSDv2/SDHC. OCR **C0FF8000**.
  Capacité dérivée indépendamment **62 333 952 secteurs512 =31 914 983 424octets**.
- LBA **0 /2048 /62333951** lus deux fois chacun :égalité bit àbit, six lectures,
  zéro écriture. CRC32 des témoins recalculés sur PC :971274781 /2940511467 /
  2997515640 ; payloads réels et SHA fournis.

Une erreur du script GDB est survenue **après les dumps complets et la réussite P1** :
macro `GPIOF` indisponible dans le contexte terminal CMSIS. Code retour1 conservé ;
contrôle corrigé hors cible àpartir du champ `cs_high` capturé. P1, flash et init
**non répétés** ; tous les octets déjà collectés ont été réutilisés/vérifiés.

## P2 — écritures explicites et intégrité

La carte32Go a été déclarée **intégralement sacrifiable** par l'utilisateur et le
contrat. Avant les écritures :CID/capacité vérifiés, gate fermée sur cette identité,
secteurs/protocole publiés dans le README du code7c2f07a5. Ancien MBR capturé :partition
FAT32 type0C, LBA2048,33554432secteurs, avec espace non partitionné restant.

**Choix réalisé :initialisation brute explicite de cette carte libre.** Secteur0
remplacé par marqueur `TR2 R2 SPI2 RAW TEST V1` +CID, ancienne signature de partition
invalidée ; puis motifs dans **LBA2048..2216**. L'espace non partitionné aurait permis
une autre zone, mais n'est **pas** celle utilisée ; aucune affirmation de conservation
FAT. L'ancienne partition est inutilisable comme volume FAT. Aucun volume Windows,
autre média ou carte TR2-001 concerné. Pas de CMD38/erase ni format d'archive définitif.

169blocs de motifs, **86 528octets relus exactement**, plus512octets de marqueur,
170CMD24/170relectures inverses. Comparaison des buffers distincts et CRC, commandes
CMD13R2 après busy. Groupes croissants en **CMD24 unitaires** (pas CMD25) :

| Taille du groupe | Blocs512 | Write moyen,ms | Write P99,ms | Write max,ms |
|---|---:|---:|---:|---:|
| 512 B | 1 | 137.825 | 137.825 | 137.825 |
| 4 Kio | 8 | 136.327 | 138.799 | 138.799 |
| 16 Kio | 32 | 15.337 | 143.830 | 143.830 |
| 64 Kio | 128 | 2.587 | 3.287 | 4.067 |

Latences ci-dessus **par secteur**, regroupées par taille ; les lectures inverses
sont chronométrées séparément, distributions brutes jointes. Marqueur secteur0 :
130,070ms de write,128,390ms de busy. Pire P2 :**write143,830ms, busy142,150ms**.
Aucune erreur CRC/token/transfert/status/timeout ni réémission d'une écriture échouée.
Ces longues queues initiales ne sont pas effacées du verdict par les bons débits suivants.

## P3 — charges16 000B/s, puis conservation intégrale sur PC

Producteur **synthétique** équivalent XYZ int16 entrelacés, version de motif1 et graines
séparées. Arrivées simulées selon horloge MCU, véritable ring32Kio, paquets512B.
Après chaque charge :relecture complète depuis la carte et comparaison embarquée,
puis **nouveaux CMD17** et retour RAM→PC en fenêtres64Kio après CS haut, sans halt
dans les transactions/init/busy/charge. La fenêtre60s a été lancée seulement après
succès et comparaison PC intégrale de la courte.

| Charge | Payload exact,B | Blocs | Write moyenne /P95 /P99 /max,ms | Busy max propre,ms | File observée max,B | Production+drain,s |
|---|---:|---:|---:|---:|---:|---:|
| 10 secondes | 160,000 | 313 | 2.597 / 2.738 / 3.217 / 5.721 | 4.042 | 512 | 10.002460 |
| 60 secondes | 960,000 | 1875 | 2.625 / 2.745 / 5.419 / 5.774 | 4.095 | 512 | 60.002583 |

Pour10s, padding final256octets FF :**160256octets physiques** récupérés et comparés,
160000payload. Pour60s :**960000octets**, aucun padding. Octets générés=écrits=vérifiés,
overflow0, timeout0, CRC0, faute préclose SPI0. Débit utile calculé sur production+
drain, **hors relecture** :15996.065 /
15999.311 B/s ; arrivée nominale voulue16000B/s.

SHA-256 du **payload réellement relu**, identique àl'oracle Python indépendant :

- 10s :`d39e9bac0ecc8bd4f78aa74b616443b7864641c98cdbd218136ca5bd77ea63ae`.
- 60s :`107febd4f8e2ac302e28f6907e0e83a3516aaf598291440eaeb62728130d2b90`.

Les18retours successifs (3+15) sont des **relectures du même contenu**, pas de nouvelles
charges. Les flux complets sont publiés, découpables selon les offsets/tailles/SHA
individuels. Ce n'est pas une comparaison avec laRAM source conservée ni une conclusion
tirée du seul SHA fourni :tous les octets ont effectivement été comparés sur PC.

**Interprétation de la file :**512B est le maximum comptabilisé dans la boucle de
foreground. Le producteur simulé comptabilise les arrivées après chaque appel SD
bloquant ; ce n'est pas un producteur IRQ concurrent. Une borne conservatrice du pic
incluant ces arrivées différées est calculée sur chaque paire réelle :
`backlog_avant + ceil(write_us ×16000 /1000000)` → **604B /605B**, largement sous32768B.
Ce sont des bornes calculées depuis les mesures, pas deux nouveaux maxima matériels.
L'outil hors ligne rend cette distinction explicite ; aucune perte silencieuse ou
marge FIFO ne se déduit de la seule valeur512B.

## Ressources, erreurs et limites

BIN **25556octets**, SHA-256
**`6fb2bafb3cda869ef4248421d88b9afbaaed8ac96199339005cab1de910737d4`** ; ELF
**`313c4910515cbf7d0146085673bf370e63dfe0583b562f936295708c9fea856b`**.
GCC ARM14.2.1/Cortex-M33/Thumb/O2/soft ABI, CubeU5v1.9.0, HAL SPI observé de hash
originala1668346…8c22a3, algorithme ST intact. MAP/options/symboles/attributs/.su fournis.
Text25536/data20/BSS191080B, dont ring32768, retour65536 et latences81920. Marqueur
pile4Kio :profondeur observée1032B/plancher3064B intact ; exclut startup et garde256B,
pas borne exhaustive. Aucun test de DMA ou charge globale CPU revendiqué.

Tests nouveaux pertinents :modèle SDHC réel au niveau protocole PASS (CRC connues,
aller-retour secteur, refus CID/zone, fautes CRC/token/réponse/I/O/busy avant et après
acceptation, quiescence, arrêt fermé sans retry), deux tests outil/oracle/corruption
PASS. Cross-build isolé et canonique `tr2_validate.sh --cross-build-only` PASS. Le coeur
partagé n'a pas changé :**150tests antérieurs et micro/FIFO R1 non rejoués**. Validation
proportionnée au nouveau banc. Références et état actuel ne viennent pas d'un ancien ZIP.

- Attentes bornées :réponseR1 huit octets, ACMD41≤3s, token≤200ms, busy≤500ms,
  HALexchange≤30ms, bornes de boucles secondaires. Une faute ferme le segment et
  conserve sa première cause. Une écriture incertaine ne serait pas rejouée ;
  quiescence uniquement par clocksready≤5s, sans terminal GDB si encore busy.
- **Physiquement observé :aucune faute SD.** Retrait/absence carte, timeout/recovery,
  coupure pendant write :**NON EXÉCUTÉS physiquement**, fautes couvertes seulement
  par modèle ; pas de manipulation supplémentaire demandée pour compléter artificiellement
  une campagne. Une seule initialisation froide observée, pas étude de dispersion.
- **142,150ms de busy** initial >réserve FIFO historique :un SPI2 distinct ne suffit
  pas àl'intégration. Même le write de charge5,774ms dépasse la référence
  historique de gapFIFO5ms. Il faudra ordonnancer les busy hors drainage FIFO dans une
  mission distincte. Pas de coexistence SPI3/FIR/FRAM/RTU, de fichier FAT lisible,
  de résilience coupure, d'endurance10min/1h/8h ou de qualification industrielle.

## État final et publication

TR2-002 conserve **l'image R2**, pas de restauration inutile. Dernière capture :
stage4/phase4/error0,960000octets retournés, CS PF12haut, SPI sans transaction active,
quiescence non sollicitée, debugger détaché et serveur fermé. Compteurs cumulatifs :
**2358writes /4552reads**, aucune erreur. Au reboot :WAIT avant toute commande SD,
aucun autowrite. Dans la phase4 actuelle, aucune nouvelle commande de write acceptée.
MicroSD de test présente, état brut de banc, zone finale **LBA2048..3922**, secteur0
marqueur ; état des autres secteurs non qualifié. Retour FIFO d30c8b et backup initial
intacts, récupérabilité vérifiée, pas de rollback FRAM ou d'OptionBytes.

**TR2-001 non accédé** :aucun flash/debug/reset/RS485/START/STOP/FRAM/service Windows.
Alimentation actuelle du primaire non relue ; pas de «vérifié OK» ou RUNNING inventé.
Deux stashes6251b988/f3b829e6, worktrees et ancien dépôt principal préservés. Branche
locale `codex/r2-spi2-microsd-isolated`, destination autorisée explicite
`refs/heads/main`, push normal uniquement.

Code7c2f07a5 publié ; checkpoint **212e0a7e1afaaa76c0b30bc372ca38fd6e5a4a33** poussé
et relu depuis GitHub (20fichiers exacts). Présent bilan et preuves finales àpublier,
puis relire avant clôture DONE. Journal actualisé :montage réel, carte, charges et
intégrité, busy/borne de backlog, état sûr et prochaine étape. **Suite àChatGPT :**
revue indépendante A-01, puis mission distincte de coexistence FIFO/FIR/SPI2 avec
ordonnancement borné, avant endurance ou format de campagne.

[Projet /sécurité](../../../experiments/spi2_microsd_isolated/README.md),
[checkpoint](preuves/TR2-20261029-R2-SPI2-MICROSD-ISOLATED/checkpoint-short/BUNDLE.json),
[preuves finales](preuves/TR2-20261029-R2-SPI2-MICROSD-ISOLATED/final/README.md).
**287pièces finales +16pièces de checkpoint**, compressées sans perte avec SHA
brut/stocké ; originaux dans `/home/lolo/dev/msm/reports/TR2-r2-spi2-microsd-isolated/`.
Les sources BSP externes ne sont pas redistribuées, les journaux d'erreurs et le script
GDB initial sont conservés. Publication ne vaut pas qualification indépendante.
