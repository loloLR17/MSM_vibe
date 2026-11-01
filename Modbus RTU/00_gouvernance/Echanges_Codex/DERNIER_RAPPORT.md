---
mission_id: TR2-20261031-R3-8H-FASTSTART-PC-MACHINE
status: IN_PROGRESS
base_ref: main
base_sha: f5850ad2de81a6b7639d947d54751305cde3e75b
initial_head: d5af0828e370780320bda67fac5d537bf3fd5016
result_sha: 7b815b103a49b7fb1a8ec681a65206312b0ec16b
created_at_utc: 2026-10-31T18:24:20.286731+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Nouveau montage qualifié sur essai court8MHz — préparation8h rétablie

**VALIDÉ PHYSIQUE BORNÉ : FIFO/FIR/SD2s avec module emprunté001, aucune endurance anticipée.** Le montage002 via breadboard fonctionne avec N8Fduplex8MHz. **PRÉPARÉ : lancement8h sur signalLANCE8H seulement**, aucun long lancé. Mission resteIN_PROGRESS enattente decesignal etdesrésultatslongs. [Preuves intégrales](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/breadboard-qualified-N8F8MHz-20261101/README.md), [runbook courant](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/breadboard-qualified-N8F8MHz-20261101/PRET_8H.md).

Après calibration/file2s rétablis à4MHz maisfaiblemargeFIFO399/work983,775µs etrefusducollecteurbrut81, checkpoint**a7805d6dc2e768f2e04226d4c88f6815eab6fb38** publié puisrelu113fichiers/108artefacts exacts avantflash. Échec ducollecteur4MHz conservé, aucunegardeassouplie.16WHO8MHz corrects justifient leprofilde référence8MHz/N8F, avecnouveauxnoms. Aucunalgorithme ni gain/plage/FIR/ODR/v3/importeur/modèleV1 changé ; seuldiviseurprofil/préfixe/provenance, codeévalué7b815b10 déjàsurmain. BuildARMcibléPASS/anciensPASSinchangésréutilisés. Image**53032B/c2dd380de58f3581337b22f2957ce2265f0eb7f98c06cbeaab2d17bf64494c61**, text53008/data24/BSS549096, warningsnosys conservés. Unflashstandard/resetnormal/readbackBINentierexact aprèsSN/UID/imageEpréservée/étatsûr vérifiés ; aucunresetsupplémentaire/masserase/OptionBytes.

Calibration8MHz**24µs/198brackets/6577ancres**,WHO7B/CID/CSD/FAT32/nomsneufs/libre conformes. UniquegateN8F0003.TR2 : **52592XYZ/5260sorties/31560B/68records**,2000143µsMCUnominales/QPCintérieur1,0390489s, aucuneduréeSI2s étalonnée. **0flags/FULL/OVR/rails/clips/FIFO/SD/fileoverflow**, collecteurinchangépass_references=true/103sortiesnumériques exactes. Workmax**641,10625µs/1000**, service**1348,51875/2500**, gap**1606/5000** ; FIFO**48**/file5120B, STOP133µs/drain8421µs.8XYZfinauxsansancrefuture explicitementnonémis, paspertemasquée. CTRL1AC/±8g configurés etcontrôlés, pascalibrationd'amplitude.

**Relecture indépendante :** fichier131072B intégralementcopié enlectureseule parFatFs/GDB versfichierhôte, puislecteurPythonstrictinchangé :COMPLETE/tousCRC/index/session/footer/qualité/SHA exacts. PayloadSHA**30e0de232ceddbab4697a6a4768ea75b3bfcf88d012e4045c181c10dbf6939a3** égalauproducteurRAM ; SHAfileentier**e8227c336ef2e488b0dc12da807402dd1209f8268c44a0fe4ef1def6bfa54e6d**. Copie n'aécritaucunsecteur. PasuneextractionlecteurPC, tenueàcoupure etendurance nonqualifiées ; anciensfichiersnonremplacés/nonre-hachés, nouveauxE/F conservés.

**READY local vérifié07:04:03UTC01/11 :** lanceurexistant `launch_8h.py --check-only` PASS, UID/image/CIDCMD10/FAT32/libre≥20480clusters/N8F0005absent, COM5parent002/libre/exclusif, secteur/veilleAC0/hibernationAC0. Carte resteN8Fstage4/run3/error0/FIFOoff/writeroff/queue0/SDquiet/CShaute/GDBdétaché. `python3 /home/lolo/dev/msm/reports/TR2-002-breadboard-N8F8MHz-20261101/launch_8h.py --signal 'LANCE 8H'` prêt **sans avoirétéexécuté**. Aucun locklong ni longcréé, garde30600sMCU/640MiB inchangée. Garderalimentation ; reboot/capteur/câblagechangé invalideprêt, pasforcerRAM ourejouergateoccupé. SignalLANCE8H resteconfirmationmontageauPCmachine/alimentation seloncontrat.

Uneerreur dechemin d'unhelpercopié estsurvenue avantlecturehôte/avantgate ; corrigéeverscopiePSlocale, preuvePREPARATION_CORRECTION.json, aucuneprotection modifiée. DiffcheckconditionnementdesJSONWindowsCRLF corrigé parzlibsansperte danscommitdocumentairedecheckpoint ; contenuoriginalinchangé. AucunFRAM/COM4/001/serviceWindows touché danscettequalification, firmware001 conservéoff. Le capteurinitial002 resteNONVÉRIFIÉ ; deuxNucleo fonctionnentavec lemoduleprêté dansleursconditionsrespectives, pasdedéfautNucleo002démontré ni causalitéexclusivebreadboard.

Rapport/journal/PRET_8H/preuvesactualisés, code/testsnormatifsinchangés. Travailisolépréservé, deuxstashesintacts. **Résultats publiés aucommit5cd9193b01fa9f654fec4bb2a4db1d771da56e3b puis relus depuisorigin/main à07:09:26UTC :142fichiers intégraux identiques /134artefacts originaux vérifiés**, images etfichiercomplet conformes. Preuve `breadboard-qualified-N8F8MHz-20261101/remote-readback.json`. Cettepublicationqualifie lecheckpoint, paslendurance8h. Aucunmatériel/testsupplémentaire pendantrelecture ; bilanfinalàpublier/vérifier, SHAfinaldansréponse. Lessectionsci-dessous sonthistoriquesetn'invalidentpaslapréparationcouranteN8F décriteici.

# Qualification du nouveau montage — calibration et fichier2s rétablis, marge4MHz à traiter

