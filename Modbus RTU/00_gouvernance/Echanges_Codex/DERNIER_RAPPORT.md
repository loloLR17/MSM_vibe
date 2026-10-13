---
mission_id: TR2-20261014-M017-SDMMC-1BIT-REALISTIC-QUALIFICATION
status: IN_PROGRESS
base_ref: main
base_sha: 9dd90081ec6e51ec5c6fa84bd5b66f8369717ceb
initial_head: df5050f41c180d080529229cfd836cb55a230b0e
result_sha: 65ff1fa93e3793c1ad4e18fe105034bedf1b5146
created_at_utc: 2026-10-13T22:31:32.497979+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M017 — campagne physique réellement exécutée, qualification échouée

**Résultat technique : FAILED.** Première campagne lancée le **13/10/2026 à 22:23:22 UTC** (14/10 à00:23:22 Paris), arrêt sur **overflow après0,402315s**, et non après600s. **65536octets écrits et intégralement relus conformes**, puis conformes après réinitialisation contrôlée. Aucun retry, aucun RUN2/3, aucun succès annoncé sur préparation seule. Le statut provisoire IN_PROGRESS couvre seulement la publication/relecture distante du rapport final ; la qualification a échoué.

L'écart C2c/M016 est **partiellement caractérisé, pas totalement expliqué** : la lenteur est reproduite sans producteur, sur DMA comme sur polling ; la majorité du temps est dans le transfert, pas dans READY final. La seule hypothèse corrective évaluée, ACMD23, n'améliore pas suffisamment la performance et n'a pas été adoptée. Aucun défaut matériel ni incapacité intrinsèque du bus1bit n'est établi.

## Tranches et preuves

1. HEAD distant récupéré `df5050f41c180d080529229cfd836cb55a230b0e` ; AGENTS, contrat, protocole, gouvernance, état courant, H3h-A/P8-B et preuves C2b/C2c/M016 lus. Worktree isolé `MSM_vibe-M017`, branche `codex/m017-sdmmc-qualification` ; original divergent conservé, aucun pull/merge/rebase/reset. M016 archivée byte-identique depuis9dd9008 ; anciennes archives/gels/dettes intacts.
2. Diagnostic court différentiel : **8 writes DMA +8 polling**, sans producteur, chacun suivi d'une relecture et comparaison entière de64KiB (65536octets). **16/16, zéro mismatch/HAL error**. Image source `df5050f` + `diagnostic-source.diff`, BIN SHA-256 `b8ac313e646f7c9571b924fea9785bc934ebbe4c428c7620acf4534471628db9` et readback exact conservés. [Log final](preuves/TR2-20261014-M017-SDMMC-1BIT/diagnostic-final.log), [CSV avant](preuves/TR2-20261014-M017-SDMMC-1BIT/baseline-diagnostic-latencies.csv).
3. Harness instrumenté : temps transfert/READY et coût producteur, symbole terminal noinline, **ring128KiB** et runner durable à marker unique. Build spécialisé réussi, code/protocole publiés **avant RUN** au commit `65ff1fa93e3793c1ad4e18fe105034bedf1b5146`, message « M017 checkpoint: instrument ST transfer phases and publish bounded qualification runner ». Flash standard vérifiéCLI et readback intégral, INIT/preflight réussis, RUN réellement effectué avec unique hardware breakpoint terminal, **aucun halt intermédiaire**. [Runner](../../../experiments/sdmmc_st_reference/run_m017.py), [log charge](preuves/TR2-20261014-M017-SDMMC-1BIT/run-01.log).
4. Après premier échec, répétitions arrêtées ; totalité du préfixe écrit vérifiée en lecture seule par oracle Python indépendant. Puis unique hypothèse ACMD23 testée par **16 transferts/relectures**, zéro corruption et commandes acceptées, mais pas d'amélioration démontrée. Aucun nouveau RUN spéculatif. Dernière relecture complète du préfixe initial après le flash/reset logiciel/INIT nécessaires à cette expérience : toujours exact. Cette dernière vérification ajoute la frontière de réinitialisation demandée par le contrat, sans prétendre tester un power-loss physique.
5. Publication des scripts/logs/images/diffs/mesures et journal ; relecture distante à consigner avant statut final FAILED.

## Comparaison C2c / M016 / M017

