---
mission_id: TR2-20261117-SHIELD-DUAL-BRINGUP
status: IN_PROGRESS
base_ref: main
base_sha: f019dfbaa4e2cf9cf181031d0534013b89d989e1
initial_head: d5d3da95697607ad21cc57f95f48534e49ed3bb6
result_sha: 41c55205a61eb26d406edf52bfed68f7de145fc9
created_at_utc: 2026-11-17T19:58:16Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Qualification progressive — SD de001 réussie, comparaison FRAM préparée

**Mission en cours, non clôturée.** TR2-001 a réussi le test SD/FAT32 avec création d'un fichier neuf4096octets, relecture intégrale/CRC et suppression. Sa FRAM est identifiée mais deux lectures diffèrent sur9octets ; aucune écriture FRAM n'a été armée. Le capteur renvoie00 au lieu7B. TR2-002 est hors tension, démonté pour reprise des soudures de masse par l'utilisateur. Aucune communication RS485/Modbus qualifiée.

Ce checkpoint préserve les résultats physiques001 et un seul essai différentiel FRAM préparé à250kHz, **pas encore flashé**. StatutIN_PROGRESS car une comparaison logicielle autorisée peut se poursuivre ; les obstacles physiques distincts restent documentés. Ne pas confondre logiciel compilé, données lues et qualification matérielle.

## Identités et état physique rapporté

| Ensemble | ST-LINK et UID frais | État utilisateur / alimentation |
|---|---|---|
|002|0032001F3235510C37333439 ;001B0046/38425018/20333157|Première qualification sousST-LINK/JP6STLK,CN15débranché ; ensuite entièrement débranché etshield démonté pour soudure, confirmation utilisateur|
|001|003500463235510B37333439 ;004A003D/38425018/20333157|ST-LINK/JP6STLK,CN15débranché ; inspection hors tension C2/modules/soudures/sanspont oucontrainte confirmée ;3,3V rapportés auxbreakouts|

Les périphériques ont pu être permutés :aucune identité ou association module déduite de la Nucleo. RDIDFRAM etWHOcapteur ne donnent pas un numéro de série module. Même32Go transférée parutilisateur vers001, identifiée ensuite parCIDexact. Aucun accès àla512Go.

L'ordre002puis001 etla bascule vers001pendant réparation002 ont été explicitement demandés. La réponse intermédiaire «TR2 débranché,shield séparé,prêt à souder» était ambiguë ; elle a ensuite été explicitement attribuée à002. Les valeurs de résistance du shield001 n'ont pas été fournies et ne sont pas inventées ; son contrôle visuel etles3,3V ont été confirmés avant accèsMCU.

## Résultats séparés

| Interface | TR2-002 | TR2-001 |
|---|---|---|
|FRAM SPI1|**PASS borné** RDID047f4803,doublelecture262144octets identique, motif64octets relu puisrestauration intégrale zéroécart|**FAIL stabilité à1MHz** RDIDconforme,9octets différents entre lectures complètes ; aucun write/testrestore|
|IIS3DWB SPI3|**FAIL WHO00** à8 puis2MHz ;HAL_OK/RXcomplet ; aucunXYZ|**FAIL WHO00** conservé sousCN0, puisuniqueWHO2MHz du banc ;HAL_OK/RXcomplet ; aucunXYZ|
|SD SPI2/FAT32|**FAIL init/timeout**,CID nonlu,écriture0|**PASS borné** CID/CSD/capacité/FAT32 ;fichier4096octets neuf,sync/close/relecture complèteCRC/suppression,erreur0|
|RS485/Modbus|NON_TESTÉ ;J2Clickseulconfirmé|NON_TESTÉ ;J2ClicketbranchementRS485rapportés, autresgatesenattente|

Aucune endurance, coexistence générale, tenue mécanique/industrielle ou mesure vibratoire qualifiée. L'échec d'identification du capteur interdit ses configurations/XYZ ; aucune répétition aveugle du même essai.

## Préservation et flash

