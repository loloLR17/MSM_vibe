---
mission_id: TR2-20261013-M016-SDMMC-REFERENCE-PHYSICAL
status: IN_PROGRESS
base_ref: main
base_sha: 167b8cbcf1ba93209789ea6890799c77bbc7fc96
initial_head: 087dd9e3f1947a5b8b2906fba0ca862b968185bd
result_sha: d9a223e4de996e01ffc701eecc17c5038398e8c5
created_at_utc: 2026-10-13T20:52:06.733465+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — Résultats physiques du second banc SDMMC2

**Le fonctionnement bidirectionnel en 4 bits n'est pas démontré.** Deux campagnes échouent pendant leur première relecture. Les diagnostics supplémentaires confirment un échec CRC en 4 bits à 4 MHz, sur les fronts descendant et montant. En revanche, une lecture 1 bit à 4 MHz restitue exactement les 4096 octets écrits, après réinitialisation. Ce succès limité ne remplace pas le critère 4 bits.

La préparation, les essais et les diagnostics autorisés sont réalisés. La poursuite nécessite une vérification électrique du banc ; la cause précise reste indéterminée. Statut IN_PROGRESS uniquement pour publication/relecture, avant rapport BLOCKED étayé. L'utilisateur a répondu « le câblage semble correct » : contrôle rapporté pris en compte, sans mesure de continuité ou correction précisée.

## Identités et périmètre exercé

Une seule sonde détectée : ST-LINK **0032001F3235510C37333439**, V3J15M7, NUCLEO-U575ZI-Q. UID réel GDB **001B0046 / 38425018 / 20333157**, vérifié avant programmation. Le principal 003500463235510B37333439 / UID004A003D n'a jamais été sélectionné.

MicroSD de test : CID **03534453 / 44333247 / 858BF092 / 3C01927A**, différent du CID principal historique ; **62333952 secteurs ×512 =31914983424 octets**. L'utilisateur et le contrat autorisent son contenu intégral à être remplacé. La campagne se limite à **LBA2048..2055**, huit secteurs/4096 octets. Aucun effacement, formatage ou repartitionnement n'a été utile ni exécuté.

Autorisations exercées : développement isolé, cross-build, trois flashes standard avec verify/readback (readonly, essai1, essai2), debug, resets logiciels nécessaires, deux déclenchements RAM uniques et diagnostics de lecture. Aucun mass erase, Option Bytes/RDP, trafic Modbus, accès FRAM, capteur ou matériel principal. La seconde Flash est jetable selon décision permanente M015-M§4 : aucune sauvegarde2MiB ou restauration historique automatique ajoutée. Câblage H3h-B confirmé par l'utilisateur ; cela ne constitue pas une mesure indépendante.

## Résultats observés

| Essai ou diagnostic | Configuration effective | Résultat |
|---|---|---|
| Préflight readonly | 4 bits,24MHz, front descendant | HAL init OK, CID/capacité conformes, stage2, CLKCR14001, R1=0900 (TRANSFER+READY), erreurCMD13=0, CFSR/HFSR=0 |
| Campagne1, code0778351 | Un nœud DMA,4 bits/24MHz | Stage99/phase5 (lecture),0cycle complet ; HAL/DMAerror1002 ; écriture991ms, intégrité non validée |
| Campagne2, coded9a223e | Deux nœuds contigus,4 bits/24MHz | Stage99/phase5,0cycle complet ; HAL/DMAerror1002 ; écriture135ms, relecture invalide |
| Demande ClockDiv6 via HAL_ConfigWide | Registre reste14001 | HAL_ERROR : **la demande n'applique pas4MHz** ; lecture diagnostique encore24MHz échouée1002 |
| Lecture différentielle | 4 bits/4MHz descendant, CLKCR14006 | DATA_CRC_FAIL02 ;50/4096 octets concordants, aucune intégrité validée |
| Lecture différentielle | 1 bit/4MHz descendant, CLKCR10006 | CMD55/ACMD6/LL init OK, lectureDMA réussie, erreursHAL/DMA/CMD13=0, **4096/4096 octets exacts** |
| Comparaison de front, après reset | 4 bits/4MHz montant, CLKCR4006 | DATA_CRC_FAIL02 ;50/4096 octets concordants ; le changement de front ne résout pas l'échec |

Les trois répétitions prévues par chaque campagne ne sont **pas** déclarées réussies : chacune s'arrête avant son premier cycle complet. Aucun retry d'écriture automatique. Aucun arrêt GDB pendant les deux campagnes ; les lectures sous GDB suivantes servent au diagnostic, pas à mesurer un débit soutenu.

