---
mission_id: TR2-20261012-M015-I-PHYSICAL-RESUME
status: DONE
base_ref: main
base_sha: 7a2aca1901162144142f3e13964a4abcc7dbed9d
initial_head: c33303b78f29e4a11289f3c3dd0ff4b36d0974ac
result_sha: 6c504a9d4035c2922d186a03350963bdac33379b
created_at_utc: 2026-10-12T18:25:50.604629+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-I reprise physique — 256 XYZ FIFO acceptés, M014-H restauré

**M015-I-PHYSICAL-RESUME DONE : configuration groupée franchie et deux bursts128/256 mots réels acceptés, résultats publiés et relus intégralement.** Une seule campagne, aucun fallback ni garde affaibli. M015 globale IN PROGRESS ; capture finie ne qualifie pas le flux continu V1.

## Résultat et preuves physiques

Code évalué **6c504a9d4035c2922d186a03350963bdac33379b**, artefactsvalidés réutilisés sans rebuild :BIN 35132 SHA **760aab56cf32aa458c861bf5f22e2801dd595f3bd73ad6a98602c1cff0cdd258**, ELF **7893fda8b3e43e215cbab0a1bcc515d51b0a6986cdf9ff742974e53b8cbb076e**. Firmware entier inchangé par comparaisonGit à6c504a9 ;26 scénarios/crossdéfaut-profil I conservés, pas répétitionA-H ou nouvelle explorationSSSS/GS.

Configurationgroupée : trace 14/07..0A RX00 00 81 0A 00 ;trace 15/0F..14 RX00 7B A8 00 44 00 00. WHO7B/CTRL1=A8/IF_INC/CTRL4=0/CTRL5=0 et sept contrôles conformes. Aucunread simple suradmission. AprèsFIFOpleine, entrée coupéeCTRL1=00 : traces 24/25 montrent07..0A=00/81/0A/01 etCTRL1/CTRL3/CTRL5=00/44/00, avantvidange. Pas vote/majorité/substitution.

| Observation réelle | Résultat |
|---|---|
| Fin harness | stage 10/result 0/restore 0/accepted 1 |
| FIFO polls |6 niveaux monotones0,53,106,158,212,256 ;statuts0000,0035,006A,009E,00D4,A100 |
| Avant/aprèsfreeze | filled_status=stopped_status=A100, niveau256 |
| Bursts |2×128 mots,897octetsRX chacun ; niveaux256→128→0 |
| Intégrité |256tagsid2/paritépaire/compteursmodulo4 séquentiels ; erreurs/tag/parité/gap/perte/saturation0 |
| SPI |40 transfers,HAL_OK/error0/Rxleft0/Txleft0, GPIOCS1→0→1, CFG1=50070067/CFG2=87400000 |
| Premierrefus | valid0, aucun refus ; autreschampsnonrenseignés, pas mesures |

Décodeur offline indépendant des compteurs reconstruisant les bruts : **1794octetsRX**,256XYZ int16 little-endian ettags publiés ;CSV+octetscommande reconstruisent les deux frames byte-identiques, SHA **9255e1e109d149379115a20913d2a96392b955c37372e2ed55f27aa8112adaa3**. Les40traces reconstruisent3680 octets du dump exact ;slotsnonrenseignés exclus. LespréfixesSPI deconfiguration incluent commande, et payloadFIFO complet est publié enhex ; aucun bufferBSS présenté comme donnéesacquises.

| Axe, valeurs brutes LSB | Minimum | Maximum | Moyenne | Écart-type |
|---|---:|---:|---:|---:|
| X |597|1154|874,133|87,073|
| Y |4961|5428|5204,199|82,832|
| Z |-6050|-5460|-5737,277|99,688|

Aucune immobilité, calibration, référence1g ou excitation externe présumée ; pas de seuil gravité utilisé pour admettre les mots. Zéro saturationobservée desint16. Contrôle de tags/niveaux cohérent pourcetlot, compteurmodulo4 pouvant masquer des pertes multiplesde4 : pas garantie universelle d’absence depertes. Ce succès ne résout pas la cause0x50 deH/B/D ni ne réécrit leurs verdicts historiques.

## Timing et limites V1

DWT160 MHz, SPI2,5 MHz calculé depuisHAL/prescaler64, mode 3 observé ; aucune mesure électriqueSCK/CS au fil. Premierburst DWT18744194→19205924 = **2,8858125 ms**, second19342873→19804601 = **2,8858000 ms**, totaltransfert **5,7716125 ms**. Débit pendanttransfert ≈44355 mots/s, pas débitcontinu du système. ActiverFIFO→finalstatut : **20,62093125 ms**, incluantpolls/freeze/délais/vidange. Leshorodatages encadrent lestransactions, pas chaqueconversionXYZ.

ODR 26667 Hz est configurée, pas mesurée indépendamment ;256 mots correspondent théoriquement9,59988 ms. Ce lotvaut6,25% du minimum4096 et0,78125% de32768, pas une fenêtreV1. V1ODR/fenêtres4096..32768/continuité/store-and-forward inchangées. Ni DMA/FFT/bande6kHz/calibration/production/stockage/Modbusvibration/Web/robustesse/endurance ou timing wire qualifiés. READY0/stockage0/configUNAVAILABLE demeurent.

## Sauvegarde, flash, restauration et reprise

