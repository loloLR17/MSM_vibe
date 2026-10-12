# État courant TR2 — référence reconstruite

Mise à jour : 12 octobre 2026. Mission `TR2-20261012-M015-O-ST-OFFICIAL-DEMO-PORT`.

**Ce document est un nouvel état de référence reconstruit à partir de preuves vérifiables. Il n'est pas une restitution du journal historique HP.** Sa création a été explicitement autorisée par l'utilisateur pour la mission `T1000-20261010-CLOTURE-001`, après escalade AGENTS.md §18. Aucun original n'a été retrouvé dans l'historique Git, les répertoires Linux locaux ou les documents Windows accessibles. Les archives d'échanges du 8 octobre mentionnent un journal local HP, sans en fournir le contenu. Aucun état historique inconnu n'est inventé.

## État opérationnel actuel — M015-O, checkpoint logiciel

- **VALIDÉ logiciel/provenance** : fonction ST iis3dwb_fifo réellement compilée, sans bench_run ; source1f848c38 originale exacte, gitlinkdriverd995 inaccessible, driverSTf83 compatible déclaré. Mode3 dansprojetSTV3 vsmode0 K/N identifié.14mocks et4ARM passent ;formatsconsole/buffer512/plateforme/guards/cleanup explicités dansofficial/AUDIT.
- **PRÉPARÉ / NON VÉRIFIÉ** :témoinmode3/getterinchangé,puisaliasniveau10 etmode0 sousconditions,confirmation512XYZ si vert. AucuncibleO encore ;pas capteurprimaire ni média. ArchivesN/stashes/worktrees préservés ;preuves `/home/lolo/dev/msm/reports/TR2-mission015-O-20261012T214009Z`.

## État opérationnel antérieur — M015-N, 12 octobre 2026

- **VALIDÉ harness/transport borné** :réservecleanup12traces/24snapshots/32octets,400polls/500ms/1ms,agrégat etrefusdistincts ;5CTest/43scénariosintégration/14Pythonqualification/4ARM/GCCfanalyzer réussis. Checkpoint64f55db publié/38fichiersrelusavantmatériel ;vendorSDKinchangés,WHOpollingmocknonrégressés/acquisphysiquesLnonrejoués.
- **OBSERVÉ / BLOCKED XYZ** :référenceST512TS/0XYZ(285traces),no-tsFIFOvide500ms(543traces),bdr-last512TS/0XYZ(285),word7huitTS/0XYZ(257). ToutesHALcomplètes/erreurs0,cleanup0/readbacksXLoff/bypass/TSoff,aucunsaturationjournal. Brutsreconstruits,paritébonne/pasTS24ticks,aucunXYZ ;compteursconstants nequalifientpasperteszéro. Troisdiagnosticsconsommés,512XYZ/4096NONENGAGÉS ;causebatchingXL/étatcapteur/écrituresàdiscriminer indépendamment,mêmesvaleursBDR0A/XLACreluesconformes.
- **PRÉSERVÉ / SUITE** :secondeSN0032001F…/UID001B0046…vérifiésavant4flashs/verify/readbacks exacts,word7inactiflaissérunning/capteurau repos,faults0 ;aucunbackupFlash2MiB/restaurationsecondaire. TR2principal/M014-H/FRAM/SD/Modbus jamaisaccédés ;stashes/worktrees/archivesMpréservés. Prochainepreuvewireouautretémoin sousnouveaucontrat,aucunessairépété. Preuves `/home/lolo/dev/msm/reports/TR2-mission015-N-20261012T210706Z`,rapport/preuves publiés au5f965411b5628f60669a529d2100b41df1b15598,162fichiers relus intégralement byte-identiques ;BLOCKED technique maintenu.

## État opérationnel antérieur — M015-M, 12 octobre 2026

- **VALIDÉ LOGICIEL / SPI LONG BORNÉ** :14scénariosC/2Python/deuxbuildsFIFOARM, checkpointa7a2ebc publié/23fichiersrelus. SnapshotavantCloseTransfer reproduit O0HAL_TIMEOUT/OVR :211/896consommés,685restants,SR104A. SeulHALSPI O2 reçoit deuxbursts896complets/SR100A/erreurs0 en3,632ms ;CPU16MHz/SPI2MHz/mode0/réglagesST/délaisinchangés. Saturation serviceHALDebug causeopérationnelle étayée, pas mesurewire ni qualificationgénérale.
- **BLOCKED NOUVEAU / STOP** :O2 atteint1024traces/2048snapshots après256entrées,cleanupinitial empêché ;256timestamps/paritécorrecte/pas24ticks,0XYZ. Fenêtre512 nonqualifiée et donnéesaccepted=false ; pas troisièmeacquisition. Préparer réservecleanup/pollsbornés puisdiagnosticabsenceXYZ sousnouveaucontrat ;I/J/M015V1 restentouverts.
- **MISE AU REPOS / SECONDE FLASH JETABLE** :imagecleanup dédiée crossbuildée/flashée/reluetriggerunique,9transactionsvalides,CTRL1=0C(XLoff)/FIFO_CTRL4=80(bypass)/CTRL10=00(TSoff),result0. AucunbackupFlash2MiB ni restaurationinitiale conformémentnouvelle règle ;secondeSN0032001F…/UID001B0046… vérifiés avantchaqueflash,firmwarecleanupinactiflaissérunning. TR2principal/M014-H/FRAM/SD/Modbus jamaisaccédés,worktrees/stashes/archivesLpréservés. Preuves `/home/lolo/dev/msm/reports/TR2-mission015-M-20261012T204419Z` ;rapport/preuves publiés au281eadaa166c8723305c9db774973255f413c37b,127fichiers intégralement relus byte-identiques. BLOCKED technique maintenu.

## État opérationnel antérieur — M015-L reprise essai n°2, 12 octobre 2026

- **VALIDÉ PHYSIQUE BORNÉ** : utilisateur annonce correctionSPI ; secondeSN0032001F…/UID001B0046… reconfirmés, Flashfraîche doubleidentique àrestaurationessai1. WHO n°2unique7B/aucunwritecapteur ;pollingST64XYZdistincts/0saturation/309tracesvalides/result0/cleanup0. SourcesetartefactsK inchangés,41C/5Python/4ARMnonrejoués ;premieréchec00 etpreuvespréservés/rapportarchivéexact.
- **FIFO LONG BLOCKED / DIAGNOSTIC** : FIFOstrict API0/rawsuivant257/840tracesHAL_OK →refusgetter03F connu,0drain. VarianteSTATUS10séparée, configurationSTinchangée, premierburst896 demandé→trace834HAL_TIMEOUT3/error104=OVRSPISTM32+timeout,0entréeacceptée/cleanup0. 841traces/2548octets, limitesRAMnonépuisées ; tailleRXréellenonrenseignée, HALfermeture remetcompteurs0. Bufferpréinitialisépartiellementrempli publiécommeREJETÉ, pas128XYZréels ;paspreuve d'overrunFIFOcapteur ouabsenceglobaleperte.
- **RESTAURÉ / SUITE** : Flash2MiB entièreexacte au backupfrais etessai1/SHAfcea004e…, seulssecteurs0..3 ;UIDexact/faults0/detach/serveursexit0. TR2principal/M014-H jamaisaccédé. Prochainevérificationlogicielle ciblée SPI_SR/RXremainingavantCloseTransfer/serviceRX4µs/DebugsansO/core16MHz ;saturationCPU hypothèse, pasbypass/timeoutallongé ni essairejoué. WHO/pollingvalidés, FIFOlongBLOCKED/M015INPROGRESS, I/Jinchangés. Résultats publiés au commit 0eda53843e64becf8dfa5ae4ec02dfb23a04c77d, 115 livrables relus intégralement et identiques, preuvesessai1/archives/stashes préservés. Preuves `/home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z`.

## État opérationnel antérieur — M015-L essai n°1, 12 octobre 2026

- **OBSERVÉ / BLOCKED physique WHO** : seconde NUCLEO SN0032001F3235510C37333439/UID001B00463842501820333157/RevU3,28V ; câblage/GNDmarqué/VDD/VDDIO3,3V confirmés utilisateur. Artefacts ST K rehashés sansrebuild/retests. WHO_ST_STRICT sourcee222dc8 flashé/verify/readback ; unique trigger reçoit00 au lieu7B, HAL_OK/error0/counts0/traceunique, aucunwritecapteur/XYZ. STOPpremierécart : polling/FIFO/STATUS10 nonexécutés. GPIOCShaut/MISOdigitalbas en snapshot, paspreuvewire ni causeattribuée.
- **RESTAURÉ SECONDE CIBLE / TR2 PRÉSERVÉ** : backupFlash2MiB doublelecture exacte, aprèsWHOtailhors32KiBinchangé, seulssecteurs0..3restaurés depuisbackup ; Flash2MiB entièreidentique SHA fcea004eb0c91af44f8e36ca61206b5f6bf25fd4c5db4dfdab42d1a19dcea9d9. PC08000c80/UIDexact/faults0/detach/serveursexit0, firmwareinitial nonqualifiéfonctionnellement. AucuneconnexionauTR2principal/COM7/média, M014-H resteinchangé sansnouveauconstat.
- **SUITE** : résultats publiés au commit f36425707e97e233f079a9d37381567d92ea761b, 62 livrables relus intégralement et identiques ; BLOCKED technique maintenu ; avantrepriseexplicitementcadrée, vérificationphysique ciblée MISO/SDO→PC11, orientationnappe/GND/VDDIOjusqu'àMKI208. PasrejouerWHOconsommé/mode3/fallback ici. I256/J4096BLOCKED/M015INPROGRESS inchangés ; erreurmacroGDB corrigée parcapturemémoire seulement, sansautretransactioncapteur. Preuves `/home/lolo/dev/msm/reports/TR2-mission015-L-20261012T195811Z`, archivesK/worktrees/stashes préservés.

## État opérationnel antérieur — M015-K, 12 octobre 2026

- **VALIDÉ LOGICIEL / READY_FOR_PHYSICAL** : projet autonome ST dans `experiments/iis3dwb_st_reference`, pilote/exemples figés et byte-identiques ; WHO readonly, polling et FIFO strict/STATUS10 compilés. CTest2/2 :41scénarios C et5tests Python, quatre images ARM ; mocks seulement. Deux erreurs getter ST reproduites (niveau03F, batching case10 au lieu0A), aucun patch vendeur ni attribution à l'anomalie J.
- **PRÉPARÉ / NON VÉRIFIÉ PHYSIQUE** : pinout constructeur CN8/PC9..12 ↔ MKIGIBV2 documenté, point GND et référence/révisions/orientation/câblage réel à confirmer. Seconde carte SN/UID non identifiés ; protocole progressif WHO/polling/FIFO et backup/verify/readback préparé. Aucun accès MCU/STLINK/COM7/flash/debug/reset/média, aucun essai pendant montage. M014-H permanent première carte intact, dernier état observé reste celui de J.
- **SUITE** : code/projet publié au commit e222dc8ddb7cfd0829c655533ac8e646220721f2, 51 livrables relus intégralement et identiques ; READY_FOR_PHYSICAL confirmé logiciel, aucun accès cible ; après montage utilisateur et identité seconde cible, qualifier sa chaîne puis exécuter le protocole borné. Aucune identité du TR2 recyclée. I256 figé reste acquis, J4096 BLOCKED, M015 IN PROGRESS ; aucune qualification continue/V1/stockage/FFT. Archives J et worktrees/stashes préservés. Preuves `/home/lolo/dev/msm/reports/TR2-mission015-K-20261012T192739Z`.

## État opérationnel antérieur — M015-J, 12 octobre 2026

- **BLOCKED physique continu4096** :unecampagne code24ccec0, premierburst128reçu puisgroupé07..0Apostvidange=80/00/14/0C contre80/00/0A/06 attendu, firstref09=14/attendu0A. HAL_OK/error0 ;stage10/result9/restore0/loss4/windowINVALID/wordcount0, parserXYZfirmware nonatteint. Bruts128reçus décodésoffline/rejetés, globalsXYZBSSnonmesurés ;configured[] partiellementréécrit, dernierCTRL4réel0C dansraw. Pasdeuxièmeessai/correctifheuristique, causeprécise nonattribuée.
- **VALIDÉ logiciel / M014 PRÉSERVÉ** :18casneufs/crossdéfaut-profil réussis,19ecaspostrefusajouté→19/1CTestpassé sansmodifiercible/rebuildARM. RAM101536BSS+réserves/margevalides, serviceborné32bursts/250ms/guardsstricts. Flash2Mio restauréeexacte/BINb462c29a,septregistres0/0/0/0/0/4/0 ;HPB0/B1passé/journalentier hash77ce465d…/uptime140s, gen12/configUNAVAILABLE/SDOFF/IRQ49-66/faults0/MCUrunning,aucunmédia/partition.
- **SUITE** :résultats publiés/relecture intégrale de 60 livrables identiques au commit 2b96d8215a93c5012b813499abd61aea408f534d, JBLOCKED/M015INPROGRESS, I256figéacquisinchangé, continu/fenêtresV1/FFT/stockage nonqualifiés. Prochaincontratlimité readbackgroupé aprèsFIFO_OUTactif, éventuellementABmodule sousinterventionutilisateur siutile ;aucunactephysiqueadditionnelniinstrument demandéici. Preuves /home/lolo/dev/msm/reports/TR2-mission015-J-20261012T184145Z, archives/stashes/worktrees préservés.

## État opérationnel antérieur — M015-I reprise physique, 12 octobre 2026

- **VALIDÉ PHYSIQUE borné** : campagneunique source6c504a9, contrôlesgroupés passés/CTRL1A8,2bursts128/256XYZ réels acceptés,6niveaux0/53/106/158/212/256 puis256→128→0 ; erreurs/tags/parité/gaps/flagsperte/saturation0, result0/restore0. Brutsreconstruitsbyte-identiques ;40traces. Bursts2,8858ms chacun, FIFOenable→final20,6209ms ; ODRconfigurée nonmesurée, compteurmodulo4 n’exclut pas toutespertes. Aucunrejeu/secondessai, testsI26/crossréutilisés sansrebuild.
- **M014 PRÉSERVÉ** : sauvegardefraîche puisFlash2Mio restaurée exacte/SHA556fedf5/BINb462c29a, contrôles0/0/0/0/0/4/0 ; repriseHP B0/B1réussie/journalentier hash2ab64625…/uptime125s. FRAMgen12/configUNAVAILABLE/SDOFF/IRQ49-66/faults0/MCUrunning, aucuneécrituremédia niaccèspartition.
- **STATUT/SUITE** : M015-I-PHYSICAL-RESUME DONE dans portée bornée ; résultats 2cebe602ee5946de3adadb84a6361ab502c2ccdd publiés/43livrables relus intégralement et identiques, worktrees/stashes/archives intacts ; M015 INPROGRESS, fluxcontinu/fenêtresV1/FFT/stockage nonqualifiés. Basegroupée àqualifier sousnouveaucontrat pourserviceFIFO/continuité/ressourcesowned ; pas substitutioncapteur ouinstrumentation nécessaireici. Preuves /home/lolo/dev/msm/reports/TR2-mission015-I-physical-20261012T181644Z ; archivesI/worktrees/stashespréservés.

