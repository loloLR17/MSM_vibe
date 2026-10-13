---
mission_id: TR2-20261013-A05-AUDIT-TESTS
status: IN_PROGRESS
base_ref: main
base_sha: d0238953ee0b0245f8540041fe6593939615c703
initial_head: 7c4af323429768fc7ef0f5dd8d0ca872e0a06b3a
result_sha: 7c4af323429768fc7ef0f5dd8d0ca872e0a06b3a
created_at_utc: 2026-10-13T14:19:22.818710+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# A-05 — Pertinence des tests et matrice minimale avant corrections

**Audit terminé, sans correction ni nouveau test produit.** Les 125 entrées CTest du firmware sont reliées aux sources réelles ; les résultats M015-V/A-04 sont vérifiés et réutilisés. Dix configurations CMake ont été caractérisées (deux acceptées, huit refusées par les gardes attendues), et les six tests Python existants du générateur d'autorité ont réussi. Une matrice de **25 scénarios proposés**, regroupables dans les tests existants, précise niveaux, méthodes, critères, preuves, priorités, coût/risque et couverture à réutiliser.

**Aucune dette A-04 n'est corrigée, clôturée ou reclassée.** Le registre, le journal, la roadmap, AGENTS.md, le firmware, CMake et les tests sont inchangés octet par octet. Ordre conservé : A-05 → correction A04-01 → correction A04-02 → A04-03/M016. A04-08 avant activation SD, A04-09 facultatif/différé. IN_PROGRESS correspond seulement à la publication/relecture à terminer ; aucun blocage de cet audit. DONE A-05 ne vaudra ni implémentation des 25 scénarios, ni clôture A04-04, ni qualification A-01/M016.

## 1. Référence, méthode et garde-fous

HEAD initial/distant : `7c4af323429768fc7ef0f5dd8d0ca872e0a06b3a`, base `d0238953ee0b0245f8540041fe6593939615c703` vérifiée ancêtre. Worktree neuf `MSM_vibe-A05`, branche locale `codex/a05-audit`, push normal vers `main`. Sources firmware/tests, projet ST et lanceur inchangés depuis `ef468539b9943cc9b9b4d832e513a0f8ab05c7ae`, SHA physiquement testé M015-V. L'audit A-04 courant, livré au commit `f8e05ace01aa236e86ae7a38dc5c9589a9e29f48`, a été archivé [byte-identique](archives/TR2-20261013-A04-AUDIT-FIRMWARE/RAPPORT.md) avant remplacement.

Méthode : inspection CMake/source/tests et manifests ; découverte CTest **sans exécution** dans le build M015-V préservé, correspondance125noms→cibles/fichiers/empreintes ; revue des oracles, des mocks et des traces existantes ; nouvelles caractérisations CMake et test du générateur en mémoire seulement. Pas de relance des 125 tests ou du build cumulatif déjà reproduit en A-04 : aucun changement de code ne la justifie. Aucun téléchargement/CI lourde/RTOS/refonte.

Permissions flash/debug **false, non exercées**. Aucun STM32, ST-LINK/GDB, reset/flash, HP/COM7/RS-485, FRAM ou microSD accédé. L'état de la carte reste celui rapporté en M015-V, pas une télémétrie nouvelle. Préservation worktrees/stashes et image cumulative ; aucune restauration/sauvegarde matérielle par automatisme.

## 2. Inventaire réel et force des preuves

[Inventaire exhaustif des 125CTest](preuves/TR2-20261013-A05/firmware-tests-inventory.json) : nom, cible, sources réelles, empreinte de chaque source, inclusions C directes et variantes. [Découverte CTest](preuves/TR2-20261013-A05/ctest-discovery.json), [inventaire complémentaire](preuves/TR2-20261013-A05/other-tests-inventory.json).

