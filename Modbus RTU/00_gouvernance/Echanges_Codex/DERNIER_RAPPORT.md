---
mission_id: TR2-20261013-M016-SDMMC-REFERENCE-BENCH
status: IN_PROGRESS
base_ref: main
base_sha: 1c634e3a16c78feed3a5564fd59a6b2fd2298a93
initial_head: 15d6519e253a7ad6a092557227491ba7e7aad0cd
result_sha: null
created_at_utc: 2026-10-13T20:24:08.372367+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — Banc de référence SDMMC2 4 bits

**Préparation logicielle terminée ; qualification physique PENDING matériel.** Ce checkpoint est IN_PROGRESS en attente de publication/relecture. Aucun ST-LINK détecté par CubeProgrammer v2.23.0 (`-l stlink`, retour0 mais « No ST-Link detected! »). Montage, carte de test et zone d'écriture non confirmés ; question physique envoyée à l'utilisateur. Aucun flash, debug, reset, accès SD/FRAM ni trafic Modbus exécuté dans cette mission. Aucun processus Linux GDB/qualification concurrent observé ; cela ne démontre pas l'état interne d'une carte déconnectée.

## Référence et réalisation

Projet indépendant [experiments/sdmmc_st_reference](../../../experiments/sdmmc_st_reference/README.md), sans lien avec les cibles du firmware principal. Référence primaire : [ST CubeU5 v1.9.0, STM32U575I-EV SD_ReadWrite_DMALinkedList](https://github.com/STMicroelectronics/STM32CubeU5/tree/v1.9.0/Projects/STM32U575I-EV/Examples/SD/SD_ReadWrite_DMALinkedList), checkout réel d88042df24f16799957c24f00c2b234c9e306188. Aucun exemple exact SD natif NUCLEO-U575ZI-Q trouvé dans cette version locale ; pas une affirmation exhaustive sur tous les SDK futurs. Original main/MSP/README conservé byte-à-byte, hashes/provenance SOURCES.json, licence exemples SLA0044 et HAL BSD-3-Clause conservées ; HAL/CMSIS externes inchangés.

Adaptation : SDMMC1→2, GPIO H3h-B, clockST160MHz/HSI48, div1/falling/4bit (24MHz nominal), DMA interne lié, un node4K noncirculaire au lieu de2nodes circulaires/24Mo. Aucun effacement, formatage, filesystem, passage high-speed facultatif ou intégration CampaignDataStore. Trois écritures/relectures mêmes huitsecteurs avec patterns distincts et comparaison octet par octet ; triggerRAM unique, CID attendu, bornes de capacité et UID secondeNUCLEO exact avant init. Default readonly compilé ; buildécriture seulementaprès autorisation explicite d'un LBA de test. Timingsmillisecondes et traces statut/phase/R1/erreurDMA disponibles, pas encore observés.

Écart ST vérifié par lecture du SDK : les noms completioncallback de l'exemple ne sont plus invoqués par HAL_SD_IRQHandler livré ; l'adaptation emploie HAL_SD_RxCpltCallback/TxCpltCallback effectivement appelés àDATAEND. DriverST non modifié. Attente CMD13 contrôlée/bornée avec étatTRANSFER et READY_FOR_DATA ; pas de délais enIRQ. Cet écart documentaire/API est établi ; aucune causalité avec les échecs du TR2 n'en est déduite.

[Adafruit4682 pinouts](https://learn.adafruit.com/adafruit-microsd-spi-sdio/pinouts) confirme3,3V, CMD/DAT0–3 et pullups. [Exemple Adafruit sdioio](https://learn.adafruit.com/adafruit-microsd-spi-sdio/using-sdioio) concerne CircuitPython/SAMD51, pas une référence HALU5 exécutable tellequelle. [ST UM2861 Rev10](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf) confirme Morpho ; voir tableau et protocole dans README. Câblage réel non constaté ; restrictions MB1549 fonctions partagées à vérifier, aucun strap modifié.

## Validation effectivement exécutée

- Deux configurations/builds ARM indépendants réussis : default readonly, et BENCH_WRITE_ENABLE1/LBA2048 **compile-only, aucun média attribué ni exécution autorisée à ce LBA**. GCC arm-none-eabi14.2.1, Debug+O2/g3, STM32U575xx. Logs de configuration/build complets conservés ; avertissements newlib nosys sur syscalls inutilisés, pas d'erreur de compilation.
- ELFreadonly : writeblocks et erase absents aprèsGC. ELFarmed : write/read DMA et callbacksréels liés ; erase/campagne/FRAM/capteur/Modbus absents des deux. Refus attendu CMake d'un buildwrite sans LBA explicite nonzero observé.
- Pas de tests127/128 ou buildsproduction rejoués : applicationisolée, aucune interface ni source fonctionnelle partagée modifiée. Aucun précédent succès transformé en preuve de cette application.
- BINreadonly : 20108octets, SHA256 `b1091e8908614fb99576134b8cc18a334449073c3c9851fcc13e460e38e752f9`. BINarmedcompileonly : 21328octets, SHA256 `c183e3296715af7aebee83c316532a237b7ab0535943087ef92177314f8011b9`. ELF/MAP et symboles dans [artifacts.json](preuves/TR2-20261013-M016-SDMMC-REFERENCE/artifacts.json). Artéfacts binaires locaux sous `/home/lolo/dev/msm/reports/M016-sdmmc-reference-build` et `M016-sdmmc-reference-armed-build`.

## Préservation et état matériel à reprendre

Ancienne mission STORAGE-PREREQUISITE suspendue par nouvelle décision, non qualifiée ni clôturée comme succès. [Contrat et dernier checkpoint](archives/TR2-20261013-M016-STORAGE-PREREQUISITE/RAPPORT.md) archivés byte-identiques àc4a91c137093acb332a3deb8036d16ad77fad7cf ; volumes/allocation/preuves et derniercorrectif inchangés. Tentative3 préparée **jamais flashée/exécutée**. Deux échecs physiques distincts conservés : préfixes récupérables131072 et655360octets ; cause physique non démontrée. Derniers contrôles anciens FRAM262144exactA04-02 SHA d732acc557152180fc394097d6a5269ea2f45c1da387c8643f512318adf92fce, MBRprefix8192exact SHA22e2c58d886232b8c4d858b03d9575b574b27ec2eabfc1b4e89e22882316e3f3. Ces contrôles ne sont pas une lecture actuelle ni un hash de la partition intégrale.

**Dernier état connu TR2principal** : firmwareisolé tentative2/code38c027364a1d51465ae26678deacec7369df3f3d, BIN00ba4f5b6750f3c20b2bd18832c99eaf10ac257449df6df12a53f6500ed81e5d ; bootreadonly/recovery aprèsreset, capteurODRnon démarré, config/storage non disponibles, debuggerdétaché. **Cumulatif A04-02 non restauré avant cette nouvelle mission.** Contrat actuel interdit de modifier/reflasher leprincipal ; son image actuelle n'a pas été relue ici faute desonde. Image cumulatif A04-02 de retour vérifiée conservée, aucune restauration silencieuse.

SecondeNUCLEO : identités historiques M015-L SN0032001F3235510C37333439/UID001B00463842501820333157 ; aucun étatFlash actuel vérifié. La règle mission autorise image de diagnostic isolée sur secondecarte ; aucun flash aujourd'hui ni restaurationautomatique. Avantflash contrôle réel obligatoire, sauvegarde vérifiée si nécessaire ; identitéprincipale refusée.

Journal ETAT_COURANT_TR2 actualisé pour corriger ses indications opérationnelles périmées (firmware cumulatif/readonly et absence de write) : ces éléments décrivaient d'anciens checkpoints. Aucune spécificationgelée ni règle modifiée. DettesA04 et gelsH3h inchangés ; aucune capacitéV1/provisioning fictif. La démonstration4bit resterait distincte du stockageV1/latence/endurance/coexistence.

## Git, publication et reprise

Dépôtoriginal propre à82169af7e76797731358852bec83bfb0ceb54ccf, historique divergent volontairementpréservé. Worktreestorage propre àc4a91c1 conservé. Nouveauworktree `MSM_vibe-M016-sdmmc-reference`, branche `codex/m016-sdmmc-reference`, baséorigin/main15d6519 ; pasdemerge/rebase/reset/clean. Stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2 etf3b829e6188f491920edf74a5f9ed0f559d08b87 conservés. Autorisations exercées : développement/build/docs, puis commit/pushnormalmain ; allowflash/debug présents mais nonexercés. Mission/gouvernanceChatGPT inchangées.

Publication normale avec destinationHEAD:refs/heads/main, fetch et relecture intégrale des livrables/diff/empreintes avant statutfinal. SHAcode évalué sera renseigné aprèscommit, sans autoSHA. Aucune qualification indépendante ni tag créé.

**Reprise physique nécessaire (AGENTS§9 et contrat§4–5)** : connecter le secondbanc ; confirmer pins/alimentation/modules déconnectés ; choisir microSDtest et autoriser huitsecteurs sacrifiables identifiés (ou annoncer données à préserver). Pas de formatage/repartition implicite. Puis préflightSN/UID/Flash et readonly, buildLBAautorisé/CID, troisrounds, observationerreurs/bus/clock et comparaisonintégrale ; rapportphysique et comparaisonminimalavecTR2 si succès. Aucun essai sur TR2principal ; la restauration de son cumulatif relève d'une clarification de périmètre ultérieure si souhaitée.