## État opérationnel antérieur — M015-I, 12 octobre 2026

- **IN_PROGRESS, logiciel validé/checkpoints** : mission/rapport H archivés exacts, artefacts H rehashés, worktrees/stashes préservés. Harness adapté aux readbacks groupés avec WHO/IF_INC/voisins et gardes transfert ;26scénarios ciblés et cross-build défaut/profil réussis ; aucun acquis A-H rejoué. Checkpoint logiciel 6c504a9d4035c2922d186a03350963bdac33379b publié et intégralement relu, point de reprise complet disponible ; session arrêtée sans accès cible.
- **PHYSIQUE REPORTÉ** : budget annoncé12%, aucun accès cible/flash/reset/HP/média en I. Dernier état H : M014-H restauré/Flash identique/gen12/SDOFF/MCUrunning ; état historique, pas nouvelle observation. Campagne unique et restauration à assurer après renouvellement. M015 IN PROGRESS, V1 inchangée.

## État opérationnel antérieur — M015-H, 12 octobre 2026

- **OBSERVÉ, diagnostic exploitable** : campagne unique code54e8deb,43transactions/21,392ms. S G G S actif = A8/A8/A8/50 immédiatement et après20,276ms ; arrêté/baseline=00 partout. Deux simples actifs corrects/deux incorrects/quatre groupés corrects ; première divergence trace27/CTRL1_XL10 attenduA8/reçu50/HAL_OK, result9/restoration0. Association à position/historique et actif, pas cause électrique/HAL attribuée ; aucun fallback/FIFO/XYZ/rejeu.
- **VALIDÉ logiciel / M014 PRÉSERVÉ** :53scénarios ciblés et cross-build défaut/profil réussis, gardes/V1 inchangés. NVIC sixsnapshots conforme au profil sansIRQexternes, ISPRFFFFFFFF absent ; pas HOTPLUG. Septcontrôles restaurés0/0/0/0/0/4/0 et Flash2Mio byte-identique ; reprise HP B0/B1 réussie, journal complet hash7bb9a159… ; FRAMgen12/configUNAVAILABLE/SDOFF/IRQ49-66/faults0/MCUrunning, aucun accès partition/écriture média.
- **SUITE / STATUT** : M015-H DONE pour diagnostic exploitable ; résultats 1ccec7f7d7916564322ddfb6437e20afa7f4b269 relus intégralement,56livrables byte-identiques, worktrees/stashes/archives préservés ; M015 IN PROGRESS/FIFO non validée. Prochain audit logiciel ciblé transitions7→2 et ordre SSSS/GS sous contrat distinct ; BM869s nonutilisé/sanscaractéristiques présumées, substitutionbreakout proposée conditionnellement plus tard, aucun geste demandéH. Preuves /home/lolo/dev/msm/reports/TR2-mission015-H-20261012T155415Z ; acquis/worktrees/stashes/archives G préservés.

## État opérationnel antérieur — M015-G, 12 octobre 2026

- **OBSERVÉ, refus localisé** : tentative unique code5f85dbe, premier écart CTRL1_XL/10 attenduA8/reçu50, lecture simple TX90 00/RX00 50, HAL_OK. Groupé07..0A conforme00/81/0A/00 ; stage10/result9/loss4/restore0, zéro poll/burst/XYZ.38traces sauvegardées avant reflash ; aucun deuxième essai. Valeur historique F non récupérable, même chemin de refus reproduit ; cause électrique/HAL/capteur non démontrée.
- **VALIDÉ logiciel / M014 PRÉSERVÉ** :21scénarios ciblés et cross-build défaut/profil réussis, gardes inchangés, acquis A-F non rejoués. Sept contrôles restaurés0/0/0/0/0/4/0 ; Flash2Mio byte-identique/M014BINb462c29a. Deux FC03 B0/B1 de reprise réussies, journal complet hash47546b38… ; FRAMgen12/configUNAVAILABLE/SDOFF/IRQ49-66/faults0, MCU running. Aucun accès partition/écriture média.
- **SUITE / STATUT** : M015-G DONE pour refus exploitable ; résultats b3a3c428a9f2d35801f5601db1a38d58a120246a relus intégralement, 50 livrables byte-identiques, worktrees/stashes/archives préservés ; M015 IN PROGRESS, FIFO/fluxV1 non validés. Prochain contrat : diagnostic limité write/readback CTRL1 au passage actif, sans bypass A8 ni instrument absent imposé. Preuves /home/lolo/dev/msm/reports/TR2-mission015-G-20261012T152038Z ; anciens worktrees/stashes/archives préservés.

## État opérationnel antérieur — M015-F, 12 octobre 2026

- **BLOCKED physique** :unique tentative mode3/2,5MHz, code571b0d8, arrêtgardeconfiguration avantactivationFIFO (result9/loss4/restore0/HALerrors0),0poll/0burst/0XYZ. Valeurdivergente nonexportée par cetteversion ;configured tout0=BSSnonrenseigné,aucune donnée synthétique. Pas deuxièmeessai ni rechercheexhaustive.
- **VALIDÉ logiciel / M014 PRÉSERVÉ** :harness2bursts128 préparé,testneuf1/1/crossdéfaut-profilréussis,A-Einchangésnonrejoués. Contrôlesrestaurés0/0/0/0/0/4/0 ;Flash2Mioidentique,M014BINb462c29a,FRAMgen12/configUNAVAILABLE/SDOFF/IRQ49-66/faults0,2FC03derepriseB0/B1réussies,MCUrunning. Aucunmedia dansdiagnostic,partitionpréservée. M015F BLOCKED,M015INPROGRESS.
- **PUBLICATION VÉRIFIÉE** :résultats0843c62782457780d85af9b87b3404d196a1101f,31livrables relus et byte-identiques ;BLOCKED technique conservé. V1inchangée :256mots9,6msnevalentpas4096..32768/continuité/store-and-forward ;aucunequalificationFFT/ODR/SD. ArchivesEexactes,worktreeFisolé etanciensworktrees/stashespréservés. Preuveslocales /home/lolo/dev/msm/reports/TR2-mission015-F-20261012T145521Z.

## État opérationnel antérieur — M015-E, 12 octobre 2026

- **PRÉPARÉ, décision documentaire** : garder direct A pour démonstrateurRAM borné ;viserFIFO128bursts puisDMA/serviceV1 aprèsqualification. Prochainepreuve :wire↔RAM88/87/BA à625kHz siinstrument3,3V/4canaux identifié ;fiche avecpoints àconfirmer/masse/timing/profondeur/critères, aucun câblage demandé.
- **VÉRIFIÉ horsligne** :468recordsD reconstruits37440octets identiques auCSV ;CTRL2seulement2writes,26/28divergencesactives et0/32figées conservées. CalculsFIFO128+status≥1,500019Mbit/s ;625kHzinsuffisant,burst128897octets reste souslimiteSPI3documentée1024. Vitessesplushautes/DMA/flux/stockageNONQUALIFIÉS ;V1inchangée.
- **PRÉSERVÉ** :aucunhardware/test/build/flash/debug/HP/FRAM/SD ;M014étathistoriqueD inchangé, pasnouveaurelevé. E DONE documentaire :publication10bcdeaf5e492dd5b87535ca8d536b42218dfc5f,11livrables intégralement relus et byte-identiques ;M015INPROGRESS etB/C/DBLOCKED conservés ;archivesD exactes etworktrees/stashespréservés.

## État opérationnel antérieur — M015-D, 12 octobre 2026

- **OBSERVÉ, défaut discriminé / BLOCKED cause précise** : campagneunique468transactions/28groupes actifs puis32figés. CTRL2single81 attendu :80×23/81×2/02×2/00×1, burst07..0A toujours81 ;26/28divergences. AprèscoupureCTRL1 sansvidange,32/32comparaisonsconcordantes. WHO7B/CTRL3=44stables, HALerrors0 ;28statutsgroupésmonotones1→256. Pas proofwire ni causefastpollB ; aucunecorrection/XYZ/DATA_OUT.
- **VALIDÉ logiciel / M014 PRÉSERVÉ** : test ciblé neuf1/1 etcrossdéfaut/profilréussis ;A/B/Cnonrejouées. Contrôlescapteurrestaurés0/0/0/0/0/4/0 ; imageM014restaurée/Flash2Mio byte-identique, FRAMgen12/configUNAVAILABLE/SDOFF/IRQ49-66/faults0 ;2FC03derepriseB0/B1réussies,MCUrunning. AucuneFRAM/SDdansprobe,partitionpréservée. M015-D BLOCKED,M015 IN PROGRESS ; prochainepreuvewire↔RAM àpréparer avecinstrumentidentifié/contratdistinct.
- **PUBLICATION VÉRIFIÉE** : résultats73cbdee155d88580b5db52372c84ac58abe2f40f,30livrables intégralement relus et byte-identiques ; BLOCKED technique conservé. Archives C exactes depuis37b97f7 ; worktree D isolé, autres worktrees/stashes préservés. Preuves locales /home/lolo/dev/msm/reports/TR2-mission015-D-20261012T135359Z.

## État opérationnel antérieur — M015-C, 12 octobre 2026

- **VALIDÉ logiciel seulement** : audit statique A/B/ST/HAL ; profil isolé de lectures SPI,512transactions maximum, sans écriture capteur ou média ni FIFO/XYZ/UART. Nouveau test ciblé1/1, cross-build défaut/profil réussis ; validations A/B inchangées nonrejouées.
- **OBSERVÉ** : GDB seul identifie image permanente M014 et UID attendu, sauvegardeFlash2Mio/RAM768K ; FRAMgen12/IRQ49-66/faults0/configUNAVAILABLE, cible détachée running. Diagnostic unique exécuté :512lectures/128ms, WHO7B/CTRL3=04/FIFOvide0, singles etbursts concordants, HALerrors0 ; causeB etfiabilitéFIFOactive nonisolées. ImageM014 restaurée/Flash2Mio byte-identique,2lecturesderepriseB0/B1réussies, gen12/configUNAVAILABLE/SDOFF, MCUrunning. M015-C BLOCKED, M015 globale IN PROGRESS.
- **PUBLICATION VÉRIFIÉE** : résultats au8ce2a3b6246a1139a186af46061e4b464aa5f224,31livrables intégralement relus et byte-identiques ; BLOCKED technique conservé. Archives B exactes depuis cfb9918 ; worktree C isolé, tous worktrees/stashes antérieurs préservés. Preuves `/home/lolo/dev/msm/reports/TR2-mission015-C-20261012T131552Z`.

## État opérationnel antérieur — M015-B, 12 octobre2026

- **BLOCKED FIFO/cadence** : une seule campagne mode3/625kHz, code15c3369446a568ff2dd2d7a13453d6cfaa1cdbf0 ;171statuts/11reculs du niveau avant toute vidange,0x40FE au niveau254 puis0xA100/256, FIFO_CTRL2 relu0x80 après0x81 vérifié. Cause inconnue, HALerrors0 ne prouve pas intégrité. Garde refuse avant vidange :0XYZ accepté ; contrôle négatif non atteint. Pas nouvelle variante/essai, ni supposition d'immobilité du voilier.
- **VALIDÉ logiciel seulement** : prototypeFIFO256/stopwatermark, tags/parité/niveaux/overrun/temps/restauration ; testhôte1/1 et cross-build défaut/profil réussis. Cœur/driver inchangés,114firmware/539.NET nonrejoués. PasDMA/production/stockage/B5/FFT ; portée réduite du contratB prévaut sur propositionhistoriqueA.
- **M014 PRÉSERVÉ** : firmwarepermanent SHAb462c29a restauré ; Flash2Mio byte-identique initiale, SHA556fedf58a88f7de0ad63826af6b4941de8bb72dcd6e97a886c1017da5ba93f0. BootFRAMV2/gen12/configUNAVAILABLE/READY0/stockage0, IRQ49/66/faults0/UART0/filevide ; MCU détaché enexécution. AucuneFRAMwrite/provisioning/SDlecture-write-allocation, partitionLBA65536 préservée.
- **VALIDÉ reprise** : HP10:06:21..22UTC,2FC03 B0/B1/2réponses/sansretry/retour0, B0completconforme/B1uptime215s. Journalintégralpublié/SHA8B31E409B02580B5D109D281C506D69B9F470C56A7C9B5812A4BDBF14831278F ; GDBaprès sansreset. Pas rerunpolling/perteM014.
- **PUBLICATION/SUITE** : code déjàpublié15c3369/relecture11fichiers ; résultats/rapport final publiés au 8c39ff1, 30 livrables relus intégralement et byte-identiques. CSV171statuts/replay/audit/publicpreuvesB ; locaux `/home/lolo/dev/msm/reports/TR2-mission015-B-20261012T095010Z`. M015globale IN PROGRESS, acquisAconservés ; futurcontrat de fiabilitéregistresFIFO avant nouvellecapture. Worktreesinitial/H/A et2stashes préservés, archivesAexactes.

- **ANALYSE HORS LIGNE / NON VÉRIFIÉ** : certains octets après le passage 127 évoquent décalage/perte du bit7, sans modèle expliquant toutes les anomalies. Overrun observé dans une chaîne de lecture incohérente, pas preuve indépendante d’écrasement. Aucun nouveau test ou accès matériel ; complément et résidus publiés, qualification BLOCKED conservée.

## État opérationnel antérieur — M015-A, 12 octobre 2026

