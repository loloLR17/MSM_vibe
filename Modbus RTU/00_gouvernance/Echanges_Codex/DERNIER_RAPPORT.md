---
mission_id: TR2-20261029-R3-COEX-FIFO-FIR-SPI2
status: IN_PROGRESS
base_ref: main
base_sha: c7fb2386f749159cd83b0ac091aad6e05a50e61a
initial_head: 00ebf0ffe5587524585c93ba7e2eee7084ce364f
result_sha: ebd433e34dbb413649817be5b19b35116b9f06a2
created_at_utc: 2026-10-29T18:50:00.942565+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3 — checkpoint4MHz, premier échec FIFO conservé et correction en cours

C0/C1 préparés et testés :un seul propriétaireFIFO/DEC8/FIR Q15 inchangés, writer
SPI2 coopératif128B,32KiB de file empruntée jusqu'àacquittement, digestSHA256 producteur
avant file/TX. Source b6dfb04c puis **ebd433e3** (garde d'acceptation incertaine SD au
CRC, test rouge puis vert conservés). SPI3=4MHz/SPI2=4MHz, profil NON-PRODUCTION.
BSS554232B, text43720/data20 ; startup/vector réels et imageARM/canoniquecross-build
PASS. Modèle réel parser/DEC8/FIR +SD :busy150ms et500ms injectés,31998octets exacts,
maxgap656µs/queue8580B **simulés seulement** ; filepleine/refusCID/zone/fautes/source
invalide/SHA connus couverts. Branche observateurSPI2 opt-in, préprocesseur par défaut
identique sur135631octets. Aucun coeur de production modifié, pas de nouveau test R1.

TR2-002 SN0032001F3235510C37333439/UID001B0046-38425018-20333157 relus, ancienneimageR2
25556B exacte6fb2…37d4. R1/R2 et backupinitial préservés, pas de backup complet répété.
Unflash normal verify/readback exact de l'imageR3 **9585913afb28526be468cc4548057471ad4d2ed8a2201469d44e32d67b20f6ec**,
zéroreset supplémentaire commandé ; aucun debug/flash/commande/service TR2-001.
MontageSPI3+SPI2 repris de confirmationsutilisateur R1/R2, aucun recâblage demandé.

C2 :WHO7B, carteCID/CSD/capacité attendus et marqueurRAW R2 reconnus ; calibration2s,
193brackets/6575ancres/résidu23µs. BaselineFIR2s sanswriter :52600XYZ/5260sorties,
service max1701,306µs, work710,538µs, gap1957µs, FIFO58, aucune faute ;103sorties du
préfixe vérifiées indépendamment. Les sondes/SHA diffèrent de R1, pas comparaison
artificielle àstimulus identique.

**C3 négatif réellement observé :** capture couplée prévue2s arrêtée après1,067492s.
FIFO_OVR flag0x20 (flagsOR0xA1), pic510mots, FIFOerror7, error5/stage99 ;27496XYZ
acceptés et2750sorties avant faute, aucune perte masquée/aucun succès revendiqué.
SDerror0, queueoverflow0, backlogmax864B. Maxservice2367,525µs/work837,981µs/
gap2624µs/SDquantum488,969µs :les références individuelles sont tenues mais le
**drainage soutenu à4MHz/quantum64 ne suffit pas dans ce scénario**. STOPFIFO190µs,
ODRoff/bypass/TSoff contrôlés par le propriétaire, SDquiescente et debugdétaché.

Compteur résultatsd_completed31/15872B capturé avant cleanup ; l'objetcarte confirme
**32writes** après achèvement du seul bloc encore en vol pendant cleanup. C'est une
limite de comptage du premier harness, pas une archive qualifiée ; aucun header de
succès écrit. DigestRAM source encore hérité de baseline sur ce cheminfaute, donc
**non utilisable pour qualifier le préfixe négatif**. Ces captures/images sont toutes
archivées avant modification. Données négativesLBA8193..8224 conservées surcarte ;
le nouvel essai2s utilisera une autrezone. DonnéesR2/marker0 jamais écrasés.

**Poursuite autonome :** SPI3 à8MHz (profilprévu par le bancST), SPI2 toujours4MHz,
quantum64 et références1000/2500/5000µs inchangés ; nouvellebaseline puiscouplecourt
nécessaires car image/horloge changées. Pas de10s/60s avant un2s sain avec relecture
complète. Corriger aussi le comptage du bloc confirmé pendantcleanup et figer le
digest source en erreur, sans changer ce que montrent les preuves négatives.

Ce checkpoint protège les acquis ; **pas BLOCKED**, correction autorisée et essais
encore réalisables. Pas deDONE, de FRAM/RTU/FAT/endurance ou de qualification système.
Journalactualisé. [Projet](../../../experiments/r3_coex_fifo_fir_sd/README.md),
[preuves4MHz](preuves/TR2-20261029-R3-COEX-FIFO-FIR-SPI2/checkpoint-4mhz/BUNDLE.json).
Publication/relecture puis poursuite ; anciensstashes/worktrees et brancheR0 préservés.
