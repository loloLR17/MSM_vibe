---
mission_id: TR2-20261013-A04-02-FRAM-MODBUS
status: IN_PROGRESS
base_ref: main
base_sha: ba6f34c4e536f430be74b2e6c27f6c8c9cb873e2
initial_head: 2cbb9bbc88e6ee3a9392b662c23a33b6b64e3d34
result_sha: 8bcb8436822ce7444074865617cb3696f877a6ad
created_at_utc: 2026-10-13T16:09:08.902564+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# A04-02 — FRAM/Modbus : correction et coexistence bornée

**Critères logiciels et essai physique prévus satisfaits dans la portée ci-dessous.** B0/B1/B3 ne préparent plus B5 inutilement ; B5 est explicitement indisponible par **exception V1 existante04** pendant armement/capture/cleanup non vérifié, avant toute mutation du mailbox ou I/O journal. Après cleanup, projection fraîche et chemin transactionnel inchangé. **126/126 tests hôte + cross-build cumulatif réussis.** Une capture physique32768XYZ+4096timestamps, six readsHP au total/retry0, aucun FULL/OVR/UARTdrop observé, données persistantes contrôlées. Firmware cumulatif corrigé laissé sur TR2-001, debugger détaché.

**IN_PROGRESS uniquement pour publication du rapport physique et relecture distante**, aucun blocage extérieur. La marge nominale calculée sur cet essai est **1,445ms** : suffisante pour les observations acquises mais étroite, sans garantie générale de continuité ou d'acceptation M016. ChatGPT décide de la qualification/clôture de dette ; registre inchangé et aucun tag créé.

## 1. Référence, code et invariants

HEAD initial `2cbb9bbc88e6ee3a9392b662c23a33b6b64e3d34` récupéré sur main ; base du contrat `ba6f34c4e536f430be74b2e6c27f6c8c9cb873e2` vérifiée ancêtre. AGENTS, charte/protocole, journal intégral, audit A-04/A-05/matrice IO-01..05, A04-01 et sources/specification V1 examinés. Worktree isolé `/home/lolo/dev/msm/projects/MSM_vibe-A04-02`, branche locale `codex/a04-02-fram-modbus`, push normal destination `main`. Main historique `82169af7e76797731358852bec83bfb0ceb54ccf`, autres worktrees et deux stashes6251b988/f3b829e6 conservés. Aucun merge/rebase/reset/clean/force push.

- **Code effectivement évalué/exécuté : `8bcb8436822ce7444074865617cb3696f877a6ad`**, `A04-02: guard synchronous B5 FRAM I/O during cooperative capture`,11 fichiers/+476/-26. [Diff exact](preuves/TR2-20261013-A04-02/code-change.patch), [provenance et préservation](preuves/TR2-20261013-A04-02/provenance-and-preservation.json).
- **Checkpoint publié : `5088d50f8325c3b81f8c4eafd7e8e0f837129d6a`**, `A04-02: checkpoint validated software before physical coexistence`, intégralement relu :28 fichiers conformes, [preuve](preuves/TR2-20261013-A04-02/readback-5088d50f.json). Essai physique poursuivi après sauvegarde des acquis, pas arrêt au checkpoint.
- [Archive A04-01](archives/TR2-20261013-A04-01-SAFE-BUILD/RAPPORT.md) préexistante et byte-identique, non écrasée. Gouvernance/contrat, spécifications, dette, fichiers de persistance/configuration, drivers ST officiels et CMake STM32 A04-01 **inchangés**. Le seul changement du moteur FIFO est un prédicat d'admission, sans modifier acquisition/parsing/cleanup.

ActiveConfiguration conserve son autorité ; configuration toujours structurelle/activation différée sans stockage caractérisé. Aucune nouvelle publication V1 de vibrations, source métier raccordée, allocation SD ou refonte. Aucun ACK de succès avant durabilité : une requête B5 retirée reçoit04, n'est ni admise ni mise en file, et son retry après cleanup suit le chemin normal. Aucun code06/business-busy inventé. Le refus temporaire peut affecter tous les accès B5, y compris staging/cancel : c'est la politique de qualification de ce cumulatif, **pas une implémentation de STOP/acquisition métier M016**.

