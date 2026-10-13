---
mission_id: TR2-20261013-A04-AUDIT-FIRMWARE
status: IN_PROGRESS
base_ref: main
base_sha: 82a151273d214b07bee033082824d4c0dc0abba6
initial_head: c45a197416ad30fabf932da364eb540935693219
result_sha: c45a197416ad30fabf932da364eb540935693219
firmware_physically_tested_sha: ef468539b9943cc9b9b4d832e513a0f8ab05c7ae
created_at_utc: 2026-10-13T13:44:26.063715+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# A-04 — Audit du firmware cumulatif avant M016

**Audit technique terminé, sans correction.** Le BIN cumulatif est reproductible : une reconstruction neuve depuis le HEAD courant produit exactement les169380octets de M015-V, SHA-256 `8529630a5a6c9aaa2449fde512b06d6ca16a89a08b7afea2abc9ecca3b75d480`. Les acquis M014/M015 ne sont pas invalidés. Trois prérequis ressortent pour la qualification physique M016 : choix sûr du profil, maîtrise des accès FRAM synchrones, et raccordement autoritatif des fenêtres FIFO aux traitements/publications. Aucun flash, debug, reset, accès HP/COM7, transaction capteur/FRAM/SD ou nouvelle sauvegarde matérielle en A-04.

L'état IN_PROGRESS porte uniquement sur la publication/relecture finale de cet audit. Les constats « bloquants avant M016 » ne sont pas des obstacles à l'achèvement d'A-04. A-04 n'est pas la revue indépendante de qualification A-01 ; aucun tag ni verdict de qualification ajouté.

## 1. Référence, méthode et préservation

HEAD initial et distant : `c45a197416ad30fabf932da364eb540935693219` ; base du contrat `82a151273d214b07bee033082824d4c0dc0abba6` vérifiée ancêtre. Worktree neuf `MSM_vibe-A04`, branche locale `codex/a04-audit`, destination autorisée `main`. Le diff entre le code M015-V `ef468539` et ce HEAD est vide pour `Modbus RTU/05_Firmware`, `experiments/iis3dwb_st_reference` et `tr2_validate.sh`. Les changements intermédiaires sont documentaires.

Inspection des sources/CMake/linker, générateurs, drivers, tests et preuves M015-V. Relecture du manifest M015-V : **98 fichiers conformes**. Recalcul indépendant des304CRC des bursts à partir de la RAM publiée : tous concordants,36864mots, RAM SHA `b4b2526d755dabb3dd1a47efa44a524dce40c9d3137b51559ae5125f707c2a53`. Le rapport M015-V a été archivé **byte-identique**, sans écrasement, dans [archives/TR2-20261013-M015-V-COOPERATIVE-CLOTURE/RAPPORT.md](archives/TR2-20261013-M015-V-COOPERATIVE-CLOTURE/RAPPORT.md).

Seuls ce rapport, cette archive prescrite et les preuves A-04 sont ajoutés/modifiés. Firmware, tests, CMake, SDK, règles, journal courant et roadmap inchangés conformément au contrat. Dépôt principal, worktrees historiques, stashes et artefacts M014/M015 préservés. L'état matériel rappelé est celui des dernières preuves M015-V, **pas une observation matérielle nouvelle**.

## 2. Build cumulatif canonique constaté

« Canonique » désigne ici la configuration exacte reconstruite et correspondant au BIN flashé M015-V, **pas un nouveau preset adopté ni une correction**.