| Élément | C2c historique | M016 | M017 observé |
|---|---|---|---|
| Source |3b0b9089|c6c56b63|65ff1fa + diagnostic conservé|
| Bus/horloge |1bit/10MHz documentés|1bit/10MHz registres|1bit/10MHz carte et registres|
| Write API |HAL_SD_WriteBlocks polling|HAL_SDEx_DMALinkedList,2nodes|DMA et polling comparés sur même banc|
| Producteur |aucun|cadencé SysTick|absent dans diagnostic, cadencé dans RUN|
| Chunk |64KiB|64KiB|64KiB|
| Tampon borné de charge |non testé|512KiB|128KiB|
| Chronométrage |HALtick ms|DWT étendu|DWT étendu, phases séparées|
| Fin durable technique |TRANSFER|TRANSFER+READY_FOR_DATA|mêmes contrôles stricts M016|
| Latence |54–55ms ×8|142–324ms ×20|sans producteur plateau≈190ms ; RUN248,141ms|
| Intégrité observée |512KiB selon rapport|1310720octets exacts|16×64KiB diagnostic ;65536octets RUN exacts|

Sources C2b/C2c : rapports versionnés `VALIDATION_FIRMWARE_P12H3H_C2B_SDMMC2_WRITE_THROUGHPUT.md` / `...C2C_SDMMC2_SUSTAINED_WRITE.md`. Le rapport de débit C2c ne donne pas UID/CID, donc **la même carte et le même banc ne sont pas démontrés** ; carte/state/history et différences de méthode restent des variables non éliminées. Pas de reproduction C2c ni de sollicitation du TR2 principal.

Sur M017 sans producteur, DMA total106,023..954,529ms (pause954ms conservée), polling189,800..321,918ms. READY final1,068..3,025ms ; le plateau est dans la phase data. Revenir au polling ne reproduit pas les55ms. L'ajout du producteur n'est donc pas l'explication unique du ralentissement, et READY final n'est pas son goulot dominant. Le coût producteur RUN est25,185ms cumulé et82,975µs max/tick, environ8,2% de l'intervalle d'arrivée dérivé avant overflow ; pas100% CPU.

## Configuration, mémoire et image évaluée

Seconde NUCLEO-U575ZI-Q seule : **SN0032001F3235510C37333439**, **UID001B0046/38425018/20333157**. MicroSD de test SanDisk32Go sacrifiable, **CID03534453/44333247/858BF092/3C01927A**, **62333952secteurs512**. Broches anciennes DAT1/DAT2/DAT3 libérées INPUT (`GPIOB_MODER=2FFFFC3F`), 1bit côté carteSSR0 et contrôleurWIDBUS0, CLKCR8, PLL1kernel160MHz ⇒ **10MHz calculés**, front montant. Pas de mesure électrique de CLK/alimentation inventée ; câblage/alimentation confirmés dans la transition utilisateur précédente, registres et carte revérifiés ici.

Zone planifiée RUN **LBA2048..502054** ; effectivement écrit **LBA2048..2175**. Diagnostics **LBA503000..504023** sur la même carte de test. Aucune écriture secteur0, aucun formatage/erase explicite ou accès média principal. Motif déterministe par offset conservé M016, records techniques16octets ; ni capteur ni codecP8 métier qualifiés.

Arrivée426672B/s,600s ⇒256003200octets logiques,256003584 physiques avec384octets paddingA5. FIFO réellement bornée131072octets, données en vol incluses, aucun overwrite/regénération ni retry. Pourquoi128KiB : cumulatif A04-02 historique `_ebss=0x2009283c` sur768KiB ⇒ environ186KiB libres avant stack ; ring128KiB laisse55236octets avant autres besoins. BufferSD existant à réutiliser et instrumentation à exclure lors d'une future intégration ; **aucun fit de production démontré**. Augmenter simplement la RAM n'a pas été utilisé comme correctif. Le scénario M017 a donc une capacité différente de M016, explicitement déclarée ; les charges ne sont pas prétendues identiques entre missions.

Build isolé CubeU5v1.9.0/GCC ARM14.2.1, CMake `BENCH_LOAD_ONLY=ON/BENCH_WRITE_ENABLE=1/BENCH_TEST_LBA=2048`, O2/g3. BIN RUN **25840octets**, SHA-256 **`17bd4d235b742276ea674297c894ebe2d8417a6104d2268c0a61fcb8f2410edd`**, ELF SHA dans [load-artifacts.json](preuves/TR2-20261014-M017-SDMMC-1BIT/load-artifacts.json), BSS248960octets. Cross-build spécialisé instrumenté et variante ACMD23 réussis ; aucune suite hôte/cumulative inchangée rejouée, car pas de fonction ni interface production modifiée. Avertissements newlib syscalls non implémentés conservés, sans usage de ces fonctions sur cible.