**IN_PROGRESS : reprise fonctionnelle réelle, pasREADY8h ni long lancé.** Après autorisation utilisateur de qualifier le montage, UID/SN002/imageN8E/source4307b95c exacts, réutilisés sans reconstruction de N8E ni flash/reset. Une init1 : WHO7B/bonCID/CSD/FAT32, calibration**18µs/199brackets/6577ancres**, zéroerreur ; contre-calcul18µs exact. Ancien refusFIFO86 absent dans cet essai, ±8g/ODR/FIR/gain/v3/gardes inchangés. NomsN8Eneufs/libre vérifiés. [Preuves, refus du collecteur et analyse](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/breadboard-qualification-4MHz-20261101/README.md).

Un seul gate2s : N8E0003 créé sans remplacer les anciens, **52312XYZ/5232sorties/31392octets utiles**, duréeMCUnominale2001484µs, intervalleQPCintérieur1,0451505s,0rail/clip/FULL/OVR/SD/filefaute ; STOP167µs/drain13844µs. Les31392octets utiles relus parFatFs etSHAproducteur/CRC/index/session/footer conformes ; 103sorties préfixe vérifiées indépendamment. Ce n'est pas une extraction lecteurPC ni une qualification SI.

**Réserve explicite : le collecteur échoue en AssertionError/pass_references=false**, flagsbruts0x81 au lieu0 requis par son prédicat. Il reste inchangé et l'échec est archivé. Décodage ST : watermark+bitniveau, aucunFULL/OVR/latch (masque68h=0) ; pas de fauxoverrun annoncé. NiveauFIFOmax399 élevé, work983,775µs sur1000/service2349,4375sur2500/gap2607sur5000 ; faiblemarge, pasdequalificationlong4MHz. IntégritéfichierPASS distincte duverdictglobal. Champbytes du seul JSON de relecture corrigé d'une ancienne constante vers la mesure31392, pasdecontrôleCRC/SHA modifié.

Contrôlelectures8MHz justifié :16WHO7B/16retours0 après arrêt, aucunregistre-capteurécrit ; prescaler/handle rétablis4MHz. **Suiteautonome : préparerprofilN8Fduplex8MHz de référence pour retrouver la marge**, nouveaupréfixe/gardes strictes ; build ARM ciblé N8F déjà réussi (53032B/c2dd380d…94c61/source7b815b10), aucun ancienPASS rejoué, puis flashstandard si préflight/étatsûr etcheckpointpublié. Aucunalgorithme ou contratnormatif changé, aucun long8h autorisé sansLANCE8H. FirmwareN8E laissé sûr àcecheckpoint ; fichiersN8Eacquis intacts, T1000secteur observé, aucune001/FRAM/COM4/protectiontouchée. La breadboard n'est pas déclarée causeexclusive : recâblage/cyclealimentation/étatinitial concomitants. Journalactualisé ; rapport/preuves àpublier-relire avantflash8MHz.

# TR2-002 recâblé par breadboard — contrôle rapide en lecture positif

À la demande utilisateur, **contrôle rapide uniquement**, sans nouvelle acquisition : tous les fils Dupont repris par breadboard selon utilisateur ; capteur confirmé **emprunté àTR2-001**, TR2-002 connecté et001 débranché. Le comparatif prévu avec le capteur initial002 sur001 n'a donc pas eu lieu et reste suspendu. [Preuves](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/TR2-002-breadboard-quick-20261101/README.md).

SN/UID002 et firmwareN8E/source4307b95c vérifiés ;53036octetsFlash strictementidentiques au BIN59e65cd66ecdfe76b7ace5f2ad0affb3c05ed871c804ec2549d11f6228fbc186, aucunflash. SPIduplex4MHznominal/mode3 inchangé. **Huit lectures WHO :8×0x7B, huit retoursHAL0**. Lectures CTRL1_XL/FIFO_CTRL4/CTRL10_C=00/00/00 : ODRoff/FIFObypass/TSoff observés, retours0. Aucune écriture registre-capteur ni changement de fréquence ; scratchRAM seulement.

**Limite : le défaut d'écriture80/config86 n'a pas été retesté.** WHO était déjà correct à4MHz avant recâblage ; pas de réparation ou effet causal de la breadboard déclaré. Mission8h demeureBLOCKED/READYfalse jusqu'à configuration/calibration/gate conformes. Une éventuelle init/calibration serait une étape distincte, non lancée ici au titre d'une vérification rapide.

État vérifié06:48:25UTC01/11 : WAITstage2/error0/run0, FIFOoff/writeroff/clockfault0/SDwrites0/CShaute, GDBdétaché/serveursfermés. Aucunbuild/reset/flash/init/calibration/START/SD/COM4/001/FRAM ni protectionWindows touchés. Étendue des changements physiques sur les filsSD non précisée, carte non interrogée ici. Les états99 précédents décrivent l'ancien boot, pas l'état WAIT actuel ; leurs échecs restent acquis. Le result_sha correspond de nouveau au firmware002 réellement contrôlé, les preuves positives001/source72a7 restent ci-dessous.

DépartGit5c78bf84 propre/synchronisé, modifications limitées au rapport/journal/preuves ; anciens testsPASS nonrejoués. Checkpoint à publier puis relire, SHA final dans réponse. Journal actualisé avec nouveau montage/résultat et limites.

# Extension humaine TR2-001 — capteur fonctionnel sur son banc de référence

**Sous-test positif, mission8h toujours BLOCKED sur TR2-002.** L'utilisateur autorise explicitement le diagnostic TR2-001 en lecture/debug, sans flash/reset/START-STOP/écritureFRAM, puis confirme le retour du capteur emprunté sur001 hors tension/câblage d'origine et002 arrêté. Exception limitée documentée ; contrat ChatGPT non réécrit. [Preuves et analyse complètes](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/TR2-001-reference-readonly-20261101/README.md).

SN001003500463235510B37333439 etUID004A003D/38425018/20333157 vérifiés ; Flash150976octets **entièrement identique** au firmware opérateur72a7b573, BINe1846c87017fd7667daf1b79cb440af1024346c5deae8d5065f8525c00aa7b80. Pas de restauration ni compilation. Config active3/gen3, **±2g/code0 inchangée**, SPI3nominal4MHz/mode3/duplex ; CFG1/CFG2 identiques au relevé N8E4MHz sur002, autres différences non éliminées.

