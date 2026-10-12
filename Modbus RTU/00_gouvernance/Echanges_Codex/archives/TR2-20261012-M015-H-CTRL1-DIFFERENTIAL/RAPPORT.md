---
mission_id: TR2-20261012-M015-H-CTRL1-DIFFERENTIAL
status: DONE
base_ref: main
base_sha: 6357b667a45b0979e5edaf394cb273595295414d
initial_head: 168ddbd444d0d8d6aa3ceabe115cc315d2ad5452
result_sha: 54e8deb1003fd02dd0b586d20211e89e89433a68
created_at_utc: 2026-10-12T16:08:19.565291+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-H — Divergence associée à la séquence de lecture active

**M015-H DONE : diagnostic différentiel exploitable, résultats publiés et relus intégralement.** Une seule campagne courte, aucune acquisition FIFO/XYZ ; M014-H restauré et reprise minimale HP réussie. M015 globale IN PROGRESS.

## Faits observés

| État, ordre fixé S G G S | S début | G premier | G second | S fin |
|---|---|---|---|---|
| Baseline sans écriture | 00 | 00 | 00 | 00 |
| CTRL3=44, CTRL1 arrêté | 00 | 00 | 00 | 00 |
| Après unique write CTRL1=A8 | A8 | A8 | A8 | **50** |
| Actif après délai mesuré20,27554 ms | A8 | A8 | A8 | **50** |
| Après write CTRL1=00 | 00 | 00 | 00 | 00 |

S est lecture simple10, TX90 00 ; G est lecture groupée0F..14, TX8F puis six zéros, contenant10 et les contrôles voisins. WHO7B/CTRL3=44/CTRL4-5=0 restent conformes en actif. Byte11 réservé est capturé, pas interprété comme une valeur normative.

Premier écart **trace 27, phase 3, CTRL1_XL/0x10 : attendu 0xA8, reçu 0x50**, TX90 00/RX00 50 ; valid_mask15, expected/observed_valid1. Copie immuable byte-identique à la trace source après restauration. WriteA8 trace 23/HAL_OK, S 24 correcte puis G 25/G 26 correctes avant S 27incorrecte. S 31reproduit50 après délai, S 28/G 29/G 30 restentA8. Ce n’est pas un défaut de toutes les lectures simples : deux simples actives correctes, deux incorrectes ; quatre groupées actives correctes.