## 2. Diagnostic et choix de correction

Lecture du code : `modbus_system_server.refresh_request` évitait seulement le snapshot journal pour FC03 B0/B1. B3 et autres requêtes déclenchaient `command_engine_snapshot` → `latest_completed` →512 lectures de copies70octets, même sans commande B5. Les commandes B5 peuvent ajouter find/admission/réservation/exécution/terminalisation et commits. Ces opérations sont synchrones ; un commit transactionnel complet reste de l'ordre51914octets écrits puis relus, sans suppression de durabilité.

| Chemin | Déclenchement/coût | Traitement ou limite |
|---|---|---|
|B0/B1/B3 et autres blocs|Préparation B5 historique inutile ; ~141ms historique non rejouées comme mesure nouvelle|Préparer B5 seulement pour requête FC03/FC10 B5 mappée/structurellement valide ; validation PDU/CRC/adresse préservée|
|B5 read/command hors acquisition|Scan/fresh projection, journal et commits synchrones|Conservés lorsque capteur au repos ; service Modbus max **144,138ms observé** pendant baseline/après capture|
|B5 pendant phases sensibles|Budget nominal FIFO512mots /~30000mots/s ≈17,067ms depuis vide|Retirer lecture/mailbox/submit **avant** snapshot ; réponse04 explicite ; aucun I/O induit par ces requêtes|
|Boot/probe/prepare|Scans de recovery, deux validations contexte init/start, double lecture262144FRAM et CRC après UART-start|Capteur encore au repos ; payload double lecture ≥1,678s estimées à2,5MHz. Non découplé ; A04-07 reste ouverte|
|Après cleanup|Read512FRAM/poll, CRC global et B5 synchrones|Sensor ODRoff/bypass/timestampoff vérifiés avant réouverture ; hors deadline acquisition|
|Commit physique|Plancher payload write+read51914×2×8/2,5MHz ≈332,25ms, hors commandes/CRC|Estimation, pas nouvelle mesure de commit ; aucune commande persistante envoyée physiquement|

Solution proportionnée : prédicat de quiescence du moteur FIFO et guard optionnel de binding dans le **single main-loop** cumulatif. Il refuse pendant CONFIGURING/CAPTURING/CLEANUP, avant démarrage en attente, et après cleanup défaillant/non confirmé. ARMED sans trigger ou READY/INVALID **avec cleanup vérifié** réouvrent l'accès. La composition injecte le guard après préparation ; init/start conservent la validation complète fail-fast. Les requêtes suivantes repartent d'un contexte effacé : snapshot faute =>pas de stale B5 ni submit authority, compteur de faute et04. **Aucun cache** à invalider/reconstruire, aucun journal/format/commit modifié, pas RTOS/DMA généralisé.

Instrumentation DWT passive uniquement dans le cumulatif : max durée Modbus globale/en phase protégée, max intervalle entre entrées du poll FIFO pendant démarrage/capture/cleanup ; compteurs deny/snapshot fault. Le compteur32bits est traité par différences modulo ; campagne~1,247s, opérations observées bien inférieures à son tour~26,84s à160MHz. Cette instrumentation ne mesure pas le boot ni la vérification FRAM post-cleanup comme charge d'acquisition.

## 3. Validation logicielle IO-01..04

Validation complète requise car interface partagée et préparation fonctionnelle modifiées :

```sh
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh
```

Retour0, **126/126 CTest**, cross-build cumulatif. [Commande/retour](preuves/TR2-20261013-A04-02/validation-full-command.json), [sortie complète](preuves/TR2-20261013-A04-02/validation-full.log), [résultats détaillés126](preuves/TR2-20261013-A04-02/host-126-results.log). Aucun test désactivé/affaibli ; pas de relance de validations réussies après le checkpoint. Les scripts physiques/preuves/docs ajoutés ensuite ne modifient pas le code testé. GCC14.2.1 20241119/SDK U5v1.9.0 `d88042df24f16799957c24f00c2b234c9e306188`, Debug/O2 ciblés M015 conservés.

