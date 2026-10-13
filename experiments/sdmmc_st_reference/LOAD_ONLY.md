# M016 — scénario unique SDMMC2 1 bit LOAD-ONLY

Profil additionnel isolé ; main ST de référence et HAL inchangés. `BENCH_LOAD_ONLY=ON`, `BENCH_WRITE_ENABLE=1`, `BENCH_TEST_LBA=2048` sélectionnent load_main.c. SDK CubeU5v1.9.0 et provenance/licences existantes conservées.

**Une campagne manuelle unique** : 600 secondes,26667 records techniques16octets/s =426672B/s, total256003200octets logiques. Production réelle de données dans un ring SRAM fixe512KiB par SysTick prioritaire ; arrivée calculée depuis DWT160MHz étendu64bits, y compris rattrapage de ticks retardés. Compteurs distincts généré/produit/consommé, backlog et overflow ; aucune régénération implicite à la place d'une file pleine. Aucun ISR SD ne bloque le producteur de priorité supérieure. Tampon SRAM occupé jusqu'à écriture+retourcarteTRANSFER/READY ; pas de retry.

Écritures DMA liées ST, deux nodes contigus, chunks64KiB. Dernier payload19584octets, taillephysique19968 (384octets paddingA5) ;256003584octets physiques,3907chunks. Réserve ring + bufferrelecture64KiB + métriques4096latences ; bss609152octets, projetisolé, pas une allocation de productionTR2. Chaque octet logique déterministe : `(offset*37 + (offset>>8) + (offset>>16) + (offset>>24)*113) &255`. Records techniques, pas codec/samplecapteur P8 validé.

Après600secondes, drainfinal puis **relecture de tous les secteurs écrits et comparaison de chaque octet**, y compris padding. Compteurs écrits/relus/logiques/physiques, mismatches et premieroffset ; latencechaquechunk enµs, quantiles calculés horsligne. Fréquence1bit10MHz fixe : PLL1kernel160MHz/div8/rising, cheminclock C2c. Aucun format, erase, provisioning ni intégrationCampaignDataStore/FRAM/Modbus.

## Sécurité et déclenchement

Transition depuisGPIO : couperalimentation/retirerSD par l'utilisateur avantflash ; UIDseconde001B0046/38425018/20333157, SN0032001F3235510C37333439, principalrefusé. Àboot PB15/PB3/PB4 sont INPUT, aucune sortieGPIO basse ; contrôleurPOWERoff et aucuneinitialisationcarte automatique. Vérifier cetétat aprèsflash/verify/readback, puis insertionSD horsalimentation et remise soustension par utilisateur.

Àstage1, RAMtrigger4C31494E initialise seulement la carte. Stage2 exige CID03534453/44333247/858BF092/3C01927A, capacité/région suffisantes, largeurcarteSSR0/CLKCRWIDBUS0, clock10000000 et ready sans erreur. Médiasecond de test intégralement sacrifiable confirmé ; région LBA2048..502054 (500007secteurs). Trigger4C314C44 démarre une seule fois. Pas d'auto-repeat ni autowrite aprèsreset.

GDB matérielbreakpoint à la boucleterminal du **BIN évalué**, résolu par DWARF et désassemblage (code c6c56b6 :08001666). `continue` jusqu'à terminaison : aucunepause intermédiaire, aucun watchpoint/probeHOTPLUG. Lire résultats seulement au terminal (stage5 succès ou99 échec), puisdetach. Le readback peut prendre plusieursminutes après lacharge. Erreur/overflow arrête le scénario, pas une permission de relancer une campagne. Pour une future répétition explicitement demandée : mêmeprofil et mêmesparamètres, nouvellepreuve/marker, reset/INIT/RUN manuels ; préserver preuve précédente.

## Build

```sh
cmake -S experiments/sdmmc_st_reference -B /home/lolo/dev/msm/reports/M016-1bit-load-build \
 -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake \
 -DSTM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
 -DBENCH_LOAD_ONLY=ON -DBENCH_WRITE_ENABLE=1 -DBENCH_TEST_LBA=2048 -DCMAKE_BUILD_TYPE=Debug
cmake --build /home/lolo/dev/msm/reports/M016-1bit-load-build -j4
```

Pas de garantie7jours, checkpoint/powerloss, fonctionnementcapteur, coexistence ou capacité disponible du runtime. Clock calculée depuis registres, pas mesureoscilloscope. Test10minutes + intégritéintégrale constituent uniquement preuve bornée du banc à ce débit/tampon/carte.

## Arrêt anticipé et intégrité du préfixe

En cas d'échec de charge, `campaign()` retourne avant sa vérification automatique. Un `load_verified=0` et un `load_mismatches=0` ne démontrent alors aucune intégrité. Pour le seul RUN du 13/10/2026, tous les octets effectivement écrits ont été vérifiés par une continuation en lecture seule, sans reset ni retry : appels à `transfer(0, rx, lba, 65536)`, dumps puis comparaison Python octet par octet. Les compteurs embarqués sont conservés ; les résultats externes sont publiés séparément dans `integral-prefix-verification.json`, avec le script `m016_load_verify_prefix.py`, les vingt lectures et leurs empreintes. Voir le [rapport](../../Modbus%20RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md). Cette vérification du préfixe ne valide pas les 600 secondes. Aucune répétition de charge n'est implicite.
