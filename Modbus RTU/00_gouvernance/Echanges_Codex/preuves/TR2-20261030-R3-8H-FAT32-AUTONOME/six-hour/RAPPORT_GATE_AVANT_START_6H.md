---
mission_id: TR2-20261030-R3-8H-FAT32-AUTONOME
status: IN_PROGRESS
base_ref: main
base_sha: 7dcceb510862be2d6a25c2c9fa82d2a2e8741f70
initial_head: ec42b9e55e30b3dacd8167b9cc085d90794f185a
result_sha: c3cd40ce9e96cef2241fb5bc6e8c83acd2a3824e
created_at_utc: 2026-10-31T00:15:00.781018+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Campagne exploratoire de six heures — préparation acquise, START non encore exécuté

**Décision utilisateur nouvelle :** « lance une nouvelle acquisition de6h sans modifier le code pour limiter la saturation. Je veux juste acquérir des données ». L'utilisateur confirme microSD dansTR2/ST-LINK, puis montage définitif sûr et alimentation maintenue, CN15 débranché/T1000secteur sans veille. Cette instruction précise la durée de l'addendum exploratoire **ec42b9e5** : campagne6H de données, **pas qualification8H**, pas v4 ni correction du FIR. Le blocage P1 de représentation est différé par cette décision explicite ; le premierE8 reste REJECTED_QUALITY et son rapport exact est conservé soussix-hour/RAPPORT_AVANT_CAMPAGNE_6H.md.

## Modifications minimales et état logiciel

Même FIR107/Q15 /10, gain,244µg/compte, capteur±8g, formatv3/int16, SD/FIFO/quanta/file64KiB et importeur strict. Aucune modification destinée à réduire les saturations. Le gardebc0f déjà préparé est conservé : après STOP/vidange et digest producteur, rails/clips empêchent le footer de succès ; préfixe récupérable INCOMPLETE et compteur de qualité préservé.

Seuls paramètres du profil v3 rendus configurables : `R3_CAPTURE_DURATION_US` et `R3_CAPTURE_PREFIX`, valeurs par défaut historiques30600000000/E8N conservées. Campagne sélectionnée **22000000000µs nominalesMCU / E6N**, QPC passif **21601s** à l'intérieur de la capture. La garde22 000s n'est pas du SI ; selon le ratio antérieurement mesuré, arrêt attendu vers6h04 après START, marge pour ≥6h indépendantes, sans commande PC de STOP. Durée enu64, même archivev3 ; pas de promesse d'arrêt à la seconde civile exacte.

Nouveaux fichiers **E6N0003.TR2** (gate2s) et **E6N0005.TR2** (long), uniquement s'ils sont absents. Préallocation640MiB conservée dans l'enveloppe déjà qualifiée30600sMCU, supérieure à la fenêtre22 000 ; à titre de budget conservateur30 000XYZnatifs/s donneraient66 000 000sorties et433 231 872B engagés, <671 088 640B. Ce budget n'est ni fréquence SI certifiée ni garantie arbitraire ; allocation et place libre sont contrôlées réellement parFatFs avant acquisition, dépassements restent fail-closed. Aucun reformatage/écrasement/réemploiE8 ni historique.

## Validations réellement exécutées

- Nouveau test hôte **test_reader_campaign_guard.py** : duréeu64 de22e9µs, fichierv3 sans footer de succès, flagsclip et préfixeCRC/count/identité/session/SHAproducteur externe conservés, record déchiré jamais compté comme valide. PASS surfixture ; aucune acquisition physique n'en est déduite. Les anciens tests inchangés ne sont pas rejoués.
- Cross-build ARM du profil exact6H/E6N PASS (avant commit, SHAembarqué à reconstruire ensuite). Cœur fonctionnel inchangé : validation proportionnée **`STM32CUBE_U5_ROOT=... bash ./tr2_validate.sh --cross-build-only`**, PASS. Pas de suite hôte intégrale inutile. Text54528/data24/BSS546272B à ce stade ; stubsnewlib non utilisés signalés au lien, pas de stdout matériel ajouté.
- Préflight GDB qualifié : ST-LINK **0032001F3235510C37333439**, UID **001B0046/38425018/20333157**, ancienBIN1d entier54 536B relu exactSHA7e9e…b3af, stageWAIT/run0/FIFOoff/writeroff/CShaute/clock_fault0. Aucun reset/flash/commandeSD dans ce préflight ; ancienne image et preuve de retour disponibles, aucun besoin de sauvegarde massive répétée.
- Lecture Windows fraîche : COM5 parentUSB contenant exactement leSN002, PowerOnlineTrue, veille et hibernationAC0. Pas de réglage/politique Windows modifié, COM4/TR2-001 non accédés.

## Tranches suivantes autorisées

