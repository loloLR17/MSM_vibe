---
mission_id: TR2-20261012-M015-G-CONFIG-READBACK
status: DONE
base_ref: main
base_sha: e11088772987a50ce729eb4ca1148d657b2cc673
initial_head: fc60d98dd2d1dc64f377f613dbf6ce0ec65ff11a
result_sha: 5f85dbe8b4015059a7d943421b22590d4211affa
created_at_utc: 2026-10-12T15:34:45.876002+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-G — Premier refus localisé, M014-H restauré

**M015-G DONE : refus reproduit et caractérisé, résultats publiés et relus intégralement.** Le premier écart est **CTRL1_XL, adresse 0x10 : attendu 0xA8, reçu 0x50** en lecture simple. L’acquisition FIFO reste NON VALIDÉE ; M015 globale IN PROGRESS.

## Cause observable et limites

L’audit F retrouve le rejet dans les sept comparaisons après configuration, avant activation FIFO. F n’exportait pas `actual[]` à ce point ; la restauration écrasait sa valeur. Son registre et octet historiques ne peuvent être récupérés. G conserve désormais le premier refus immuable et reproduit le même chemin stage10/result9/loss4, avec le registre exact. Cela caractérise la condition de refus F ; cela ne prouve pas que l’octet historique F était50, ni une cause matérielle définitive.

| Observation G avant restauration | Preuve |
|---|---|
| Write CTRL1 | trace 17, TX10 A8, HAL_OK |
| Readback refusé | trace 24, TX90 00, RX00 50, attenduA8 |
| Groupé07..0A | trace 23, TX87 00 00 00 00, RX00 00 81 0A 00 ; quatre valeurs conformes |
| CTRL3/CTRL5 | traces 25/26,44/00 conformes |
| HAL / SPI | status0/error0/RxXferCount0/TxXferCount0, CFG1=50070067/CFG2=87400000, SR E001→C007 |
| CS / temps | ODR1→0→1, DWT613759→616104,2345cycles/14,65625µs à160MHz |
| Résultat | stage10/result9(INTERNAL)/restore0/loss_flags4 ;0poll/0burst/0word/0accepted |

38 échanges SPI enregistrés, tous HAL_OK ; le premier refus reste byte-identique à la trace source après restauration. Les sept contrôles sont relus restaurés0/0/0/0/0/4/0. JSON/CSV permettent une reconstruction byte-à-byte des3496octets des38 records. Masques explicitent les valeurs mesurées ; `configured[]`, polls, métadonnées et RX des bursts à zéro sont BSS NON RENSEIGNÉE. Ils ne sont pas publiés comme XYZ. Aucun DATA_OUT ni FIFO_OUT lu, aucun enableFIFO exécuté, aucun tag/parité/gap/cadence évalué physiquement. Aucune deuxième tentative ou variante.

Mode3/prescaler64 et HCLK160MHz observés ; SPI2,5MHz **calculé depuis HAL**, non mesuré au fil. CS est GPIO ODR, pas tension mesurée. HAL_OK ne prouve pas l’intégrité SPI. L’instrumentation ajoute du temps ; elle ne reproduit pas strictement le timing F. Les causes possibles côté write/read/liaison/capteur restent ouvertes ; aucune faute HAL ou panne capteur attribuée, aucun rapprochement causal avec CTRL2 de B/D.

## Tranches logicielles et validation

Modifications minimales : `stm32_iis3dwb_burst_capture.c/.h`, test associé ; copie immuable du refus,96 traces bornées TX/RX/HAL/SPI/CS/DWT et validités. Aucun attendu, ordre de configuration, garde, fréquence, ODR ou transfert supplémentaire changé ; aucun bypass/substitution. Diagnostic exclusif OFF par défaut ; cœur/driver/mapping/validator/V1 inchangés. Nom `physical-m015-g-restored` ajouté aux deux wrappers HP, limité à Steady/deux lectures.

- Test ciblé : CMake host, build du seul `test_stm32_burst_capture`, `ctest --test-dir …/build-host -R '^stm32_burst_capture$' --output-on-failure` : **1/1 passé, 21 scénarios**. Nominal, gap, parité, timeout FIFO, overrun, configuration préexistante, sept divergences readback, sept writes en timeout, timeout lecture groupée ; refus et restauration vérifiés sans assertions affaiblies.
- Cross-build dédié DebugNinja/`TR2_STM32_BURST_CAPTURE=ON` et défaut par `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only` : réussis. text 34936/data 20/bss 16788, BIN 34956 octets. Warnings Newlib habituels conservés. Aucun symbole média/production/UARTstart du profil ; isolation aussi vérifiée dans main/CMake. Les tests A-F,114 firmware et539.NET inchangés ne sont pas rejoués.
- Wrapper : Prepare/Offline/Physical refusés pour nom G, Steady accepté ; parse PowerShell réussi sans ouvrir le port. Pas de rebuild PC.

Validation proportionnée : changement de harness sans risque plausible sur le cœur, tests ciblés des nouvelles traces plus cross-build défaut/profil. L’extension de scénarios a seule nécessité une nouvelle exécution du test hôte ; pas de recompilation ARM après ce changement limité au test. Erreurs d’outillage corrigées sans impact firmware : CRLF des logs/CSV/JSON publics normalisés UTF8/LF pour diff-check, originaux intacts ; alias Python utf8-sig invalide corrigé en utf-8-sig dans l’analyse hors ligne.

