---
mission_id: TR2-20261029-R3-1H-FAT32-ENDURANCE
status: IN_PROGRESS
base_ref: main
base_sha: c9d7333e5ea1747bd5744daabd0c87a247608b09
initial_head: d6bd8e45ba43a5736d6f7642ad1113cf49042d6d
resume_head: 39287c8c3612dfe79549014ee2c4066b4fc2c823
result_sha: 24aaf06a4f975e54b8a6199d6c625fab612cf005
created_at_utc: 2026-10-30T12:55:59.222156+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-1H — critères techniques satisfaits, relecture de publication en attente

**Une capture continue contient un intervalle indépendant de 3 601,0368139 secondes QPC, puis son fichier FAT32 réellement retiré et copié sur PC est intégralement conforme.** Zéro faute FIFO/SD/file/temps/clipping observée. Ce rapport reste IN_PROGRESS uniquement jusqu'à publication et relecture distante de cette livraison ; aucun blocage technique ou physique restant. La revue indépendante A-01 appartient à ChatGPT.

## Preuve temporelle et intégrité finale

Essai long Q1N0005 déclenché le30/10/2026 à11:44:05UTC par trigger RAM, GDB immédiatement détaché. Le MCU capture et termine seul selon sa garde de4000s nominales ; le PC ne pilote ni START ni STOP. Témoin COM5 exclusif lié au ST-LINK002 par ascendancePnP, COM4 explicitement refusé. Aucun GDB pendant capture ; collecte seulement après terminal UART puis contrôle FIFO arrêté, writer inactif, file vide, CS haute, erreurUART0. Contrôleur terminé à12:50:23UTC, code retour0. UTC ne sert que d'annotation.

Trames QPC2 CRC/UID conformes : A séquence40/5928XYZ acceptés ; B séquence41/95810520XYZ, toutes deux actives dans la session **8444552830526285**, temps MCU croissant ; terminal séquence78/105704424XYZ/flags1284. Fréquence QPC10000000Hz. Intervalle intérieur `[réception A, émission B]` = **3601,0368139s** ; distance entre snapshots bornée3601,0368139..3601,1415827s. START précède A et STOP suit B ; latences USB/OS ne peuvent gonfler cette borne intérieure. QPC est indépendant du STM32 et des ajustementsUTC, **pas un étalonnage SI**.

Retrait après STOP/SDquiescente/GDBdétaché demandé conformément àAGENTS§9 ; utilisateur confirme «carte SD dans le lecteur PC, TR2 hors tension». Lecture réelle du **Realtek PCIE CardReader**, capacité31914983424B, volume FAT32/cluster32768B découvert ànouveau. Copies complètes puis comparaison de leur SHA au fichier présent sur carte. Lecteur Python indépendant : tous records CRC/indices/temps/comptes/footer contrôlés ; chaque octet XYZ utile comparé au digest **producteur amont sauvegardé séparément en RAM**, pas seulement au hash du footer. Session/compte natif UART-RAM-PC identiques.

| Fichier réel PC | Taille totale | Sorties XYZ / octets utiles | Durée MCU nominale | Intervalle QPC intérieur |
|---|---:|---:|---:|---:|
| Q1N0003.TR2 |131072B|5260 /31560B|2,000336s|1,0547623s|
| Q1N0005.TR2 |75497472B|10570443 /63422658B|4000,000029s|3601,0368139s|

SHA-256 **utile** court : `4afde10c70ba7ec6676d31272321a7e4d61b63c8db10a99f9a872965f3f79142` ; long : **`d101531be87f3a921b30ef78cde3071ba79f7304c453a3fa36e6393f2c3dad5d`**.

SHA-256 **fichier entier copié** court : `39ea1685e9fe5d6617b029de752920175c2588326264b744e265f1153bcc9049` ; long : `c6098f96bf1fc8f76d2645cd5b6c97223217ba35138cd666bc6358ef4c08575e`.

Les **huit** anciennes captures RUN/R2N/H1N et TR2INFO.TXT sont re-hachées inchangées. Aucun formatage/écrasement. Préfixes128sorties réellement persistés conformes aux dumpsRAM ;103sorties FIR indépendamment recalculées par run, **pas recalcul scientifique de toute l'heure**. Warmup11 par run identifié. Un XYZ natif en attente au STOP long reste explicitement non publié ;105704424XYZ acceptés→10570443sorties (/10 avec phase initiale). Aucun échantillon refusé masqué.

