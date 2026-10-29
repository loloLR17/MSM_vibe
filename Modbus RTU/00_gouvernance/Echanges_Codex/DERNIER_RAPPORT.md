---
mission_id: TR2-20261029-R2-SPI2-MICROSD-ISOLATED
status: IN_PROGRESS
base_ref: main
base_sha: bc090a048fc75968d9869b5d7a49f694dee0f7e2
initial_head: 8aeccd5d48fc84b8582d9ff6d0b2c095f212d0ff
result_sha: 7c2f07a51d112f7d034dab422bb60a26af832ac9
created_at_utc: 2026-10-29T16:45:23.851134+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R2 SPI2 — checkpoint physique acquis jusqu'aux10secondes

**P0/P1/P2 et charge10s réussis dans cette portée isolée.** Fenêtre60s lancée après
relecture intégrale indépendante des160000octets de la courte ; résultat non encore
collecté au moment de ce checkpoint. Aucun nouveau diagnostic SDMMC ni essai R1 rejoué.

Utilisateur confirme montage et tension. Àsa demande, pas de soudure : **CLKPB13/CN7-5,
MISOPD3/CN9-10, MOSIPD4/CN9-8, CSPF12/CN7-20,3V3CN8-7,GNDCN8-11**. Ce choix conserve
SPI2 enAF5 et évite les ponts PB4. Anciennes lignes déconnectées, capteurSPI3 préservé
et inactif. Tension rapportée par utilisateur, pas mesurée par Codex.

SN0032001F3235510C37333439 et UID001B0046/38425018/20333157 relus réellement ; FIFO
non actif, préfixeFlash34192octets exactd30c8b…f868d avant changement. Ancienne image
FIFO et backupFlash initial2MiB conservés/vérifiés, pas de sauvegarde redondante.
Un flash standard R2, verify/readback exacts, zéroresetlogiciel supplémentaire commandé.
BIN **6fb2bafb3cda869ef4248421d88b9afbaaed8ac96199339005cab1de910737d4**, source7c2f07a5, Cortex-M33/O2/160MHz,
SPI2mode0/250kHzinit puis4MHz. Pas de HAL_SD/SDMMC, SPI1/FRAM, transportSPI3 ouRTU.

P1 : CMD0/CMD8/ACMD41/CMD58/CMD59 et CRC7/16 actifs. CID SPI
**03534453/44333247/858BF092/3C01927B**, identique au CID natif historique avec bit de
fin SPI1 (R2natif fin0), capacité62333952secteurs512=31914983424octets. Doubles lectures
LBA0/2048/62333951 exactes,6lectures/0écriture/0CRCfault ; CRC32 etSHA témoins recalculés
sur PC. Erreur scriptGDB après tous les dumps (`GPIOF` absent du contexte macro) :
correction hors cible, **P1 non rejoué**, captures complètes réutilisées.

P2 : carte intégralement sacrifiable autorisée, identité verrouillée avantwrite.
**Ancienne partition FAT32 non conservée** : ancienMBR capturé, secteur0 remplacé par
marquage brut de banc (signature de partition invalidée), puis LBA2048..2216 écrits
et relus immédiatement. Le MBR montrait aussi de l'espace non partitionné ; le choix
réalisé est l'initialisation brute explicite de cette carte libre, pas une prétention
d'utiliser cet espace libre ni un formatage de volume Windows.169blocs=86528octets
de motifs exacts, plus512octets de marqueur.512B/4KiB/16KiB/64KiB groupés parCMD24
unitaires, pas un essaiCMD25.170writes/170reads inverses, zérofauteCRC/transfert ;
**busy maximal142150µs** (write143830µs), sousborne500ms mais incompatible avec une
attente bloquante dans une future boucleFIFO. Aucune coexistence testée.

P3court : arrivée16000B/s/10s, ring32768B,160000générés=écrits=vérifiés,313blocs512
avec256octets de paddingFF. Drain terminé en10002460µs, backlogmax512B, overflow0.
Lecture intégrale embarquée puis **nouveauxCMD17** pour retourPC en3fenêtres après
STOP/CShaut ; comparaison indépendante de chaqueoctet etpadding, SHA256payload
**d39e9bac0ecc8bd4f78aa74b616443b7864641c98cdbd218136ca5bd77ea63ae**, identique àl'oraclePython.
Les distributions brutes sont jointes ; une fenêtre60s est en cours sansGDB pendant
la mesure et sera relue intégralement avant verdict.

Tests : modèleSDHC neuf PASS (CRC connues, refusCID/zone, fautesCRC/token/réponse/I/O,
busy avant/après acceptation, quiescence sansretry),2tests oracle/corruption PASS,
ARMisolé et canoniquecross-build-only PASS. Les150testshôte/FIR et essaisphysiques R1
inchangés ne sont pas réexécutés ; aucun coeur fonctionnel partagé n'a été modifié.
BSS191080B incluant sondes, pile empirique1032B/plancher3064B ; pasborneexhaustive.

TR2-001 horsflash/debug/reset/commandes ; alimentationprimaire non relue, service et
données conservés sans déclarerRUNNING. Deuxstashes et anciensworktrees préservés.
TR2-002 firmwareR2 conservé ; capturesP1/P2/courtCShaut, puis essai60s actif aucheckpoint.
Aucun accèsSD avant confirmationcâblage ; aucun masserase/OB/RDP, SDprincipale, COM4,
FRAM oumodificationWindows. PasdeGOproduction/FIFO+SD/endurance/formatarchivefinal.

[Projet et protocole](../../../experiments/spi2_microsd_isolated/README.md),
[preuves acquises](preuves/TR2-20261029-R2-SPI2-MICROSD-ISOLATED/checkpoint-short/BUNDLE.json).
Journalactualisé. Code7c2f07a5 publié ; cecheckpoint sera poussé et relu, puis poursuite
60s/intégrité et bilanfinal. Référence constructeurST/Adafruit et limites de licence
consignées dansSOURCES.json ; implémentationBSP non copiée.