- CMake STM32 : `Modbus RTU/05_Firmware/platform/stm32/CMakeLists.txt`, générateur Ninja, `CMAKE_BUILD_TYPE=Debug`, toolchain `cmake/arm-none-eabi-gcc.cmake`.
- Compilateur : `/usr/bin/arm-none-eabi-gcc`, **14.2.1 20241119**, paquet `15:14.2.rel1-1` ; CMake3.31.6. SDK Linux STM32CubeU5 tag exact **v1.9.0**, commit `d88042df24f16799957c24f00c2b234c9e306188`, statut Git propre ; sous-modules enregistrés dans les preuves.
- ON : `TR2_STM32_PRODUCTION`, `TR2_STM32_RS485_TX_QUALIFIED`, `TR2_STM32_IRQ_DIAGNOSTICS`, `TR2_STM32_FIFO_COOPERATIVE`. `TR2_STM32_FIFO_SPI_MHZ=4`. Tous les autres profils diagnostic/provisioning/silence sont OFF, adresse diagnostic0.
- Entrées : `COMMISSIONING_M014_H.json`, `REGISTRE_ATTRIBUTION_TR2.json`, linker et HALconf dont les empreintes sont dans [canonical-input-hashes.json](preuves/TR2-20261013-A04/canonical-input-hashes.json).
- CPU160MHz/MSI-PLL/VOS1/FlashLAT4 ; SPI3 HSI16/div4/mode3/threshold1 sur PC9–12, SPI1 SYSCLK/div64/mode0 soit2,5MHz sur PA5–7/CS PD14. UART LPUART1 PG7/8 et TIM6 ; SDMMC2 n'est pas initialisé dans cette composition.
- Debug ne signifie pas optimisation uniforme : seul HAL SPI instrumenté, parser et services coopératifs/adaptateur sont compilés **-O2** (`CMakeLists.txt:336,342`). Les autres sources et le cœur portable n'ont pas ce réglage. Les fonctions HAL SPI sont communes à SPI1 et SPI3 : leurs registres/fréquences restent distincts, mais l'optimisation HAL concerne les deux périphériques.

Commande réellement exécutée, voir [canonical-build-command.json](preuves/TR2-20261013-A04/canonical-build-command.json) pour les chemins absolus :

```sh
cmake -G Ninja -S "$repo/Modbus RTU/05_Firmware/platform/stm32" -B "$build" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE="$repo/Modbus RTU/05_Firmware/platform/stm32/cmake/arm-none-eabi-gcc.cmake" \
  -DSTM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  -DTR2_STM32_PRODUCTION=ON -DTR2_STM32_RS485_TX_QUALIFIED=ON \
  -DTR2_STM32_IRQ_DIAGNOSTICS=ON -DTR2_STM32_FIFO_COOPERATIVE=ON \
  -DTR2_STM32_FIFO_SPI_MHZ=4 \
  -DTR2_COMMISSIONING_MANIFEST="$repo/Modbus RTU/00_gouvernance/COMMISSIONING_M014_H.json" \
  -DTR2_COMMISSIONING_REGISTRY="$repo/Modbus RTU/00_gouvernance/REGISTRE_ATTRIBUTION_TR2.json"
cmake --build "$build" -j 4
```

`$repo` représente ici le worktree A-04 et `$build` son répertoire neuf sous `/home/lolo/dev/msm/reports/TR2-A04-20261013T132613Z/`; la commande JSON contient les valeurs effectivement utilisées. Configure/build **code0**, BIN byte-identique à la preuve M015-V. Nouvelle ELF SHA `35bb0d89dc3e045e69d8c34c4b9dc379ff3e64b515d872c303bfad04787e2df9` ; elle diffère de l'ELF historique, notamment par les chemins de debug, alors que les octets programmables sont identiques. Aucune nouvelle validation physique déduite.

### Profils et fichiers réellement liés

| Profil | Différence pertinente | Portée |
|---|---|---|
| Par défaut / `tr2_validate.sh` |Production/FIFO/TX/observer OFF, aucun build type explicite ; entrée harness historique, SPI3 mode0/SYSCLKdiv64|Validation portable/compilation, **pas image cumulative sûre**|
| M014-H production |Production/TX ON, FIFO OFF ; source directe IIS3DWB, backend stockage indisponible|Acquis B0/B1/perte-reprise historiques|
| M015-U ST_FIFO_WINDOW |Exclusif, sans Modbus production ;4096 RAM,CPU160/HSI16/SPI4|Preuve FIFO bornée, différente de V|
| M015-V cumulatif4 |Production/TX/observer/FIFO ON ;ST configuration+parser coopératif|Image reproduite par A-04|
| Cumulatif8 |Même famille, HSI16/div2 autorisé par CMake|**Pas qualifié sur TR2-001** ; pas construit/rejoué ici|
| Autres profils principaux |Commissioning readonly/manufacturing/diagnostic/CTRL1/register/active/burst/FIFO/continuous/acquisition ; exclusions CMake|Outils historiques spécialisés, jamais interchangeables|
| Projet ST seconde NUCLEO |CMake séparé, BENCH_CORE_MHZ16/160, SPIdiv8/4/2, polling/who/FIFO, mode et STATUS10 explicites|Ne vaut pas configuration primaire ; aucune utilisation A-04|