43 transactions, HALstatus/error/RxXferCount/TxXferCount=0 partout, CS GPIO1→0→1 ; stage 10/result 9(INTERNAL)/restore0/errors0/mismatches2. DWT de la première lecture erronée 591951→594352,2401 cycles/**15,00625 µs** ;groupée33,425 µs. HCLK160 MHz, SPI 2,5 MHz calculé HAL/prescaler64, mode 3 ; CFG1=50070067/CFG2=87400000, SR A001→C007. Premier S correct commence4,925 µs après finwrite ; premier S erroné 105,09375 µs après. DélaiHAL20 ms demandé,20,27554375 ms réellement observé ; diagnostic21,39171875 ms hors attente du trigger. Timestamps de transactions MCU, pas instants de conversion ni mesures au fil. GPIO ODR ne mesure pas tension CS.

Les43 records JSON reconstruisent exactement3956 octets du dump binaire. Sept contrôles avant/après complets0/0/0/0/0/4/0, restoration 0. Slots43..63 et NVIC phase0 non renseignés exclus des mesures ; validités explicitent inconnu/zéro. Aucun DATA_OUT/FIFO_OUT lu, aucune write FIFO ou self-test/reset capteur. Un trigger unique4D313548 ; zéro deuxième campagne, variante ou correction spéculative.

## Conclusions et hypothèses

- **Observation discriminante** : l’écart50 apparaît à la dernière position simple de S G G S en état actif, immédiatement et après20 ms. Le délai seul ne supprime donc pas le défaut. L’histoire/position de transaction et l’état actif constituent la prochaine piste ciblée.
- **Limite** : ordre, méthode et phase temporelle restent partiellement confondus. Impossible d’attribuer formellement l’effet aux seuls bursts précédents, à HAL, à CS ou au capteur. Les lectures conformes A8 favorisent une incohérence de lecture plutôt qu’un writeA8 systématiquement incorrect, sans établir l’état réel du registre indépendamment. Aucune fiabilité générale SPI ou méthode de production qualifiée.
- **Garde conservé** : divergence latched/result 9, aucun vote/fallback/substitution50 ni admission d’une acquisition ; source F/G intacte. H autorise uniquement la comparaison déclarée en lecture après divergence CTRL1. Une erreur de sûreté HAL/CS/compteurs/WHO/CTRL3-5/NVIC arrête immédiatement vers restauration.
- **M013-C/D distinct** : précédent HOTPLUG/NVIC relu, aucune reproduction ni HOTPLUG. Six snapshots CPU H ISER/ISPR0..2=0/masks0 (validphases126), SWD avant/après idem/faultregs0 ; SysTick/délaiHAL progressent. ISER0 normal pour ce profil sans IRQexternes ; signature ISPRFFFFFFFF absente. HAL SPI est bloquant par flags, sans IRQ SPI. Snapshots ne prouvent pas absence de toute perturbation transitoire.

## Modifications et validations

Nouveau module `stm32_iis3dwb_ctrl1_differential.c/.h` réutilisant les types de traces G ; nouveau mock/test. Option `TR2_STM32_CTRL1_DIFFERENTIAL` OFF et exclusive, branche main avant médias/transport avec mode 3/2,5 MHz ; aucun changement de cœur/driver/mapping/validator/V1. Cinq writes volatiles au nominal : CTRL3=44, CTRL1=A8, CTRL1=00, CTRL1=00 et CTRL3=04 pour restauration. Baselines refusées si préexistants, inconnus ou contrôle critique nonconforme. Limite64traces,43 transfers nominales,20 comparaisons,uneactivation. Contrairement à F/G, aucune préparation batching/STOP_ON_WTM ; comparaison ciblée, pas reprise exacte de leur campagne. Instrumentation/buffers et cadence de transactions diffèrent, limites documentées.

Test ciblé `cmake --build …/build-host --target test_stm32_ctrl1_differential` puis `ctest --test-dir …/build-host -R '^stm32_ctrl1_differential$' --output-on-failure` : **1/1 passé,53 scénarios**. Nominal, dépendance méthode, deuxméthodes incorrectes, anomalie transitoire restantlatched, anomalie retardée, préexistants/WHO/CTRL3/NVIC/DWT, timeout à chacune des43 transactions nominales. Assert de refus et restauration, aucun test supprimé/affaibli. Pas échec firmware ou compilation rencontré.

Cross-build profil DebugNinja/optionON et défaut `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only` réussis ; profil text 33280/data 20/bss 9852, BIN 33300. Warnings Newlib usuels `_close/_lseek/_read/_write`. Profil concurrent avec burstON refusé comme attendu, pas un échec de validation ; symboles média/production/UARTstart/acquisition absents. WrapperHP nouveau nom H limité Steady/deuxFC03, rejets Prepare/Offline/Physical et parser PS vérifiés sansport. Validation proportionnée : harness/main isolés uniquement ; anciennes campagnes/test A-G,114 firmware/539.NET inchangés nonrejoués. Commandes exactes danspreuves/validation-commands.json ; pas recompilation ou essai répété après succès.

## Sauvegarde, restauration et reprise M014-H

Code évalué **54e8deb1003fd02dd0b586d20211e89e89433a68**, commit `M015-H: add bounded CTRL1 SPI method and delay comparison`, poussé puis28 livrables intégralement relus avant matériel. BIN SHA **f0909839219b700c3a085abbd122c581e55350b10c4b094a74ba3834f144b262** ; ELF **131018425b4333a034450818ef53b6d7be10ccb9ef0ccf44f11f47404cd01596**.

UID **004A003D3842501820333157**, STLINK **003500463235510B37333439**/V3J15M7/NUCLEO-U575ZI-Q, STM32U575/U5850x482RevU/3,29V reconnus, image M014-H identifiée avant flash. Flash2 Mio/RAM768 K sauvegardées/hashées. Deux flashs standards NORMAL08000000/verify/reset (diagnostic puis restauration en finally), aucun mass erase/OptionBytes/RDP/bootpersistant/câblage. Readback GDB exact puis reset qualifié après dernierCLI ; ensuite GDB seul. Debugger détaché pendantcampagne, bruts/RAM sauvegardés avant reflash.