Le firmware conservé a réalisé de lui-même sa calibration FIFO au boot après remise sous tension utilisateur. Relevé RAM après achèvement, **aucune calibration relancée ni START envoyé par Codex** : **52442XYZ /6556timestamps /195brackets /résidu23µs<100**, erreurs0. Contre-calcul indépendant des195brackets :23µs exact. Préfixe2048mots,1820XYZ/228TS : parités correctes,227intervalles complets de8XYZ, axes variables ; pas de capture intégrale ni métrologieSI. Traces réelles : WHO7B/CTRL1A0/FIFO_CTRL4**86** en fin de calibration, puis **80** conservé à l'arrêt et relu après écriture80. Le champ défaillant sur002 fonctionne donc sur001 dans cette configuration.

**Conclusion : le capteur emprunté n'est pas totalement défaillant.** Son fonctionnement borné sur001 est observé ; cela ne désigne pas la Nucleo002 seule comme cause. Carte/câblage/alimentation/firmware/état initial et plage±2g vs±8g restent confondus. Le capteur initial002 n'a pas été essayé sur001 ici. Aucun résultat de long/SD/AC1/Modbus ni ±8g qualifié. Le `result_sha` désigne maintenant le code001 réellement lu/évalué ; celui du candidat002 est4307b95c, toujours décrit ci-dessous.

Dernier état001 vérifié : stage7/error0/work.error0, les deux FIFO arrêtés, config3/gen3/FS0, CS capteur haute ; dernier terminal RAM STOP5/25/run17/gen126 inchangé. Aucun appel de fonction cible, flash/reset/COM4/START-STOP/écritureFRAM ni accèsSD commandé ; FRAM entière non relue, pas de hash actuel inventé. Le compteur stops=1 relève du boot autonome, pas d'une commande agent. GDB détaché/serveurs terminés, firmware intact. La tentative002 préalable a échoué avant connexion car son ST-LINK venait d'être remplacé par001 ; logs préservés, aucun fallback cible et aucune nouvelle lecture002 prétendue.

Prochaine étape proposée : comparer le capteur initial002 sur **ce même ensemble001**, sans changer firmware/config/câblage, après transfert hors tension confirmé selonAGENTS§9. Aucun nouvel essai ou geste demandé/exécuté avant publication de cette référence. START/STOP, flash et modification de plage restent hors extension actuelle. Journal actualisé ; code/tests inchangés non rejoués. Départ Git1f03cc94 propre/synchronisé, travaux historiques et stashes préservés. **Rapport/preuves publiés au commit34190aa3d2f9b683a5a045ebb626fbe09b1e5b60 et relus depuis GitHub à06:31:35UTC :42fichiers intégraux identiques /36artefacts bruts conformes**, preuve `TR2-001-reference-readonly-20261101/remote-readback.json`. Bilan final de relecture à publier/vérifier, SHA final dans la réponse ; état8hBLOCKED maintenu honnêtement. Après publication, transfert hors tension du capteur initial002 vers001 demandé selonAGENTS§9, mêmes références/brochage requis, fils/code/config001 conservés, puis ST-LINK001 seul et attente10s avant relevé du boot. **Retour utilisateur attendu, second module non testé à ce stade.**

# N8E duplex 4 MHz — premier refus identifié, préparation 8h bloquée

**BLOCKED sur configuration SPI/FIFO non conforme ; aucune acquisition longue lancée.** Le profil 4 MHz reconnaît WHO=0x7B, mais ne passe pas l'initialisation : FIFO_CTRL4 relu `0x06` au lieu de `0x86`. La calibration n'a pas commencé, aucun gate SD ni fichier N8E créé. [Preuves, décodage et limites](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/duplex4-new-wires-20261101/README.md).

Le changement logiciel minimal est publié séparément au code évalué **4307b95cf1f85266266574a04beab96e2fc14b20** : paramètre CMake du diviseur SPI3 existant, défaut 8 MHz préservé, 4 MHz uniquement pour le profil 8H duplex explicite. Mode SPI3 conservé. Aucun changement ODR, ±8 g, gain244µg/compte, FIR107 Q15 /10, format v3, importeur ou seuil. Build ARM ciblé réussi ; anciens PASS sur code inchangé réutilisés, pas de nouvelle campagne hôte/canonique. Avertissements nosys conservés. BIN **53036 octets / SHA-256 59e65cd66ecdfe76b7ace5f2ad0affb3c05ed871c804ec2549d11f6228fbc186** ; ELF/MAP/options inclus dans les preuves.

Un flash standard qualifié après UID/SN/N8A entier/état sûr vérifiés, reset normal de programmation, readback BIN entier exact et WAIT2. Une seule init/calibration commande1 : stage99/error1/run1/FIFOerror6/stop_error0, zéro bracket/anchor/sortie. Carte CID/CSD attendus/FAT32 montée, **trois secteurs lus, zéro écrit**. Script arrêté explicitement en erreur GDB6 ; contrôles noms/libre non atteints, aucune prétention READY. Aucun reset supplémentaire. Le relevé Windows de cette tranche indique aussi PowerOnline=false/Discharging=true : le secteur reste à confirmer pour tout futur long, indépendamment du défaut SPI.

Les traces montrent l'écriture demandée `0x80` à FIFO_CTRL4, puis sa relecture `0x00` ; le RMW suivant écrit `0x06`, ce que la garde refuse correctement. Un contrôle borné du seul champ volatile, FIFO bypass / ODR et timestamps off, ne retrouve pas davantage `0x80` avec écritures/lectures croisées à 1 et 4 MHz. Valeur originale restaurée et SPI3 remis à 4 MHz avant detach. Les codes HAL zéro n'établissent pas la validité physique du bus. La cause exacte reste inconnue : transport, état du capteur ou conditions électriques à départager, pas de défaut exclusif du câblage ou du capteur affirmé.

**État final vérifié à 23:41:29 UTC le31/10 (01/11 local)** : N8E en Flash, stage99/error1/run1, FIFO off/writer inactif/clock_fault0/SD_CS haute/SD writes0/failed=false, GDB détaché et serveur fermé. READY=false, boot WAIT sans auto-start. Les anciens fichiers et preuves, les worktrees et les deux stashes historiques sont préservés. Aucun accès FRAM/COM4/logiciel TR2-001 ni changement Windows de sécurité ; seul le capteur emprunté suivant décision utilisateur reste installé.