| Ensemble | Constat vérifié | Exécution A-05 et limites |
|---|---|---|
|Firmware CTest principal|125 entrées,120 fichiersC de tests dans `05_Firmware/tests` tous référencés,6 headers mocks et1Python ; variantes TX/IRQ et deux sources de tests ST externes|Découverte seule ;125/125historique vérifié, pas125preuves physiques ni mesure de couverture en pourcentage|
|Unitaires cœur|Codecs/mapping/projections, configuration, indicateurs, temps, journal/persistance/transaction images|Réutiliser le log complet125/125 ; petits fixtures et mocks mémoire ne modélisent pas les temps SPI|
|Intégration hôte|`test_modbus_system_server`, `test_production_commands`, runtime/campagne/B3, recovery/configuration|Callbacks métier réels avec plateforme simulée ; pas composition complète STM32/FIFO/FRAM|
|Adaptateurs STM32 mockHAL|Source directe, anciens FIFO/captures, transport ST, RX/TX/IRQ/DE/TC/watchdog|Appellent des sources réelles, mais clocks/statesHAL fictifs et instrumentation sélective|
|Moteur coopératif|`fifo_cooperative_v1_windows_and_faults` : quatre tailles,11modes,30 positionsI/O, stop/timeout/cleanup|Ne compile pas le glue `stm32_fifo_cooperative.c` ni la boucleproduction ; FIFO simulé constant300, temps +1/poll|
|Projet ST autonome|Six entrées CTest sous `BENCH_HOST_TESTS` : fenêtre, SPIpreclose, driver/exemples, qualificationoffline, HALinstrumentation, tools|Inventoriées, pas exécutées A-05 ; ne pas les additionner aux 125en ignorant le partage de sources/oracles|
|Générateur autorité|`tests/platform/test_commissioning_generator.py` : six unittest|**6/6 exécutés A-05** ; non enregistré dans les 125CTest principaux|
|Supervision .NET|Sept projets de tests réels sous `Supervision TR2/tests` (Architecture/Protocol/Transport/Service/Application/Domain/Persistence)|Inventoriés, pas exécutés ; aucun nouveau nombre de tests passés revendiqué|
|Validation ARM|Default compile historique M015-V ; cumulatif A-04 reconstruit avec options explicites|Résultat historique vérifié, pas exécuté A-05 ; compilabilité/ELF ne qualifient pas timing ou média|
|FT documentaires V1|Fiches sous `02_Validation/FT_*` et mapping gelé ; définition des exigences|Références documentaires, pas nouvelles campagnes ni équivalence automatique aux tests exécutables|
|Matériel|M014-H B0/B1/perte-reprise et M015-V acquisition32768+deuxreadsB0/B1 ; état/proofs du rapport archivé|Historique vérifié selon les fichiers disponibles ; aucun essai A-05|

Les quatre cibles `stm32_rtu_tx_0/1`, `stm32_irq_observer_0/1` partagent une source mais compilent des macros différentes ; elles restent distinctes. À l'inverse, plusieurs scénarios résident dans un seul exécutable CTest : le nombre125n'est pas un dénombrement d'injections de panne.

### Commandes et provenance réellement vérifiées

- **A-05, non exécutant les tests :** `ctest --test-dir '/home/lolo/dev/msm/projects/MSM_vibe-M015-V/Modbus RTU/05_Firmware/build-host-validation' --show-only=json-v1` ;125 noms reliés au CMake actuel et à ses sources identiques.
- **A-05, exécutant six tests :** `python3 -B 'Modbus RTU/05_Firmware/tests/platform/test_commissioning_generator.py' -v`, retour0/6 tests/OK. Cas : B0 autoritatif, UIDdupliqué, entréeRESERVED, autoritéSD non permise, capacité non qualifiée, génération non revue. Pas accès média.
- **A-05, configure uniquement :** dix commandes CMake dans [cmake-current-characterization.json](preuves/TR2-20261013-A05/cmake-current-characterization.json), directories distincts, SDK qualifié et inputscommissioning ; huit échecs **attendus et vérifiés par message précis**, pas une erreur de compilation déguisée.
- **Historique125/125 :** `preuves/TR2-20261013-M015-V/validation-after-refresh-fix.log` ; sa sortie porte la validation hôte complète et ARMdéfaut. SHA et manifest dans [historical-proofs-verified.json](preuves/TR2-20261013-A05/historical-proofs-verified.json). Sources/tests identiques, pas relance de la suite.
- **Historique build cumulatif :** A-04 `canonical-build-command.json`, `canonical-reproduction.json`, `sections.log`, `symbols.log` etprovenance. Code audité `c45a197`, identique au code ef468539 ; BIN **8529630a5a6c9aaa2449fde512b06d6ca16a89a08b7afea2abc9ecca3b75d480**,169380 octets byte-identiques à M015-V. Les 98 fichiers du manifest V et les 28 fichiers du manifest A-04 sont vérifiés A-05. Aucun nouveau buildARM ici.
- **Historiques physiques V :** `attempt02-independent-audit.json`, RAM complète/304bursts, SPIraw dernière4096, journalHP complet `attempt02-hp-full-physical.jsonl` ; SHA recalculé historique et contenu publié. Configuration CPU160/HSI16/SPI4/mode3, codeef468539, UID004A003D3842501820333157, SN003500463235510B37333439. Pas nouvelle identité lue en A-05. Le premier FAIL et le retourM014 restent séparés.