Après publication de ce checkpoint logiciel, reconstruire avec le SHA commité et garder BIN/ELF/SHA ; flashstandard qualifié/verification/readback/WAIT. Puis initWHO/CID/CSD/capacité/FAT32 et noms neufs, gatecourt unique en détachantGDB, vérifier ses données et sa vraie relectureFatFs avant long. **START long seulement après ces contrôles**, premier groupeSD acquitté et témoinQPC natif actif ; publier checkpointléger et terminer complètement le tourCodex. Aucun moniteurCodex/WSL/GDB pendant les6h, STM32 seul responsableSTOP/flush.

Si saturation en fin de campagne : ne pas modifierlesflags ni l'importeur, relever RAM/digest après terminalerreur26/SDquiescence et avant coupure, récupérer tous les records CRCvalides, garder verdict qualitéINCOMPLETE/REJECTED_QUALITY. Cela reste une acquisition de données utile, pas unPASS. Une perteFIFO/SD/échantillons ou un risque non maîtrisé bloque le départ ; aucun réessai automatique.

## Préservation et preuves

Répertoire local `/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/`, worktreeMSM_vibe-R3-6h-data basé surmainec42, autresworktrees et deuxstashes préservés. OriginauxE8 et onze historiques/Info déjà sauvegardés/contrôlés dans les répertoiresPC antérieurs, **pas de nouveau transfert des530Mo surGitHub**. Preuves compactes soussix-hour/prepare. Journal actualisé avec décision6H et préparation ; contratChatGPT non modifié. Rapport et publication à relire, aucun START revendiqué à ce checkpoint.

## Gate physique et image désormais acquis — avant STARTlong

Préparationlogicielle **c3cd40ce9e96cef2241fb5bc6e8c83acd2a3824e** publiée/relecture72fichiers18artefacts. Reconstruction explicite au SHAcommité, BIN **54552B / b70438c7930ee2d928ec441a71a66ee23305d0fe5a102b2bc29e1453b6925330**, ELF **778e901d6f7989e99c2ced9c78341c21b7817112588f548695b522234061a7fc**. **Un seul flashstandard**, vérificationoutil + readbackBINentierexact, WAIT/UID/SN/clock_fault0/GDBdétaché. Aucunmass-erase niOB/RDP, aucuneFlashTR2-001.

Init réelle : WHO7B/CID0353445344333247858bf0923c01927b/CSD400e00325b590000edc87f800a4040c3,62333952secteurs, FAT32/cluster32768B. Calibration192brackets/6619ancres/résidu39µs. NomsE6N3/E6N5 **absents viaf_openFA_READ/FR_NO_FILE4** ; freeclusters **947703**, suffisants pour20480+4 demandés. Zéroformatage et zéroécriture dans cette vérification. PremieressaiGDB arrêté AVANTI/O car symbole `r3_return` éliminé parlinker ; correction du seulscript versscratchr3_header (réinitialisé parsession), pasderelancecalibration/flash/reset. Logs négatif et positif préservés.

**GateE6N0003 surcible : PASS**, 2000809µsnominalesMCU/52952natifs/5296sorties/31776B utiles,68records/34816B écrits ; rails0/clips0/FIFO-SD-file-tempsfaute0. QPC intérieur **1,0763139s**, même session6755700272993600, séquences20/21 actives puisterminal22/772. SDgroupe déjàacquitté avantACTIVE. Maxwork/service/gap **713,29375 /1319,025 /1577µs**, SDquantum491,6125µs, file5120B, pile1104B, drain9696µs ;103sortiesoracle/préfixe128conservés. DumpRAMaprèsseulementterminal, FIFOoff/filevide/writeroff/CShaute/GDBdétaché ; COM5observateurcourt fermé. Le fichier court doit encore être relu réellement parFatFs+SHA àl'entrée de la commande5, puis auPC après long ; aucunelecturePC actuelle inventée.

Helpers postrun préparés : `read_six_after.py` conserve le résumé natif même en `Terminal error`, calcule séparément la borneQPC sans effacerl'erreur ni assouplir `active_bounds`; `recover_terminal.py` garde le stage/error réel et n'autorise recoveryque stage4/error0 oustage99/error26, FIFOoff/queue0/writeroff/SDsafe/CShaute. Digestproducteur32B àconserver avant toutretrait. En erreur26, le bufferRAMdurable_footer peutencore contenir le pied du **gatecourt** : nepasleconfondreavecunfooterlong ; le secteurfin du nouveaufichier aétéinvalidé avantcapture.

Prochaineaction : publiercegate puisuniqueSTARTlong. Observateur natifWindows passif21601s, contrôleimmédiatd'uneACTIVE/SDack, checkpointléger puis fin effectiveCodex. Durée6H pasencoreacquise, aucun STARTlong encoreannoncé àcecheckpoint.