| Acceptation | Preuve acquise |
|---|---|
|IO-01|Test RTU existant renforcé : B0/B1 sans snapshot même si celui-ci échoue ; B3 avec fixture host de supervision, séquence cohérente, changement de valeur puis retrait d'image stale/indisponible ; vrai CRC/réponse ; backend média count0 dans le nouveau test pendant capture|
|IO-02|Chaque B5 disponible vient du vrai journal avec source_generation ; fault retire image et write authority et produit04 ; pas réouverture du store après simple retour du média, recovery réel requis puis dernier résultat47 retrouvé ; pas stale/cached-success|
|IO-03|Nouveau test `modbus_fifo_io_budget` : vrais RTU/PDU, dispatcher/journal et moteur FIFO ; vrai média host enveloppé avec coûts2,5MHz +20µs/transaction, commit51914×2octets ; producteur indépendant30words/ms/profondeur512, SPI4MHz et100µs loop/UART. **Simulation** : scanB5=131584µs ; capture32768XYZ/4096TS/maxFIFO212/OVR0 ;17refus et zéro média I/O en phases protégées ; contrôle négatif sans guard dans fixture =>OVR1/INVALID|
|IO-04|Même transaction après cleanup admise ; retry sans nouvelle écriture/commit ; collision04 sans mutation ; défaut/recovery. Suite préexistante réutilisée : D1..D8/corruptions/coupures/réadmissions/power-loss/dernières générations. Oracles de durabilité conservés ; aucune nouvelle coupure physique ou mutation média pour les reproduire|

Le changement d'oracle du test snapshot est **fonctionnel et renforcé** : ancien silence `TR2_ERROR_INTERNAL` sansTX remplacé par réponse04/CRCvalid et absence d'image/mailbox ; il découle du contrat d'indisponibilité explicite, pas d'un contournement. Échecs initiaux des nouveaux fixtures (nom du type recovery et génération source B3 incohérente) corrigés à partir des headers/guard réels, sans enlever le guard B3 ; logs conservés. Les avertissements newlib/nosys sont conservés dans le build, sans erreur finale.

## 4. Qualification physique bornée IO-05

**Banc observé :** TR2-001, STM32U575, SN **003500463235510B37333439**, UID **004A003D3842501820333157** ; HP **LAPTOP-3F6J7STJ**, FT232 COM7 **1152008E1**, slave1. Seconde NUCLEO non accédée. Procédures déjà qualifiées relues, listingST-LINK et probeHP non série avant toute action. Flash courante avant intervention : préfixe byte-identique au golden M015-V ; sauvegarde Flash2Mio vérifiée contre ce préfixe. GDB confirme capteur quiescent/gen12 ; sauvegarde logique51914octets.

**Image exécutée** [BIN complet en hex](preuves/TR2-20261013-A04-02/evaluated-cumulative-spi4.bin.hex) :169948octets SHA-256 **b4301d6f3325c266edc19297413fd0885828e5625ec030a8acad340444b3fd9b** ; ELF SHA **3a1310a77ba379bbc513dd57d0fbfdb7cab89aac50ab2e67400abddc903f40a1**. Profil explicite CUMULATIVE/Debug/SPI4/production/TX/observer/FIFO, autoritéM014-H, ELF sans SD bringup/WriteBlocks. Runtime modifié (+568octets), donc nouveau flash utile et essai physique exigé. Pas de claim BINgolden identique.

**Un flash standard** qualifié `-c port=SWD sn=... freq=8000 -w <image> 0x08000000 -v -rst`, verify réussi et readback GDB exact. Queue Flash à partir de172032octets (21pages8Kio) byte-identique au backup. Reset logiciel GDB après sortie du programmeur justifié par la perturbation NVIC documentée M013-D ; **pas de HOTPLUG**, aucun autre reset/flash après la capture. Aucun mass erase/Option Bytes/RDP/déprotection, provisioning, écriture SD ou commande métier FRAM physique. Double lecture FRAM existante au boot validée avant armement ; logiquegen12 byte-identique au préflight initial.