Commande `tr2_validate.sh` : valide le cœur hôte et le build **par défaut** ; elle ne suffit pas pour vérifier le cumulatif ni les tests Python/.NET externes. Les commandes du futur contrat doivent distinguer ces ensembles au lieu d'annoncer « tout validé ».

## 3. Lacunes significatives et réutilisation ciblée

### A04-01 — sélection du build et sûreté des profils

Actuel : les huit gardes négatives examinées rejettent production sansTX, FIFO sansproduction/observer, production+manufacturing/probe, FIFO+U/silence et SPI16. Défaut et qualificationU sont acceptés à la configuration. Le cumulatif4 et son absence de SDwrite lié restent démontrés par A-04. **Aucune preuve de sûreté du défaut actuel** : l'ancien chemin `Sdmmc2_Bringup` reste le risque déjà enregistré.

Manque : test du choix d'artefact sûr, refus d'une sélection production absente/ambiguë, matrice de modules réellement atteignables, garde contre confusion des profils. Une absence de symbole nommé seul ne prouve pas absence de toute écriture : vérifier sources/profil, callbacks, modulelist/ELF et empreinte d'image. Après correction purement de sélection, BINcumulatif identique ⇒ pas de nouveau flash ni reprise physique M015parautomatisme ; binaire/runtime modifié ⇒ analyse de delta et qualification ciblée.

### A04-02 — FRAM/Modbus et temps

`test_modbus_system_server.c:320–343` : spies snapshot, B0/B1 sansB5, uptimefrais, B5backend indisponible (`TR2_ERROR_INTERNAL`, sansTX) puis reprise. **B3 sans scanB5 manque**, ainsi que le coût de la boucle complète et de la FRAM physique. Les interfaces de media des tests de journal utilisent memcpy/counters : durabilité et mutations bien couvertes, tempsSPI non modélisé.

Réutiliser sans dupliquer : bounded reader/store, `test_p12h3c3c4d1_fault_media` (rollback et donnéeslisiblesnondurables), D2..D5 coupures admission/mutation/terminalisation/réadmission, D6..D8 copiecorrompue/retry/collision etnonterminal, `test_p12h3d2b_transactional_image_power_loss` etrecovery. Fonctions D7 vérifient qu'un retry aprèsreboot ne réexécute/réécrit pas. Ajouter uniquement les transitions rendues nouvelles par la correction (snapshot/cache/service d'I/O borné).

B5error silencieux actuel est un **résultat hôte observé**, pas une preuve d'indisponibilité intelligible pour lePC. Le futur contrat doit préciser le retourautoritatif/exception/étatV1 attendu, sans inventer un codehorsmapping ni servir un cachepérimé. Conserver ACK/durabilité/recovery/CRC/adresse etlastautoritatif.

### A04-03/A04-04 — configuration et vrai glue STM32

Le transport réel est inclus dans `test_st_window_adapter.c:1`, mais `HAL_GetTick` y retourne0 et les préclose sont fabriqués par le mock ; ce test vérifie les guards, pas les budgets. Le moteurcoopératif n'inclut pas l'adaptateur STM32 ni les doubleslecturesFRAM/CRC, le recyclage des traces, l'armementRX et la publication finale. Les 30 positionsI/O ne signifient pas toutes les transactions des 304bursts ni tous lesreadbackscleanup.