Chaîne Windows qualifiée, outils relus :CubeProgrammer2.23.0/GDBserver7.14.0/GDB15.2.90 CubeCLT1.22,ST-LINKV3J15M7 ; sourcesLinux/SDKCubeU5v1.9.0 aux chemins prescrits. Source001 de procédure standard relue dans les preuves versionnéesCN0 du02/11 ; chaîne002 documentée par`QUALIFICATION_M015_L_BANC_ST.md`. CLI NORMAL/SWD/SNexplicite/8000/adresse08000000/verify/reset, puisresetGDBaprèsdernierCLI. PasHOTPLUG/mass erase/OptionBytes/RDP.

Avant modification de chaque carte :UID frais,Flash2MiB etRAM768KiB sauvegardées. Toutes les images exécutables comparées intégralement au préfixeFlash ; pas d'identification depuis les octets résiduels de fin.

| Sauvegarde | SHA256 |
|---|---|
|002Flashinitiale|ca275a54998a0730d9067f4107b06830aacc59e3e5789d6e25df579e91731a7e|
|002RAMinitiale|76839fdf2f691cab553b4d2d9176cc61dde1a1f8aa1bd877adc63444509f4be7|
|001Flashinitiale|65f4ab9f828ee358e30a1df1b07f21d03848bdaab837b5ac130e311be333fcf5|
|001RAMinitiale|ded90b5dcc0cbfb6c8d1f47eec897b973217cb6d02236eed61db0fc84d1324f5|

002portaitOBE2h/source`dd2f27ca3d886742b0ab37ab378cd524012a0370`,53032octets/SHA20785017… ;001portaitCN0/source`6633b0154bf67b4c5437f1d259732fc1b1f21e4a`,53340octets/SHAd3186380… . Sur001, la RAMinitiale conservait déjà unWHO00 valideHAL/reg0F,len1,RX2complet sousancienfirmware :extraction horsligne depuis dumppréservé, adressesELFexactes etstructures ducode. Aucun nouvelordreSPI pour cettepreuve.

Un flash du banc indépendant41c55205 sur chaque carte, pages0..4 uniquement ; readback entier imageexacte ettail>40960inchangée. BIN`631b05dbc27843a7cdc780818418790057e193ed454fdd7e26d78dc7390782d7`. BootWAIT observé, séquence/commande/armement0,CSrelâchées,PG4/ODR0 (liaisonDE nonvérifiée),zérocommandeSD,aucunUART. CLI3,28V sur002 et3,29V sur001 côtéMCU :ces indications ne remplacent pas les mesuresmodule. Les sauvegardes originales permettent une récupération desoctets, pas une qualification du firmwarehistorique.

## FRAM : résultats et protection

002 :doublelecture262144octets identique,exporthôte CRC32e20eea22/SHA`8a39d2abd3999ab73c34db2476849cddf303ce389b35826850f9a700589b4a90`. Zone0x3FFC0..0x3FFFF **non prétendue libre**, méthode sauvegarde/restauration autorisée :64octets de motifécrit/relu, restauration etcomparaison de chaqueoctet du média au snapshot,zéroécart. Aucune identité provisionnée, contenu complet horsGit.

001 :RDID047f4803, comparaison intégrale **9écarts**,error113,CRCpremièrelecture1351935669. Export du premier snapshot conservé etCRC hôte égal auMCU ; **ce n'est pas une sauvegarde stable qualifiée**. `001-fram-unstable-read-manifest.json` distingue export exact etlectureinstable. Aucun test d'écriture/restauration etaucunwriteFRAM exécuté sur001. La secondelecture n'a pas été conservée intégralement :adresses/valeurs des9écarts non identifiées par ce banc, pas de précision inventée.

**Comparaison préparée** :ajout minimal de`BENCH_FRAM_DIVIDER` au seul bancisolé, défaut16/1MHz inchangé, variante64/250kHz ; autresvaleurs refusées. Deuxcross-builds réussis ; leBINdéfaut est **byteexact** àcelui déjàévalué41c55205, seulslesoctets de réglage changent dans la variante250kHz. Configuration32 explicitementrefusée. BIN250kHz`019569f9d58d09189f12b167be7e71b87ee4fcbbc5f95e3f354fee1ffd0795fd`,compilé **nonencoreflashé**. Gardes/comparaisons/armements identiques, zéroécart toujours exigé ; abaisser la fréquence ne qualifie pas1MHz ni une soudure. Liredeuxfois etexportervérifié avant toute éventuelleécriture ; pas d'usage du snapshotinstable comme référence de restauration.

