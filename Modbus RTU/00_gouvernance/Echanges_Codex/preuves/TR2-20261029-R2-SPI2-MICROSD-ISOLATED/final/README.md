# R2 — preuves physiques SPI2 sur TR2-002

Les `.zlib` sont les octets exacts des pièces listées dans BUNDLE.json, tailles et
SHA-256 brut/stocké. Décompresser avec `zlib.decompress`. Les16pièces du checkpoint
court restent séparées et immuables, dont le retourcomplet160256B (payload160000).
Le retourlong960000B est fourni ici. Les captures des18retours RAM correspondent à
3+15 nouvelles séries CMD17, sans refaire les charges : fichiers retourbin individuels
non dupliqués dans Git, reconstructibles en découpant les retourscomplets selon les
offsets/tailles/SHA des manifestes d'intégrité. Toutes les captures de contrôle,
compteurs et scripts restent présentes. Les répétitions de latencies/trace dans les
capturesretour sont des tableaux RAM inchangés, **pas de nouveaux tests d'écriture**.

Sourceimage7c2f07a5, BIN6fb2…37d4. Un flash verify/readback, P1lectureseule, P2petits
motifs,10s puis60s à16000B/s. CRC7commandes/CID/CSD et CRC16données ; premiers64 et
derniers192 événementscommande seulement : ne pas prétendre àun enregistrement
fil àfil de chaqueoctetSPI. Tous les octets des deux charges sont comparés sur PC
après relecture physique complète, àun oraclePython indépendant de laRAM source.

P1a terminé correctement/captures complètes avant une erreurGDB de macroGPIOF dans
le contrôle final. Runnercorrigé horscible : pas de reflash/reset/relanceP1. Les traces
et code retour1 sont conservés. Les validationshôte sont des simulations distinctes.
Les sources STAdafruit non redistribuées (licencecomponent absente du répertoire),
URLs/commits/SHA et adaptations documentés ; HAL/startup/clock utilisent leurs licences
ST existantes. Les téléchargementsPDF directs ont échoué, références officielles
consultées par l'outilweb : aucune prétention d'avoir archivé cesPDF.

La carte32Go sacrificielle a été initialisée en rawtest ; ancienMBRcapturé, secteur0
marqueur, LBA2048..2216 motifs puis2048..3922 chargefinale. Ce n'est plus un volumeFAT
utilisable. Aucun autre média/volumeWindows/TR2-001 accédé. CarteCShaut, transactions
terminées, debugdétaché au dernier retour ; pas de restaurationFIFO inutile.

Attention : busyinitial142150µs ; moyenne favorableencharge ne garantit pas ses queues.
BacklogRAM échantillonné512B ; borneconservatrice605B avec arrivéesthéoriques pendant
l'appelbloquant, calculée par paire de mesure, pas mesurée par producteurIRQ. Ce banc
n'acquiert aucun FIFO et ne qualifie pas coexistence/formatarchive/endurance/production.