`test_stm32_iis3dwb.c` couvre déjà les quatre échelles etconversion±1000, saturation, masques1/7, indisponibilité etrestauration de l'ancien diagnostic2048. Ce n'est pas une preuve du FIFO sous configurationmétier. Réutiliser son oracle deconversion en le confrontant à desvecteurs indépendants au seamFIFO. Les quatre tailles du moteur sont déjà couvertes ; ajouter bornes etfin/réarmement du nouveauglue, pas dupliquer ce moteur.

`test_p8j_system_runtime_b3_projection.c` utilise une fenêtre de **deux samples**, construite par fixturevalidée synthétique. Il prouvele cheminstatistiques→B3, le retraitlive etl'absence de restaurationdeB3 au reboot, pas une fenêtreV1 physique. `test_indicator_calculator.c` vérifie sums/sqrtentiers, zérovalide, overflow etchronologie. Ajouter une intégration sous configurations V1 réelles, avecownerSPI3 unique et génération/échelle liées auRAW ; aucune nécessité de112capturesphysiques pour tailles×échelles×axes.

Point d'oracle à conserver : `test_supervision_service.c:109–120` rejette zéro valid_sample et **conserve le dernier snapshot byte-identique**. La matrice n'impose pas d'effacer l'historique : elle exige absence de nouvelle valeurpartielle présentée complète, pas de fauxrafraîchissement de date/séquence/génération, et qualité conformeV1. Les attentes doivent être figées au seam métier, pas copiées du parser ou d'un résultat actuel.

### A04-05 — timestamps, phases et preuves de perte

Parser : parité/tag, modulo4, monotonie et8XYZ entreTS ; moteur : TS régressif/espacement, mais pas borne supérieure de delta. La caractérisationA-04 accepte24→48 ; ce résultat **ne devient pas un oracle de conformité**. `test_fifo_window.c:78` teste le wrap du **tick MCU**, pas le wrap d'un timestampcapteur. AjouterdeltaTS32wrap,TSnul/répété/rétrograde/gap excessif, tolérancejustifiée etfrontières de fenêtre.

Ne pas confondre lesdeux points d'entrée : le test `fifo_window_run(4096)` attend4096XYZ/511TS, alors que le servicecoopératif viseN/8TS (512pour4096) et sonfixture démarre parTS. Cette différence de phase/admission n'est pas une correctionni un allégement proposé : formaliser l'alignement initial/terminal pour le seamFIFO ettester sa politique réelle. Aucun compteurmodulo4 niCRCdes RXRAM ne prouve une absenceglobale de perte.

### A04-06/A04-07/A04-08 et A04-09

- **Mémoire :** sections A-04 vérifiées,183784 octets statiquement libres (~179,48Kio), pas high-water pile/heap. Budget explicite avantbuffersDSP ; contrôler source/sections, sentinelles/ownership etstack-usage, puis mesure souscharge lorsque code pertinentexiste. Pas imposerFFT ici.
- **UART/startup :** `test_stm32_rtu_tx` couvre OVRHAL/DE/TC, erreur etwatchdog50ms ; manque débordement de la **file logicielle de 64 emplacements (63 événements utilisables : `event_push_from_isr` réserve un emplacement pour distinguer plein/vide)** avec scheduler/FIFO/FRAM, et requêtespendant startupFRAM. L'essaiVattendait bench_ready et n'a envoyé que16octets. Ne pas confondre cespreuves avec stabilité sous bus saturé.
- **Erreurs HAL :** tests de retours!=OK ne mesurent pas10/20/100ms de stall ; une clockinjectée doit avancer et fairecroître le FIFO pendantI/O. Un timeout peut violerle budgetnominal : attendu = défaut/invalidation/cleanup sûr, **pas maintien artificiel deREADY**.
- **SD :** SDoff à vérifier sur nouveauxprofils ; backendSDsynchrone/futur4bits reste missiondistincte avantactivation. Aucun test sur la microSD existante ni backendstockage actif requis pour M016sansSD.
- **A04-09 :** facultatif/différé inchangé ; aucune tâche de simplification ou CI lourde ajoutée par cet audit.