## Première campagne — mesures exactes

| Mesure | RUN1 |
|---|---:|
| Durée jusqu'à terminal |0,402315s|
| Générés / acceptés / écrits |131440 /131072 /65536octets|
| Vérifiés externes intégralement |65536octets, zéro différence|
| Backlogmax / overflow / perte initiale |131072 /1 /368octets (23records)|
| Acceptés encore en SRAM après write |65536octets|
| Débit utile écrit/durée totale |162897.23B/s|
| Write total / transfert / READY final |248,141 /244,116 /4,008ms|
| HALreturn/HALerror/DMAerror/CMDerror |0 /0 /0 /0|
| Gap producteurmax |1000µs|
| Producteurcycles cumulé/max |4029666 /13276|
| CFSR/HFSR |0 /0|

`stage99/error3` est l'overflow prouvé. Le producteur stoppe à la première perte, puis l'écriture en vol termine et libère64KiB. L'intervalle d'arrivée dérivé est0,308059s environ, distinct de0,402315s jusqu'au terminal ; la durée hôte5,718s comprenddebug/capture. `generation_us=0` signifie600s non atteintes. Les368octets sont les pertes du premier refus, pas une extrapolation sur600s.

P50/P95/P99/min/max=248,141ms pour **n=1** ; il serait trompeur d'en déduire une distribution qualifiée. Les distributions pertinentes ici sont les deux diagnostics de16writes, intégralement publiées en CSV et [quantiles par transport et phase](preuves/TR2-20261014-M017-SDMMC-1BIT/diagnostic-analysis.json), pas une qualification statistique des longues pauses. Sans ACMD23 : P50/P95 total DMA190,877/954,529ms, polling194,444/321,918ms ; avec ACMD23 :193,052/328,272ms et189,911/359,590ms. [Résultat brut](preuves/TR2-20261014-M017-SDMMC-1BIT/run-01-result.json), [analyse](preuves/TR2-20261014-M017-SDMMC-1BIT/run-01-analysis.json).

Compteurs embarqués `verified=0/read_chunks=0/mismatches=0` au terminal : vérification automatique court-circuitée par l'erreur, **ces zéros ne prouvaient aucune intégrité** et n'ont pas été forcés. [Script oracle indépendant](../../../experiments/sdmmc_st_reference/verify_m017_prefix.py), [préfixe avant réinit](preuves/TR2-20261014-M017-SDMMC-1BIT/run-01-prefix-verification.json), [préfixe après réinit](preuves/TR2-20261014-M017-SDMMC-1BIT/prefix-after-reinit-verification.json) ; dumps bruts complets en hex et hashes concordants.

## Unique hypothèse ACMD23 — résultat négatif, pas d'adoption