- **VALIDÉ PHYSIQUE dans la portée M015-A** : IIS3DWB réel WHO=0x7B, SPI3/CS PC9, mode 3/625 kHz/±4 g, registres A8/44. Capture de 2 048 XYZ, zéro erreur/saturation ; norme au repos 984,75 puis 984,65 mg. Auto-test mécanique interne positif/négatif : écarts par axe de 1,6 à 1,9 g, puis retour au repos. Pas d'excitation externe ni calibration ; **M015 globale IN PROGRESS**.
- **OBSERVÉ, limites temporelles** : DWT 160 MHz ; timestamps encadrant les lectures, cadence ≈6 538 lectures/s. ODR 26,667 kHz configuré, non mesuré indépendamment. Polling sans FIFO/DMA, pertes intermédiaires possibles ; pas de simultanéité XYZ ni de flux continu qualifiés.
- **ANOMALIE CONSERVÉE** : trois captures mode 0 (TX/RX séparés à 2,5 MHz, duplex à 2,5 MHz, duplex à 625 kHz) restent bimodales, avec 1/3/6 saturations. Le mode 3 à même vitesse donne une capture cohérente. Cause électrique précise non mesurée ; aucun filtrage correctif. Les 8 192 données brutes et analyses sont publiées dans `Echanges_Codex/preuves/TR2-20261012-M015-A/`.
- **M014 PRÉSERVÉ, CIBLE EN EXÉCUTION** : BIN permanent SHA b462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611, source 0b2a901. Après chaque diagnostic, Flash 2 Mio entièrement identique à la sauvegarde. Capteur restauré CTRL1/3/5=0/4/0 ; FRAM V2/génération 12, identité 1, configuration UNAVAILABLE, READY/stockage=0. IRQ49/66 actives, erreurs UART/faults=0, file vide. Aucun accès SD ni provisioning FRAM ; partition LBA65536 préservée.
- **VALIDÉ, reprise production** : le 12/10 à 09:22:43 UTC, deux FC03 B0/21+B1/20 sur HP, deux réponses/retour 0/sans retry ; B0 complet conforme et B1 uptime=73 s. Journal complet récupéré, SHA CFBF72C1716288EAA2F845B82D662BA62C4520C8647EC40BF7502EE3A1208E78 ; GDB après sans reset. Pas de répétition du polling/perte M014. Sa revue indépendante est indiquée par le contrat M015-A.
- **VALIDÉ LOGICIEL, M015-A DONE** : driver réel réutilisé, transaction duplex, invalidité effacée, profil RAM borné avec restauration et sans médias/UART. 114 tests et cross-build après changement du driver ; cross-only pour les derniers changements du harness. Les 539 tests .NET inchangés ne sont pas rejoués. Code capture évalué : 3317535897931589cd2d26e44ce1c9900032b9bf. Résultats publiés au SHA 6d10287e85fc499c9f474e3d656253b00b071106, 47 livrables relus intégralement et identiques ; revue indépendante M015-A à faire.
- **SUITE / GIT** : préparer M015-B pour le raccordement production en mode 3, le débit/FIFO/DMA, les timestamps de conversion et l'excitation externe. Stockage V1 nécessaire avant campagne. Le firmware permanent M014 conserve le mode 0, sans acquisition ; la nouvelle voie reste un diagnostic temporaire. Worktree M015 isolé, dépôts initial/H et deux stashes préservés, archives H exactes. Preuves complètes : `/home/lolo/dev/msm/reports/TR2-mission015-A-20261012T085627Z`.

## État opérationnel antérieur — M014-H, 2026-10-12T08:37:50.450052+00:00

- **VALIDÉ physiquement dans la portée M014** : identitéFRAMV2/gen12, B0/B1réels, polling3+3/perte de disponibilité/fermeture-réouverture driver/repriseidentité et données fraîches ; uptimeB1corrigé9/11/14→217/219/221s. Deux campagnes9requêtes (première uptimefigé, corrigéevalidante), puis2lectures surimagepermanente ;20requêtes/18réponses/2timeouts planifiés, retry0.
- **CIBLE EN EXÉCUTION** : production permanente sansobserver ni silence, BIN143108 SHAb462c29a775fec17fccf8036b5612742b1366210359bcd3f192c95435740d611, source0b2a901. STLINK003500463235510B37333439/UID004A003D3842501820333157/DeviceId1/unit1/TR2-000001/MSM. BootrecoveryV2/gen12, IRQ49/66actives, UART/pollerrors0/filevide ;debuggersfermés.
- **NON QUALIFIÉ opérationnel/industriel** : configuration seulement structurellement valide mais UNAVAILABLE sans ressourcesowned ; READY etstockage0. SDtotalementdésactivée/zéroaccèsH, aucuneallocation/écriture ni risque surpartitionLBA65536. Acquisition, bulkSD/E4, timingDE/TC/CEM/endurance restentouverts ; M013acquis bornés/réserves, pasqualificationgénérale.
- **VALIDÉ logiciel** :6tests générateur + répétitionoffline backup réel ; nouvelle régressionB1 et113tests/cross-buildsurcorrectif, PC539Gconservés nonrelancés. JournauxHP complets récupérés/hachés/versionnés danspreuves/TR2-20261012-M014-H.
- **PUBLICATION VÉRIFIÉE** : résultats publiés au SHAae14c4c8e6081086ded87c5d505e83fa74c7a496, relectureintégrale 32livrables byte-equal ; rapportH DONE etM014DONE pourB0/B1/polling/perte-reprise. revueindépendanteChatGPT attendue ; M015prochaine mission proposée, pasengagée.
- **GIT / REPRISE** : worktreeactif depublication `/home/lolo/dev/msm/projects/MSM_vibe-M014-H`, branchelocale codex/m014-h-integration, pushnormal versmain ; dépôtoriginalmain82169af et2stashespréservés après divergenceautorisée intégréeisolément, nepasrebase/pullautomatique suroriginal. Preuvescomplètes `/home/lolo/dev/msm/reports/TR2-mission014-H-20261012T073729Z`. HPespacefaible, paquetexistant conservé/réutilisé ; dossierpartiel de secondpackage conservé, seulecopieZIPtemporairedeCodex retirée aprèshashégal àoriginal.

## Référence et gouvernance de la reconstruction T1000 (historique)

- Référence des sources au début de mission : `main`, `75eba6b431725b858db3ac5380884934026a1475`.
- AGENTS.md V2-B3 et `Echanges_Codex/PROTOCOLE_ECHANGE_V2.md` applicables ; les spécifications V1 et freezes restent inchangés.
- Autorisations de la présente mission : `target_branch: main`, `allow_commit: true`, `allow_push: true`, `allow_flash: false`, `allow_debug: false`. Elles expirent à la clôture. Aucun nouveau développement fonctionnel TR2.
- `Echanges_Codex/MISSION_EN_COURS.md` reste un document historique rédigé par ChatGPT ; le contrat courant est la demande utilisateur du 10 octobre et sa confirmation explicite de dérogation/reconstruction/permissions, pas cette ancienne mission.

## Résultats effectivement établis avant migration

- **VALIDÉ logiciel** : correctif autorisé du setup de `test_time_history_store.c`, `PersistentStorageCore storage = {0};`, sans changement de source de production ou d'assertion. Validation précédente : 102/102 tests firmware et cross-build ; supervision .NET restore/build et 466/466 tests.
- **VALIDÉ physiquement pour le poste** : mission distincte du 10 octobre, flash vérifié, démarrage, GDB, breakpoints matériels, reprise et reset reproductible sur TR2/ST-LINK CN1 `003500463235510B37333439`, STM32U575, Device ID `0x482`, Flash 2 Mio. MicroSD absente, selon confirmation utilisateur. Aucun mass erase/Option Bytes.
- Artefact réellement programmé : BIN 74 412 octets à `0x08000000`, SHA256 `5bb8d47bf370c99596b8e782611375f1ef086b121e11652618274925f376c0f1`. Référence de source ci-dessus. Rapport physique et sauvegardes Flash conservés hors Git dans le dossier de preuves T1000 du 10 octobre.
- **NON VÉRIFIÉ par ces missions** : qualification fonctionnelle complète TR2, stockage microSD/E4 power-loss, communication RS-485 réelle et debug source ligne par ligne. Les séquences E1/E4 peuvent écrire sur microSD au démarrage : toute mission physique future doit recontrôler les prérequis et autorisations.

## Environnement courant et mission de clôture

Clone actif migré dans Linux : `/home/lolo/dev/msm/projects/MSM_vibe`. SDK officiel CubeU5 v1.9.0 : `/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0`. Scripts et procédures T1000 dans `00_Environnement_developpement/T1000/`.

L'historique Git et les modifications locales ont été sauvegardés puis conservés à l'identique lors du déplacement ; comparaison des empreintes de 3 408 fichiers et `git fsck` réussis. Aucun nouveau flash/debug n'est nécessaire pour cette migration.

**VALIDÉ depuis la nouvelle architecture** : 102/102 tests firmware et cross-build ARM ; .NET restore/build sans erreur ni warning et 466/466 tests. Après arrêt/redémarrage WSL : 102/102 et 466/466 à nouveau, outils et authentifications opérationnels, binfmt actif et aucun service en échec. Codex natif charge réellement AGENTS.md, compte ChatGPT, défaut read-only/on-request inchangé. Git/gh utilisent un jeton vérifié dans le coffre Windows, retiré du fichier en clair ; helper Git local, aucun paramètre global modifié.

BIN et ELF identiques à la qualification physique précédente ; seule la MAP change avec les chemins. Aucun nouveau flash/debug. Copies anciennes archivées et vérifiées avant nettoyage ; dossiers personnels et preuves préservés. Réserves complémentaires non bloquantes selon décision utilisateur : contrôles Windows administrateur/sécurité séparés ; extraction installateur Windows encore verrouillée, conservée sans arrêt forcé.

**VALIDÉ publication** : correctif de test `dc6dac73bbbdd19ecf921f84808065f816a5bc9c`, première publication globale `61b1217fd8d0a337fdbc07445c056061a7e93c1c` sur main, push normal puis relecture Codex de 39 fichiers et contrôle API du rapport/journal. Git propre. Les commits locaux initialement refusés pour email privé restent sauvegardés hors publication ; identité locale passée à noreply sans désactiver la protection GitHub. Le rapport de clôture est mis à jour après cette preuve ; son SHA final sera communiqué après vérification de la seconde publication.

## Prochaine étape

Clôture technique T1000 : VALIDÉ AVEC RÉSERVES complémentaires non bloquantes. Le contrôle indépendant du rapport distant par ChatGPT n'est pas établi par la présente mission. Si l'original HP est retrouvé ultérieurement : relever sa provenance et son empreinte, comparer les faits et décisions à ce nouvel état, puis intégrer uniquement les éléments vérifiés sous mission autorisée ; ne pas remplacer silencieusement ce journal.

## Développement réception RTU — 10 octobre 2026

Mission précédente : `TR2-20261010-DEV-RS485-002`, sur `main`. HEAD évalué : `4591e13a37e99724c35e88fc03105e5e082c90b8` avec modifications locales non commitées. Contrat versionné `Echanges_Codex/MISSION_EN_COURS.md` ; commit/push/flash/debug interdits. Session Linux Debian dans le clone canonique, arbre initial propre ; mise à jour fast-forward depuis `36c33b7981c61376dae45586509327b899e5f3b8`.

- **Fait observé par lecture du code** : après démarrage UART, le harness ne consommait aucun événement série et ne faisait que clignoter avec `HAL_Delay(250U)`.
- **PRÉPARÉ** : `platform/stm32/main.c` consomme maintenant les événements dans la boucle principale, alimente le récepteur RTU portable existant, décode les ADU terminées et expose les compteurs `tr2_rtu_rx_*`. Les erreurs réinitialisent le récepteur. Le clignotement utilise `HAL_GetTick()` sans attente bloquante. Aucun traitement PDU, réponse, filtre d'adresse ou pilotage DE//RE n'est ajouté.
- **VALIDÉ cross-build uniquement** : `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only`, sortie réussie. Choix proportionné : modification limitée au harness, cœur et interfaces inchangés. Invocation directe refusée car script non exécutable ; usage `bash` documenté dans le script. Avertissements newlib au linkage concernant `_close`, `_lseek`, `_read`, `_write`. Pas de nouvelle campagne hôte.
- **NON VÉRIFIÉ** : exécution de cette nouvelle boucle sur cible, réception UART réelle, précision temporelle et liaison RS-485. Les compteurs de trames valides prouvent seulement le décodage ADU/CRC lorsqu'ils seront observés ; ils ne qualifient pas une requête PDU. Aucune opération matérielle effectuée.
- **Dette ouverte** : E4 microSD/power-loss inchangée. Prochaine action : revue de cette tranche locale par ChatGPT puis mission autorisée de qualification de réception ; la composition du serveur PDU de production et le pilotage du transceiver restent à traiter avec les prérequis documentés.

## Finalisation et publication — mission 003

Contrat de 003 (clôturé) : `TR2-20261010-DEV-RS485-003`, `target_branch: main`, commit/push autorisés, flash/debug interdits. HEAD initial `4591e13a37e99724c35e88fc03105e5e082c90b8`, mission distante récupérée au SHA `23e6de29ad4baa229987a9c972ef522b088a6791`. Les deux modifications locales de 002 ont été sauvegardées dans un stash conservé, puis réappliquées après fast-forward sur arbre propre ; comparaison au stash sans différence. Aucun code refait ou changement de spécification.

**VALIDÉ cross-build** : même commande `bash ./tr2_validate.sh --cross-build-only` avec le SDK Linux, relancée le 10 octobre 2026 en mission 003, code retour 0. Quatre avertissements newlib `_close`, `_lseek`, `_read`, `_write`, sans erreur de compilation/linkage. Diff inspecté et contrôle whitespace réussi. BIN SHA256 `dfc08e8c1e0bcd0da1cae0921215215e02289f17c2e0af5149656af70173ed60`. Pas de tests hôte relancés : seul le harness est modifié, sans modification du cœur ni des interfaces.

Publication et relecture distante consignées dans `Echanges_Codex/DERNIER_RAPPORT.md`, seul rapport de clôture de 003. **NON VÉRIFIÉ matériel** : réception, timing, bus RS-485 et réponses Modbus ; aucun flash/debug effectué. E4 reste ouverte. Prochaine action : contrôle indépendant du rapport publié par ChatGPT, puis mission autorisée pour la qualification de réception et la suite de l'intégration de production.

## Intégration applicative RTU — mission 004, 10 octobre 2026

Contrat 004 clôturé `TR2-20261010-DEV-RTU-INTEGRATION-004`, sur main ; commit/push autorisés, flash/debug interdits. HEAD initial propre `3f5706780b76707a3be339ac4fb424997d7e794b` ; mission récupérée par fast-forward au SHA `d04e580f8113b3a76f9122a64eb0106a4b1112c6`. Le stash historique 002 est conservé. Aucun changement de freeze, matériel ou travaux E4.

