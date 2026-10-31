---
mission_id: TR2-20261030-R3-8H-FAT32-AUTONOME
status: IN_PROGRESS
base_ref: main
base_sha: 7dcceb510862be2d6a25c2c9fa82d2a2e8741f70
initial_head: ec42b9e55e30b3dacd8167b9cc085d90794f185a
result_sha: c3cd40ce9e96cef2241fb5bc6e8c83acd2a3824e
created_at_utc: 2026-10-31T00:28:35.011739+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---


# Acquisition exploratoire6H lancée — Codex quitte après publication

**IN_PROGRESS : acquisition active observée, durée6H et récupérationPC non encore démontrées.** Un uniqueSTARTlong a été réellement effectué surTR2-002. Premièreactivité/écritureSD acquittée observée à **2026-10-31T00:25:17.363316+00:00 UTC**, soit **01:25:17 heureParis**. Après le push/relecture de ce checkpoint, **ce tourCodex se termine** : aucun outil/moniteurCodex, WSL, GDB ou commandePC de contrôle n'est laissé sixheures. Le STM32 acquiert, exécute STOP et vide ses écritures seul ; seul l'instrumentQPC Windows natif passif reste actif.

## Décision et paramètres conservés

Autorisation utilisateur explicite : nouvelleacquisition **6h de données**, sanscorrectionducodepourlimiterlasaturation ; microSD/STLINK confirmés, montage définitif protégé et alimentationmaintenue/CN15débranché/T1000secteur sansveille confirmés. Contratec42b9e5 et permissionscommit/push/flash/debugtrue, targetmain conservés ; instruction directe6h précise la durée, **pasqualification8h**. BlocageP1surformat reporté : **FIR107/Q15 /10, gain,244µg/compte, ±8g, v3int16 et importeurstrict inchangés**. Gardebc0f empêcheuniquementunfauxfooterCOMPLETE enrails/clips ; aucunflag masqué ni gainbaissé.

Firmware physique **c3cd40ce9e96cef2241fb5bc6e8c83acd2a3824e**, BIN54552B SHA256 **b70438c7930ee2d928ec441a71a66ee23305d0fe5a102b2bc29e1453b6925330**, ELF778e901d6f7989e99c2ced9c78341c21b7817112588f548695b522234061a7fc. Unseulflashstandard/vérification/readbackBINentierexact, UID/SN/WAIT conformes. Duréegarde **22000000000µs nominalesMCU**, QPC **21601s intérieurs**. Arrêtestimévers **07:29 heureParis**, pasuneattestationd'heuredefin. Sourceoscillateur nonétalonnéeSI ; seul le futurQPC attestera l'intervalle actif.

Nouveau **E6N0005.TR2/640MiB**, gate **E6N0003.TR2**, noms8.3 vérifiésabsentsviaFatFs avantécriture, free947703clusters/32768B ; fichiersE8etdixhistoriques+Info intouchés. Profil garde-tempsu64/préallocation déjàéprouvé conservé, bornescampagneinférieuresàl'enveloppe8H antérieure, aucuneFATbloquante pendantFIFO. SPI3 8MHz/SPI2 4MHz, file64KiB/quantaFIFO64/SD128B/groupes1..8 etbudgets1000/2500/5000µs inchangés.

## Preuve immédiate du vrai START

ST-LINK **0032001F3235510C37333439**, UID **001B0046/38425018/20333157**, WHO7B. CarteCID **0353445344333247858bf0923c01927b**, CSD400e00325b590000edc87f800a4040c3,62333952secteurs/FAT32. Avantcommande5 : run3/stage4/error0/clock_fault0/FIFOoff/writeroff/CShaute. La commande5 relit réellement le fichiercourt **FatFs+CRC+SHAproducteur** avantd'ouvrirlenouveau fichierlong ; sinonlefirmwarenepourraitpaspasserACTIVE.

Premièretrame indépendante **QPC2 ACTIVE/run5**, CRC/UID conformes, **séquence42/session8444550407367521**, **12464XYZ natifsacceptés**, tempsMCU **497843243µs** ; dansceprofil ACTIVEexige déjà `sd_written>0`, doncunpremiergroupeSDacquitté. QPC10MHz : send **12699926269069**, receive **12699926907772**. Secondpoint attenduaprès21601sQPCdanslamêmesessionactive ; aucune6H déjàqualifiée.