Une baseline B5 (1requête), puis **une seule capture** armée par trigger RAM existant sur prochaine réception, avec5requêtes bornées. Aucun debugger pendant les deux phases de trafic ; sessions fermées et gate récent avant chacune ; mutexCOM7, contrôle identité/PnP/processus, timeout borné et retry0. Aucun nouveau cycle pour améliorer les résultats.

| Read physique | État MCU observé | Réponse HP complète / résultat |
|---|---|---|
|B5-before5000×20|ARMED, capteur au repos|FC03 normal/CRCvalid ; délaiHP241,378ms|
|B0-trigger0×21|CAPTURING, XYZ0 au service|FC03 identité attendue/CRCvalid ;74,615ms|
|B1-active1000×20|CAPTURING, XYZ6826|FC03 statut/uptime courants/CRCvalid ;21,312ms|
|B3-unavailable3000×48|CAPTURING, XYZ12629|**01 83 04 40 F3**,10,127ms ; source métier non disponible, ne pas fabriquer vibrations V1|
|B5-denied5000×20|CAPTURING, XYZ17976|**01 83 04 40 F3**,16,621ms ;1deny,0snapshotfault|
|B5-after5000×20|READY après cleanup, XYZ32768|FC03 frais, payload byte-identique à baseline,158,862ms|

DélaisHP incluent transport/Windows et ne sont pas des coûts CPU. Les six trames de requêtes/réponses et leurs CRC sont **indépendamment recalculés** depuis les bytes des [journauxHP complets](preuves/TR2-20261013-A04-02/hp-full-capture-result.json), pas déduits de leurs SHA. Baseline journal [complet](preuves/TR2-20261013-A04-02/hp-full-baseline-result.json). Relecture SSH des fichiers HP entiers et hash vérifiés : SHA baseline `0f77d9b79e93f5e8b9a0e0c05ed290953ca9574c5a99d64d24b39429e3ef6209`, capture `11a9274372cda26806d37932dd4bfbc84afc4235ba5526b2175a6d59bb0d9222` ; [preuve](preuves/TR2-20261013-A04-02/hp-evidence-readback.json). Aucun accès série lors de cette récupération.

### Acquisition, marge et conservation

[Audit indépendant offline](preuves/TR2-20261013-A04-02/physical-independent-audit.json) sur dumps/bytes, [304bursts décodés](preuves/TR2-20261013-A04-02/decoded-bursts.json), [six observationsModbus](preuves/TR2-20261013-A04-02/decoded-modbus.json).

- **32768XYZ +4096TS /36864mots**, durée début→cleanup1247ms ; huit préfixes4096XYZ, dont quatre taillesV1.304bursts contigus avec CRC de **chaque** payload concordant à la RAM ; parity/counter/timestamp monotone/saturation contrôlés sur la RAM complète. Reconstruction SPI intégrale seulement sur les derniers4608mots, soit4096XYZ+512TS, avec préclose/longueurs/offsets/state et comparaison byte-à-byte. Ne pas revendiquer un SPIjournal intégral sur32768XYZ.
- **FIFOmax293/512**, flags agrégés **0x81** : watermark0x80 et bit haut de niveau0x01 attendus ; **masque critique0x68 nul**, aucun FULL/OVR/latch observé. L'audit offline initial réutilisait improprement flags==0 de M015-V (max158) : corrigé à partir du masque réel inchangé du driver, données conservées ; aucun firmware/test de garde affaibli, aucune nouvelle capture.
- À CPU160MHz : **gap maximal entre servicesFIFO5,855106ms**, pollFIFOmax2,879294ms ; serviceModbusmax en phase protégée **5,836194ms**, global hors/protégé **144,138431ms**. À débit nominal26667×9/8=30000,375mots/s, réserve au max293 =219mots ≈**7,299909ms**, marge comparative≈**1,444803ms**. Minima/marges estimés par débit nominal, mesuresDWT pour les durées ; maxima non nécessairement simultanés. Cette comparaison et l'absence FULL démontrent la portée bornée, **pas WCET ni endurance/perteglobalezéro**.
- IRQ :48RXcallbacks,6TXstarted/6TXcomplete,60eventspushed/polled ; drops/errors/fail-safe0, UARTErrorCode0, txbusy/faultfalse, productionpollerrors0, CFSR/HFSR0. Cleanup **0C 80 00**, stateREADY/FIFOvalid, capteur au repos.
- **FRAM** : CRC32 avant/après **0xB422080A**, indépendant depuis backup262144octets ; SHA256 de la lectureavant `d732acc557152180fc394097d6a5269ea2f45c1da387c8643f512318adf92fce`. Image logique51914octets avant/après **byte-identique**, SHA `560f0551315df7b3146f79850f67b72dd7cdf8a3234bbc136e00e89e4d26b239`, recoveryVALID/gen12. Pour la FRAM complète après capture, preuve **CRC32 MCU**, pas nouvelle lecturehost SHA256 complète ; ne pas confondre ces niveaux.