Le motif de la campagne2 est récupéré intégralement après reset logiciel et réinitialisation de la carte, puis passage diagnostique en1bit. Comparaison hôte de tous les octets avec le motif indépendant `((i*37)+(i>>8)) &255`, sans se limiter à un CRC. SHA256 du dump exact : **9ec45ab0c8505527af5931e27722ed5908748e5073169b4571c84e006eb90140**. Pas de coupure d'alimentation physique observée ni validation H3h-E4/powerloss.

## Correction logicielle et interprétation

Le port est repris de [ST CubeU5v1.9.0, STM32U575I-EV SD_ReadWrite_DMALinkedList](https://github.com/STMicroelectronics/STM32CubeU5/tree/v1.9.0/Projects/STM32U575I-EV/Examples/SD/SD_ReadWrite_DMALinkedList), checkout d88042df. Référence SDMMC1 sur carteEV, adaptée à SDMMC2 sur NUCLEO ; provenance, différences et licences conservées dans [README/SOURCES](../../../experiments/sdmmc_st_reference/README.md). HAL/CMSIS inchangés. Les callbacks réels HAL_SD_RxCpltCallback/TxCpltCallback sont utilisés, contrairement aux anciens noms figurant dans l'exemple.

L'entrée HAL DMA liée prépare IDMALAR avec ULA et sizeof(node), donc head+12. Le premier port ne fournissait qu'un nœud ; GDB confirme head200020EC et IDMALAR E000000C. Correction minimale : retour aux **deux nœuds contigus ST**, chacun2048 octets, liste finie. Build réussi et nouvel essai distinct, sans changement d'horloge/bus/câblage. L'échec persiste : cette correction de fidélité au modèle ST **n'est pas une cause racine démontrée**.

**Correction explicite d'une erreur d'analyse :** le code1002 a d'abord été annoncé comme CRC+RX_OVERRUN. Les [constantes du HAL utilisé](https://github.com/STMicroelectronics/stm32u5xx-hal-driver/blob/2552682ee5a0d6f826a6435750fc9267be65868b/Inc/stm32u5xx_ll_sdmmc.h) établissent **DATA_CRC_FAIL=02, COM_CRC_FAILED=1000, RX_OVERRUN=20**. Ici1002 signifie donc CRC données + CRC de la commande précédente ; aucun RX_OVERRUN n'est établi par ces valeurs. Codes bruts et logs inchangés, analyses initiales conservées et versions corrigées identifiées, empreinte/source dans error-decoding-correction.json. La baisse de fréquence retire le bit CRCcommande, pas un overrun.

Les hypothèses ont été testées séparément : disposition DMA, fréquence24→4MHz en lecture seule, bus4→1bit à4MHz, puis front descendant→montant en4bits4MHz. ST CMD55/ACMD6 et SDMMC_Init utilisés avec contrôle des retours et du CLKCR réel ; aucune disponibilité simulée ni erreur manuellement effacée. Une première préparation du test de front s'arrête sur CMD55/COM_CRC_FAILED1000, avant changement et lecture ; reprise propre par reset puis réglage LL depuis l'initialisation4bit. Ce contrôle échoue encore surCRC02.

## Ce qui reste indéterminé

Dernier contrôle : GPIOB_IDR=C018 (DAT0–3 hauts au repos), fonctions alternatives PB14/PB15/PB3/PB4 AF12 et PD6/PD7 AF11 conformes au [datasheet ST, DS13737 table28](https://www.st.com/resource/en/datasheet/stm32u575zi.pdf). DBGMCU_CR=6, TRACE_IOEN=0 ; PWR_UCPDR=1, dead battery désactivé. Ces registres ne prouvent pas la qualité des signaux pendant un transfert.

**Déduction limitée :** le chemin commande/DAT0, la lecture DMA et la persistance fonctionnent dans le contrôle1bit. La défaillance est associée au mode4bit dans cette série, sans preuve d'un contact inversé, d'un rail ou d'un composant défectueux. Le contrôle visuel utilisateur et les vérifications logicielles ne suffisent pas à identifier la cause.

24/4MHz sont calculés depuis le noyau réel48MHz et le CLKCR observé ; **aucune mesure électrique à l'oscilloscope**. Les991/135ms sont des temps d'écritures de campagnes échouées, pas un débit4bit qualifié. Aucun débit de lecture1bit calculé à partir du temps GDB. Endurance, débit soutenu, coexistenceFIFO/Modbus et stockageV1 restent non qualifiés.

## Builds, preuves et publication

Artéfacts précédemment validés rehashés exacts, sans recompiler le code inchangé. Après correction deux nœuds : un cross-build isolé réussi, GCC ARM14.2.1, O2/g3 ; logs complets. Aucune suite127/128 ni build production rejoué : pas de source/interface partagée modifiée. Chaque Flash programmée est comparée intégralement au BIN attendu par GDB.

| Image exécutée | SHA256 BIN | Taille |
|---|---|---|
| readonly/0778351 | b1091e8908614fb99576134b8cc18a334449073c3c9851fcc13e460e38e752f9 | 20108 |
| essai1/0778351 | c183e3296715af7aebee83c316532a237b7ab0535943087ef92177314f8011b9 | 21328 |
| essai2/d9a223e | 092a1db046c5821e3401418f92a0a3c83f9b46ae7414e2af4ed75741fb2c166d | 21372 |

[Preuves](preuves/TR2-20261013-M016-SDMMC-PHYSICAL/artifacts.json) : commandes CLI/GDB, scripts, logs, BIN/dumpshex, analyses et manifesteSHA256. Preuves locales : `/home/lolo/dev/msm/reports/TR2-M016-SDMMC-PHYSICAL-20261013T202934Z`.

Erreurs d'outillage conservées : guillemets du filename UNC dans le premier dumpGDB →commande corrigée sans reflash ; HAL_SD_DeInit éliminé parGC →arrêt avant action, puis diagnostic avec primitivesST vérifiées. stdoutGDB enregistré en continu ; aucun timeout pendant cette campagne. Aucun échec masqué ni résultat attendu affaibli.

Code évalué **d9a223e4de996e01ffc701eecc17c5038398e8c5**, commit « M016 physical checkpoint: restore official two-node SDMMC DMA layout ». Les variations diagnostiques RAM/registre sont détaillées ci-dessus ; elles ne sont pas des changements Flash. Checkpoint **14e1fe083aaccd82e97eb30963b2204d98eeedf5** publié et relu intégralement :108fichiers byte-identiques. La présente finalisation fera l'objet d'un push normal puis relecture avant statut BLOCKED.

## État final et reprise

SecondeNUCLEO : image deux nœuds, au repos stage2/trigger0, debugger détaché. **Dernière configuration effective temporaire :4bits/4MHz/frontmontant, CLKCR4006, dernière erreurHAL02.** Les variables benchclock/bus cachées au boot restent4bits24MHz ; les registres réels font foi pour les diagnostics. Reset/powercycle rétablit le profil Flash4bits24MHz ; trigger et CID attendus nuls interdisent l'écriture automatique. Pas de nouveau write ni flash après campagne2.

TR2principal non accédé et non restauré, conformément au contrat. Dernier état historique : probe STORAGE2/code38c0273, cumulatif A04-02 non restauré ; pas de lecture actuelle sur ce matériel. FRAM, volumesM016 historiques et sauvegardes préservés par absence d'accès. Aucun changement des spécificationsV1, des gelsH3h, de la detteA04 ou de la gouvernance/mission. Archive du contrat/rapport REFERENCE-BENCH byte-identique à167b8cb. Journal ETAT_COURANT_TR2 actualisé avec identités, échecs4bit, succès1bit, correction du décodage et état temporaire final.

Dépôt original propre à82169af, worktrees antérieurs et stashes6251b988/f3b829e6 conservés. Worktree courant `MSM_vibe-M016-sdmmc-physical`, branchecodex/m016-sdmmc-physical ; destination explicite HEAD:refs/heads/main, sans force/merge/rebase/reset. SHAfinal après publication fourni dans la réponseCodex ; aucune qualification indépendante ni tag créé.

**Prochaine action indispensable (AGENTS§9)** : mesurer la continuité, hors alimentation du second banc, de DAT1→PB15/CN12-26, DAT2→PB3/CN12-31, DAT3→PB4/CN12-27 ; contrôler absence de connexion partagée et3,3V/GND. Si conforme, observer électriquement CLK/CMD/DAT1–DAT3 pendant un transfert pour différencier réception, liaison ou composant. Décrire les résultats/corrections avant un essai supplémentaire justifié. Aucun formatage ne résout une erreurCRC de liaison ; aucun format lancé.

Comparaison production : banc DMAinterne/IRQ/HSI48/4bits/falling versus portTR2polling/PLL160MHz/div16/5MHz/rising/1bit et RMW partiels. **Aucune correction du principal recommandée comme qualifiée avant preuve4bit** ; ne pas attribuer ses DATA_TIMEOUT antérieurs au nombre de nœuds du projet isolé. Restauration du principal et intégration stockage sont des missions distinctes.