La [spécification SD Association7.10 §4.3.4](https://www.sdcard.org/cms/wp-content/themes/sdcard-org/dl.php?f=Part1_Physical_Layer_Simplified_Specification_Ver7.10.pdf) recommande l'annonce de la taille avant l'écriture multiple pour certaines cartes. Extrait officiel indexé consulté ; accèsPDF direct refusé403, aucun document intégral prétendu vérifié. Lecture du HAL courant : ni polling ni DMA n'envoient ACMD23. **Hypothèse testable**, pas défaut obligatoire de HAL ni cause affirmée d'avance.

Variante isolée après premier échec : CMD55 vérifié APP_CMD puis `SDMMC_CmdBlockCount` officiel pour les128blocs avant CMD25, sans changer HAL, fréquence, GPIO, producteur ou ring. Seize commandes acceptées (`preerase_count16/error0/R1=920`), seize écritures/relectures exactes. **DMA188,953..328,272ms ; polling148,608..359,590ms**, READY1,212..2,844ms ; la lenteur persiste. [Log après](preuves/TR2-20261014-M017-SDMMC-1BIT/hint-diagnostic-differential.log), [CSV après](preuves/TR2-20261014-M017-SDMMC-1BIT/acmd23-diagnostic-latencies.csv).

Pas de correctif de performance démontré, donc **pas de nouveau RUN spéculatif**. Variante retirée du harness actif par annulation exacte de notre patch, après conservation intégrale source/diff/images/ELF/MAP. [Source expérimentale exacte](preuves/TR2-20261014-M017-SDMMC-1BIT/hint-load_main.c), [patch](preuves/TR2-20261014-M017-SDMMC-1BIT/preerase-hypothesis.diff), [empreintes](preuves/TR2-20261014-M017-SDMMC-1BIT/hint-artifacts.json). Elle est publiée comme preuve distincte, pas comme adoption ou firmware qualifié.

## Critères, arrêt et prochaine décision

Critères **3×600s identiques, zéro perte/overflow/corruption/erreur** : **NON SATISFAITS**. Le contrat demande d'arrêter les répétitions si la première charge échoue, puis au plus un correctif fondé sur preuve. Aucun correctif suffisant établi ; l'unique hypothèse testée est négative. L'arrêt est donc un échec physique mesuré conformément au contrat, pas un simple checkpoint logiciel ni une attente utilisateur artificielle.

Reste à attribuer la durée dans transfert (temporisations internes du support, backpressure sur bus, différence effective banc/carte C2c), sans présumer une panne. Prochaine mission recommandée : établir l'identité/méthode du C2c historique et comparer un chemin de référence sur un média/banc identifié, avec une mesure ciblée des blocages entre blocs ; décider ensuite d'une correction démontrée ou d'un média alternatif. Pas d'oscilloscope exigé, pas de rejet définitif du1bit, pas de matrice4bits.

Instrumentation à corriger avant un futur long succès : `load_producer_cycles` cumulé estuint32 et peut dépasser2^32 sur600s ; ici le compteur est exact sur0,308s. Le DWT étendu64bits d'arrivée n'a pas cette limite. Ce point n'explique pas l'overflow observé et aucun600s réussit n'a été évalué avec un compteur CPU tronqué.

Non qualifiés : stockageV1 disponible, intégrationruntime/capteur/codec/checkpoints, power-loss/endurance7jours, coexistenceModbus et4bits. Intégrité technique du préfixe et diagnostics réussie ≠ cadence soutenue qualifiée.

## État laissé, Git et publication

Seconde carte laissée avec **image expérimentale ACMD23 sûre** (BIN SHA **aa18a0c3f40ca1eed5b5cf88f49a182e1bec281f1d891317b87296b35fa163f6**, **26036octets**, source65ff1fa+patch conservé), au repos **stage2**, producteurdisabled, aucun RUN automatique, GDBdétaché. Carte présente1bit10MHz, GPIOB_MODER2FFFFC3F, R1=900 TRANSFER/READY, HALerror/CFSR/HFSR0. Pas de restauration GPIO ni de flash de confort. Cette image diffère du code de qualification ; provenance exacte explicitement conservée, pas présentée comme adoptée.

Principal003500463235510B37333439, FRAM/microSD principales, HP/Modbus **jamais sélectionnés/accédés**. Firmware principal historique probe stockage inchangé, aucune restauration automatique M014-H/A04-02. Pas de mass erase/OB/RDP/formatage, pas de manipulation physique demandée ou effectuée pendant M017.

Dépôt original conservé82169af7e76797731358852bec83bfb0ceb54ccf ; deux stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2 etf3b829e6188f491920edf74a5f9ed0f559d08b87 préservés, autres worktrees/checkpoints intacts. Permissionsexercées : développement, builds spécialisés, flash/debugseconde, commit/pushmain. Fichiers contractuels, gouvernance, dettes et gels non modifiés.

[Preuves et index](preuves/TR2-20261014-M017-SDMMC-1BIT/README.md), manifestSHA-256 et bruts locaux `/home/lolo/dev/msm/reports/TR2-M017-20261014`, artefactsprojets `/home/lolo/dev/msm/reports/M017-diagnostic-build` ; images/ELF/MAP avant/après sauvegardés distinctement. Scripts et logs conservés même en cas d'interruption client ; markerRUN01 refuse une répétition silencieuse.

Journal ETAT_COURANT_TR2 actualisé : diagnosticdifférentiel, RUNéchoué, préfixe intègre aprèsréinit, étatimageisolée et prochaineslimites. Publicationprovisoire à vérifier, puisstatutFAILEDfinal ; SHAdistantfinal et étatGit dansréponsefinale, pasd'auto-référence.
