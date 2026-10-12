---
mission_id: TR2-20261012-M015-M-SPI-LONG-TRANSFER
status: READY
base_ref: main
base_sha: f8e782a41e2cca94ab12ff3f180a2dc6461611aa
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-M — Diagnostic et correction du transfert SPI long sur banc ST

## Contexte et objectif
L'utilisateur valide la poursuite de l'investigation M015-L. Lire AGENTS.md et la gouvernance courante, vérifier le HEAD main réel et l'état Git, archiver mission et rapport M015-L sans altérer les preuves. M015-L démontre WHO=0x7B, polling ST 64 XYZ valides, FIFO ST_STRICT getter niveau0 vs statut brut257, puis premier burst STATUS10 896 octets refusé avec HAL_TIMEOUT et HAL_SPI_ERROR_OVR (0x104), zéro donnée FIFO acceptée. Cause racine inconnue. Ne pas conclure que le capteur ou le driver ST sont défectueux sur cette seule base.

Identifier la cause précise du timeout/overrun SPI côté STM32U575 et qualifier une correction minimale **de la couche plateforme/transport**, en conservant les sources officielles ST inchangées, les réglages du capteur, la séparation ST_STRICT / STATUS10 et les critères stricts de données acceptées. Ne pas modifier le TR2 principal.

## Plan technique
1. Analyser le code HAL STM32CubeU5 v1.9.0 réellement utilisé, SPI mode0/2MHz calculés, CPU MSI16MHz, polling bloquant TX/RX 896 octets, FIFO/packing/seuils et SPI_SR, comportement SPI_CloseTransfer, erreurs OVR/TIMEOUT. Confronter budget de service RX (4µs/octet théoriques) à la capacité CPU/HAL, au build Debug -O0 et aux traces réelles. Distinguer hypothèses, reproduction et cause démontrée.
2. Instrumenter de façon bornée et non intrusive SPI_SR, registres/compteurs/offsets et chronologie **avant nettoyage HAL**, sans masquer les erreurs. Établir des tests host/mocks et cross-build avec captures explicites. Préférer une correction ciblée et justifiée (horloge CPU, stratégie transfert, configuration SPI, optimisation, etc.) plutôt qu'un simple allongement de timeout ; choisir sur preuves. Ne pas modifier le pilote ST ni changer simultanément plusieurs paramètres expérimentaux. Documenter toute différence au code ST.
3. Qualifier sur **seconde NUCLEO de test seulement**, STLINK SN `0032001F3235510C37333439`, UID `001B00463842501820333157`; refuser absolument SN `003500463235510B37333439` et UID `004A003D3842501820333157` du TR2 principal. Vérifier identité et image/hashes avant chaque flash. Exécuter une campagne physique bornée à étapes, avec conservation de toutes traces, STOP sur anomalie nouvelle ; aucune relance opportuniste. WHO/polling déjà acquis : ne les refaire que si nécessaires à la correction, en les distinguant. Tester FIFO, exporter payloads bruts, niveaux, tags, timestamps et démontrer intégrité et comptage ; ne jamais assimiler données partielles/rejetées à des mesures valides. Aucune qualification V1 4096..32768 sans preuves dédiées.
4. Le banc secondaire est explicitement **jetable au niveau du firmware Flash** : décision utilisateur permanente pour ce banc, aucune nécessité de sauvegarde Flash 2MiB ni restauration du firmware initial à chaque campagne. Ne pas faire ces opérations chronophages ; garder vérification identité, intégrité image, programmation/verify/readback, traces et état final. Cette dérogation ne s'applique **jamais** au TR2 principal ni à ses données, et n'autorise ni mass erase, ni modification Option Bytes/RDP, ni manipulation destructive inhabituelle. Les gestes physiques restent à l'utilisateur ; si un doute câblage/mesure apparaît, demander un contrôle précis et reprendre ensuite sans abandonner le processus.
5. En cas de succès FIFO sur ST, caractériser précisément les limites du résultat et proposer la comparaison ultérieure avec TR2 (un facteur à la fois). En cas d'échec, publier les faits, cause démontrée ou hypothèses hiérarchisées et prochaine expérience discriminante.

## Gouvernance et livraison
Autorisation permanente commit/push/flash/debug confirmée par AGENTS.md et ce contrat, sous garde-fous de cible et sécurité. Développement et tests autonomes sans validations répétitives. Travail Git isolé si nécessaire ; ne pas écraser les worktrees/stashes. Publier code, tests, preuves brutes, documentation, roadmap/journal et DERNIER_RAPPORT.md ; pousser main puis relire à distance les fichiers et SHAs. Statut honnête (DONE seulement si critères réellement qualifiés). Aucun travail sur M014-H, FRAM, SD, Modbus ou firmware permanent TR2.