## Marges et observations sur cible

4 160 636 services observés,135519records/16940groupes, payload63422658B. FIFO pic50/aucunOVR ; file pic10240B/65536B, overflow0. Sources64bits :2885773535..6885762752µs, franchissement réel du seuil2^32 géré et vérifié dans le fichierPC.4000000SysTick,39787contrôles horloge/résidu47µs. CTRL1_XL=0xAC/WHO0x7B ; amplitude±8g non étalonnée.

| Mesure en µs MCU nominales | P95 | P99 | Maximum | Limite inchangée |
|---|---:|---:|---:|---:|
| work |490|499|683,325|1000|
| service |1190|1225|1411,219|2500|
| gap |1446|1481|1668|5000|
| quantum SD |265|430|430,419|1000|

Busy par secteur : P95≤2ms/P99≤3ms/max159675µs ; busy FD : P95≤4ms/P99≤6ms/max30444µs. Groupe complet : durée max356137µs/busy cumulé286158µs ; P95/P99 de busy groupe≤20ms. Ces attentes restent coopératives, pas des blocages FIFO de même durée. STOP152µs/drain15601µs ; pile peinte, profondeur observée1768B/plancher2328B, pas une preuve WCET. Durées moyennes instrumentées service73,33%/work31,83%/SD5,36% **ne prouvent pas un budget CPU libre** (attente active hors périmètre). Histogrammes couvrent tout le run ; traces brutes bornées, groupes premiers256+derniers1792, pas une chronologie exhaustive des grappes du milieu.

Débit moyen entre témoins :26603,95..26604,72XYZ acceptés/secondeQPC, donc environ2660,4sorties/sQPC ; endpoints fichier donnent2642,618sorties/seconde **MCU nominale**. L'échelle MCU avance d'environ1,006742..1,006771seconde nominale par secondeQPC. Ne pas confondre ces références, déclarer2666,7Hz exact, ni modifier la V1 historique2000Hz.

## Reprise, corrections et validations conservées

L'audit hors matériel a montré que H1N/E4, quoique intègre, n'a pas de couple monotone prouvant3600s physiques : UTChôte3584,726090s, correction des seuls sautsWindows→3586,639256300s sous hypothèses insuffisantes. **Cette ancienne heure n'est pas rétroqualifiée.** Rapport E4 et `RAPPORT_AVANT_TERMINAL_QPC.md` conservés avec toutes preuves.

Probe indépendant : premier démarrage d379 enéchec avantUART (PWR/SMPS omis), corrigé f76c3431 selon R1. Mesure courte unique QPC/DWT borne le core160,962..161,174MHz relativementQPC ; pas un étalonnage de la capture passée. Candidat **24aaf06a** ajoute seulement le témoin QPC2 UART nonbloquant/versionné et Q1N/garde autonome4000MCU ; format archivev2, FIRQ15/DEC8 /10, file64Kio, SPI3=8MHz/SPI2=4MHz, quanta64mots/128B/groupes1..8 et gardes inchangés.

Tests neufs C/Python trames/CRC/UID/replay/reset/brackets/temps/STOP prématuré et ARM isolés PASS conservés ; aucun rejeu des tests scientifiques/captures déjà validés. Gate physique neufQ1N3 PASS puis **un seul** long justifié après préflight et relecture du court parfirmware. Trois flashs standards dans cette reprise (probe initial/corrigé/recorder), readbacks complets exacts ; aucun nouveau flash/reset lors du gate→long→PC. BIN55324B SHA `6cb4c7f93db96bca888a3950e99345acd7deedb1cd7e23d75296cdcac193f7d8`, ELF SHA `a022b8384979bc9cabb9543bb39e6eea3c8ab0eabae44bfed51ae8322ed59e39`. Source réellement évaluée24aaf06a, pas le HEAD documentaire.

Analyse finale : un chemin historique `physical-hour` oublié dans le helper de métriques a causé FileNotFoundError, corrigé vers `qpc-long-data` puis analyse hors ligne réussie ; aucune acquisition ni validation physique relancée. Un filtre de suffixes du packager omettait le point : inspection avant indexation a exclu les copies entières/queues reconstruites du bundle neuf et corrigé le générateur. Aucun octet de zone inutilisée n’a été commité ou publié. Pas de correction du cœur pendant cette reprise.

## Incident Defender et publication contrôlée

