---
mission_id: TR2-20261012-M015-E-ACQUISITION-DECISION
status: BLOCKED
base_ref: main
base_sha: dbfcec4027200c5d5c989da52000166311fa6954
initial_head: 82d38bf02925d05ce6191ae5e2f38bff4bbe1846
result_sha: null
created_at_utc: 2026-10-12T14:40:26.018428+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-E — Décision et protocole prêts, relecture distante en attente

Les trois lots documentaires sont réalisés. **BLOCKED provisoire pour publication/relecture seulement** ; M015 reste IN PROGRESS, causes etqualifications B/C/Dinchangées. Aucun accès matériel ou essai réalisé.

## Décision livrée

[Note de décision](../DECISION_ACQUISITION_M015_E.md) : chemin directA conservé commedémonstrateurRAM borné ; cibleV1 proposée FIFO128enbursts puisDMA/services seulement aprèsfiabilité etmesure deslatences. Prochaine tranche prioritaire : identificationinstrument puismesurefil/RAM de88/87/BA à625kHz souscontratdistinct. Sansinstrument, exploitation/contrats horsligne desdonnéesA existantes ;pasrejeu ni fauxREADY.

[Fiche mesure](../MESURE_LOGIQUE_M015_E.md) :instrument4entrées3,3V, pointsversionnés àconfirmer, masse logique,25MHz conseillé/profondeur/trigger/exports, tablesattendu/décision ;availabilityNONVÉRIFIÉE, aucun câblage demandé. Bonne trace numérique necertifie pas timing5/20ns. La présence d’un analyseur n’est pas supposée.

AnalyseE des468recordsD :préfixeRAM37440octets reconstruit etidentique auCSV ;seulement2writesCTRL2 (indices15=81 et453=00).26/28divergences singles↔bursts enalimentation,0/32figées. Codephysiquehistoriqueca00da976de487ff6fd52fbd3973af105f07876a ;aucuncode cible évalué enE, result_sha=null. Hypothèsessignal/framing/réceptionMCU/HAL/interfacecapteur conservées ;causephysique nonisolée.

Calculs àODR26667 :XYZ160002B/s,FIFO186669B/s. DirectSTATUS+XYZ exige1,920024Mbit/s ;FIFO128+status1,500019Mbit/s. À625kHz, occupationidéale240%pourFIFO128 :DMAnepeutcorrigercette insuffisance. À2,5MHz candidat :60%fil et2,88ms parbloc128 ;overhead/ordonnancement nonmesurés. SPI3 est l’instance limitée :burst128/897octets compatible avecborneconstructeur1024 ;pas1793octets/256mots enunetransaction. Vitesses2,5/5/10MHz ne sontpas qualifiées enmode3 surbanc. DMA RX/TX requêtesSDK10/11existent, cheminactuelbloquant sansraccordementDMA/IRQcapteur ;INT1/2noncâblées. Mémoire ettemps traitement distinguésde capacitédurable.

V1bloc4§8 conserve26667Hz, fenêtres4096..32768, campagne1/stockagestore-and-forward1/capacitéowned ;M014configUNAVAILABLE/READY0/stockage0. Aucune spécification oufirmware modifié, aucunesubstitution deburst auxlecturesfausses ni masque dedéfaut.

## Vérifications et préservation

Inspectioncode/auditsA-D,CSV A/B/C/D,hachagesdesentrées,reconstructiontraceD etcalculsarithmétiques reproducibles ;lectureELFM014 existant par `arm-none-eabi-size` uniquement horscible (bss62912/data24). Recherche initiale dechemins nonprésents corrigée parinventairedu dépôt, sanscréationdefichierssupposés. Validation appropriée :relecture documentaire/diff/intégrité ;aucuntest inchangé, build, cross-build, GDB, flash, reset, HP, FRAM/SD oucâblage. Lesflagsflash/debugtrue n’ont pasété exercés :restrictiondocumentaire ducontrat respectée.

ArchivesD mission/rapport copiesexactes depuisdbfcec4,sansécrasement ;missionactiveChatGPT inchangée. WorktreeE isolé/codex/m015-e depuis82d38bf ;mainlocaloriginal82169afdivergépréservé, autresworktrees et2stashes intacts, paspull/rebase/merge/reset/forcepush. Permissions exercées :modificationsdocumentaires, commit/pushnormal surmain explicite ;aucunepermissionmatérielle exercée.

Firmwarepermanent M014 BINSHAb462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611 etFRAMgen12/SDOFF sontl’étatdernièrementobservé enD, pas unnouveaurelevé E. Aucunrisqueintroduit surpartitionSD :zéroaccès. Journal actualisé pourdécisionPRÉPARÉE/limites etroadmap gardeM015INPROGRESS.

Preuvespubliques `preuves/TR2-20261012-M015-E/` :analyseD/calculs/inputs hachés. Preuveslocales **/home/lolo/dev/msm/reports/TR2-mission015-E-20261012T143539Z** :commandes/étatGit/sourceanalyse. SHAdepublication etrelecture consigné aprèspush ;SHAfinal fourni dansréponseCodex, pas hashautoréférent. Aprèsrelecturevérifiée, statutDONE uniquement pourEdocumentaire, pasM015 ouqualificationphysique.