## 5. Limites et décisions suivantes

Politique dépend du propriétaire unique main-loop et du binding cumulatif audité ; callers futurs concurrents/hors ce chemin ne sont pas automatiquement protégés. B5 reste synchrone au repos, boot reste bloquant après UARTstart, et cleanupfaute maintient l'indisponibilité jusqu'au diagnostic approprié. Le test physique ne couvre pas un commit/coupure/commandeB5durantcapture (refus précoce, tests hôte de ces garanties), trafic soutenu ni combinaisons arbitraires ; marge1,445ms limitée. Timestamp monotone/mod4/CRC ne démontrent pas l'absence de perte globale ; A04-05 demeure. Pas FFT/métrologie/configuration V1 active/bulkSD/chaîneWeb ou STOPmétierM016 qualifiés.

Firmware cumulatif **A04-02 conservé** sur carte ; capteur nettoyé, débogueur détaché, portHP fermé ; aucune restauration M014-H ni reflash inutile. Permissions commit/push/flash/debug exercées ; aucun geste physique demandé ni règle permanente modifiée. Backups, images et preuveFlash complète locaux `/home/lolo/dev/msm/reports/TR2-A04-02-20261013T152741Z/`. Données existantes/microSD non écrites ; pas d'ordre OptionBytes.

Journal `ETAT_COURANT_TR2.md` actualisé : code/binaire courant A04-02, preuvebornée, politiqueB5, marges et nextreview ; anciennes références durables corrigées vers leurs archives. Roadmap et registreA-04 **inchangés**. ChatGPT : contrôler code/tests/bytes/limites, décider clôtureA04-02, traiter liens historiques/statuts du registre si pertinent ; A04-03/M016 prochaine mission distincte, A04-07/A04-05/A04-06/A04-08 restent suivies.

## 6. Publication et état Git

[Manifest](preuves/TR2-20261013-A04-02/manifest.json) :113 fichiers, empreintes exactes, 1539192octets de preuves ; BINhex, RAM/SPIbornés, commandesGDB/CLI/SSH réellement exécutées, scripts et logs. Sorties brutes conservées (whitespace des logs/caches/patch : attendu, non modifié pour masquer un résultat) ; code/documents vérifiés via diffcheck. Contrats/archives/dette et protectionA04-01 préservés. Worktree propre au checkpoint5088d50, actuellement seulement rapport/journal/preuves de clôture à publier ; aucun code instable ni blocage.

Après push normal explicite `HEAD:refs/heads/main`, relecture intégrale des fichiers depuis GitHub à effectuer, puis statutDONE. Aucune validation réussie, transmission, flash, reset ou lecture physique supplémentaire nécessaire. Le SHA du commit final distant sera fourni dans la réponse finale ; ne pas l'inscrire dans son propre commit.