L'ELF cumulatif conserve le serveur de production, le moteur/transport/parser coopératif, driver ST et journal FRAM. La collecte de symboles confirme l'absence de `Sdmmc2_Bringup`, `HAL_SD_Init`, `HAL_SD_WriteBlocks`, manufacturing et anciens buffers FRAM U/provisioning ; le linker GC élimine les chemins non utilisés. L'exemple original ST `iis3dwb_fifo` est généré à la configuration mais **n'est pas lié/exécuté** dans V : la configuration utilise les API du driver ST, et le séquencement/parser sont locaux. Ne pas présenter V comme l'exécution inchangée de l'exemple ST.

Driver ST `iis3dwb-pid` f83e74fb… et exemples ST 1f848c38… : toutes les six empreintes du manifest vendor concordent ; sources immuables/licences conservées. Le gitlink historique d995… indisponible est explicitement documenté, aucune substitution cachée constatée. HAL SPI original SHAa1668346… contrôlé par le générateur ; copie build avec normalisation LF et un observer à l'entrée de SPI_CloseTransfer, SDK non modifié. Le niveau FIFO10bits est décodé localement et évite le getter ST masqué03F documenté ; pas de suppression des guards.

## 3. Chemins synchrones, budgets et concurrence

Un seul thread coopératif : `stm32_production_board.c:205` poll Modbus, puis `:216` poll FIFO. Deux SPI distincts ne donnent pas deux capacités CPU concurrentes. Les IRQ UART/TIM restent actives ; la file UART64emplacements n'évite pas un retard de traitement de la boucle principale.

| Chemin | Preuve et durée | Conséquence |
|---|---|---|
| FIFO128 à4MHz |M015-V : quantum max2,916ms ; budget nominal896/210000=4,267ms ; cadence moyenne4,066/max5,005ms|Marge bornée ; maximum du quantum n'inclut pas tout Modbus, ni vérification FRAM après capture|
| FIFO capteur512mots |Débit nominal26667×9/8≈30000mots/s ; remplissage depuis vide≈17,067ms, réserve354 au max158≈11,8ms|Estimations nominales, pas garanties système|
| FC03 B0/B1 |refresh_request évite B5 ; deux requêtes M015-V seulement ; HP54,006/31,815ms ; TC observé5/5ms|Acquis limité à ces deux lectures, pas saturation du bus ou forte cadence|
| B5 **et autres requêtes hors B0/B1** |refresh_request→snapshot→latest_completed→256slots×2copies×70octets→FRAM physique|141ms de trou de service observés avant correction B0/B1 ; durée exacte du callback non instrumentée|
| Scan journal |35840payload+2048octets commandes ; plancher fil à2,5MHz≈121,242ms, hors CPU/CS/HAL/CRC|Déduction vérifiable compatible avec141ms, pas mesure causale isolée|
| Commit transactionnel FRAM |media_commit_cb écrit le payload51914 puis validate_image le relit ; plancher payload aller/retour≈332,25ms|Potentiellement pire que le scan ; durée totale sur cible inconnue, pas exécuté en A-04|
| Préparation FIFO au boot |262144octets lus puis relus par chunks512 avant premier poll ; plancher double payload≈1,678s +CRC/commandes|Startup bloquant, capteur encore au repos ; pas overrun FIFO d'acquisition à ce stade|
| Vérification FRAM après cleanup |512octets/poll,CRC intégral ; non comprise dans cooperative_max_cycles|Seulement post-capture actuellement ; ne pas la déplacer sous capture sans budget|
| HAL SPI défaut |Callbacks transport timeout20ms par appel ; FRAM100ms par appel|Quanta petits **au nominal**, pas garantie4,267ms sur erreur ; invalidation/cleanup nécessaires|
| SDMMC existant |HAL_SD_Read/WriteBlocks synchrones et wait_transfer busy-loop ; anciennes limites2000ms I/O/ready|Incompatible avec FIFO continu sans adaptation de service ; chemin absent du cumulatif4|
| Supervision métier |campaign_service_drive_acquisition_step lit un échantillon puis append synchrone ; fin fenêtre publie via supervision_service|Source directe actuelle ; chemin non conduit par la boucle cumulative et stockage indisponible|