- **VALIDÉ logiciel hôte** : composition portable `ModbusSystemServer` reliant les autorités d'un SystemRuntime déjà booté au serveur RTU/PDU existant ; actualisation après acceptation CRC/adresse, B0 optionnel provisionné, projections runtime et staging B2/B4, inventaire B6, port explicite de soumission B5. Sans autorité d'écriture, exception 04 ; une anomalie préexistante renvoyant 02 à tort pour une autorité absente a été corrigée et couverte pour B2/B4/B5/B6.
- **Résultats réellement observés** : validation complète firmware finale via `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh` : 103/103 tests hôte et cross-build réussis. Test d'intégration nouveau : lectures, staging, invalidation workflow, soumission jusqu'à REFRESH_INDICATORS réel, CRC/adresse/broadcast, exception, défauts RX/TX sans replay automatique, readiness. Pas de tests supervision relancés. Quatre avertissements newlib de linkage conservés.
- **PRÉPARÉ STM32** : raccordement `main.c` → hook `stm32_modbus_application_bind` → init/start/poll du serveur applicatif. Le hook par défaut renvoie NOT_AVAILABLE, observable ; le firmware livré reste en diagnostic RX et n'émet pas de réponse applicative. Le runtime STM32 durable et le transport DE//RE qualifié doivent être fournis avant activation. Les instances temporaires du harness de qualification ne sont pas réutilisées comme runtime production. Dispatcher universel B5 non livré : port de soumission explicite, écritures indisponibles sans gestionnaire ; refus fonctionnel ID 0/code 14 et annulation relèvent de ce gestionnaire, pas d'une réussite simulée.
- **PRÉPARÉ essais** : `PLAN_ESSAI_RTU_INTEGRATION_004.md` précise les prérequis et commandes. Outil host offline `tr2_rtu_request` construit, génération/décodage et rejets contrôlés sans ouverture série. PG7/PG8 documentés ; PG5/PG4 seulement candidats DE//RE, domaine VDDIO2 à qualifier, transceiver réel à identifier (références historiques ADM2587E vs plan Click ADM2867E).
- **NON VÉRIFIÉ physique** : toute exécution de cette nouvelle composition sur STM32, réponse RS-485, timing, turnaround, débit. BIN SHA256 `4479d4e257273308b9dbe0a82da912e93a7d91918b5ec165d06f2a57541c3c77`. E4 microSD/power-loss reste ouverte. Publication et SHA du code relu : rapport Codex 004.

Prochaine étape à transmettre à ChatGPT : contrôler le rapport distant, puis traiter le raccordement runtime durable/autorités B5 et la décision matérielle DE//RE/VDDIO2/adresse/transceiver ; qualifier la réception/réponse sous une mission physique explicitement autorisée. Le périmètre portable utile est livré, pas un firmware industriel intégralement qualifié.


## Inventaire matériel RS-485 — correction confirmée par l'utilisateur, 10 octobre 2026

**RECTIFICATIF :** l'inventaire initial de ce jour inversait les références du composant installé sur MIKROE-3863. L'utilisateur confirme, après signalement de l'incohérence par Codex pendant la mission 005, que **le composant réellement monté sur MIKROE-3863 et installé sur le TR2 de test est ADM2867E, et NON ADM2587E**. Cette correction remplace l'affirmation erronée du précédent inventaire ; ne pas la traiter comme une nouvelle variante.

- **TR2 de test actuel :** ADM2867E sur carte MIKROE-3863, déclaré par l'utilisateur ; câblage et caractéristiques électriques encore à vérifier sur le montage réel.
- **Stock déclaré :** deux exemplaires ADM2867E au total (dont celui du TR2 actuel) et un exemplaire ADM2587E ; le second TR2 n'est pas encore qualifié. **Aucune carte support n'est attribuée ici à l'ADM2587E.**
- **Adaptateur USB–RS485 :** reçu mais jamais branché au PC ni au TR2 à la date de déclaration ; modèle, pilote et bornage non qualifiés.
- **Conséquence pour mission 005 déjà lancée :** privilégier les preuves constructeur et l'identification physique du couple ADM2867E / MIKROE-3863 ; l'ancienne mention ADM2587E / MIKROE-3863 dans la mission publiée est erronée. La mission READY n'est pas modifiée pendant son exécution ; Codex ayant identifié l'incohérence, le présent rectificatif fait foi pour les travaux suivants.
- **Limites :** inventaire déclaré, pas validation électrique. Ne rien déduire sans documentation sur DE, /RE, VDDIO2, niveaux, alimentation, isolement, terminaison, polarisation et bornes A/B. Aucune connexion ni flash implicite.


## Production RTU — mission 005, 10 octobre 2026

`TR2-20261010-DEV-RTU-PRODUCTION-005`, main ; commit/push autorisés, flash/debug interdits. HEAD initial propre `966737ee122fb4fb04489496691667f659607a23`, fast-forward de mission vers `ee95b8e3c0359f79cd6d208aa2ce0e9fae3d5b2b` ; base `ff19cf40a017233b967b95fea3365bad0f631e11`. Correction documentaire distante `463d69a8a6a2a36fe8a88f2905c6cad3ff5c3fac` intégrée après sauvegarde 005, sans merge/rebase ; code restauré octet pour octet. Stash 002 conservé.

