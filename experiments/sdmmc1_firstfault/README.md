# SDMMC1 READONLY-FIRSTFAULT

Probe isolé ; production, FRAM et Modbus non liés. Source externe inchangée
`VILLO88/NUCLEOU575@1e3c9183bad023ebf7d234134823d9270a0e8b64`.
Le générateur crée une copie instrumentée dans le build et des diffs/provenances.
Suppression des seules lignes injectées restaure toutes les instructions HAL/LL
originales. Les hooks sont sur frontières et sorties d'erreur, jamais dans les
boucles FIFO. R1/R6 et CRC/timeout sont capturés avant flags clear ; R3 sans CRC
n'est pas classé fautif sur le seul CCRCFAIL normal. Le premier record reste figé.
`kind=1`: erreur LL ; `2`: STA data ; `3`: bits R1 ; `6`: bits R6 ;
`4`: erreur logicielle HAL ou garde wrapper (source_line peut alors être phase).

## Build A

```sh
cmake -S experiments/sdmmc1_firstfault \
 -B /home/lolo/dev/msm/reports/SDMMC1-firstfault-A-build \
 -DCMAKE_TOOLCHAIN_FILE="$PWD/experiments/sdmmc_st_reference/cmake/arm-none-eabi-gcc.cmake" \
 -DREFERENCE_ROOT=/home/lolo/dev/msm/cache/VILLO88-NUCLEOU575-SDMMC1-PoC \
 -DFF_PROFILE=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS_RELEASE=
cmake --build /home/lolo/dev/msm/reports/SDMMC1-firstfault-A-build -j4
```

GCC14.2.1, CortexM33/hardfloat/Os/g3, linker/startup tiers. Pas de build global
répété : ce build compile le probe réellement évalué. Un profil absent est refusé.
B n'est compilé/exécuté qu'après PASS physique complet de A.

## Contrôles et exécution

Image attendue, UID seconde carte, CID/capacité, GPIO AF/PUPDR/OSPEEDR,
HSI48/CLKCR2 ou4002, IRQ USB désactivée, SSR et readiness sont gardés.
HAL_SD_Init conserve ses transactions d'origine ; les erreurs internes sont
capturées sans interrompre artificiellement son chemin HAL. Le harness s'arrête
au premier retour fautif ou première capture, sans commande de récupération
ajoutée. Les commandes internes d'origine ne sont pas masquées ni remplacées.

Boot au WAIT, **aucun init/accès SD avant triggerRAM**. Un seul RUN, puis terminal
sans nouvelle commande. Lectures CMD17/512 puis CMD18/8192 sur506304, comparaison
embarquée complète et indépendante hors cible. Aucune commande de write/erase,
TinyUSB/MSC ou FatFs présente dans le graphe ELF contrôlé.

`observe.py` n'exécute rien à l'import. Préparation flashA exige
`physical-preflash-A.json` avec retour utilisateur réel : ST-LINK seul, SD absente,
CN15 débranché. Identité et ancienne image vérifiées avant flash standard.
`physical-start-A.json` consigne l'insertion hors tension et le départ électrique
confirmés ; tension0V mesurée ou limite explicite. Les fichiers ne doivent jamais
être créés à partir d'une hypothèse.

Après flash, WAIT vérifié puis insertion hors tension. `run-A` crée un marqueur
exclusif avant trigger, détache GDB, attend10s sans halt et collecte au terminal.
En dépassement, arrêt diagnostique et disconnect sans detach/resume ; pas de
reset/retry et limites conservées. Aucun `HAL_SD_GetCardState` depuis GDB.
`decode.py` utilise l'ABI ARM vérifiée : Result2260/Phase200/Snapshot84octets ;
hash et comparaison des données uniquement si retour HAL data conforme.

La présence de fonctions HAL non utilisées dans le source externe ne permet pas
leur invocation : elles ne sont pas liées au probe. Préserver BIN/ELF/MAP et
readback vérifiés ; pas de distribution du source tiers ni de flash automatique.