Le checkpoint précédent **ae098d301fd2e182e8d874f47425b715629ca3ab** a été relu intégralement : 64 fichiers identiques, 60 artefacts vérifiés ; preuve `new-sensor-wires-20261101/remote-readback.json` ajoutée. Départ de cette tranche : HEAD ae098d30, puis commit logiciel 4307b95c ; branche locale isolée codex/r3-8h-faststart, destination autorisée main. **Code et preuves publiés au commit d60832013b01b5f4ae3709a564859eab264eee71**, puis relus depuis origin/main le01/11 à06:16:46UTC : **90 fichiers intégraux identiques / 80 artefacts vérifiés**, images et readback exacts. Preuve `duplex4-new-wires-20261101/remote-readback.json`. Cette publication ne qualifie pas le candidat. La reprise de publication n’a effectué aucune opération matérielle ; l’état cible cité reste le dernier observé à23:41:29UTC. État Git après le push : worktree isolé propre et synchronisé, dépôt historique et stashes préservés. Bilan de relecture à publier puis vérifier, SHA final dans la réponse.

**Prochaine action utile :** distinguer les trames réellement envoyées et reçues sur SCK/CS/MOSI/MISO, avant nouvelle calibration ou gate. Aucun relevé de forme d'onde disponible ; une instrumentation physique relève d'AGENTS.md §9. Pas de nouveau flash identique ou garde abaissée pour partir. Après correction démontrée seulement : calibration, gate FIFO+SD et relecture complète, puis READY et signal LANCE8H. La mission n'est pas clôturée comme acquise. Journal technique actualisé ; rubriques suivantes historiques, leurs états READY/IN_PROGRESS antérieurs ne décrivent pas l'état actuel.

# Reprise après remplacement des fils : dépendance SPI observée, profil4MHz à qualifier

**IN_PROGRESS**, nouvelle piste testable en autonomie ; pasPRÊT8h. L'utilisateur confirme lesfils du **capteur seulement** remplacés, lecteurSD inchangé, mêmebrochage, VDD/VDDIO3,3V, STLINK/SDenplace, capteur encoreemprunté001. Aucunbuild/flash/resetagent danscettetranche. UID002/N8A54556B c319…3ddc17 exacts etWAITinitialobservés. **Initialisation refusée avantcalibration/SDinit : WHO0x7A, error91/run0/99.**

[Preuves et séquences exactes](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/new-sensor-wires-20261101/README.md). Contrôlehôte distinct : portsGDB61236/61237 dansplageTCPWindows exclue61235–61334 ; pasdeconnexioncibleinitialement. Ports23336/23337 libres utilisés, sansmodifierpolitique/protection/exclusions niSTLINKfirmware ; mêmesSN/attach/SWD. L'accèsGDBfonctionneensuite.

**Résultat discriminant enlectureseule, sansregistre-capteurécrit :**16WHO à8MHz tous0x7A ;16à1MHz tous0x7B puisretour8MHz16×0x7A ;16à4MHz tous0x7B puisretour8MHz13×0x7B/3×0x7A. RetoursHALtous0. PrescalerSPI3/handle seulsmodifiésvolatils, **retour8MHzeffectifavantdetach** ; WHO/error91 initiauxnonremplacés pardeslecturesfavorables. Pasdecalibration/gate/écritureSD/STARTlong. Cesrésultatsoriententversunproblèmedemarge/transportdépendantfréquence ; neprouventpasl'originecâblage/électrique/HAL exclusive ni la stabilitéencontinu4MHz.

**Suite annoncée :** checkpointpreuves puispréparerduplex4MHzdepuisvoieexistante, gardes etODR/FIR/gain/FS/v3inchangés, calibrationetgateFIFO/SDobligatoiresavantREADY. SPI4MHz désigneletransport, paslacadencemesure. Leséchecs4MHzsplit antérieurs restentacquis ; l'identificationcorrecte à4MHz nesuffitpas. SignalLANCE8Htoujoursrequispourlong, aucunenouvellemiseàlamasseoumodification physique demandéeàcestade. Données/prêtsensor001 etFRAM/logiciel001préservés.

Checkpointàpublieretrelireavantmodificationlogicielleversionnée ; rubriquessuivanteshistoriques.

# N8A exact remis sur TR2-002 — restauration vérifiée, calibration toujours refusée

À la demande de l'utilisateur, **l'image N8A sauvegardée a été reprogrammée sans reconstruction** : source `d5af0828e370780320bda67fac5d537bf3fd5016`, BIN **54556octets/SHA`c31904feca48fd06d6102eee40b54b373bbfe5e9896d0ddc007434cf253ddc17`**. IdentitéUID002/STLINK003200…3439 vérifiés ; ancienBIN D entier préservé/comparé, état sûr avantflash. Un flashstandard qualifié avecresetdeprogrammation, aucunmass-erase/OptionBytes ni resetsupplémentaire. **Les54556octets relus sont strictement identiques à N8A initial**, WAIT2/run0/error0 confirmé. Pasdepreuveprétendue surtousles2Mio deFlash.

Le contrôle prévu s'est limité à **une initialisation/calibration, commande1**, sans gateSD ni long. Il échoue de nouveau : **stage99/run1/error2/FIFO9, parserreason4** (5XYZ entre timestamps pour8 attendus),3brackets avantarrêt, WHO7B/SDerror0/stoperror0. AucunfichierSD créé, aucun ancienN8A0003 écrasé. [Preuves de restauration, GDB, résultats et bruts](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/restored-exact-N8A/README.md).

Les configurations SPI3(00070007/87440000) etSPI2(10070007/84400000) correspondent aux snapshots du succèsinitial. Les mesures etétats ne sontpasforcés pouradmettrelecapture. **Le capteur actuellement installé reste celui emprunté àTR2-001 suivantdéclarationutilisateur** : le mêmeBIN a doncétérestauré, pas l'ensemble des conditions physiques initiales. Ce résultat renforce que les modificationsrécentes du firmware nesuffisentpasàexpliquer lepremierdéfaut ; il n'exclutpasunproblème latent de transport/étatinitial/liaison/conditionscommunes.

RCC_CSR/sélections horloges/GPIO conservés avantflash etCSR après/final sansclearflags ; CSR0x1c004400 reflète desflagscumulés après lesresets antérieurs, **pasuneidentification rétrospective de lacause dupremierreset**. Cette collecte ne comblepasla lacunehistorique21:16.

**Étatfinalvérifié : N8A conservé enFlash, stage99/run1/error2, FIFOoff/writeroff/stoperror0/clockfault0/SDCShaute, GDBdétaché/serveurfermé.** READYfalse, préparation8hBLOCKED, aucunSTARTlong niqualification. Pasd'accèslogiciel001/FRAM/COM4/format/politiquesWindows, anciencapteur etfichierspréservés. Le `result_sha` désigne désormais le codeN8A réellement évalué surcarte ; HEAD documentaire120842b0 avantcettetranche. LesversionsdiagnostiquesetleurcodeGit nesontpasécrasés ourebasés.