## Essai unique, sauvegarde et restauration

Code évalué **5f85dbe8b4015059a7d943421b22590d4211affa**, tranche `M015-G: preserve first configuration refusal and SPI evidence`, publiée/relecture23 livrables byte-identiques avant essai (`software-readback.json`). BIN SHA **a5cd02dad4266633064c7c704e7899a176a2629d603e77d47a780021e50c2653** ; ELF **33e1127deba135a558dbf0fb2e3caff9feff3a85d559c5eef2692fb987bfedeb**.

UID **004A003D3842501820333157**, STLINK **003500463235510B37333439**/V3J15M7/NUCLEO-U575ZI-Q, U575/U5850x482 RevU/3,29V identifiés. Image M014-H reconnue avant flash, sauvegardes Flash2 Mio/RAM768 K et hashes conservés. Deux flashs standards NORMAL à08000000 avec verify/reset : diagnostic puis restauration en finally. Aucun HOTPLUG/mass erase/OptionBytes/RDP/câblage/boot persistant. Chaque dernier CLI suivi de readback et reset logiciel GDB qualifié ; ensuite GDB seul. Un trigger4D313546, debugger détaché pendant exécution ; refus immédiat par le garde, traces/RAM conservées avant reflash.

**Flash2 Mio restaurée intégralement byte-identique**, SHA **556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0**. Permanent M014-H/source0b2a901e185c9cbf8c02410bf2a29a0a3a75491e : BIN 143108 octets SHA **b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611** ; ELF c37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de. Diagnostic sans accès FRAM/SD ; génération12 observée au retour. Pas de comparaison intégrale FRAM revendiquée ; aucune écriture/provisioning ni accès partition microSD LBA65536, SD OFF.

Contrôle minimal de reprise HP :371 fichiers du paquet H rehashés, deux FC03 B0/21 puis B1/20, COM7/1152008E1/unité1, retry0/retour0/connexion fermée. Garde fraîcheur calibrée via SSH read-only, serveurs GDB fermés pendant trafic. **Journal complet7 événements récupéré et lu**, hash distant recalculé identique **47546b3839b521b9434b1e4fcaaec9277912aa07b14dd435bf212f1df40c378b** ;2026-10-12T15:30:28.9123539Z→15:30:29.6568994Z, passedtrue. B0 complet conforme, B1 uptime32s/status1/flags0/stockage0/acq0. Ce sont des registres décodés, pas une capture brute wire ; pas requalification générale M014.

GDB après sans reset : stage2/result0/pollerrors0, FRAMV2/gen12/identité1/configUNAVAILABLE, IRQ49/66 actives, UARTerror0/TXidle/fault0/filehead=tail20/CFSRHFSR0. MCU détaché en exécution, serveurs exit0. Aucun nouveau trafic après ces deux lectures.

## V1, suite et preuves

V1 reste ODR 26667 Hz, fenêtres 4096..32768, continuité/store-and-forward. Les256 XYZ prévus mais non acquis représentent 9,59988ms, pas une fenêtreV1 ;4096..32768 donnent 153,598..1228,785ms. À2,5MHz,897 octets prendraient idéalement 2,8704ms et burst128+statut demanderaient au moins 1,500019Mbit/s : calculs de faisabilité, **aucun débit/délai FIFO mesuré**, aucune acquisition continue/FFT/bande6kHz/calibration qualifiée. READY0/stockage0 demeurent.

Prochain contrat conseillé : distinguer un échec de write d’un readback incohérent de CTRL1 au passage en mode actif, par un diagnostic de configuration limité, avec lecture simple/groupée et délais explicitement choisis. Préserver A8 et le refus ; ne pas accepter50, inventer un correctif ni prolonger G. Choisir une méthode et vitesse conservatrices d’après les acquis A/D et les besoins de débit, sans imposer un oscilloscope absent. Aucun DMA, production, FFT/B5/Modbus vibrations ou accès médias dans cette mission.

Preuves publiques : `preuves/TR2-20261012-M015-G/` (38 traces complètes de configuration, premier refus, analyses, logs CLI/GDB, journal HP entier, hashes, restaurations et validations). Bruts BIN/ELF/Flash/RAM originaux conservés dans **/home/lolo/dev/msm/reports/TR2-mission015-G-20261012T152038Z** ; logs/CSV/JSON publics normalisés seulement pour encodage/fins de ligne, manifest compare les hashes. Les buffers non acquis ne sont pas des mesures.

Journal actualisé avec refus exact, restauration et limites ; roadmap M015 IN PROGRESS. Archives F exactes depuis e110887, mission active immuable. Worktree G isolé codex/m015-g ; original et sept worktrees antérieurs, deux stashes 6251b988/f3b829e6 préservés. Permissions commit/push main/flash standard/debug qualifiés exercées ; aucun merge/rebase/reset Git/forcepush ni configuration permanente. Push normal réussi au b3a3c428a9f2d35801f5601db1a38d58a120246a ; 50 livrables relus intégralement depuis la référence récupérée de GitHub et identiques octet par octet (`preuves/TR2-20261012-M015-G/publication-readback-b3a3c42.json`). Worktree G propre, huit worktrees antérieurs propres et HEAD inchangés, deux stashes intacts, archives F exactes et mission active inchangée. La présente clôture DONE sera contrôlée après son push ; son SHA final et l’état Git seront fournis dans la réponse Codex. Contrôle indépendant ChatGPT attendu.