## 4. Matrice minimale proposée

[Matrice détaillée25scénarios CSV](preuves/TR2-20261013-A05/proposed-test-matrix.csv) : ID, dette, scénario, niveau, méthode, critères mesurables, preuves, priorité, coût/risque, couverture etstatut **PROPOSED : validation future après correction**, avec les observations actuelles indiquées séparément. Ces25groupes ne sont pas25nouveauxexécutables ni25essaisphysiques.

| Tranche/IDs | Vérifications prioritaires | Niveaux / réutilisation |
|---|---|---|
|A04-01 : BUILD-01..03, SD-01|Sélection sûre, combinaisonsinvalides, autorité, ELF/callgraph/BIN etSDoff|CMake/statique/ARM ; reprendreA04 et8guardesA05, testeruniquement delta|
|A04-02 : IO-01..05|B0/B1/B3sans scan non demandé ; B5autoritatif/indisponible ; coûts scan/commit, durabilité/retries ; coexphysique|Hôte/intégrationtemps puisune campagnephysique bornée aprèscorrection|
|A04-03/04 : FIFO-01..04, DATA-01..02, CALC-01|Ownerunique,4tailles/4 échelles/7 masques, génération/admission, défautscleanup/CRC, oracleindicateurs/projection|Vrai gluemockHAL +coreexistant ; scénariosgénéréslogiciellement, pas grillephysique exhaustive|
|A04-05 : TIME-01|Gaps/wrap/tolérances, alignementdébut/fin|Oraclelogiciel indépendant ; politiqueàfiger avant revendicationcadencestrict|
|A04-04/07 : UART-01, BOOT-01, ERR-01|Queue64, traficboot, stallsHAL, défauts/prérequisREADY|Scheduler +clockinjectée ; physiqueconditionnelle au chemin modifié|
|A04-06 : MEM-01|Sections/ownership/budget, pileheap mesurées au bonmoment|ARM/statique d'abord ; mesuresdynamiques futures|
|M016 : PHY-01|RAM/config→unités/indicateurs→B3 physique, fraîcheur etB0/B1 conservés|Campagnefuture limitée au code nouvellementintégré ; pas replayVinchangé|
|Ultérieur : STAB-01, SD-02|Endurance,coexistenceSD/4bits etpartition|Hors corrections1/2/M016sansSD ; contrat dédié|
|Chaque tranche : PROOF-01|Traçabilité, manifests, code/artefact, brutsacceptés/refusés, relecturedistante|Réutiliserle protocole etles outils existants|

### Budgets et critères chiffrés à mettre dans les prochains contrats

Pas de seuil V1 nouveau adopté en A-05. Proposition de méthode mesurable : débit nominalmots `rho = 26667 × 9/8 ≈30000/s`, servicepayload128mots budgetnominal `T128 = 128/rho ≈4,267ms`, FIFO512 depuisvide≈17,067ms. Pour chaque intervalle entrevidanges, enregistrer niveauinitial `L`, durée `dt` ettailledrainée ; vérifier le cumulproduction/consommation et une margeconvenue plutôt qu'une moyenneglobale.

Proposition conservatrice à **figer dans le contrat de correction**, pas qualification actuelle : serviceFIFO128 complet(parser/guards/copie/CRC)≤T128 au nominal, et `L + ceil(rho_max × dt) < 512 - R`, avec réserve `R` approuvée (exemple128mots) et `rho_max` intégrant la tolérance d'horloge. ÀL158 etrho nominal, R128 donne un maximum sansvidange≈7,53ms ; ceschiffres sont desestimationsdeplanification, pas une bornephysique déjà prouvée. Distinguercadenceavecattente, serviceactif, boucleModbus complète, coûtFRAM, postcleanup etcheminerreur.