Restaurationphysique réalisée selonordreuser ; contrôlecourtréitéréjustifiéparchangementd'image/deconditions, aucun ancienbuild/testhôte/canonique relancé. Pasd'autrevarianteouessaipendantcettetranche. Rapport/journal/preuves publiés91501661 puis50fichiers relusintégralement/46artefactsbruts conformes aprèsdécompression siapplicable ; preuve restored-exact-N8A/remote-readback.json, bilanfinalàvérifieretSHAfinaldansréponse ; rubriquessuivanteshistoriques.

# Étude demandée : repartir du dernier succès, sans nouvel essai matériel

Analyse hors ligne sur HEAD97fa3d0a. [Différentiel complet](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/last-working-review/ANALYSE_DIFFERENTIELLE.md), [comparaisons exactes](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/last-working-review/COMPARISON.json). Aucun build, flash, reset, debugger, Windows/COM/SD ni répétition d'essai réussi ; seul un script de lecture/comparaison des preuves conservées a été exécuté, code0/assertions satisfaites.

**Référence la plus récente : N8A a encore réussi avant le déplacement**, à18:19–18:22UTC hôte : calibration16µs, gate2s et relectureFatFs/SHAamont conformes. La6hE6N reste le dernier succèslong, avec ses réserves historiques et le lieuPCmachine rapporté par utilisateur. Le profil8h30600sMCU était donc déjà actif et fonctionnel lors du gateA.

**Preuve binaire : A→B ne change que36octets de provenanceSHAASCII/préfixe ; B→C ne change qu'un octet du nom.** Tous les autres octets des imagesA/B/C, options compilateur et définitions hors ces champs sont identiques ; BINlocaux identiques aux copies déjà publiées dansGit. Les snapshots SPI3(00070007/87440000), SPI2(10070007/84400000), UID/CID et masquesIRQ sont identiques. Cela écarte une modification de code exécutable comme différence entre dernier succèsA et premier défautB, **pas un défaut latent de transport/état/timing**.

Chronologie importante : déplacement rapporté → RAMreset observée21:16 avec FlashA intacte → nouveauflashB21:30 → **calibrationB réussie40µs** → erreur de parité au gateB0,205253s → Ctaginconnu. Duplex, fronts,4MHz et capteur001 sont tous ultérieurs au premier défaut. La réussite6h déjà auPCmachine interdit aussi de traiter le seul lieu «machine» comme cause démontrée. Cheminement réel, contraintes, masses et conditions électriques environnantes non caractérisés comparativement.

Limites : **les flags RCC de cause du premier reset n'ont pas été sauvegardés avant les reflashes** ; sa cause exacte reste inconnue. L'imageAexacte n'a pas été réessayée après déplacement. Le chemin de démarrage n'inclut pas un reset global explicite du capteur ; resetMCU et coupure globale ne prouvent donc pas le même état capteur. Les erreurs de tags et de timestamps/configuration des variantes ultérieures ne prouvent pas une cause unique.

Suite proposée, **non exécutée** : revenir à la référenceAexacte et aux conditions matérielles identifiées, relever les états initiaux pertinents puis contrôler seulement l'initialisation/calibration, sans écrire de fichier ni écraserN8A0003 ; toute modification d'état capteur doit ensuite être isolée comme un facteur. L'étude ne conclut ni «câblage fautif» ni «capteurinitial défectueux» ; elle recentre le diagnostic sur le changement d'état/conditions et les défauts préexistants possibles. **BLOCKED de préparation8h reste exact**, étude documentaire terminée mais aucune réparation/qualification physique nouvelle. Matériel laissé dans l'état sûr précédent, sans intervention lors de cette étude.

Journal actualisé avec ces preuves et lacunes. Étude et rapport publiés au commit a3d6535be4b1973fe59f63904fd87ac9dafd785d, puis six fichiers relus intégralement depuis origin/main et comparés aux fichiers locaux : tous identiques. Preuve last-working-review/remote-readback.json ; publication finale de cette preuve à vérifier, SHA final dans la réponse. Les rubriques suivantes restent l'historique des essais antérieurs.

# Comparatif capteur de TR2-001 — toujours refusé, réponse immédiate avant départ

L'utilisateur confirmecapteuréchangé etdépartdans5minutes. Contrôle effectuéimmédiatement avecimageDexistante, **aucunbuild/flash**. IdentitéUID002 etimageBIN889c551f…43cfe/53032B entièrementconformes ; RAMd'abordencore99/run1/error1 antérieur. Deuxgardespréflight/refusavantcommande ontpréservécetétat, puis **unresetlogiciel nécessaireàréarmement**, aprèsvérificationarrêtsûr/image, sansforcervariablesmétier. Repriseinit/calibrationunefoisdepuisWAIT.

**Résultat : error7/stage99/run1, résidutemporel1875862µs > seuil100µs.** WHO7B,6577ancresFIFO/198brackets,0FIFOerror/0SDError ; calibrationnonvalide. **AucungateSD, aucunSTART8h, aucuneécriturefichiernouveau.** Le même symptôme persiste avec lecapteurderéférence rapporté paruser : onnepeutpasconclureàundéfautpropredupremiermodule. Liaison/transport/configuration/conditionscommunes restentàdépartager ; remplacerlecapteur n'apasrétablilapréparation.

[Preuves](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/reference-sensor/README.md). Étatfinalcontrôlé : FIFOoff/stoperror0/writeroff/clockfault0/SDCShaute/GDBdétaché. ImageDduplex8MHz/HIGH conservée/bootWAITsansautostart, READYfalse. Pasd'accèslogiciel001/FRAM/COM4, donnéeshistoriquespréservées. Capteur001emprunté suivantautorisationuser, référencephysiqueexactenonfournie/WHO7B neconstituepasunidentifiantunique. Pasdequalification8h ni miseàjourV1 ; BLOCKEDdemeureunobstacleconcret avantacquisition, pasuncheckpointlogicielprésentéprêt.

Réponseimmédiateuser : **NONPRÊT**, recueilliesetpréservées, aucunessaisupplémentaireaveugleavantappareillage. Pourpoursuivre, ilfautuncontrôlediscriminantduSPI(SCK/MISO/CS, références/liaisons/transport) ouuneautrevoieexplicitementprévueetqualifiée, sansignorerlesrefusdelacalibration. Checkpointàpublierpuisrelire, SHAfinaldansréponse.

# 3,3V rapportés et comparaisons SPI — tous refusés, capteur de référence proposé