Chaîne B5 vérifiée jusqu'au matériel : `modbus_system_server.c:61,65`→`command_engine.c:256`→`command_journal_bounded_reader.c:120`→`command_journal_bounded_slot.c:115`→`persistent_storage_core.c:51`→`transactional_image_media.c:197`→`stm32_fram_storage.c` (HAL Receive). Les lectures logiques **ne proviennent pas du candidate RAM**. `transactional_image_media.c:204` met les écritures en candidate, mais `:220–225` publie le payload complet et le valide physiquement. La correction V n'évite le scan que pour FC03 entièrement B0/B1 : une lecture B3 destinée à M016 le déclenchera encore, y compris avant que le PDU ne décide si B3 est disponible. CRC et unité RTU sont contrôlés avant ce callback (`rtu_server_runtime.c:20–44`).

## 4. Constats priorisés et propositions NON exécutées

Les trois premiers sont **bloquants pour engager la qualification physique intégrée M016 sans traiter les prérequis** ; ils n'empêchent pas une préparation DSP hors cible. Une correction ne devient autorisée qu'avec un contrat suivant.

### A04-01 — BLOQUANT : choix de build non sûr par défaut

**Localisation/preuve :** `tr2_validate.sh:102–104`, `platform/stm32/tr2_build_stm32.sh` configure sans options production ; `CMakeLists.txt:35,51` OFF. `main.c:1802` appelle Sdmmc2_Bringup et `:1064–1069` écrit2048octets àLBA2048 après init/capacity réussies ; écritures supplémentaires ensuite, sans nouvelle confirmation ni trigger RAM à cet endroit. Ces accès concernent les premiers32MiB réservés, pas seulement la partitionLBA65536. L'ELF cumulatif n'a pas ces symboles.

**Risque :** flasher par erreur le BIN produit par le lanceur générique sur la carte avec sa vraie microSD. Risque démontré par chemin du code ; **aucune écriture nouvelle observée**, ce binaire n'a pas été flashé. **Proposition minimale :** preset/commande cumulative versionnée, identité claire de l'artefact et isolation explicite du harness destructif sous option dédiée OFF ; default non intrusif ou refus de flash sans profil. Pas refonte main nécessaire. **Non-régression proposée :** matrice CMake/ELF vérifiant cumulatif et default sans symbols d'écriture SD, refus des mélanges, hash BIN cumulatif conservé si changement purement de sélection. **Impact/incertitude :** protège M014/M015 ; comportement physique exact du mauvais profil non testé et ne doit pas l'être sur le média existant.

### A04-02 — BLOQUANT : opérations FRAM longues dans le chemin Modbus

**Localisation/preuve :** chaîne B5 ci-dessus ; `modbus_system_server.c:105–117` ne sélectionne que B0/B1 ; commits `transactional_image_media.c:208–225`.141ms historiques et planchers fil dépassent17ms FIFO. **Risque :** future lecture B3 ou commande persistante pendant capture peut provoquer FULL/OVR ; augmenter SPI3 ne supprime pas le blocage CPU FRAM.