Validationproportionnée :bancexpérimental seulement,cœur/driver/HAL/V1 inchangés. Builds ciblés desprofils nouveaux ; tests hôte de sécurité1/1PASS déjàconservés, nonrejoués car logiqueinchangée. Avertissementsnosyslinker conservés, aucunfilesystemnewlib utilisé. Pas de validationmatérielle déduite du build. Aucunrejeu duSDPASS ouWHOrefusé prévu lors du changement de profilFRAM.

## SD001 : preuve physique complète du test court

CID **0353445344333247858bf0923c01927b**, CSD **400e00325b590000edc87f800a4040c3**,62333952secteurs/31914983424octets,SDHC/FAT32,csize64,n_fats2. LeCRC desregistres/données etlesgatesCID/capacité conservés ; compteurlecture4 avant scratch,aucunwriteàl'identification. Seulecarteadressée par le firmware,unvolumeFatFs, aucun accès disque Windows ou512Go.

Fichierneuf **`0:/T1700101.TMP`**,CREATE_NEW après`f_stat` inexistant,armementRAMone-shot.4096octets déterministes écrits,sync/close,rouverture/relecturecomplète/EOF,memcmp MCU puisexport/comparaisonhôte. **CRC32c0e65e2b** égalMCU/hôte,hash dans`001-sd-scratch-roundtrip.json`. Suppression danscommande séparée armée,`f_stat=FR_NO_FILE` etquiesceSD réussis.15secteurs confirmés au total incluent lesmétadonnéesFAT ;zéroerreurCRC/timeout,writerinactif/nonincertain. Aucunformatage niécrasement d'unfichierexistant. Les autresfichiers ne sontpas déclarés hashvérifiés après ce test :ils n'ontpas été relus intégralement auPC.

SD002 :timeoutinit,error2,lastcmd0/R1FF,commands0. Ce compteur compte lesréponses reconnues ; ilnepermetpas de distinguer attenteprécommande etR1aprèsCMD0. Correction explicite de la premièreinterprétation donnéeenconversation ;CID/FAT nonlus,écritures0.

## Refus capteur et erreurs d'observation

WHO00 à2MHz/mode3 surchacun,traceuniqueHAL_OK/error0/remaining0/précloseRXoffset2,valid1,payload00. Anciennes traces8MHz conservées ;différence de firmware,reset ettemps depuisalimentation empêchent une attributioncausale isolée àlafréquence. Aucunwritecapteur niXYZ ; causecapteur/câble/soudure/alimentation nonétablie. GPIO/SPI instantanés ne sontpas desmesuresanalogiques.

Deuxerreurs de scripts ont été conservées puis corrigées sans rejeuSPI :sur002,nomerroné`bench_traces` au lieu`bench_trace` ;sur001,`r3_command` absent duELFautonomeCN0. Correction deslectures depuis les vrais symboles, pas de reset/flash pourcorrigerlacapture. Premierdump001 sansELF volontaire, puischargement de l'ELF exact aprèsidentificationFlash ;pasd'hypothèse surlesymbolage.

## Défaut de masse002 et reprise physique

Mesures utilisateur :2,3V TP1/TP2 etauxbreakouts contre3,3V J2-7/J2-11. Premierrepérage9/11 explicitement corrigé7/11 :aucuneanomalie5V déduite. NetlistSTC05 relue :CN8-7=3V3,9=5V,11/13=GND ;shieldJ2-7↔TP1 et11/13↔TP2 directs,aucunerésistancesérie prévue.