L'utilisateur rapporte **VDD=3,3V/VDDIO=3,3V** au multimètre. Mesurestatiqueutilisateur, pasqualificationdesfrontsSPI. Investigation ciblée surtracesdéjàsauvées etcode : précédentR4-Udebitshiftapparent avecvoieHALduplexexistante. Ajoutminimum **R3_SPI_DUPLEX** opt-in OFFpardéfaut, limitéauprofil8H ; aucun nouveaupilote, algorithme/gain/FS/ODR/FIR/v3/importeur/contrôleschangés. Codeévalué **98457b5aa5c82dc44fe2305225790234654ea2c1** ; buildsARMciblésréussis, configurationprécommitpuisrebuildSHAcommitépourimageexacte, aucunancienPASSrépété. Validationcanoniqueancienne réutilisée car cibleinchangée ; physique requise avanttouteadoptiondevariante.

[Traces, images et résultats distincts](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/duplex-differential/README.md), [synthèse](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/duplex-differential/SUMMARY.json).

| Comparaison | Observation réelle | Issue |
|---|---|---|
| Duplex8MHz, HIGH, nouveauN8D | BIN53032B/SHA889c551f625b9d5d5594b637a535446ed71455fbf46722b469d71cfdd6e43cfe ; flashstandardunique/readbackexact.6625ancresFIFO/197brackets pendant2s, aucuneerreurFIFO, mais timestampsdirectsincohérents/résidu1842816µs | error7, garde100µsconservée, calibrationrefusée |
| Mêmeimage8MHz, SCKMEDIUMvolatile | Aprèsunresetlogicieljustifié, PC10OSPEEDR seulHIGH→MEDIUM ; calrésidu214061379µs | error7, garde100µsconservée, aucunbénéficeétabli |
| Mêmeimageduplex4MHzvolatile/HIGH | Aprèsunsecondresetlogicieljustifié, SPI3désactivéauWAIT/prescalerMBR1(HSI16/4), ODRinchangé | error1/FIFOconfig6 avantcalibration, refus |

AucunSTARTlong ni gateSD ni nouvellecréationfichierN8D pendantcescomparaisons, calibrationscommand1/seulementlecturesSD ; contrôleabsencedesnomsnonatteint. Aucunefaussequalification. Le duplexnepeutpasêtreprésentécommecorrectionphysiquementvalidée. Lesrésultats n'isolentpasune causeexclusivecapteur/nappe/électrique/transport ; pasdepreuveCEM. Touslesrefus etbrutspréservés, pasdebitcorrigé/mesureignorée pourforcerPASS.

**Étatfinalréel : imageDduplex conservée, stage99/run1/error1, FIFOoff/writeroff/SDCShaute, card.failedfalse/clockfault0/GDBdétaché.** Overridesvolatilesrestaurésàl'image (SPI3=8MHz/HIGH) sansreset/flashsupplémentaire ; bootWAITsansautostart. Unflashstandard+deuxresetslogiciels danscettetranche ; pasmass-erase/OptionBytes/format/FRAM/COM4/TR2-001logiciel. READYfalse, BLOCKEDjustifié : aucuneconfigurationtestée n'admetcalibrationetcapturedepréparation.

**Dernièreinstructionutilisateur : emprunterlecapteurdeTR2-001 pourcomparer.** Décisionexpliciteconsignéecommeexceptionlimitéeauprêtphysiquedumodule ; aucuneautorisationdéduitepourflash/debug/FRAMdeTR2-001. Interventiondemandée : deuxbancshorstensionavantretrait, mêmeréférence/connecteursconfirmés, moduleuniquement surTR2-002/nappe/brochage/3,3Vinchangés ; sicompatibilitéincertaine, préciserréférencesavantrecâblage/retoursoustension. ReconnecterSTLINK002seul/CN15off/SDenplace puisconfirmation. Pasdenouvelessaid'ici-là. Reprendresansbuild/flashsiimageDintacte, mêmecondition8MHz/HIGH pourcomparaisonnechangeantquelecapteur. Toutchangementcapteur/pose seratracédansprovenance, pasdéclaréidentiqueaux6h.

Code98457b5a etpreuves05b01525 publiés, 139fichiers relusintégralement/132artefactsbrutsconformes aprèsdécompression siapplicable ; preuve remote-readback.json. Bilanpublication final àvérifierpendantlapréparationphysiqueutilisateur. MissionBLOCKEDsurinterventioncomparative, pasDONE niSTART8h. Rubriquessuivanteshistoriquespréservées.

# Après réenfichage utilisateur — défaut reproduit, mesures électriques attendues

**Retour utilisateur : nappe etSTLINKdébranchés/rebranchés, contrôlevisuel sansanomalie.** Le firmwareN8C est intact : relectureintégrale54556B identiqueauBIN0844bc7a…2009a, UID002 etWAITinitial corrects. Réutilisation de l'image/ELFexistants, **aucunbuild/flash/resetCodex** lorsdecettevérification.