**Proposition minimale :** projection limitée au bloc demandé, et stratégie explicite pour snapshot/commit persistants : état RAM autoritatif actualisé aux transitions, ou progression I/O bornée ; à court terme exclure les longues opérations de la phase capture avec état indisponible explicite, sans retirer la fonctionnalité B5 ni contourner sa durabilité. Étudier la stratégie avant correction, ne pas imposer cache périmé ou modification de format persistant. **Non-régression proposée :** réponses B3/B0/B1 ne déclenchent pas un scan B5 non demandé ; B5 conserve corruption/recovery/retry et actualité ; budget de boucle et pertes sous trafic représentatif. **Impact/incertitude :** B0/B1 V restent acquis ; ni temps exact du scan isolé, ni durée de chaque commande mesurés ; tester après correction sous mission bornée, pas refaire l'échec historique.

### A04-03 — BLOQUANT : FIFO qualifiée non raccordée au chemin métier

**Localisation/preuve :** `stm32_fifo_cooperative.c:40` impose32768/±8g (configuration ST `fifo_cooperative.c:27`), armement RAM `:49`, terminal READY/INVALID sans reprise automatique (`fifo_cooperative.c:17`). La boucle production appelle ce diagnostic, pas `system_runtime_drive_acquisition_step`. Le runtime lie toujours `stm32_iis3dwb_vibration_source_interface` (`stm32_production_runtime.c:59,66`), lecture OUTX directe et échelle locale, sans timestamps FIFO. Stockage toujours indisponible (`stm32_production_board.c:65–91`), configuration UNAVAILABLE. READY RAM ne vaut pas READY V1.

**Risque :** utiliser l'ancienne source ou publier le buffer diagnostic comme une fenêtre configurée alors que taille, pleine échelle, axes, génération, disponibilité et nettoyage ne sont pas liés à l'autorité métier ; propriétaires SPI3 concurrents logiquement. **Proposition minimale :** décider/formaliser une interface de fenêtre valide et son propriétaire unique avant intégration M016 ; choisir lecture RAM des fenêtres validées, configuration/génération et invalidation explicites, sans activer stockage par effet de bord. Réutiliser les traitements/projections existants, pas recréer une chaîne parallèle. **Non-régression proposée :** 4tailles/4échelles/axes, conversion signée et saturation, nettoyage/CRC obligatoires, absence de publication partielle/périmée après erreur/reconfiguration, B0/B1 inchangés. **Impact/incertitude :** lacune attendue de raccordement, pas réfutation de M015 ; cette intégration relève du prochain contrat, aucune valeur vibratoire V1 physiquement qualifiée aujourd'hui.

### A04-04 — IMPORTANT NON BLOQUANT : tests du glue STM32 et de la concurrence incomplets

**Localisation/preuve :** CMakehôte`:349–360` couvre transport mock et moteur coopératif, mais le test moteur ne compile pas `stm32_fifo_cooperative.c` ni la boucle production réelle. `test_fifo_cooperative.c:8–42` fournit FIFO300constant et avance le tick une fois/poll, sans coût HAL, UART ou FRAM. Les125tests ne qualifient pas deadlines ou concurrence de la composition finale.

**Risque :** régression d'armement, recyclage des traces, CRC, publication différée ou scheduling malgré tests parser verts. **Proposition minimale :** tests ciblés de l'adaptateur réel avec fauxHAL/FRAM/UART et temps maîtrisé, plus matrice build ; pas duplication des tests existants. **Non-régression proposée :** frontières4096, dernièrefenêtre, défauts mémoire/transport/cleanup/CRC, queue UART et RX pendant quantum. **Impact/incertitude :** la preuve nominale V complète ces tests mais ne remplace pas les scénarios d'erreur du glue ; pas défaut d'exécution nouveau démontré.

### A04-05 — IMPORTANT NON BLOQUANT : timestamp monotone ne prouve pas la cadence ni toute perte

**Localisation/preuve :** `fifo_window.c:34,41–43`, compteur modulo4 et delta positif seulement. Une caractérisation **nouvelle hors cible**, utilisant le parse() actuel sans le modifier, accepte TS0→24→72 avec8XYZ entre chaque paire et returns0. Voir [parser-gap-characterization.json](preuves/TR2-20261013-A04/parser-gap-characterization.json) ; elle ne prouve aucune perte physique en V.