Pointes0,1Ω ;cheminpositif0,3Ω ;J2-11/TP2 d'abord≈26kΩ(nonreproduit),puis11/TP2=35Ω,13/TP2=35Ω,11/13=0,1Ω ;TP2/GNDlecteur=0,1Ω. Cesmesures orientent vers entréeGNDshield/connecteur/soudures, sans localiser encorela cause ;11/13assemblés peuvent être reliés viaNucleo. Résistancesen-circuit ne permettentpas de calculer simplementle courant sous tension.

Utilisateur rapporte ensuite desdéfauts apparents auxsoudures11/13,annonce leurreprise,puis confirme002entièrement débranché,shield séparé. Repèreclarifié **J2shield/CN8**, pasunautreJP2. Pasdephotoinspectée parCodex ni réparation confirmée finie. Aprèsreprise etavantalimentation :R11/TP2,R13/TP2 prochesdespointes,RTP1/TP2stabilisée,absencepontvoisin ;puisrails. Aucunshunt oualimentationcontournantledéfaut. Aucunaccès002pendanttravauxphysiques,aucuneactionmanuelleparCodex.

## RS485 : gates matériels non levés

Sources primaires :[Click3863](https://www.mikroe.com/rs485-isolator-2-click) et[schémav102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf). J2DE/RE,pontconfirméutilisateur,distinct deJ7A/Y etJ8B/Z halfduplex. VCCSEL3V3,J3/RE,TERMJ5/J9,BIASJ4/J6 etbus àrelever réellement ;GND2 reste isolé deGNDlogique etpolaritéconvertisseur nonprésumée.

PG4/CN12-69→J5local5→ClickDE ;présenceJ5 etcommandeDE réelles nonétablies. AucunportCOM ouvert niémission surlesdeuxbancs. InventaireinitialWMI incomplet(COM5seul),PnP completCOM4FTDIBG04KSSW :pasdedéconnexiondéduite du seulWMI. Pasd'accèsHP. AucunRX/TX/Modbusrevendiqué avantlevée desgates.

## État et suite

001aprèsSD :stage3/command7/sequence5/done_mask196,commande/armement0,writerinactif/nonincertain,SDready/CSrelâchée,PG4/ODR0 (liaisonDE nonvérifiée),GDBdétaché. Image41c55205 enplace. **Suiteautonome :** publier cecheckpoint,puisrecontrôlerUID/repos,flashstandard du seulprofilFRAM250kHz,readback/resetGDB etdoublelectureFRAM,aucunrejeuSD/WHO ;écritureFRAMseulementsnapshotstable/exportvérifié. Encasdenouvelécart,conserveretstopperlesécritures,contrôlephysiqueciblé.

002restehors tensionpourréparation. LescontrôlesRS485etlespoursuitesSPI3nécessiteront observations/interventionsutilisateur. Aucuncritèrecomplet dequalificationdesdeuxensembles satisfait ;pasdeDONE.

## Git, preuves et journal

Départd5d3da95,worktreeisolé`MSM_vibe-SHIELD-DUAL-BRINGUP`,branche`codex/shield-dual-bringup`,ciblemain. Racinehistorique82169af7 etdeuxstashes préservés,aucunreset/clean/merge/rebase/forcepush. Touslescheckpointsprécédents ont étépubliés etleursblobsmodifiés intégralementrelus ;dernieravantcetravail **baf5cf1b83679a1764d966e2e3b6f365813ce072**(3blobs).

Preuvesversionnées `Echanges_Codex/preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/` :rawGDB/scripts/invocations,hashes,identificationCID/CSD,scratchsynthétique,manifestesdebuild etdepublication,observationsutilisateur distinctes. Originaux etNVM complets restent dans `/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/`. AucuneFRAMcomplète ouFlash/RAMinitiale diffusée. LogsWindows byteexact(CRLF/espacesfabricant),whitespacecheck horslogsbruts,preuvesnonaltérées.

`ETAT_COURANT_TR2.md` actualisé avecUID001/imageCN0,sauvegardes,SDPASS,FRAMinstable/écrituresrefusées,WHOFAILetprofil250kHzpréparé ;002toujoursréparationnonconfirmée etRS485enattente. SHAduprésentcommit fourniaprèspublicationnormale etrelecturedistante. Aucunequalificationindustrielle ouadoptionnormative implicite.