GDBdétaché justeaprèstrigger etserveurterminé/lanceurretourné. ObservateurnatifWindows **PID7032**, propriétaire **lolo**, parentWmiPrvSE.exe natif observé ; lanceurponctuelPython/PowerShell terminé. Dossier **`%TEMP%\TR2-R3-6H-20261031-six-long\`** ; ready.json/queries.jsonl/summary.json. Instrumentqualifié inchangé, CIMlocal/aucuneélévation/politique/exclusionWindows, COM5liéauSN002, COM4refusé ; ilnecommandejamaisSTART/STOP. T1000PowerOnlineTrue etveille/hibernationAC0 luspourcettecampagne, aucunréglagemodifié. Sourcealimentation STLINKUSB→NUCLEO3,3V→capteurSD ; conserverjusqu'auxrelevésRAMfinaux.

## Validations et checkpoints conservés

- Préparationc3cd40ce publiée/relue72fichiers18artefacts. Nouveau test hôte durées6Hu64/salvageclip/CRC/SHA/session/recorddéchiré PASS ; ARMprofil exact et `bash ./tr2_validate.sh --cross-build-only` PASS. Harness/configurationseuls, aucune régressioncœur plausible ; testsanciens inchangés nonrejoués. Un contrôleGDBnoms aéchoué surunsymboleéliminéparlinker **avantI/O**, scriptcorrigé sansrecalibration/reset ; preuvesnégativespréservées.
- **Gatecourt physique neuf PASS** :2,000809sMCU/52952natifs/5296sorties/31776B,68records,0FIFO/SD/file/temps/rail/clip, QPC1,0763139s, session6755700272993600 etterminal772. Work/service/gapmax713,29375/1319,025/1577µs, SD491,6125µs, queue5120B,pile1104B,drain9696µs ;103sortiesoracle/préfixe128. Données/mesures danssix-hour/gate ; relectureSD/FatFsdugate exécutée avantceSTART, extractionPCaprèslongtoujoursrequise.
- **Gatepubliéfb8a36a1193bf3916b65a327a035b2b0ba99a823**, relecture175fichiers/119artefacts exacts ; sourceFIR/coefficients/formatetimporteurstrict comparésàec42inchangés. RapportavantSTARTconservédanssix-hour/RAPPORT_GATE_AVANT_START_6H.md ; rapportP0avantcampagne conservéséparément. Preuves compactes, aucune re-publication des530Mo dupremier8h.

## Reprise obligatoire après fin, sans nouveau START

Voir **experiments/r3_durable_fat32/REPRISE_6H.md** et `/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/REPRISE_COURANTE.md`.

1. Reveniraprèscapture etlirelesjournauxexistants via `read_six_after.py`, sansrequêteMCU/reset/flash. Témoinabsent/erreur nonexpliquée : pasdeSTOPsupposé, pasderetrait ; inspecterlespreuvesexistantes.
2. Terminalobservé : sauvegarder **SHAproducteurRAM32B avant toutecoupure** via `recover_terminal.py`. Gardesstrictesuid/stage4error0 oustage99error26/FIFOoff/filevide/writeroff/SDsafe/CShaute, dumpcompteurs puisdetach. Erreur26 resteenregistrée, jamaistransforméeenPASS. Le bufferRAMfooter peutencoreêtreceluidugatecourt siqualité26 ; nepasleconfondreavecunfooterlong. Le nouveaufichier a sonfooterinvalidéavantcapture.
3. Seulementaprèsceconstat sûr, demanderletransfertphysique SDverslecteurPC horsalimentation002 etattendreuser. Copierlesoriginauxintégrauxlocalement, SHAsource/copie etlecturecomplète tousCRC/index/session/temps/count/SHAutilesvsRAM. Clipsconservés commequalitédégradée, préfixeINCOMPLETE récupérableetsanspertemasquée ; aucunequalificationrétroactive. Re-hacherhistoriques/Info ; preuvesGitHubproportionnées, pasd'uploadautomatiquedescentainesdeMo.

**ÉtatGit de ce checkpoint :** worktreeMSM_vibe-R3-6h-data, branchecodex/r3-6h-data baséeec42, sourcec3 puisgatefb8 ; autresworktrees etdeuxstashespréservés, pushnormalmainuniquement, pasdeforce/reset/merge/rebase. Journalmisàjour avecSTART réel/étatmatériel. **PasDONE**, pasdequalification8H/V1/SI/powerloss/FRAMRTU. TR2-001/FRAM/COM4 intouchés. Ce checkpointdoitêtrepublié/relu puisCodexquitte ; futurretourpourconstatsaprèsfin, **aucune nouvellecaptureautomatique**.