**Risque :** une disparition de8XYZ+1TS peut conserver modulo4, espacement de8XYZ retenus et monotonie, avec un delta TS excessif ; l'absence FULL/OVR et de défaut de tag n'est pas preuve de perte globale zéro. **Proposition minimale :** préciser la politique de cadence et tolérance à partir ODR/timestamp/FREQ_FINE, exporter les écarts ; invalider les fenêtres réellement incohérentes, sans imposer arbitrairement24ticks exacts ni affaiblir les guards. **Non-régression proposée :** saut excessif, wrap32bits, écarts autorisés de fréquence, compteurmod4, troncature/TS manquant. **Impact/incertitude :** les pas24 de V demeurent démontrés ; métrologie indépendante et endurance restent hors qualification. Corriger la portée des assertions avant une revendication de cadence stricte en M016.

### A04-06 — IMPORTANT NON BLOQUANT : réserve RAM surestimée et instrumentation volumineuse

**Localisation/preuve :** linker768KiB ; ELF `.data`472, `.bss`599612, `._user_heap_stack`2564, reste **183784octets =179,4765625KiB**. Le chiffre précédent « >180KiB » du rapport V était légèrement inexact ; `.bss` de la sortie size globale602176 inclut la réservation heap/stack, ce n'est pas toute RAM utilisée. Buffers : official_words262144, bench_payload65536, bench_preclose155648, trace40960 plus contexte de production. Le buffer FRAM U supplémentaire est éliminé.

**Risque :** ajouter sans budget des buffers32768×3float (393216octets), FFT ou copies de fenêtres ; minimum stack2KiB n'est ni maximum ni preuve de marge dynamique. **Proposition minimale :** budget RAM explicite et réutilisation maîtrisée des buffers/libération des journaux de qualification ; mesure stack/heap sous future charge. **Non-régression proposée :** contrôle sections/map, bornes et ownership des buffers, pas de duplication des traces ni perte des guards. **Impact/incertitude :** image V tient et demeure identique ; aucune collision pile/heap observée ici, charge DSP future inconnue.

### A04-07 — IMPORTANT NON BLOQUANT : startup FRAM monolithique après démarrage UART

**Localisation/preuve :** `stm32_production_board.c:179–185` start serveur/stage2 puis `stm32_fifo_cooperative_prepare:34–38` double lectureFRAM etCRC sans poll Modbus. Plancher payload1,678s, durée réelle totale non isolée. En V, le gate attendait bench_ready avant HP.

**Risque :** requêtes au boot traitées tardivement/timeout ou file UART saturée sous trafic, malgré disponibilité nominale de la composition. **Proposition minimale :** préparation avant exposition de disponibilité ou progression startup bornée ; ne pas relire intégralement FRAM à chaque essai par automatisme. **Non-régression proposée :** trafic durant startup, absence de TX tardif indû, READY non prématuré, respect identité/recovery. **Impact/incertitude :** aucun overrun d'acquisition à cette étape car XLoff ; ce scénario n'a pas été essayé physiquement, la portée V après armement reste acquise.

### A04-08 — IMPORTANT POUR ACTIVATION SD, NON BLOQUANT POUR M016 SANS SD

**Localisation/preuve :** `stm32_sdmmc_bulk_media.c:5–27,39–77` busy wait/Read/WriteBlocks, paramètres historiques2000ms ; `main.c:972` bus1bit dans le harness. Le cumulatif n'a pas ces fonctions liées et aucun média prêt n'est fourni.

**Risque :** raccorder ce backend à la campagne ou changer4bits sans étude peut introduire stalls très supérieurs au budgetFIFO et écritures sur mauvaises zones. **Proposition minimale :** audit SDMMC4bits séparé, ownership/partition/autorisation et budgets explicites ; maintenir SDoff dans trancheM016 si stockage non requis. **Non-régression proposée :** bornes de partition, refus d'écriture horsautorité, média lent/absent, timings et reprise sous contrat dédié. **Impact/incertitude :** aucun effet actuel sur M014/M015 ; performances4bits et coexistence SD non vérifiées, aucune intervention recommandée sur la microSD existante dans A-04.