**Restauration Flash2 Mio entière byte-identique à l’initiale**, SHA **556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0**. Permanent M014-H/source0b2a901e185c9cbf8c02410bf2a29a0a3a75491e : BIN 143108 SHA **b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611** ; ELF c37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de. Diagnostic sans FRAM/SD ; gen12 retrouvée au boot. Pas comparaison intégrale FRAM revendiquée, aucune écriture/provisioning ni accès partitionLBA65536, SD OFF.

Deux FC03 de reprise seulement B0/21 puis B1/20, COM7/1152008E1/unit1/retry0/retour0/connexionfermée ;371 fichiers du paquet H rehashés, garde fraîcheur0..30s calibrée par SSH readonly, aucun GDB pendanttrafic. **Journal complet7 événements récupéré et lu**, SHA recalculé identique au distant **7bb9a159fb23043f840923bc4a75f55a8da0207daa3d60e61c68745afdb8a043** ;2026-10-12T16:01:26.2071809+00:00→2026-10-12T16:01:26.9022327+00:00, passedtrue. B0complet conforme, B1uptime30s/status 1/flags 0/stockage 0/acq 0. Registres décodés, pas capture wire ni requalification générale M014.

GDB après sansreset : stage 2/result 0/pollerrors0, FRAMV2/gen12/identité1/configUNAVAILABLE, IRQ49/66 actives, UARTerror0/TXidle/fault0/filehead=tail20/CFSRHFSR0. MCU détaché enexécution, serveurs exit0. Aucun trafic supplémentaire après ces deuxlectures.

## Suite, V1 et publication

Prochaine tranche conseillée : audit hors cible HAL/clôture flags SPI/CS et historique des tailles7→2, puis sous **nouveau contrat** comparaison bornée S S S S versus G S avec mêmes paramètres. L’effet de position/historique doit être isolé avant tout correctif ou qualification FIFO ; aucune acceptation par moyenne/vote ou substitution. Pas recherche exhaustive des anomalies B/D.

Pas de fiche BM869s maintenant : DC ne discriminerait pas ce motif et aucun rail capteur n’a été mesuré ici ; aucune caractéristique du multimètre inventée. Aucun achat d’oscilloscope demandé. Un protocole conditionnel de substitution du breakout identique est préparé dans [diagnostic](../DIAGNOSTIC_M015_H_CTRL1_DIFFERENTIEL.md), uniquement pour mission ultérieure si l’audit ne suffit pas : module/connexions identifiés, changement utilisateur hors tension, paramètresfigés, sauvegarde/restauration, réserves sur reseating/câblage et attribution. Aucune intervention physique exécutée ou demandée en H.

V1ODR26667 Hz/fenêtres4096..32768/continuité/store-and-forward inchangés. Ni ODRmesurée/FFT/bande6kHz/calibration/DMA/production/FIFO/B5/Modbusvibrations/Web/stockagequalifiés. M015globale INPROGRESS, READY0 etstockage0 conservés.

Preuves publiques `preuves/TR2-20261012-M015-H/` :43traces brutes enJSON/CSV lossless, divergence, quartet/NVIC/timing, logsCLI/GDB, HPintégral, validations/restauration/hashes. Bruts BIN/ELF/Flash/RAM conservés dans **/home/lolo/dev/msm/reports/TR2-mission015-H-20261012T155415Z** ; logs publics UTF8/LF normalisés, deuxJSONHP sansBOM/LF ; journal physique byte-identique original. Manifest lie hashespubliques/originaux.

Journal actualisé avec observations H, limitecause/timing/NVIC, restauration et prochaineétape ; roadmap M015INPROGRESS. Archives G exactes depuis6357b66, missionactive inchangée, worktree H isolé codex/m015-h. Original et huitworktrees antérieurs/stashes 6251b988/f3b829e6 préservés. Permissions commit/push main/flash/debug qualifiés exercées ; aucune résolutiondivergence/merge/rebase/reset Git/forcepush/configurationpermanente. Résultats publiés au **1ccec7f7d7916564322ddfb6437e20afa7f4b269**, push normal vers main et relecture intégrale de56livrables byte-identiques depuis la référence récupérée de GitHub (`preuves/TR2-20261012-M015-H/publication-readback-1ccec7f.json`). Worktree H propre ; original et huit worktrees antérieurs propres et HEAD inchangés, deux stashes intacts, archives G exactes et mission active immuable. Cette clôture DONE est vérifiée après son push ; son SHA final et l’état Git sont communiqués dans la réponse Codex. Contrôle indépendant ChatGPT attendu.