Une seule nouvelleinitialisation/calibration aprèsintervention : WHO7B/carteinitialisée/montageFAT32, mais de nouveau stage99/error2/FIFO9/**reason6 tagnonreconnu**. Quinze brackets avantéchec, aucun résultatdecalibrationvalide, **aucun gateSD ni STARTlong ni nouvelleécritureSD** (writes0). La récidive intervientpendantcalibration, sanswriteractif ; elle ne démontre pasunproblèmedechargeSD. Causeexacteinconnue, défautSPI/banc à départager, contrôlevisuel nequalifiepasl'électrique. [Preuves](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/after-user-replug/README.md).

Étatfinal vérifié : FIFOoff/writeroff/card.failedfalse/clockfault0/CShaute, GDBdétaché/serveurfermé. READYopérationnelfalse, statutBLOCKED conservé, aucunlong. L'utilisateur dispose d'unmultimètre et prépare lesmesuresVDD/GND etVDDIO/GND au modulecapteur (attendu≈3,3V chacun), bancalimentémaisarrêté, sansdéplacerlesfils. Attendrecesvaleurs avanttouteopérationdépendante. Aucune calibration/acquisition ne sera relancée pendantmesures. Lesréserves, échecs et fichiersprécédents restentpréservés.

Checkpointdocumentaire àpublier/relectureensuite, sansattendrelesmesures pourpréservercesconstats. Lesrubriquessuivantes décriventlesétapeshistoriques.

# Réarmement demandé — BLOCKED sur défaut de tags FIFO, intervention physique attendue

**Aucun long lancé. La préparation n'est pas rétablie.** À la demande explicite de réarmement, deuximages à nouveauxpréfixes ont été construites/flashées par procédurestandard et relues intégralement conformes ; aucune garde, fréquenceSPI, FIR/gain/FS/format/importeur modifiés. Images/source75ea886c, sourcefonctionnelleidentiqueauprofilacquis. Deuxflashsstandardsavecresetdeprogrammation danscettetranche, aucunresetsupplémentaire. N8A/précédentgate/donnéeshistoriques intacts ; pasdesuppression/formatage.

- **N8B** : BIN91b191228bf23b35bfad3aa41ae8863f45deefca69f269188f4bafc622a225c3,54556B. UID/SN/WHO7B/CID/FAT32 conformes, calibration40µs, nomsN8B0003/5absents vérifiés/libresuffisant. Un seulgatephysique tenté, arrêté0,205253s : error5/FIFO9/parserreason1 (paritétag),5168natifs/517sorties,0rails/clips/FULL-OVR, SDerror0 mais0groupeacquitté. STOP153µs, FIFOoff/SDquiescente. FichierN8B0003préalloué/header/invalidfooter àpréserver, pasCOMPLETE ; aucunevalidationcourte.
- La rafale fautive a été extraite enlectureseule : tagsvalides au début puis tagsincohérents avecdécalageapparent, causeexactenonisolée. Pas de correctionspéculative/filtrage des erreurs.
- **N8C**, uniquereprisebornéejustifiéeparéchecB : BIN0844bc7abdaffc163010e59526a92294ce492e31a805b0ddba4ca937a732009a,54556B. WHO7B/carteinitialisée, mais nouvelleerreur dès calibration : error2/FIFO9/parserreason6 (tag nonreconnu),0sortie/0écriturecapture. PasdegateC niSTARTlong ; absence desnomsCnonvérifiée (contrôlenonatteint).

[Preuves séparées, manifestes, builds, images, GDB et dumps](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/rearm-after-reboot/README.md). PremièreinvocationhelperB stoppéeavanttrigger pournative-host.jsonmanquant : lecturehôte effectuée ensuite ; ce n'estpasunpremieressaiphysique caché. Leséchecs sont conservés, pastransformés enPASS. Validationsantérieuresréussies nonrejouées ; contrôledecode parlecture etcompilationciblée dechaquenouveauprofil uniquement, sansnouvellecampagneannexe.

**ÉtatfinalN8C vérifié : stage99/run1/error2, FIFOarrêté/writerinactif/card.failedfalse/clockfault0/CShaute, GDBdétaché/serveurfermé.** READYlocauxfalse, aucunverrouSTARTlong. Défaut de liaison/lectureSPI suspecté, pasdecausalitématérielleaffirmée. Leserreurs ne viennentpasd'unclip ou d'unSDerror observé ; ilestinterdit delesignorerpourpartir.

**Interventiondemandée selonAGENTS.md§9 :** utilisateurcoupe uniquementSTLINKTR2-002, vérifie/réenfichelesconnecteurs de lanappecapteur auxdeuxextrémités etcontacts3,3V/GND, sanschangerbroches/pose ; SDresteenplace/USBnatifdefbranché, puisSTLINKreconnectéetretourutilisateur. Arrêtsûrconfirméavantcetteaction. Aucun nouvelessai avantconfirmation. Aprèsretour, vérifierétat/imageCconservée, réutilisersansflashsiconforme, relirecarte/noms puisvalidationcourteadaptée ; pasderelanceautomatique du longni de nouveauflashaveugle.

Rapport/journal/preuves sontpubliés commecheckpointBLOCKEDréel surinterventionphysique, pasDONE niREADY fictif ; checkpoint8446c221 publié/relu intégralement (145fichiers/141artefacts conformes aprèsdécompression siapplicable), preuve remote-readback.json ; bilanfinalvérifiéetSHAfinaldansréponse. AucunTR2-001/COM4/FRAM/serviceWindows/OptionBytes ni donnéespersistantes anciennes modifiés. Lesbilanssuivants sonthistorique et ne décriventpasunétatprêtactuel.

# Actualisation après installation — 31/10/2026 21:16 UTC

**L'état PRÊT antérieur est invalidé.** À la demande de vérification RAM de l'utilisateur, observation GDB sur UID002 conforme : WAIT initial stage2/run0/error0, compteurs0, FIFOoff/writeroff/file0, montageFATnoninitialisé, digestproducteurRAM32octets nuls. Un redémarrage a donc eu lieu depuis la préparation ; sa cause n'est pas démontrée (pas d'attribution certaine à une coupure). Le firmwareN8A préparé a été relu entièrement :54556octets strictement identiques auBINc31904fe…3ddc17. Aucun reset/flash/START/commandeSD exécuté pendantcecontrôle ; debuggerdétaché.

Le READY.json opérationnel local est passé à prepared=false, ancienne version conservée ; le lanceur refuse donc un départ prématuré. Le dossierpreuves historique et le gateSD ne sont pas réécrits ; la carte et ses fichiers n'ont pas été relus lors du contrôleRAM. [Observation et traces](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/post-install/POST_INSTALL_RAM.json). MissionIN_PROGRESS, pasblocageextérieur identifié. **Àfaire avantLANCE8H : réarmer une voie de départ compatible avec le reboot et le gateN8A0003 déjàcréé, sans l'écraser ni forcer run3 enRAM.** L'estimation antérieure20–30s suppose un étatpréparé conservé et n'est plus applicable àl'étatactuel. Cette tranche répond seulement au contrôleRAM demandé, aucunecapture nouvelle.

Le bilan ci-dessous décrit la préparation historique avant déplacement ; ses mentions PRÊT ne décrivent plus l'état courant. Actualisation àpublier puis vérifier surmain, SHAfinal dansréponseutilisateur.

# Phase A préparée — attente du signal LANCE 8H

**Préparation matérielle et logicielle acquise ; aucun long START ni auto-start.** La mission reste IN_PROGRESS : phaseB attend uniquement le signal explicite `LANCE 8H`, puis phaseC différée. Aucun blocage technique identifié. [Note PRÊT_8H et commande exacte](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/PRET_8H.md).

Image8h/source **d5af0828e370780320bda67fac5d537bf3fd5016**, BIN54556B/SHA **`c31904feca48fd06d6102eee40b54b373bbfe5e9896d0ddc007434cf253ddc17`**, ELF/SHA`45831eccc8744bce99c5f2c51b5d79564ee9c6416c6986d7a47f6e733376fbcf`. Trois options8H/QPC/1H actives, préfixeN8A/garde30600000000µsMCU/640MiB ; v3/int16/FIR107Q15/décimation10/gain/±8g logique/importeur/gardes unchanged. Sources de la chaîne comparées parGit àc3cd40ce : identiques. Aucune modification firmware fonctionnelle ni nouveau calcul scientifique.

## Faits et validations

1. Git réel vérifié : dépôtprincipalhistorique82169af propre, anciens worktrees/deuxstashes6251b988/f3b829e6 conservés. Worktreeisolé MSM_vibe-R3-8h-faststart créé surorigin/main d5af0828. Gouvernance/journal/précédentrapport et chaînes6h/8h consultés, archivesPDF préservées ; pas de requalificationR5.
2. ST-LINK0032001F3235510C37333439 et UID001B0046/38425018/20333157 réels, ancienBIN6h b704…5330 relu intégralement conforme/WAITinitial sûr. Utilisateur a confirméSTLINKbranché etSDinsérée ; pas de gestephysique supplémentaire demandé.
3. BuildARMciblé CMake du profilN8A réussi, code0, BIN/ELF/MAP hachés. Tests/cross-build canonique déjàPASS surcodeinchangé réutilisés, logs inspectés et comparaisonsGit publiées ; pas de suitehôte/ARMcanonique relancée. Le profil/date/nom/image nouveaux justifient le gate physiqueunique. Warningsnosys _read/_write linker conservés dans sortie ; buildréussi.
4. **Un flashstandard qualifié** à0x08000000 avec verification/reset normal ; zéro mass erase/OptionBytes/RDP, zéro resetsupplémentaire. ImageFlash54556B intégralement relue identique/WAITstage2, puis initialisation explicite et calibration horloge autorisées, WHO7B. Ce succès ne vaut pas8h.
5. CID0353445344333247858bf0923c01927b/CSD/capacité62333952secteurs/FAT32/csize64 exacts. N8A0003 et5 réellement absents avantwrite ; libre927219clusters/long20480requis. Aucun format/réparation/suppression ; seulnouveaufilegateN8A0003 créé. Anciennesarchives non ciblées, leurs empreintesSD n'ont pas été intégralement refaites danscettepréparation.
6. **Gatecourt unique**2,000357sMCU,52936XYZnatifs/5294sorties/31764B, session6755700148813124 ; QPCintérieur1,0422059s. ZéroFIFO/SD/queue/rails/clips, FIFO46/file5120B ; work684.281/service1262.013/gap1521.000µs sous1000/2500/5000, STOP152µs/drain9277µs. Oracle103sortiespréfixe exact. **RelectureintégraleFatFs du gate** CRC/index/session/compte/footer/SHA vsdigestproducteurRAM :PASS. Pas une extractioncartePC ni preuve8h.
7. TémoinWindowsnatifpassif QPC/COM5 lié auSN002 et générationnativeCIM démontrés surgate ; COM5exclusif reprislibre ensuite. Secteurtrue, veilleAC0/hibernationAC0 observés, aucunepolitiqueWindows modifiée. GDBetserveurs fermés, aucun moniteurCodex/WSL laissé.
8. Lanceur `launch_8h.py --check-only` réellement exécuté sansSTART : UID/image/idle/CID **relectureCMD10**/FAT32/libre/nomlongabsent/secteur/veille/COM5exclusifPASS. Gardesd'entréehorslignePASS : signalabsent/erroné etétatnonprêtrefusés, verrou exclusif conservé aprèséchec, deuxièmeinvocationrefusée avantmatériel. Aucun STARTlong simulé présentécommephysique.