NoteChatGPT `ALERTE_DEFENDER_QPC_T1000_20261030.md` préservée. Détection1116/Injuke à10:47:04UTC puis1117/Supprimer : commande complète comparée littéralement au diagnostic AST/CRC contrôlé SHA953ec007…f9e2, sourceblob2e5e6f8…eb10 ; ParseInput n'invoque pas le témoin placé dans sa chaîne, fixture48B de données CRC51D01B2D. **Faux positif probable pour cette seule commande**, pas certificat global d'innocuité du PC. Aucune commande détectée rejouée, exclusion/restauration/allow-list/désactivation/évasion ou soumission externe. Le témoin réel distinct a exécuté court etlong sous protections actives.

Collecte en lecture seule aprèslong à12:51:45UTC : protections realtime/behavior/tamper/antivirus actives, seule ancienne détection1061 retournée ; aucun nouvel événement1116 depuis lancement. Le filtre temporel natif avait renvoyé l'ancien événement horsfenêtre lors du premier contrôle ; comparaison UTC explicite corrige l'assertion trop large, sans rejouer de diagnostic. Journaux bruts avec identifiants personnels conservés localement, seuls constats expurgés publiés. [ProcédureMicrosoft](https://learn.microsoft.com/en-us/defender-endpoint/defender-endpoint-false-positives-negatives), [référenceQPC](https://learn.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps).

Divergence bf1ce5c2/e7810f71 escaladée : utilisateur autorise explicitement l'intégration isolée. Commitbf1/worktreeinitial préservés, patch seul réappliqué sansmerge/rebase/reset/force. **a6e4cad1** sécurité/gate publié et confirméAPI GitHub (fetch initial temporisé), puis **d45c26a5** lancement publié/fetch/relu439fichiers/366artefacts. Le fetch de cette fin retrouve HEADd45c26a5 identique àorigin/main. Nouvelle livraison technique puis miseàjourDONE seulement après relecture réelle conformémentV2.

## Preuves, limites et prochaine décision

Dans `preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/` : `time-reference/`, `qpc-gate/`, `security/`, `qpc-long-start/`, **`qpc-final/BUNDLE.json`**. Ce dernier contient trames/journalQPC, dumpsRAM, histogrammes, SHAamont, résultats PC/CSV, scripts exacts et **extents réels header+records/footer**. Chaque artefact compressé est manifesté SHAavant/après. `EXTENTS.json` conserve taille/hash du fichierPCoriginal et offsets. La zone préallouée jamais écrite peut contenir d'anciens octets arbitraires : **non publiée et jamais comptée commeXYZ**. Reconstruction àqueue zéro distincte, jamais présentée comme copie originale ; toutes données utiles originales restent publiées sansaltération. Originaux complets conservés dans `/home/lolo/dev/msm/reports/TR2-r3-1h-time-qualification/pc-final/`.

État final matériel : **TR2-002 hors tension confirmé utilisateur, carte au lecteurPC**, firmware24aaf06a conservé enFlash/bootWAIT sansautocapture, GDB etCOM5 fermés. CibleSTLINK0032001F3235510C37333439/UID001B0046/38425018/20333157 ; CIDtest03534453/44333247/858BF092/3C01927B confirmé avantécriture. TR2-001/FRAM/COM4/service Windows/carteproduction intouchés. Worktrees historiques et deuxstashes6251b988/f3b829e6 préservés ; nouveau worktreeisolé `MSM_vibe-R3-1h-security`/branchecodex/r3-1h-security-integration destiné aux pushnormauxmain.

**Limites :** une capture expérimentale≥1h, pas8h/production/V1/FRAM/RTU/AC1/essaiarbitrairepower-loss. DC conservée dansceflux, /10candidat ; UTCinconnue, interpolationintrarecord approximative, amplitude±8g nonétalonnée. Pas garantie contrepausesSD arbitrairementlongues malgréla grappe réelle absorbée ; pas auditglobalWindows. Prochaineétape : revueindépendanteA-01 parChatGPT puismission8h distincte. Pour8h : duréesµs28,8milliards→u64, nouveauformatversionné/header/footer/outils, préallocationenviron0,5Go+ marge, capture/STOP autonomes sansCodex/GDB/PC et qualificationdes fautes séparée. Aucun essai8h lancé.

Journal `ETAT_COURANT_TR2.md` actualisé : borneQPC réelle+fichierPC conforme, marges, état final hors tension et limites ; décisionsnormatives inchangées. Rapport finalprotocolaire àmettreDONE aprèsrelecture de cettepublication, avec son SHA confirmé.