Préflightfresh publié/relu au **0faf39519f91954aa2ab2860de0403a069a88734** avantcampagne :UID **004A003D3842501820333157**, STLINK **003500463235510B37333439**/V3J15M7/NUCLEO-U575ZI-Q, U575/U5850x482RevU/3,29V. Imagepermanente reconnue, sauvegardefraîche Flash2 Mio/RAM768 K ethashes vérifiés. Outils etscripts exacts existantscontrôlés, restoreenfinally prêt etaccèsopérationnels, budgetrenouvelé suffisant pour cyclecomplet. Aucun accèsvieille sauvegarde commeétatinitial courant.

Deuxflashs standards NORMAL08000000/verify/reset, diagnostic puisrestaurationM014-H enfinally ;secteurs0..4 puis0..17 seulement. AucunHOTPLUG/masserase/OptionBytes/RDP/bootpersistant/câblage/changementcapteur. DernierCLI suivi dereadback/resetGDB qualifié, ensuiteGDBseul ;trigger4D313546 unefois, debuggerdétaché pendantcampagne, brutsRAM avantreflash. Sourcefirmwareinchangée ; seulswrappers HP modifiés pournomIguardéSteady/deux FC03, rejets/parserPS horsport réussis. Aucunbuild outesthardware antérieur répété.

**Flash2 Mio entière restaurée byte-identique au backupfrais**, SHA **556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0**. Septregistres relus restaurés0/0/0/0/0/4/0. Imagepermanente M014-H/source 0b2a901e185c9cbf8c02410bf2a29a0a3a75491e :BIN 143108 SHA **b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611** ;ELFc37ad8370fbfe3da35c0e5c16d7239d18e7d0ad102f33db79a67a029ecfca8de. Profiltemporaire sansmédia/transport ; aucun FRAMprovisioning/SDwrite niaccèspartitionLBA65536. FRAMgen12 observée, pas comparaisonFRAMintégrale revendiquée.

RepriseHP :371 fichiers paquetH rehashés, deux FC03 B0/21 puisB1/20 seulement, COM7/1152008E1/unit1/retry0/retour0/connexionfermée ; fraîcheurcalibréeSSHreadonly, aucunGDB pendanttrafic. **Journal entier7 événements récupéré etlu**, hashrecalculé identiqueaudistant **2ab64625b2c6ce4ff3852756494014abf9f63b22cf457639d61a721750d00d85**, 2026-10-12T18:21:39.4701754+00:00→2026-10-12T18:21:40.2459795+00:00, passedtrue. B0complet conforme, B1uptime125s/status1/flags0/stockage0/acq0. Registresdécodés, pas capturewire ni nouvellequalificationgénéraleM014.

GDBaprès sansreset :stage 2/result 0/pollerrors0/FRAMV2gen12/identité1/configUNAVAILABLE/IRQ49-66actives/UARTerror0/TXidle/fault0/filehead=tail20/CFSRHFSR0. Détaché/serveursexit0/MCUenexécution. Aucun nouvel essai, trafic oudebugperturbateur ensuite.

## Publication et suite

Preuves `preuves/TR2-20261012-M015-I-PHYSICAL/` :RXhex/CSVXYZ réels,40traces/polls/meta/analyse/timing/CLI/GDB/HPintégral/hashes/restauration. BrutsBIN/ELF/Flash/RAM conservés **/home/lolo/dev/msm/reports/TR2-mission015-I-physical-20261012T181644Z** ; artefactscompilésréutilisés dans /home/lolo/dev/msm/reports/TR2-mission015-I-20261012T162708Z/build-burst. Logs publics UTF8/LF normalisés seulement, deuxJSONHP sansBOM/LF, originaux locauxintacts ; journalHP/RXoctets exacts. Manifest etscriptanalysereproductible publiés.

Journal actualisé avecpreuveFIFO bornée/limites/restauration, roadmap M015 INPROGRESS. Archives I exactesdepuis7a2aca1, missionactiveimmutable, worktree codex/m015-i-physical isolé ; onzeautresworktrees/HEAD etstashes6251b988/f3b829e6 préservés. Permissionscommit/push main/flash standard/debug qualifié etdeuxlectures dereprise exercées ; aucunmerge/rebase/resetGit/forcepush/configpermanente. Résultats publiés au **2cebe602ee5946de3adadb84a6361ab502c2ccdd**, push normal surmain puis43livrables intégralement relus depuis la référence récupérée de GitHub et byte-identiques (`preuves/TR2-20261012-M015-I-PHYSICAL/publication-readback-2cebe60.json`). Worktree propre, onze autres worktrees/HEAD et deux stashes intacts, archives I exactes/mission active inchangée. Cette clôture DONE est vérifiée après son push ; son SHA final et l’état Git sont fournis dans la réponse. ContrôleindépendantChatGPT attendu.

Suite proposée sousnouveaucontrat : utiliser cettevoiegroupée commebase à qualifier pour serviceFIFO deproduction, buffers/fenêtres4096..32768, tempsdeconversion/continuité/pertes etstockage owned V1, avantFFT/supervisionvibratoire. Pasacceptation automatique enproduction niqualificationcontinue àpartirdecettecapture. L’échecgroupé prévuparlecontrat ne s’est pasproduit : comparaisonbreakoutA/B nonnécessaire ici, aucuneintervention/achatd’instrument demandé. Arrêt aprèsl’unique campagne etpublication desrésultatsacquis.