Commandes/sorties exactes : [preuves](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/README.md), build.log, launcher-guards.log, prepared-launch-check.log, GDBtraces et analysesdugate. Helpers locaux `/home/lolo/dev/msm/reports/TR2-r3-8h-faststart-20261031/`, reproduits danspreuves/helpers. Les nouveaux gardes complètent le lanceur existant ; aucune refonte du système ni normeV1 modifiée.

## État laissé et départ

**TR2-002 idle stage4/run3/error0, FIFOarrêté/writeroff/filevide/CShaute, GDBdétaché, imageN8A conservée.** N8A0005.TR2 absent, aucun verroulong ni témoinlong lancé. Le gate et les contrôles sont terminés ; au signal, pas de nouveau build/flash/campagne/Git/revueR5 avantSTART. Préflightbref seulement, puis commande5unique et confirmationACTIVE/aprèspremierSDgroupACK. Lancementetlatence ne sont pas encore démontrés ; mesures à consigner enphaseB.

**Garder cette session alimentée sansreset et sanschangerlacarte**. Un redémarrage perd le gateRAM/état3 et sera refusé, pas réparé automatiquement au moment du signal. `LANCE 8H` confirmera fixationplancherPCmachine/configurationidentique etalimentationmaintenue seloncontrat. Aucune validationintermédiaire supplémentaire sollicitée. Ne pascouperpendantcapture.

Duréefinale à démontrer parQPC≥28800sintérieures etfichiersPC ; marge30600sMCU inchangée. Pas decollecteurautomatiqueduSHAproducteur déjàqualifié identifié : témoinQPC ne contient pasceSHA, donc revenirversCodex aprèsSTOP pourdumpRAM avantcoupure si possible. Unecoupure aprèsquiescence préservepotentiellementleFATmaisperdcettedémonstrationindépendante, comme6h. Clips/rails restent qualitédégradée/error26, jamaisfauxCOMPLETE.

Permissions exercées : debug/lecture/init/gatecourt/flashstandardunique etédition/commit/pushmain ; aucunTR2-001/COM4/FRAM/serviceWindows/SDhistorique/format/OptionBytes. Prochainesétapes : signalutilisateur, STARTunique, libérerCodex ; puisrécupérationdifférée etqualificationindépendanteChatGPT. AucunDONE8h/préqualificationSI/V1/production.

## Publication de préparation

Checkpoint léger PDF/anciennedonnées inchangés ; imageBIN etpreuvescompactesseulement. **Checkpoint `60030c5f902773247b61f817d35d81958036f141` publié par pushnormal surmain et relu intégralement après `git fetch origin main` : 56fichiers/327333octets identiques, 17sortiesbrutes décompressées/hachées conformes**, manifeste complet vérifié. [Preuve](preuves/TR2-20261031-R3-8H-FASTSTART-PC-MACHINE/remote-prepared.json). Le présent bilan et cette preuve sont publiés dans un dernier commit vérifié séparément ; SHAfinal dans la réponse utilisateur. **IN_PROGRESS reste le statut exact : préparationPRÊTE, phaseBnonlancée.** Journalactualisé avec image/étatprêt/gate/attentesignal/risqueperteRAM. Branches/stashes historiques préservés, aucun merge/rebase/forcepush/nettoyage.