### A04-09 — FACULTATIF : simplifier les dépendances de qualification sans refonte

**Localisation/preuve :** `CMakeLists.txt:315–319` génère l'exemple ST même en coopératif, alors que `:333–338` ne le lie que dans U ; garde UID/codebench/observateur requis dans le profil V. **Risque :** qualifier une image différente en simplifiant trop tôt, ou considérer un exemple généré comme exécuté. **Proposition minimale :** séparer progressivement outil d'observation et service, retirer génération non utilisée si bénéfice confirmé, et documenter le preset ; aucune migration forcée de drivers/RTOS/CI lourde. **Non-régression proposée :** symbole/modulelist et comparaison BIN là où attendue ; refaire une preuve physique seulement si chemin runtime/timing change. **Impact/incertitude :** pas défaut fonctionnel démontré ; instrumentation peut être conservée pour les prochaines vérifications, optimisations non indispensables.

## 5. Tests, invariants et limites de l'audit

- **Réutilisé et vérifié :** log complet M015-V `validation-after-refresh-fix.log` portant125/125 et ARMdéfaut, CTest -N sur build préservé montrant125sans exécution. Sources/tests identiques au SHA validé : suite hôte non répétée. Couverture existante :4fenêtres moteur,11modes de défaut,30positions I/O, TS régressif/espacement, cleanup/stop/timeout, transport mockpreclose, refus/fraîcheur B0/B1 et B5 backend indisponible/reprise.
- **Nouveau :** configure/build ARM cumulatif neuf et comparaison intégrale BIN ; inspections ELF/sections/symboles, empreintes vendor/SDK/inputs, vérification des manifests et CRC RAM ; caractérisation parse() dans bibliothèque hôte via wrapper externe, sans modification des tests du dépôt. Les logs/commandes sont publiés. Aucune erreur de build ; warnings newlib `_write`/autres stubs conservés. Des recherches de chemins historiques inexistants ont été abandonnées au profit des fichiers réellement recensés ; aucun fichier/API inventé ni correction improvisée.
- **V1 :** fréquence26667, fenêtres4096/8192/16384/32768, codes pleineéchelle0..3 et axes1..7 restent normatifs (`01_Specification_source/bloc4.md:309–325`). Driver/ST et parser diagnostic fixe±8g ne sont pas une configuration V1 active. Mapping, persistance transactionnelle, CRC/adresse, indisponibilité et autorité de configuration non modifiés.
- **Physique préservé :** preuves V de32768/4096TS/304bursts/2requêtes seulement, cleanup0/0C8000, FRAMgen12/CRC avant-après/candidate actif identique, SDoff. La RAM complète est publique ; SPIraw intégral uniquement dernière4096, pas capture logique externe. Aucun endurance/zero-loss/calibration/FFT/DSP/stockage/Web nouveaux. Firmware cumulatif supposé conservé d'après dernier rapport ; état actuel non relu sur carte.

L'audit satisfait les cinq axes du contrat sans correction. Recommandation : tranchepréparatoire limitée pour build sûr + budgets/projections FRAM et tests du glue, décision explicite sur raccordement fenêtre/configuration, puis M016 de traitement/publication avec limites de cadence et budgetRAM. Ces propositions sont **non exécutées**. A-01 demeure la revue indépendante sous responsabilité ChatGPT.

## 6. Publication et preuves

Preuves locales durables : `/home/lolo/dev/msm/reports/TR2-A04-20261013T132613Z`, nouveau build/ELF/map conservés localement ; [preuves publiques A-04](preuves/TR2-20261013-A04/MANIFEST.sha256). Permissions commit/push exercées uniquement pour rapport/archive/preuves ; permissions flash/debug accordées mais **non exercées**. Journal/roadmap volontairement non actualisés : restriction explicite du contrat. Leurs règles et états n'ont pas été corrigés dans cet audit.

Publication et relecture distante à terminer ; aucun blocage technique. Le prochain commit documentaire ne change pas le code audité `c45a197` ni le firmware physiquement testé `ef468539`. SHA de publication final et état Git fournis après contrôle distant.