LatenceB0/B1/B3/B5 : mesurer intentionPC→réponsedécodée ettraitementMCU→TC séparément ; respecter deadlinePCfigurant au contrat (référenceM0141000ms). Le succèsPC seul ne prouve pasrespectFIFO ; disponibilitéB3/B5 ourefus doivent être explicites. Instrumentationminimale : DWT début/fin du poll complet etcallbackFRAM, compteurbytes/reads/writes/commits, niveauFIFO, maxima/eventsUART, génération/cache etétatcleanup. Pasjournalverbeux paroctet dans laboucle, DMA/RTOS ni GPIOinstrumental exigeantun nouveau câblage par défaut.

Métrologie de timestamps : tolérancefréquence, résolution/compensationFREQ_FINE etpolitiquephase à définir àpartir références ST etconfigréelle ; pas « exactement24ticks » arbitraire. La caractérisation actuelle n'établit pas cesseuils. Mémoire : exiger budgetchiffré etpreuve de marge souschargereprésentative, pasinventer uneoccupationpile mesurée enA05.

## 5. Ce qui peut éviter des répétitions et ce qui reste physique

- Correction1 gardes/preset qui conserve le BINcumulatif : contrôleCMake/ELF/hash etréutilisationphysique V ; **aucun nouveau flash requis par automatisme**.
- Correction2 change le chemin timing Modbus/FRAM : conserver lesunitairesdurabilité existants, compléter le spyB3 etle modèletemps, puis **une campagnefuture** mixtebornée sur la nouvelleimage avec commandesautorisées. Pasreproduire141ms exprès niécrire surmédia pour testerun cachelecture.
- M016 introduit glue/configuration/unités/B3 : ajouter lesseams concernés etune preuvephysiquecomparative réellement nouvelle ; taillesintermédiaires peuvent êtrepréfixes si la continuitéest l'objet, mais une vérification de terminaison/reprise différente doit rester identifiée. Sourcepartition etqualité conservées.
- Pas de campagnefullcartésienne112réglagesphysiques. Paramétrer enlogiciel tailles/échelles/masques avec un oracle d'unités, puis justifier lescasphysiques par réglagesruntime/régimeslimites nouveaux, pas un compte de tests.
- Endurance/zero-lossglobal, calibration/excitationexterne, SD4bits etchaîneWeb restent qualifiés dans desmissions ultérieures. A05n'impose pasdespreuves nonpertinentes pourcloreun correctiflogiciel réversible.

## 6. Livrables, dette et limites

Preuves locales durables : `/home/lolo/dev/msm/reports/TR2-A05-20261013T140123Z`. [Manifest A-05](preuves/TR2-20261013-A05/MANIFEST.sha256),inventaires,logs des 10 configurations,6 tests Python etmatriceproposée. Les 125résultats etlesimagesARM restent au lienhistorique, nonrecopiés/relancés. RapportsA04/M015 etpreuvesimmuablespréservés ; aucune qualificationindépendanteou tag effectué.

Registre A04 SHA-256 initial **804b90967317196374a844a86c955c2e91880045061e0d7241aeac9f3cfa8a8a** ; àcontrôlerbyte-identiqueaprès publication. Huit dettes OUVERT etA04-09DIFFÉRÉ demeurent tels quels. A05 définit la matrice associéeA04-04, **pas sa clôture**. Journal/roadmap/gouvernancenonactualisés conformément au contrat. Lesliensdynamiques anciens vers DERNIER_RAPPORT ne doiventpas servirdepreuvehistorique : utiliserl'archive A04citée ; aucunlien opérationnel corrigé horspérimètre.

Limites : aucune mesuretemps/couvertureinstrumentée/pilephysique nouvelle ; aucun résultatxUnit/Supervision executéA05 ; caractérisationCMake ≠ compilationsdesdixprofils ≠ sûretéfirmware ; modèletemps etfuturematrice nonimplémentés. PreuvesRXHAL/copierRAM ne sontpas une capturewire externe ; SHA seul ne prouve pas le contenud'unjournal. La revue ChatGPTvientaprès ce rapport avanttoute correction.

## 7. Publication

Diff limité au rapport, àl'archiveA04prescrite etpreuvesstrictementnécessaires. Commit/push etrelecturedistante àterminer ; aucuneinterruption niobstacletechnique. SHA depublicationfinal etétatGit serontfournisaprès vérification. Le code etlesdettes restent inchangés.