- **VALIDÉ hôte** : propriétaire durable `ProductionApplication` (runtime et descripteurs, dispatcher, staging/workflow/adapter de configuration optionnels), boot unique/readiness, vrais services/journal borné. B5 raccordé aux exécuteurs 1/2 et runtime 3..10, validation/admission/refus persistants/retry/collision ; ID0 → code14 sans identité persistante, annulation consommée → code15 sans effet métier, last durable préservé. SELFTEST/RESET sans capacités et RAZ statistiques sans autorité → 04 avant réservation. Configuration CRC invalide → code4 ; source SYNC Modbus = 1. Aucun succès fictif.
- **VALIDÉ hôte / COMPILÉ STM32** : validation complète finale `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh`, code retour 0, 104/104 tests et cross-build. Nouvel essai d'intégration : RTU/PDU réels, configuration activée dans ConfigurationStore, reboot/restitution de refus, CRC invalide, ID0, annulation/concurrence, capacités absentes et injections de défauts RESERVED/COMPLETED. Journal en recovery_required après défaut : refus de publier une réussite, pas de retry automatique. Quatre warnings newlib usuels. Pas de tests .NET.
- **PRÉPARÉ STM32** : propriétaire statique indépendant du harness, adapters FRAM transactionnel/IIS3DWB/horloges/reset/continuité et boot réel compilés. Pas de format implicite ; EMPTY refuse. `stm32_production_binding_acquire` refuse par défaut avant préparation : firmware toujours diagnostic RX. Transport DE//RE qualifié, backend campagnes microSD durable/allocation, géométrie provisionnée, adresse/B0 et métadonnées de configuration restent prérequis de carte. Pas de recyclage des fixtures E3/E4.
- **Écart documentaire établi, matériel NON VÉRIFIÉ** : [MIKROE-3863 constructeur](https://www.mikroe.com/rs485-isolator-2-click) et schéma v102 désignent ADM2867E, contrairement à l'inventaire utilisateur ADM2587E. Écart levé par le rectificatif utilisateur publié au SHA `463d69a` : ADM2867E / MIKROE-3863 actuel, deux ADM2867E au total et un ADM2587E sans carte support identifiée. Vérifier néanmoins révision, U1 et câblage réels avant connexion. PG4/PG5 restent candidats, VDDIO2 et tous niveaux/jumpers/masses/terminaison/bias/A-B non qualifiés. Aucun pilotage direction activé.
- **PRÉPARÉ essais** : `PREPARATION_RTU_PRODUCTION_005.md` fournit matrice de preuves, identification/ouverture sans émission du câble sur PC seul sous mission future, jalon électrique séparé puis futur essai deux TR2. Câble et second TR2 non qualifiés ; aucun geste physique en 005. E4 microSD/power-loss reste ouverte.

À transmettre à ChatGPT : contrôler le rapport distant 005 ; contrôler révision et câblage après le rectificatif d’identité ; qualifier câble sur PC seul, puis montage électrique/transport et autorités de production sous missions adaptées. Rapport publié et qualification physique restent distincts.


## Préparation du premier essai RX — mission 006, 11 octobre 2026

`TR2-20261011-QUALIF-RS485-RX-006`, main, base `9753b3d6b46243f0472a0d33c5fa82b509dcea71`, contrat récupéré au SHA `c8e2b5c3f112a272912cf260c6c8a94d148ab418`, HEAD initial propre `9753b3d6b46243f0472a0d33c5fa82b509dcea71`. Commit/push autorisés ; flash/debug/émission interdits. Stashes 005 et 002 préservés.

- **Observations rapportées par l'utilisateur dans le contrat**, non réalisées par Codex : ST-LINK attaché WSL, OpenOCD détecte STLINK V3J15M7 / STM32U5 / 3,288 V ; HP FT232R COM7 ; câblage blanc A+→A, vert B−→B, noir GND→GND, rouge +5 V isolé ; alimentation USB ST-LINK ; hors tension A↔Y et B↔Z en continuité, A/B ≈110 Ω. Ces mesures n'établissent ni niveaux, ni direction, ni RX ; aucune demande de recâblage.
- **PRÉPARÉ** : instrumentation du harness RX uniquement : ready, progression polling, compteur CRC distinct, dernière ADU complète en RAM ; compteurs précédents conservés. Macro GDB à casts explicites, chargée hors cible avec succès. Aucun changement du cœur, UART, binding production ou DE//RE.
- **COMPILÉ STM32** : `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only`, retour 0. text157692 / data124 / bss129072. BIN SHA256 `f9356f279d45cb411bc5b98107e0fd291e2d1ab903c216ff052083b7abfd103d`, 157816 octets ; ELF `5ac5f0c66645692ff78579d792227ceb834f331baef9b8ed3a64d0ea55c293c3`. Symboles et tailles confirmés par nm ; aucune preuve du firmware chargé.
- **VALIDÉ hôte ciblé** : 2/2 tests existants codec et p12d_rtu_receiver réussis ; générateur/codec de production, CRC indépendant et décodage confirment `01 03 03 E8 00 01 04 7A` (FC03, B1 registre1000, quantité1, unité1 marqueur). Script Windows hors ligne par défaut, un seul envoi binaire avec switch explicite. Analyse syntaxique PowerShell réussie ; exécution complète refusée par politique Windows locale, **NON VÉRIFIÉE**, aucune modification/contournement de politique. Test hors ligne sur HP requis avant émission.
- **PRÉPARÉ essai / NON QUALIFIÉ physique** : `PREPARATION_RS485_RX_006.md` donne identification Flash par lecture/comparaison au BIN, séquence AVANT→un envoi HP→APRÈS, deltas et ADU exacte, diagnostic différentiel. **Non prêt pour essai immédiat** : autorisation hardware distincte, firmware identifié, niveaux VIO/VDDIO2 et DE=0 /RE=0 établis, contexte microSD contrôlé avant tout boot et exécution hors ligne HP restent nécessaires. L'absence d'EnableVddIO2 explicite est documentée, aucun registre ni GPIO écrit. GDB/OpenOCD WSL cible non qualifié par le seul test de détection SWD utilisateur.

À transmettre à ChatGPT : relire le rapport 006 puis cadrer une mission physique avec prérequis/permissions explicites ; si VDDIO2 ou direction doivent être corrigés, autoriser la tranche correspondante. Aucun serveur/TX RS-485 ou runtime de production activé, freezes inchangés, dette E4 microSD/power-loss ouverte.


## Diagnostic VDDIO2 / RS-485 — mission 007, 11 octobre 2026

`TR2-20261011-DIAG-VDDIO2-RS485-007`, main ; HEAD initial propre `4a0858725e6e562167ee585622f6e5e7687d2ab1`, contrat récupéré par fast-forward `19aff5eebc4437cbabf2cb40bfdc7a3c56e9c464`. Commit/push autorisés ; flash/debug/émission interdits ; stashes005/002 conservés.

- **Observations nouvelles rapportées par utilisateur** : microSD vide ; OpenOCD/GDB WSL attach/halt/lecture réussis, cible3,288416 V. Flash157816 octets SHA256 `8869e369664da06a707d8d73633fd9fcf1e7940e985b963004c9da73d7b0f480`, différente du BIN006 ; firmware chargé non identifié, ne pas utiliser ses symboles pour lire RAM. `/tmp/tr2-rx006-loaded.bin` retrouvé et hashé **hors cible** par Codex confirme taille/empreinte du relevé, sans nouvelle lecture MCU. Reprise/detach/quit invités mais non confirmés : état CPU/serveur **NON VÉRIFIÉ**.
- **Analyse logicielle établie** : ST AN5373 + datasheetU575 + HAL/CMSIS CubeU5v1.9.0 : PG7/PG8 AF8 et domainePG2..15, IO2SV bit29 commande levée d'isolation une fois VDDIO2 présente ; ce n'est pas une mesure. IO2VM seuil≈0,9 V ne prouve pas rail3V3. SVMCR NS0x46020810, contexte/protections/horloge à établir. Utilisateur a lu0 ; isolation non levée si lecture pertinente, sans lien démontré au code006. Init locale active PWR mais ne lève pas IO2SV : incomplète pour RX surPG7/8 depuis reset. PG5 AF8=CTS, PG6 AF8=RTS/DE ; PG4/PG5 restent candidats GPIO seulement.
- **Décision / PRÉPARÉ** : `DIAGNOSTIC_VDDIO2_RS485_007.md` fournit sources officielles, mesures localisées VDDIO2 SB28/SB29, VIO/VCC et DE//RE/RXD àU1 avec références logiques, et procédure de décision pour EnableVddIO2 avant AF. **Aucun patch actif** : alimentation/états sûrs au boot non démontrés, correction inconditionnelle non sûre. Aucune lecture0 transformée en absence de tension ; aucune broche de direction supposée câblée. RM0456 inaccessible via web/téléchargement(timeout), aucune citation de contenu non lu ; schémaClick textuel ne démontre pas toute liaison graphique VIO.
- **COMPILÉ STM32 / VALIDÉ logiciel ciblé** : cross-build007 `STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh --cross-build-only`, retour0 ; 2/2 tests existants codec/p12d_rtu_receiver réussis. Sources firmware inchangées, BIN/ELF identiques006, respectivement `f9356f279d45cb411bc5b98107e0fd291e2d1ab903c216ff052083b7abfd103d` / `5ac5f0c66645692ff78579d792227ceb834f331baef9b8ed3a64d0ea55c293c3`. Compiler ce BIN ne corrige pas le défaut IO2SV. Aucun test hôte complet/.NET supplémentaire nécessaire.
- **NON QUALIFIÉ électrique / RX, essai NON PRÊT** : prochaine séance d'abord état session confirmé, puis relevé électrique unique ; décision sur correctif, nouvelle validation et éventuel flash sous permission distincte/chaîne ST qualifiée ; identification exacte et IO2SV/RXready/polling avant une seule émissionCOM7. La voie WSL observée n'est pas qualifiée pour flash/reprise/reset ; propriété ST-LINK Windows/WSL à gérer explicitement. Absence microSD actuelle acquise, pas de demande de retrait inutile. Production/TX non activés et E4 ouverte.

À transmettre à ChatGPT : contrôler le rapport007 ; cadrer la séance humaine minimale et autoriser la correction d'init seulement après preuves électriques/direction/boot, puis flash/debug explicitement. Mission007 conclut l'analyse/préparation, pas la qualification du banc.

## Mission008 — bring-up logiciel RX, 11 octobre 2026

- **Observations utilisateur** : SB28 fermé et 3,31 V aux deux extrémités ; SB29 ouvert. DE câblé PG4, /RE et PG5 sans fil ; pont J2 soudé rapporté, révision/continuité non vérifiées. PG4 0,88 V au repos ; flottement hypothétique. GDB désormais fermé selon utilisateur, OpenOCD potentiellement actif ; microSD absente. Flash courant toujours non identifié, différent du BIN006.
- **Lecture documentaire vérifiée** : schéma officiel Click v102, J2 = DE–/RE, J3 = /RE–RST ; table fabricant nomme JP2 le contrôle commun. VIO reliée à VCC sélectionnée dans v102 (nouvelle preuve graphique, sans réécriture des conclusions historiques007). Carte réelle non identifiée ; tension VIO Click non mesurée.
- **VALIDÉ logiciel ciblé / COMPILÉ STM32** : PG4 latch bas puis output dès HAL_MspInit, ensuite IO2SV, puis plus tard PG7/PG8 AF8 UART. Aucun PG5, DE haut ou TX fonctionnel ajouté. Tests ciblés 3/3 (GPIO réel sur HAL simulée, codec, récepteur), cross-build-only réussi ; ordre ARM vérifié par objdump. Text157820/data124/bss129072. BIN157944 octets SHA256 `02ed2cf64d4d78225a4781b823c9d27e96a8ea693679d8a834d230568383593b` ; ELF `6546629b85dba6ba5ead029d208d9a95e67a13e0e244d477d01828b07ab4c23e`.
- **PRÉPARÉ / NON FLASHÉ / NON QUALIFIÉ RX** : procédure courte dans `BRINGUP_RS485_RX_008.md`. Avant essai unique HP : identifier pont/révision et /RE bas, vérifier VIO~3,31 V, propriété ST-LINK, autorisations distinctes flash/debug et identité Flash008. IO2SV concerne PG2..15 ; transitoire reset avant prise en charge PG4 non garanti par le pull-down logiciel. Aucune opération matérielle008. Freezes/interfaces et E4 inchangés.

## Mission009 — visibilité ST-LINK, 11 octobre2026

- **VALIDÉ hôte** : sonde SN003500463235510B37333439 présente sur USB0483:374e bus2-3, attachée WSL via usbipd5.3.0 ; aucun OpenOCD/GDB actif détecté. CubeProgrammer Windows2.23.0 inventaire `-l st-link-only` : aucune sonde avant détachement. Détachement ciblé réussi sans administrateur ni geste physique ; état Shared, disparition lsusb, inventaire Windows détecte SN attendu/V3J15M7/NUCLEO-U575ZI-Q. Sonde laissée à Windows, liaison partagée persistante conservée.
- **PRÉPARÉ** : `PREPARATION_STLINK_009.md`, sauvegarde2Mio HOTPLUG et flash/vérification BIN008 sous mission cible ultérieure. Empreinte BIN008 recontrôlée identique. Aucune connexion MCU, sauvegarde nouvelle, flash/debug/reset/émission effectuée. Firmware actuellement en Flash reste non identifié ; RX/E4 restent NON QUALIFIÉS.
- À ChatGPT : revue distante009 puis contrat séparé pour accès cible, flash et essai unique avec prérequis008. Aucun copier-coller ou intervention utilisateur nécessaire au diagnostic résolu.

## Mission010 — flash008 et diagnostic, 11 octobre2026

- **BACKUP / FLASH / VERIFY RÉUSSIS** : cible SN003500463235510B37333439 STM32U575 ID0x482 RevU,3,29V. Backup2Mio `/home/lolo/dev/msm/backups/TR2-mission010-20261011T125638Z/flash-before.bin`,SHA256 `4e49d49ac35d0e2db184ae8dc66dac5ac62b25a89d197a3749beaff12a8e6153`. BIN008157944 octets programmé0x08000000, effacement standard secteurs0..19, verifyCLI réussi puis relecture byte-à-byte identique SHA256 `02ed2cf64d4d78225a4781b823c9d27e96a8ea693679d8a834d230568383593b`. Reset/démarrage contrôlé. ELF008 exact vérifié ; identité firmware courant désormais008 établie.
- **OBSERVÉ DEBUG** : IO2SVactif0x20000000 ; PG4modeoutput/latchbas/IDRbas/pulldown ; PG7/8AF ; RXready1, polling2545194→15458093 après reprise ; compteursRX/CRC/UART0 sans émission. Binding3NOT_AVAILABLE, SDstage5 sans carte, faultregs0. Sessions STWindows détachées, serveurs terminés ; cible laissée en exécution. Niveaux physiques U1DE//RE/VIO et pont réel non mesurés.
- **BLOCKED RX** : seul COM3STLINK accessible T1000, COM7FT232R sur HP inaccessible ; aucun envoi effectué. Action HP unique après niveaux/J2 contrôlés et script offline réussi : `Send-Tr2RxProbe.ps1 -Transmit`, communiquer résultat pour snapshotAPRÈS par Codex. Aucun retry automatique. RXphysique/E4 NON QUALIFIÉS ; rapport010 publié pour revue indépendante ChatGPT.

## Mission011 — premier essai RX observé, 11 octobre2026

- **Utilisateur, contrat011** : envoi HP unique `01 03 03 E8 00 01 04 7A` COM7 1152008E1, CRC/self-checkOK, aucune réponse~1s (attendu RX-only). Aucun nouvel envoi demandé/effectué.
- **VALIDÉ PHYSIQUE POUR CET ESSAI RX UNIQUE** : relevé debug APRÈS, ready1, octets8/trames1/ADUvalides1 contre0/0/0 en010 ; erreursUART/CRC/invalides0, lastlength8/decode0/ADUexacte. Poll15458093→386978316 ; Flash008 relue157944octets identiqueBIN, ELF008 exact. IO2SVactif, PG4latch/entrée digitale bas, PG7/8AF8. Sessions détachées, serveur terminé, firmware laissé fonctionner. Preuves `/home/lolo/dev/msm/reports/TR2-mission011-20261011T131102Z`.
- **NON QUALIFIÉ** : électrique globale/reset, TX/PDU/production, endurance et E4. Blocage RX010 levé par cette observation, rapport historique010 conservé inchangé ; réception réussie n'est pas validation des niveaux physiques U1/J2. Prochaine étape : revue indépendante ChatGPT et mission cadrée du jalon suivant.

## Mission012 — tranche transport réponse RTU, 11 octobre2026

- **IMPLEMENTATION / HOST VALIDÉS, STM32 COMPILÉ** : vrai serveur applicatifV1 vers adaptateurSTM32 testé sur HALsimulée, gateTXOFF par défaut ; PG4DEhaut seulement réponse aprèsT3.5/filevide/RXNE-BUSYabsents, DEbas surTC réel puisRX/garde1750µs ; watchdogTIM6 50ms, erreurs→DEbas/faultlatched sansretry. Aucun PG5/mapping/float/runtimefabriqué ajouté. Validation complète107/107 et ARMdefault réussis ; brancheON compilée séparément, dernières extensions de tests2/2 réussies.
- **FLASH NON EFFECTUÉ / TXPHYSIQUE BLOQUÉ** : BIN008 reste sur carte, acquisRX011 non refait. ÉtatJ2/révision/VIOClick pourTX non établi ; direction communeADM2867Edocumentée mais niveaux/turnaround réels non mesurés. Bindingproductionfaible NOT_AVAILABLE conservé : adresse/B0/dépendancesFRAM/CampaignDataStore réelles toujours nécessaires. OptionTXON ne résout pas ces prérequis.
- **PRÉPARÉ** : `INTEGRATION_RTU_TX_012.md`, audit, preuves et jalonphysique minimal seulement pour faitsTXabsents, sans redemander acquisRX/SB28/câblage. GarantieDEbasreset avantinit et watchdogIRQ restent limites matérielles explicites ; E4 inchangée. Artefacts008 sauvegardés horsGit avant rebuild, ELF012 ne correspond pas au firmware sur carte.

## Mission M013-A — preflight TX et diagnostic isolé, 11 octobre2026

- **VALIDÉ logiciel / COMPILÉ STM32** : diagnostic de banc OFF par défaut, adresse explicite1..247 requise, TX reste OFF par défaut. Branche avant SPI/capteur/FRAM/SD/boot production ; seule lecture normale FC03/1000/quantité1 retourne V1 UNKNOWN=0, initialisation incomplète réelle. Autres données indisponibles→04, aucune identité/READY/température inventée, autorités d'écriture absentes. Callback RTU optionnel de disponibilité par requête, serveur/CRC/mapping V1 conservés. Validation complète108/108 + ARM défaut ; tests finaux108/108, candidats diagnosticON TXOFF/TXON unité1 compilés, jamais flashés.
- **NO-GO TX PHYSIQUE / NON VÉRIFIÉ** : matrice `PREFLIGHT_RTU_TX_M013_A.md` ; VIO Click non mesurée, route /RE–J3–RST concurrente/inconnue. Acquis3,31V MCU/SB28/SB29/J2 soudé/RX011 conservés sans demande de répétition. Un seul relevé physique ciblé demandé : VIO/DE//RE au repos contreGND1 et inspection RST/J3/révision. La continuité réelle et les transitions restent à établir si ambiguës ; reset pré-init et watchdogIRQ ne sont pas garanties matérielles.
- **PRÉPARÉ / NON EXÉCUTÉ** : future requête unique unité1 `01 03 03 E8 00 01 04 7A`, réponse diagnostic attendue `01 03 02 00 00 B8 44`. Ce marqueur de banc ne provisionne pas l'adresse production. Aucun flash/debug/reset/USB/émission013 ; dernière identité sur carte BIN008 établie011, ELF008 préservé/re-hashé. Production reste NOT_AVAILABLE, FRAM non formatée ; E4 et freezes inchangés. Archives012 exactes préservées.
- À ChatGPT : contrôler publication/code, recueillir le relevé unique puis cadrer l'essai physique si préflight sûr ; ne pas déclarer TX/production qualifiés sur ces tests hôte.

## Mission M013-B — nouveaux acquis statiques, 11 octobre2026

- **OBSERVATIONS UTILISATEUR dans le contrat** : Click MIKROE-3863 rev1.02/ADM2867E ; J1 position3,3V, J2/J3 fermés ; VIO3,27V contre masse logique ; DE0V, /RE0V. Aucune liaison RST vers NUCLEO réalisée selon utilisateur ; point RST non identifié. Acquis enregistrés sans répétition des mesures ni nouvel essai RX.
- **TOPOLOGIE DOCUMENTAIRE ÉTABLIE / EXTERNE NON VÉRIFIÉE** : le contact RST mikroBUS2 est la broche RE du Click, voisine de DE/CS3 ; J3 fermé le relie à /RE, commun à DE par J2. Aucun second étage de sortie sur ce net dans schéma v102 ; une rangée/support externe n'est pas inspectable depuis T1000. **NO-GO TX en attente d'une seule clarification sur cette route externe**, question précise dans actualisation `PREFLIGHT_RTU_TX_M013_A.md` et rapport M013-B. Pas de dessoudage ou recâblage demandé.
- **BUILD/TEST RÉUSSIS / FLASH NON EFFECTUÉ / TX NON VÉRIFIÉ** : code M013-A inchangé, candidat diagnosticON/TXON/unité1 reconstruit sur main, BIN/ELF identiques à M013-A ; trois tests ciblés réussis. Aucun accès cible, backup nouveau, flash/debug/reset/émission ; dernière image carte identifiée011 reste008. Production, E4, freezes et stashes conservés ; archives M013-A exactes.
- À ChatGPT : VIO et niveaux statiques acquis, blocage réduit à route externe RE/RST ; ne pas assimiler réponse attendue/tests hôte à validation TX. Après preuve GO, sauvegarde/flash/readback/ELF puis essai HP unique et observation dynamique restent à réaliser.

## Mission M013-C — diagnostic flashé et prêt pour l'unique probe HP, 11 octobre2026

- **GO STATIQUE, observation utilisateur** : RE/RST position2 libre de toute connexion externe, y compris support/breakout ; aucun autre pilote que PG4. Blocage spécifique M013-B levé sans répétition. VIO3,27V/J1=3,3V/J2J3fermés/DE=/RE=0 acquis, schéma v102 cohérent. Pas de qualification des transitoires/reset.
- **BUILD ET TESTS CIBLÉS RÉUSSIS** : code M013-A inchangé, HEAD évalué `fe1b9cf274d2fd51b82081e0a20c5e99dc55728c`, candidat diagnosticON/TXON/unité1 reconstruit, 3/3 tests. BIN48240 SHA256 `47d69cc286b0f12771de56eb80f2648cc3c2a7a57b2ce1a8038b34de792b97e8`, ELF `762dcf0ba6c3663d9737dd334118a9f7d52a22175dda6d2674e9cb0d7b8e1c4a`. Aucun accès médias/boot production ; UNKNOWN0 reste diagnostic.
- **BACKUP/FLASH/VERIFY/READBACK RÉUSSIS** : STLINK SN003500463235510B37333439, NUCLEO-U575ZI-Q ID0x482 RevU/3,29V. Backup2MiB vérifié par deuxième lecture identique, SHA256 `67d505e6f03d52a868d2f87b8e0a6f3a9b209cd858f06dec8ee3087b8de31740`; image008 initiale reconnue. Flash standard0x08000000, secteurs0..5 seulement, verify et reset réussis. Readback2MiB SHA256 `be56e0699ea196329c7c3a1bca98743fbdeeb7ce8e7a7a5d370fb0c37c7a83de`, candidat byte-à-byte exact et reste après0x0800C000 inchangé. Preuves `/home/lolo/dev/msm/reports/TR2-mission013-C-20261011T140835Z`.
- **BOOT OBSERVÉ DEBUG** : ready1/poll_result0/erreurs0, uwTick71285→75800, IO2SVactif, PG4bas, UARTerrorbits0, timerIDLE/tx_busy/fault0/filevide/bufferTX0. Aucun TX spontané observé dans relevés ; pas de capture continue DE/bus. ELF exact utilisé, sessions détachées/serveurs terminés/cible en exécution. Aucun mass erase/OptionBytes/formatage.
- **TX PHYSIQUE NON VÉRIFIÉ / attente utilisateur HP** : une seule exécution du script existant `Send-Tr2RxProbe.ps1 -Transmit`, seul maître HP prévu actif, COM7 1152008E1, requête010303E80001047A/réponseattendue0103020000B844. Transmettre sortie réelle sans retry ; CRC complet attendu0, éventuel écho à distinguer. DE jusqu'àTC/retourRX à observer si instrumentation déjà accessible ; un snapshot final ne prouve pas les fronts. Aucun envoi T1000/COM3 effectué. J1 global, production, reset, endurance et E4 NON QUALIFIÉS.
- À ChatGPT : vérifier rapport distant C ; blocageRE/RST levé, nouveau firmware réellement chargé prêt ; attendre capture HP pour conclure sur le premier dialogue. Archives M013-B et deux stashes conservés, aucune spécification gelée modifiée.

## Reprise M013-C après l'unique probe HP, 11 octobre2026

- **OBSERVATION UTILISATEUR** : COM7 1152008E1, requête010303E80001047A CRC/self-checkOK, aucune réponse~1s ; un seul essai, aucun retry. Instruction précédente consommée ; pas de nouvelle transmission HP dans cette reprise.
- **IDENTITÉ/DEBUG APRÈS OBSERVÉS** : mêmeBINdiagnostic48240 octets relu exact SHA47d69cc2…, ELF762dcf0b… ; aucune programmation/reset. Ready1/poll0/pollerrors0, uwTick737253→851024→913436, PG4bas/IO2SVactif/PG7-8AF8/faultregs0. UARTISR0x006000F8 **RXNE/ORE actifs** (avantD0), HALBUSY_RX/count1/RxISRcorrect/ErrorCode0 ; file64slots/head/tail0, bufferRTU/Réponses/TXentièrement0, phaseTIM6IDLE. Aucune ADU exacte/CRCaccepté/essaiTX démontrés ; défaut observable en amont RTU/TX.
- **PISTE IRQ / CAUSE NON ÉTABLIE** : vecteurs UART66/TIM649 corrects, PRIMASK/BASEPRI/FAULTMASK0 ; code et désassemblage activent bienIRQ. ISER0..2 lus0, ISPR/ICPRFFFFFFFF viaGDB ; CubeProgrammerHOTPLUG confirme ISER0/ISPRFFFFFFFF, mais mêmechaîneSWD. Visibilité/attributionSCS ou activationeffective à établir, pas de conclusion automatique toutesIRQpendantes ou fauteélectrique. Aucun RDR/ICR/NVIC écrit/consommé, état conservé. Preuves `/home/lolo/dev/msm/reports/TR2-mission013-C-after-20261011T142134Z`, dumpRAM/scripts/logs/layoutsDWARF horsGit, sessions détachées/serveurs terminés.
- **BLOCKED premier dialogue / TX PHYSIQUE NON VÉRIFIÉ** : à ChatGPT, revoir cet écart ; prochaine tranche instrumentationpassive RX/IRQ/HAL/RTU/NVICdepuisCPU, sans émission, puis correction si cause établie. Aucun nouvel essai HP demandé. VIO/J2/J3/RE-RST/acquisRX011 conservés ; timingsDE/TC/retourRX/reset/production/E4 toujours nonqualifiés. Pas de nouveau code/testfirmware, stashes/archives intacts.

## M013-D — 11 octobre 2026 : cause NVIC reproduite, accès corrigé

- **VALIDÉ diagnostic opérationnel** : CubeProgrammer Windows2.23.0 HOTPLUG désactive effectivement les IRQ (ISER0/ISPRFFFFFFFF), constat CPU et SWD. Deux sessions GDB seules préservent l'état ; lecture CLI isolée reproduit ; reset GDB restaure IRQ49/66. TrustZone désactivé selon options lues seulement ; aucune modification OptionBytes. Sous-mécanisme interne ST non isolé.
- **VALIDÉ logiciel** : instrumentation RAM passive IRQ/HAL/file/RTU/TX, gate défautOFF réservée au diagnostic isolé ; code09792c8dba98cd3959458f3148d4fae63a9584c9. Tests ciblés5/5 (observerOFF/ON × TXOFF/ON et diagnostic), cross-build requis réussi. Cœur portable/V1 inchangés.
- **OBSERVÉ sur cible** : image diagnostic unité1/observerON/**TXOFF**, BIN47812 SHA256 f9efbdd7dce624616402f4c957878651aa971df0d87f797df4689f9f3e7f1f1e ; flash/readback exacts. Après clôture GDB : ready1/polling progressant, ISER1=20000/ISER2=4, ISPR0, UART sans RXNE/ORE, PG4 bas. Compteurs trafic0, aucun nouveau probe/TX. MCU laissé en exécution.
- **PRÉSERVÉ** : 177 fichiers C copiés et vérifiés avant reset/flash, backup courant2MiB et RAM768KiB, RAM avant chaque reset. Preuves /home/lolo/dev/msm/reports/TR2-mission013-D-20261011T143352Z ; archives C sans réécriture historique.
- **DÉCISION procédure** : finir tous accès CLI avant reset GDB et contrôle NVIC CPU/SWD ; ensuite accès GDB seul. Readback GDB exact conserve NVIC. Ne pas reconnecter CubeProgrammer avant futur probe autorisé. Pas de réactivation IRQ artificielle en firmware.
- **NON VALIDÉ** : dialogue Modbus/TX physique, réception réelle sous D, chronologieDE/TC/retourRX, J1 global/production/E4. Essai C sans réponse rapporté par utilisateur reste unique et non répété ; sa requête exacte reçue n'est pas démontrée.
- **PROCHAINE MISSION ChatGPT** : autoriser séparément candidat TXON/observerON et essai HP unique instrumenté après image/NVIC/RX/polling sains. Aucun essai demandé ici, aucun contrôle physique acquis à répéter. D satisfait ses critères de diagnostic, pas la qualification bidirectionnelle.

## M013-E — 11 octobre 2026 : GO préparation, observerON/TXON

- **VALIDÉ logiciel** : code inchangé depuis09792c8, HEAD réel évalué0bfaa0e0daf3d37f723497b74984cabe1b11ec47. Candidat diagnosticON/observerON/TXON/unité1 compilé, tests ciblés5/5 réussis. BIN49200 SHA256 dc6ac75f667e10f0a4d04a7e92f82099f450de3a759677bc81c166f93c82d540 ; ELF4c496149b7b3af593269dbff7c92368d575cebe824f62ffa67ed57e522d3adc2. Pas de boot production/médias, FC03/1000/1 UNKNOWN0 seul diagnostic disponible.
- **BACKUP/FLASH/READBACK OBSERVÉS** : backup courant2MiB double lecture identique image D ; RAM768KiB conservée avant flash puis reset final. Flash0x08000000/verify, secteurs0..6 standard seulement ; readback exact, reste après0x0800E000 inchangé. Preuves /home/lolo/dev/msm/reports/TR2-mission013-E-20261011T160141Z ; D et stashes préservés.
- **GO ÉTAT CIBLE** : dernier CLI suivi de dumpRAM/resetGDB/detach ; ensuite GDB seul. CPU/SWD ISER1=20000/ISER2=4, ISPR0, RXBUSY_RX/count1 sans RXNE/ORE/error, ready1/poll0/erreurs0, tick4389→31167 et poll1496571→10627242 ; PG4bas/TXidle/fault0. Compteurs trafic0. Cible laissée en exécution, sessions terminées. Aucun accès CLI MCU à refaire avant essai.
- **PRÉPARÉ, PAS EFFECTUÉ** : après lecture GO du rapport E par ChatGPT, utilisateur lance une seule fois Send-Tr2RxProbe.ps1 -Transmit sur HP COM7/1152008E1, transmet sortie complète sans retry ; requête010303E80001047A, réponseattendue0103020000B844. Nouvelle tranche E, essai C consommé non répété. Aucun halt pendant échange et aucune émission Codex.
- **NON VALIDÉ** : réponse physique/Modbus bidirectionnel, DE/TC/retourRX, J1global/production/E4/reset/watchdogIRQ. Mission séparée après retour utilisateur pour analyser capture/CRC et compteurs. AcquisRE-RST/VIO/SB28/câblage inchangés, aucun contrôle physique répété.

## M013-F — 11 octobre 2026 : première réponse corroborée, essai E consommé

- **OBSERVATION UTILISATEUR** : unique probe E HP COM7/1152008E1, requête010303E80001047A ; sortie PC0103020000B844 (may be echo). CRC vérifié indépendamment : corps0x44B8/résidu0, FC03/unité1/UNKNOWN0 ; séquence7octets différente de la requête8octets, écho brut exclu pour cette sortie.
- **OBSERVÉ STM32** : image E TXON/observerON exact49200octets relus via GDB seul SHAdc6ac75f… ; RXcallback/octet8, IRQ UART17/TIM6=3, frame/CRCvalid/accepted1, erreur/CRCbad/drop0 ; ADU exacte requête et réponseRAM exacte7octets. TXattempt/started/callbackTC1, failsafe0. RetourRX armé, PG4bas/TXidle, NVIC CPU/SWD sain ; polling187193205→202036798/tick548991→592523 entre les deux sessions détachées.
- **DÉDUCTION FORTE POUR CET ESSAI** : aller-retour diagnostic MCU→HP corroboré par chaîne instrumentée, origine STM32 raisonnablement établie. Script dépôt ne transforme pas l'écho ; copie HP non hachée, source concurrente/proxy théoriquement non exclue sans capture bus. Pas de qualification définitive générale TX/bidirectionnelle.
- **PRÉSERVÉ / SANS NOUVELLE ACTION** : 181 fichiers E rehashés inchangés, RAM768KiB et relevés F dans /home/lolo/dev/msm/reports/TR2-mission013-F-20261011T161246Z, archives E exactes. Deux lectures GDB avec detach, zéro CLI HOTPLUG/reset/flash/nouveauprobe/émission/geste physique, stashes005/002 conservés. Cible running.
- **NON QUALIFIÉ / SUITE ChatGPT** : DE/dernierstopbit/TC/retourRX temporels, latence/robustesse/endurance/watchdogIRQ/reset/J1global/production/E4. Préparer mission séparée capture temporelle puis robustesse cadrée ; aucun essai demandé F, instruction E -Transmit consommée et non à répéter.

## M013-G — 11 octobre 2026 : préparation temporelle/robustesse hors bus

- **READY outillage HP hors bus** : rtu_campaign.py OFFLINE défaut, mock isolé et JSONLbrut, campagne fixe12max/10nominales/CRCincorrect1/unitéétrangère2 unefois, observation1s+100ms, arrêt anomalies/échos sansretry/purge. Tests10/10 et mock78événements réussis ; commitcode 158feddfa99dd5fff71aec4902cd59e3095124f0. Python/pySerialHP et branchephysique NON TESTÉS, pas GOtransmission actuel. Script E historique inchangé.
- **WAITING instrumentation physique identifiée** : aucun oscilloscope/analyseurlogique/sondedifférentielle/canaux disponible documenté ; absence physique non déduite. QUALIFICATION_TEMPORELLE_RTU_M013_G.md définit acquisitionsDE/TXD/RXD/A–B, isolement/masses sûres, incertitude, t1.5/t3.5 et critèresdebanc proposés, retourRX/limitesWindowsUSB. Aucun branchement demandé.
- **PRÉSERVÉ / NON MODIFIÉ** : imageE/observers existants suffisants pour prochaine tranche, preuvesE/F/backup/stashes préservés et archivageF exact. Aucun accèsMCU/COM7/émission/reset/flash/debug/CLI/physique en G ; état sain F reste historique, pas nouveau relevé G. V1/firmware/production/médias inchangés.
- **NON VALIDÉ / SUITE ChatGPT** : aucune mesureDE/TC/retourRX ni robustesse physique nouvelle, qualificationgénérale/J1global/production/E4 inchangées. Revue plan/outil puis mission limitée HP après environnementhorsport confirmé et/ou captures après identificationinstrument. EssaiE consommé ; ne pas relancer son instruction, aucune campagne physique autorisée G.

## M013-H — 11 octobre 2026 : NO-GO campagne, préflight HP préparé

- **OBSERVÉ cible saine** : GDB seul, image E exacte49200 octets SHA256dc6ac75f…/ELF4c496149… ; IRQ49/66 CPU/SWD actives, ISPR/masks0, RXBUSY_RX/count1, PG4bas/TXidle/filevide/fautes0. Tick2121087→2130079/poll723241806→726307644. Trafic inchangé depuis F (RX8/frameCRCvalidaccepted1/TXstartedTC1, erreurs/drop/failsafe0). Trois sessions détachées, serveurs terminés, aucun reset/flash/HOTPLUG. Baseline/Flash/RAM768KiB brutes hachées dans /home/lolo/dev/msm/reports/TR2-mission013-H-20261011T163855Z. État ponctuel, à actualiser avant une campagne future.
- **VALIDÉ logiciel hors port / PRÉPARÉ HP** : --offline explicite ajouté (défaut préservé) ; preflight_hp.py contrôle environnement/version/hashes et exerce OFFLINE/mock avec ouverture réelle interdite. 13/13 tests Linux réussis ; préflight Linux retourne1 attendu (Windows/serial absents), OFFLINE/mock réussis10nominales/2silences. Bloc PowerShell de téléchargement figé/haché et inventaire sans ouverture analysé syntaxiquement Windows. Aucune installation/politique/configuration modifiée.
- **NO-GO campagne physique** : Windows accessible Dell T1000, aucun py/python trouvé dans PATH ; HP inaccessible, Python>=3.9/pySerial3.5/piloteCOM7/environnement réel NON VÉRIFIÉS. Une seule action future hors port après revue ChatGPT dans PREFLIGHT_CAMPAGNE_HP_M013_H.md ; son succès ne donne pas GOtransmission. Confirmer HP/COM7, maître exclusif/cibleunité1/aucunrépondantunité2, revue indépendante et mission suivante. Campagne12max inchangée ; pas de COM7/RS485/essai en H.
- **PRÉSERVÉ / LIMITES** : sources firmware et preuves E/F comparées aux manifestes G, inchangées ; G archivé exact depuisfe96dcac, deux stashes/backups conservés. Aucune répétition J1/J2/J3/RE-RST/VIO/SB28/câblage. Qualification temporelle WAITING instrument identifié, répétabilité/robustesse générale/production/J1global/E4 non acquises. H clôture une préparation et un verdict NO-GO, pas une campagne physique ; contrôle indépendant ChatGPT reste à faire.

## M013-I — 11 octobre 2026 : GO technique conditionnel, campagne non exécutée

- **OBSERVATION UTILISATEUR dans contratI** : HP Laptop15-db1xxx/COM7disponible, préflightH Python3.13.3/pySerial3.5/environment_readytrue/OFFLINE/mock/hashconformes, zérotransmission. BlocageHP logicielH levé par ce retour, pas vérificationHP directeCodex ; préflight non répété.
- **OBSERVÉ cible saineI** : GDBseul, imageE exacte49200octets SHAdc6ac75f…/ELF4c496149… ; IRQ49/66 CPU/SWDactives/ISPRmasks0, RXBUSY_RX/count1, PG4bas/TXidle/filevide/faultregs0. Tick2901075→2910233/poll989199439→992322297, compteurs trafic inchangésF/H (RX8/frameCRCvalidaccepted1/TXstartedTC1, erreursdropfailsafe0). Baselinefinale992octets SHA256fadf0042… et RAM768KiB conservées /home/lolo/dev/msm/reports/TR2-mission013-I-20261011T165146Z. Trois sessionsdetach/serveursexit0, zéroflash/reset/HOTPLUG.
- **PRÉPARÉ / GO CONDITIONNEL** : commandePowerShellunique GO_CAMPAGNE_HP_M013_I.md, dossierHPpréflightexistant, hashscriptfigé, journalfixeexclusifM013-I-physical.jsonl, maximum12écritures/10nominales/CRCbad1/unité2unefois,1s+100ms, arrêttouteanomalie sansretry/purge/échosupprimé. SyntaxeWindowsanalyséeOK, commande nonexécutée ; outils/tests inchangés/résultatsHconservés, pas nouveauxtests/builds.
- **CONDITIONS / NON VÉRIFIÉ** : revueGO indépendanteChatGPT avantlancement ; HPseulmaître/cibleunité1/aucunrépondantunité2 obligatoires, topologie nonobservéeCodex et aucunéquipementautre supposé. Si incertain ouétatMCUperturbé→NO-GOd'exécution. Aucunhaltpendantcampagne, aucunCubeProgrammer/reset/flashavantanalyseAPRÈS. AucunCOM7/émissionRS485 enI ; utilisateur futurecampagneunefois seulement aprèsrevue, transmettreJSONL/sortiesansretry. AncienprobeEconsommé.
- **PRÉSERVÉ / SUITE** : archivesHexactesdepuis31fd364, preuvesE/F/G/H/stashes005/002/backupsconservés, V1/code/médiasinchangés, acquisphysiquesnonredemandés. RelevéAPRÈS sousmissionappropriée, deltaattenduRX96/frame12/CRCvalid11/CRCbad1/foreign1/acceptedTXTC10 sanserreur/drop/failsafe ; préfixeréelseulsiarrêt. Qualificationtemporelle/instrumentsWAITING, bidirectionnelgénéral/robustesseendurance/J1global/production/E4 NONACQUIS. IclôturepréparationGOconditionnel, pascampagnephysique.

## M013-J — 11 octobre 2026 : campagne unique corroborée, clôture limitée proposée

- **OBSERVATION UTILISATEUR, contrat J** : HP seul maître, COM7/1152008E1, 12 écritures8octets, dixréponsesexactes0103020000B844, mauvaisCRC/unité2sansréponse, passedtrue/durée13,3657s. Journal M013-I-physical.jsonl conservé côtéChatGPT/utilisateur, SHA annoncé23f463fb… ; fichier nonaccessible ici, hash/chronologie nonrecalculésCodex. CommandeIconsommée, ne pas relancer.
- **OBSERVÉ MCU / concordance** : baselineI→J, RX8→104 (+96), frame1→13 (+12), CRCvalid1→12 (+11)/CRCbad0→1, foreign0→1, accepted/available/TXattempt/started/TC1→11 (+10), erreurs/drop/failsafe0. T1.5/T3.5+12, eventspushedpolled+120, fileheadtail2/2=130mod64, overflow0. DernièreADU8/TX7 exactes. Les17comparaisons attendues sont conformes ; agrégats seuls ne prouvent pas ordre/délais de chaquecas.
- **OBSERVÉ état final sain** : imageE49200octets exactSHAdc6ac75f…/ELF4c496149…, IRQ49/66CPU/SWDactives/ISPRmasks0, RXBUSY_RX/count1/UARTerror0, PG4bas/TXidle/filevide/CFSRHFSR0. Tick3704607→3713609/poll1263184426→1266253855 ; traficstable entrelectures. Trois sessionsGDBseulesdetach/serveursexit0 ; aucunreset/flash/HOTPLUG/COM7/RS485nouveau. PreuvesRAM/observer/hashes/deltas /home/lolo/dev/msm/reports/TR2-mission013-J-20261011T170518Z ; E/I/backups/stashes conservés, archiveIexacte.
- **VALIDÉ dans cette portée / NON QUALIFIÉ** : chaînefonctionnellediagnosticFC03/1000/1 corroborée pour campagne12bornée, répétabilitélimitée10nominales et récupérationcompatible aprèsrejets. ClôtureM013 proposée avec réservejournalHPnonlocal et qualificationtemporelle(DE/TC/dernierstopbit/t1.5/t3.5/retourRX) toujoursouverte ; pas qualificationbidirectionnellegénérale/endurance/watchdogIRQ/reset/J1global/production/E4. Aucun acquisphysique redemandé.
- **PRÉPARÉ suiteChatGPT** : CLOTURE_M013_ET_PROPOSITION_M014.md, supervisionPC diagnosticborné via transport.NET/NModbus existant/retry0, d'abordhorsport. B1Reader lit20registres/B0SessionFactory exigeidentificationréelle ; firmwarediagnostic ne fournit normalementque1000/1UNKNOWN0 : ne pas fabriquer B0/DeviceId/READY, pas lancementpollingproduction. Propositionseulement, M014àcontractualiser, aucun nouveau testphysiqueautoriséJ.


## M014-A — 11 octobre 2026 : diagnostic .NET préparé hors port

- **VALIDÉ logiciel T1000** : entrée `--diagnostic` isolée dans le service existant avant chargement/configuration/runtime ; OFFLINE défaut sans ouverture, activation explicite pour une seule FC03/1000/1 unité1, journal CreateNew exclusif et intention persistée, paramètres explicites, retry0, aucune découverte/polling/B5/écriture/reconnexion. UNKNOWN0 reste diagnostic, production non évaluée/DeviceId indisponible ; B1Reader20 et B0SessionFactory inchangés. Code bcc69c9d07aa02f756f9e2db0d67f62613d1951e, build zéro warning/erreur, suite finale502/502 (36 nouveaux cas). CRC/framing/exceptions testés par vrai NModbus sur flux mémoire ; port occupé/timeout/fermeture simulés, pas preuve driver réel.
- **PRÉPARÉ HP / NON EXÉCUTÉ** : publish Release autonome win-x64 réussi, ZIP50086986octets SHA25651e2e36e106b76243096ccb3daaf25d8a9579020e1397e2087aed07cd6ca43a0 horsGit dans /home/lolo/dev/msm/reports/TR2-mission014-A-20261011T171855Z. Procédure DIAGNOSTIC_SUPERVISION_M014_A.md : transfert/hashes/extraction dans nouveau dossier, inventaire .NET/COM7 sans ouverture et OFFLINE ; deux blocs PowerShell syntaxiquement analysés, pas exécutés. Aucun SDK/runtime système à installer avec ce paquet temporaire ; OS/packaging produit non figés. HP non accessible directement, lancement Windows/driver .NET/exclusivité/fermeture sous panne NON VÉRIFIÉS.
- **LIMITES / SUITE ChatGPT** : timeout série100..5000ms, attente lecture limitée à4×timeout puis fermeture ; pas garantie temps réel de l'ouverture/fermeture d'un driver en panne. Journal registres décodés et attentes hex, pas capture brute wire. Revue indépendante puis contrat distinct pour préflight HP et éventuelle lecture unique, état cible/baseline avant et comparaison après séparément autorisés. Pas de commande à exécuter maintenant, pas de faux READY/DeviceId. Qualification temporelle/endurance/reset/watchdogIRQ/production/J1global/E4 inchangées.
- **PRÉSERVÉ** : campagne I consommée et acquis J conservés sans répétition ; aucun COM7/RS485/flash/debug/reset/HOTPLUG/USB/intervention physique, firmware/V1/gels/configuration permanente inchangés. Archives J exactes depuis HEAD36e0055, stashes005/002 et preuves antérieures conservés. Dock T1000 non bloquant. Journal actualisé pour cette préparation, pas nouvelle validation matérielle.


## M014-B — 11 octobre 2026 : transfert et préflight HP OFFLINE préparés

- **VALIDÉ sur T1000** : ZIP M014-A50086986octets SHA25651e2e36e106b76243096ccb3daaf25d8a9579020e1397e2087aed07cd6ca43a0 recontrôlé ; intégrité d'archive et370 fichiers manifestés identiques ZIP/disque, manifeste SHA256eeaf70a3…, EXE PE x64 et runtimes.NET/ASP.NET10.0.12 embarqués vérifiés. Paquet conservé sans rebuild. Bloc PowerShell analysé syntaxiquement sur Dell, métadonnées des options/type vérifiées ; tests existants OFFLINE2/2 réussis sous Linux sans build. Résultats502/502 A conservés, pas annoncés comme relancés.
- **PRÉPARÉ / NON EXÉCUTÉ HP** : PREFLIGHT_HP_SUPERVISION_M014_B.md décrit transfert manuel du ZIP (support compatible s'il existe ou canal fichier disponible, sans supposer partage/dock/accès distant), contrôle HP15-db1/Windowsx64/hash ZIP/manifeste370fichiers, extraction nouveau dossier local et transcript, inventaireCOM7 sans ouverture puis un lancement OFFLINE. Aucun SDK/runtime système/politique Windows à installer/modifier. ZIP toujours local T1000, pas hébergé comme binaire GitHub ni transféré auHP ; support/canal/transfert/OS et résultatHP NON VÉRIFIÉS.
- **NO-GO transmission** : succès OFFLINE futur ne donne pas GO physique. Suite ChatGPT : recueillir retour préflight utilisateur, revoir B puis contrat séparé M014-C pour une seuleFC03/1000/1 unité1 COM7/1152008E1, maîtrise exclusive, étatcible/baseline récente et preuvesaprès autorisés ; aucun debugger pendant trafic, pas HOTPLUG/reset/flash perturbateur. PrésencePnP n'est pas port libre/driver.NET qualifié. UNKNOWN pas READY, DeviceId inconnu, capture brute absente de la frontière registres ; réserves temporelles/endurance/reset/watchdogIRQ/production/J1global/E4 inchangées.
- **PRÉSERVÉ** : code.NET/firmware/V1/gels/configuration inchangés ; aucune ouverture COM7/émission/debug/flash/reset/USB/geste physique, aucune répétition électrique/campagneI. ArchivesA exactes depuis9135894, preuves antérieures et deuxstashes conservés. Preuves B /home/lolo/dev/msm/reports/TR2-mission014-B-20261011T173305Z. Journal actualisé pour préparation seulement ; exécutionHP requiert l'action utilisateur, pas de santéMCU nouvelle observée.

## 11 octobre 2026 — M014-C-END-TO-END, préparation et baseline

**VALIDÉ logiciel conservé** : paquet/code M014-A inchangés, résultats502/502 A
et2/2 B conservés ; bloc HP unique analysé syntaxiquement, non exécuté.
**Rapporté utilisateur** : ZIP conforme et OFFLINE HP/code0 sans ouverture.
**OBSERVÉ MCU** : trois GDB seules sans reset/flash/HOTPLUG ; Flash exacte E,
IRQ49/66 actives, RX104/frames13/CRCvalid12/TX11 identiques J, erreurs0,
RXBUSY/count1/PG4bas/filevide, tick et poll progressent, MCU laissé en exécution.
Baseline C finale992octets/hash28d7bff75da0eb1ed7c7fc108c69da3a17d50b744712a5b21341a909d07d0cfb,
preuves `/home/lolo/dev/msm/reports/TR2-mission014-C-20261011T180341Z`.
**PRÉPARÉ / NON VÉRIFIÉ HP** : [bloc C](ESSAI_HP_DOTNET_M014_C.md) contrôle
EXE/manifeste370/PnP/exclusivité puis une lecture .NET FC03/1000/1, retry0,
journal exclusif et dossier fixe empêchant relance accidentelle. Revue ChatGPT
requise avant exécution utilisateur ; aucun essai réalisé ici. Ensuite journal
exact/hash et deltas MCU à analyser dans **la même mission C** ; rapport BLOCKED
jusqu'alors. Aucun replay Python I. UNKNOWN0/diagnostic seulement ; réserves
physiques temporelles/robustesse/production/J1global/E4 inchangées.

## M014-C — clôture après l'unique lecture HP, 11 octobre 2026

- **RAPPORTÉ UTILISATEUR** : essai.NET unique réussi18:15:29UTC, UNKNOWN0/code0/
  passedtrue, aucune répétition ; SHAjournal annoncéAFDAB5517A13ABED1E9735A4240F0E6B53C5DC110F9CB717BB50410F698A2272.
  FichierHP nonfourni àCodex : contenu/hash/chronologie NONVÉRIFIÉS indépendamment,
  pas validationHP intégrale déduite duhash. Sorties détailléespréflightHP absentes.
- **OBSERVÉ MCU / CONCORDANT** : GDBseul sansreset/flash/CLI/émission ; baselineC→après
  RX104→112(+8),frames13→14/CRCvalid12→13(+1),accepted/available/TXattempt/started/TC11→12,
  CRCbad/foreign1inchangés, erreurs/drop/failsafe0.17comparaisons conformes,
  ADU010303E80001047A/TX0103020000B844 réellesRAM ; EFlash49200octets identiques.
  IRQ49/66actives/ISPRmasks0,RXBUSYcount1/PG4bas/TXidle/filevide/faults0 ;
  tick8062742→8071595/poll2749207528→2752226463, ciblelaisséeenexécution.
  Preuves `/home/lolo/dev/msm/reports/TR2-mission014-C-after-20261011T181712Z`.
- **VALIDÉ DANS LA PORTÉE BORNEE AVEC RÉSERVES** : lecturediagnostic.NET
  HP→STM32→HP corroborée par résultatutilisateur et compteursindépendants.
  CommandeC consommée, nepasrelancer, même missionclôturée aprèspublication/relecture.
  UNKNOWN0 pasREADY/DeviceId/B0/B1complet/production ; temporisationDE/TC/stopbit/
  t1.5/t3.5/retourRX, robustesse/endurance/reset/watchdogIRQ/J1global/E4 inchangées.
  SuiteChatGPT : contrôler rapportdistant, conserverjournalexactHP pouraudit
  sansnouveau trafic, choisirprochainjalonsouscontrat ; accèsdistantHPdifféré.

## M014-D — 11 octobre 2026 : logiciel PC validé, production bloquée

- **VALIDÉ LOGICIEL** : codeccc20526f478ab47c097528a0af33d760f33f22b,
  --system-check OFFLINE défaut/B021→B120aprèscompatibilité, maximum2lectures,
  journalCreateNew/retry0/pasécriture/polling/identitéinventée. Pertebus invalide
  tousappareils mêmefermetureéchouée ; valeurs/timestampspréservés, notification
  compatibleseulement. Reprise même/autreDeviceId/version incompatible testée,
  B0neuf puisnouvellelecture requispourréactiverdonnées, pasrecyclageancienID.
- **RÉSULTATS RÉELS** : build.NET0warning/erreur,524/524suite complète,3/3reprise
  renforcéeensuite sanschangementproduction ; testsfirmwareRTU B0/B1complets/
  actualisation/disponibilité→04/retoursource,110/110hôte +cross-buildARM réussis.
  Mocks/fluxmémoire/serviceshôte, pas physique. CLI OFFLINEréelLinuxretour0.
  PaquetDautonome370fichiers préparé/localnontransféréHP, ZIPecd7fa606a16258afcb087b388fd7a35382a5bccad8e0b34c9f87aff25a35fc2.
- **BLOCKED / M014 EN COURS** : firmwarequalifiéE conserveFC03/1000/1seul ;
  bindingproductionfaibleNOT_AVAILABLE fauteautoritéscarte explicites. Identité
  persistante/adresse/version, géométrie/provisioningFRAMVALID, backendSD/allocation
  durable etséparationharness nécessitentdécisionstructurante. Aucunformat/fixture
  READY/DeviceIdinventé, aucune substitutionbackendhistorique. Allowflashn'autorise
  pasécriturenonvolatilearbitraire. Aucunflash/debug/reset/COM7/RS485nouveau.
- **REPRISE PUBLIÉE** : [matrice et instructionsD](INTEGRATION_SUPERVISION_M014_D.md),
  rapportBLOCKED, archivesCexactes/stashes005/002/preuves/diagnosticEpréservés.
  PreuvesD `/home/lolo/dev/msm/reports/TR2-mission014-D-20261011T182756Z`.
  SuiteChatGPT : contrôlerpublications/déciderautoritésréelles, puiscomposition
  carte/testsnégatifs/sauvegarde-restauration/flashqualifié avant essaiHPborné
  B0/B1etpollingperte/reprisecadrés. Aucunessaiàlanceractuellement, pasreplayC/I.
  M014pasDONE ; limitesCjournalHPnonlocal/timing/robustesse/production/J1global/E4inchangées.


## SSH-HP-001 — 11 octobre 2026 : accès direct HP et OFFLINE observés

- **VALIDÉ infrastructure/logiciel** : SSH Windows depuis Codex WSL via alias
  tr2-hp, identitéHP LAPTOP-3F6J7STJ, PowerShell5.1, codes0/échecdistant17 observés.
  PaquetA existant retrouvé LocalAppData/TR2/M014-B-20261011-195242 ; manifeste
  et370tailles/hashes vérifiés puis --diagnostic OFFLINE réelHP retour0 sansport/
  transmission. Pas transfert/rebuild/installation. AccèsHP auparavantinaccessible
  désormaisdirectobservé, historiques conservés. Scripts temporaires documentés
  dans00_Environnement_developpement/T1000/ssh-hp, six testsPython réussis et
  refushash/concurrencemutex réels. Preuves /home/lolo/dev/msm/reports/TR2-SSH-HP-001-*.
- **OBSERVÉ / LIMITES** : COM7FTSER2K PnPOK, pasprocesscandidat identifié ; pas
  preuveportlibre/exclusif et aucuneouverture. CompteSSH administrateur, politique
  globalePasswordAuthenticationyes ; réductionfuture compte/droits/réseau proposée,
  configuration/service/parefeuinchangés. Mutexcoopérants seulement, coupureSSH
  ne garantit pas arrêt distant. Pas nouvelle observationMCU.
- **NON QUALIFIÉ / SUITE** : aucunCOM7/RS485/flash/debug/reset/écriture physique.
  M014-Dproduction/B0B1 demeureBLOCKED ; commandesC/Iconsommées nonrejouées.
  JournalHPoriginalC toujoursnoncollecté, réservesphysiques/timing/E4 inchangées.
  RevueChatGPT du rapportSSH puis contratspécifique pour tout nouvelessaiborné ;
  accèsSSH seul n'autorise pascampagne ni qualificationModbus globale.


## M014-E — 11 octobre 2026 : gate média et moteur validés, allocation bloquée

- **VALIDÉ logiciel / COMPILÉ ARM** : code00230c3334a3a9c83b4d89edf758c84a6b3fc1c3,
  probeFRAM lectureseule avec états explicites et révocation de toute ancienne
  autorité avant nouvelle recovery. Défaut ancien reproduit par contrôle négatif
  hôte puis corrigé. PréparationSTM32 utilise ce gate ; aucun format/provisioning
  au boot. Moteur explicite plan/apply sur TransactionalImageMedia existant,
  ownership/génération/UID96 entier/version/adresse/B0 réels exigés via inspecteur
  de layout fourni ; absent→refus. Aucun inspecteur/allocation carte inventé.
- **RÉSULTATS RÉELS** :111/111CTest, cross-buildSTM32 réussi (4warningsnewlib),
  six testsSSH réussis. Fixtures/RAM seulement pour transactions/idempotence/
  coupures3phases/absence/EMPTY/corruption/recovery/mismatch/refus sansécriture.
  Pas .NET modifié,524testsD conservés nonrelancés. Preuves
  /home/lolo/dev/msm/reports/TR2-mission014-E-20261011T212010Z.
- **BLOCKED COMPOSITION / M014 IN PROGRESS** : imagehistorique51818 entièrement
  occupée (BootIntent51802+16), pas réserveidentité/UID/adresse approuvée.
  Décider domaineformatpersistant/lienatomique/migration, géométrieownershipFRAM/SD,
  attributionDeviceId unique et versions/adresse réelles avant parser/composition.
  Moteur refusecréationEMPTY et changementidentité/adresse ; aucune prise en charge
  prétendue du provisioning initial. BindingfaibleNOT_AVAILABLE préservé.
- **PRÉSERVÉ / SUITE** : [audit/options/repriseE](PROVISIONING_PRODUCTION_M014_E.md),
  gouvernanceSSHcomplétée sansdoublon, rapportSSHarchivé exact et lienarchitecture
  stabilisé. DiagnosticE BIN/ELF rehashés exacts horscible ; deuxstashes intacts.
  Aucun SSHHP exécuté enE, aucun COM7/RS485/FRAMSDphysiques/GDB/flash/reset.
  Souscontrat futur : déciderallocations/codec, testercréation/migrations/coupures
  multidomaines puis autoritéscarte durables avant qualificationphysique autorisée.
  C/Iconsommés, journalHPoriginalC/timing/robustesse/E4 restent réserves ouvertes.


## M014-F — 11 octobre 2026 : identité V2 et migration logicielles, production bloquée

- **VALIDÉ logiciel / COMPILÉ ARM** : code2ce3a54ae5f5aff9492f8e68465d00921661762e, layoutV2
  51914octets = préfixehistorique51818 inchangé + identité96. Deuxzones physiques
  existantes inchangées, fin122880 < FRAM262144. Codec big-endian/TR2D/version1/
  longueur/génération64/CRC32, UID96 entier, DeviceId32 explicitement attribué,
  B0complet et adresse1..247 persistés, pas nouveauxregistres ModbusV1.
- **VALIDÉ HÔTE** : créationEMPTY explicite et migrationV1→V2 horsboot avec
  ownership/attribution/validationpréfixe/autorisation/génération exigés. Payload,
  header puis publication dans le mécanisme existant ;42scénarios de coupure
  partielle/complète et2succès, préfixe/BootIntent conservés, idempotence sanswrite,
  UID/protocole/doublonsregistry/stalegen/CRC/reserved refusés.112/112tests hôte
  +cross-buildSTM32 réussis sura5e49de, puis1/1test renforcé sur2ce3a54,4warningsnewlib conservés ; pasvalidationphysique.
- **PRÉPARÉ COMPOSITION** : candidate/identité/B0 durables, gateFRAMV2+UIDréel
  HAL3mots+protocole+ownership avantboot ; hooknepeutplusinjecterB0/adresse.
  Weakbinding resteNOT_AVAILABLE, bulkSD réel etautoritéscartedurables nonfournis.
  Probe/recovery lectureseule, aucunecréation/migrationautomatique ; bootcomplet
  existant peutécrirejournaux/BootIntent, nepasleconfondreavecprobe.
- **BLOCKED / M014 IN PROGRESS** : registreautoritatif UID96→DeviceId/B0réel,
  configuration etmanifestegéométrie/ownership/backup FRAM/SD manquants. Mécanismes
  livrés sansinventeridentité ; plan/reprise [M014-F](IDENTITE_PERSISTANTE_M014_F.md).
  API V1 deF refusepublicationV2 plusrécente intègre ; anciensbinairesavantF
  peuventreprendreV1, downgradeproduction interdit sansrestaurationdédiéeautorisée.
- **PRÉSERVÉ / SUITE** : preuves/home/lolo/dev/msm/reports/TR2-mission014-F-20261011T213911Z, archivesEexactes,
  deuxstashesintacts, diagnosticE BIN/ELF rehashés surdisque. AucunSSHHP/COM7/RS485/
  FRAMSDphysiques/GDB/reset/flash. .NETinchangé :524testsD historiques conservés.
  Fournirregistre/manifeste puisraccorderautoritésréelles, sauvegarder/provisionner
  sousprocédureexplicite, flasherimageidentifiée sousgarde-fous avantnouveauxessaisHP
  bornés/polling/perte-reprise. C/Iconsommés, journalHPoriginalC/timing/robustesse/E4
  restentouverts ; rien dephysique déduit des tests ni du seulhashHP annoncé.

## M014-G — Point intermédiaire du 12 octobre 2026

- **VALIDÉ logiciel** : registre DeviceId1/unit1/serial TR2-000001 ASSIGNED au UID96 réel `004A003D 38425018 20333157` ; inspecteur métier strict et disponibilité stockage réelle, 113 tests firmware + cross-build ; outil PC borné OFFLINE par défaut, 539 tests .NET réussis. Aucun essai réussi relancé sans nouveau changement justifiant la validation.
- **OBSERVÉ cible** : FRAM RDID 047F4803, sauvegarde complète double identique, image V1 génération10 non EMPTY et sans config valide ; IIS3DWB WHO_AM_I 0x7B. Utilisateur a signalé insertion microSD ; SD ensuite détectée 511868665856 octets. Partition type07 à 32Mio jusqu'en fin de carte, strictement préservée. Seuls 8192 premiers octets lus ; aucune écriture de données FRAM/SD.
- **ÉTAT COURANT** : probe isolé lecture seule, source69d523b, BIN45348 octets. Diagnostic E sauvegardé et restaurable selon procédure qualifiée. Aucun serveur production actif, aucune nouvelle transmission COM7.
- **PRÉPARÉ / NON VÉRIFIÉ** : fenêtre SD readonly [8,24Mio) envisagée mais non sauvegardée/attribuée ; manifeste, strong binding et provisioning restent à terminer. Pas de config/identité persistante installée. Polling B0/B1 et perte/reprise physiques non réalisés ; M014 IN PROGRESS, M015 non engagée.
- Priorité utilisateur : publier les acquis avant poursuite, sans risque sur partition. Aucun blocage matériel courant ; prochaines actions détaillées dans DERNIER_RAPPORT.md, preuves `/home/lolo/dev/msm/reports/TR2-mission014-G-20261011T220027Z`. Réserves M013/qualification industrielle inchangées.

## M014-H — Tranche production préparée, 12 octobre 2026

- **VALIDÉ logiciel** : strong binding STM32 et profils production/manufacturing compilés, génération authoritative B0 depuis registre, six tests nouveaux et répétition offline du provisioning sur copie backup réelle (10→11→12, conservation préfixe, config UNAVAILABLE sans stockage). Cross-build défaut réussi ; 113 firmware/539 .NET G conservés sans répétition.
- **PRÉPARÉ** : manifeste H et procédure offboot/trigger explicite/CRC backup/générations/UID/readback ; identité hw1/fw0.14.8/proto1/caps0xC attribuée projet, pas capacités acquisition/SD. Configuration seulement structurellement valide ; activation opérationnelle différée jusqu'à ressource owned/caractérisée. Boot env=false/capacité0, READY/stockagefalse.
- **OBSERVÉ / ÉTAT** : GDB seul confirme probe G/UID, Flash/RAM sauvegardées. Aucun flash H/provisioning/COM7 encore exécuté. Aucun accès SD en profiles dédiés ; partition strictement intouchable. Prochaine tranche : manufacturing puis readback et production, essais bornés HP ; M014 toujours IN PROGRESS.

- **OBSERVÉ M014-H après commissioning** : FRAM réellement V2/génération12 activeB, unique trigger réussi, reset et readback262144 identiques à la simulation ; SHAd732acc557152180fc394097d6a5269ea2f45c1da387c8643f512318adf92fce. Plan10 périmé refusé sans réécriture. Cible encore manufacturing en attente, pas serveur actif. HP paquet nouveau371fichiers vérifiés/OFFLINE retour0. Aucune SD accédée, aucun COM7 transmis.

- **OBSERVÉ première campagneH** :9requêtes/8réponses et timeout/reconnexion réels, journal complet vérifié, RX72/TC8/erreurs0. AnomalieuptimeB1figé1s → M014nonclose. Callback producteurstatus opt-in corrigé/113tests+cross réussis sur nouveaucode ; PC539inchangés nonrelancés. Nouvellequalification corrigée nécessaire, pas retryautomatique.

## Clôture physique M014-H — 12 octobre 2026

CritèresB0/B1/polling/perte-reprise effectivement observés surimagecorrigée, confirmé parMCURX72/TC8/erreurs0. Profilpermanent sanssilence flashé/relu/booté puisB0/B1réels lus (2FC03/réponses), journalcomplet1386octets hashDBCD4482F7CB2CF3CA089BE23D6D2362D4D025BB7B339C6526DF4ECF3136A996. Ciblelaisséeenexécution ; aucuneSDaccédée. DernierRapportHporteSHAs/limites/relecturedistante ; suiteM015àcontractualiser.

Publication complèteH vérifiée auSHAae14c4c8e6081086ded87c5d505e83fa74c7a496 ; marqueursDONE documentaires finalisés aprèscettepreuve. SHAdu derniercommit fourni aprèscontrôlefinal, revuedistanteChatGPT nonprésumée.
