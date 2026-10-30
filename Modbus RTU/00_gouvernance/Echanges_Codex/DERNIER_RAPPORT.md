---
mission_id: TR2-20261029-R3-1H-FAT32-ENDURANCE
status: BLOCKED
base_ref: main
base_sha: c9d7333e5ea1747bd5744daabd0c87a247608b09
initial_head: d6bd8e45ba43a5736d6f7642ad1113cf49042d6d
result_sha: a7ca102f857310e139d26f52c6332032ca8e9381
created_at_utc: 2026-10-30T08:41:54.829024+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-1H — intégrité PC complète acquise, réserve de durée indépendante

**E4 TERMINÉE / INTÉGRITÉ CONFORME. Mission BLOCKED sur la seule qualification indépendante de durée ; pas de DONE strict.** Le transfert de la microSD au lecteur T1000 a été confirmé par l'utilisateur via la note ChatGPT publiée au commit **b1c25830f3d29c26a1d4d6a75ddd8b4ec257f143**. Le blocage physique précédent est **levé**. L'état électrique actuel de TR2-002 n'est pas confirmé et n'a pas été inventé.

Les deux fichiers ont été réellement copiés et entièrement vérifiés sur PC : **57 088 872 octets utiles** du run long et **31 554 octets** du gate correspondent exactement aux SHA producteurs sauvegardés avant framing/queue/TX. Six fichiers historiques et TR2INFO.TXT inchangés. Cette preuve d'intégrité ne constitue pas une référence temporelle indépendante : **3600,000743 s nominales MCU observées, 3600 s physiques indépendantes non établies**.

Reprise limitée à E4 selon la note : **aucun accès STM32, aucun flash/debug/reset/START, aucune nouvelle capture, aucun formatage et aucun test/build déjà acquis rejoué.** Publication normale sur main et relecture distante prévues ci-dessous. Aucun geste physique ou accord logiciel supplémentaire demandé pour cette extraction.

## 1. Référence, périmètre et préservation

Code réellement évalué **a7ca102f857310e139d26f52c6332032ca8e9381**. Image BIN54212B SHA256 **e6168e5c999e27d38cc718bdc6cdf98c05b23766da0270f226b7c42b6d9b9239**, ELF **e9991679d9df514477b6a48709a9c4e9dc8f175209fad585e070857f414057e6** ; GCC ARM14.2.1/CubeU5v1.9.0, profil `R3_ENDURANCE_1H=ON`, Cortex-M33/O2, paramètres exacts archivés. Un seul flash normal : programmation ciblée à0x08000000, vérification outil puis readback de **tous les54212octets du BIN**, pas une empreinte de l'intégralité2MiB Flash. Aucun mass erase/OptionBytes/RDP ni reset logiciel supplémentaire. Backup initial2MiB et images R1/R2/R3 vérifiés/réutilisés, non multipliés.

Cible exclusive TR2-002 : ST-LINK **0032001F3235510C37333439**, UID **001B0046/38425018/20333157**, WHO7B. Câblage acquis SPI3 PC9/10/11/12 ; SPI2 PB13/PD3/PD4/PF12,3,3V/GND CN8-7/11, USB natif débranché demandé, réinsertion sous ST-LINK confirmée par l'utilisateur. Aucun changement de câblage demandé dans ce lot.

MicroSD test seule : CID **0353445344333247858bf0923c01927b**, CSD **400e00325b590000edc87f800a4040c3**, OCR C0FF8000,62333952×512B. Avant transfert sur banc : véritable lecteur PC Realtek/FAT32,31876808704B libres ; **six anciens RUN/R2N re-hachés intégralement identiques** et TR2INFO.TXT343B SHA71e626c7e69644ff7aec1047fbb0417bc9ff80b6ec21a47659d68e70fbd11553 exact. Les deux noms H1N absents. Recontrôle après nouvelles écritures effectué en E4 : six fichiers et TR2INFO.TXT conformes. Aucun formatage/effacement/nom réutilisé.

TR2-001/FRAM/COM4/service Windows et carte de production **non accédés** ; état actuel non prétendu observé. Les seuls événements Windows lus concernent l'horloge système. Branche distante archive/tr2/point-retour-pre-spi2-20261029 toujours a3f5ac33a728a5afd028192654c17e5b04ee8d7b.

## 2. E0/E1 — adaptations minimales et validations

- Risque trouvé : ancien sink refusait les temps absolus>UINT32_MAX et brackets tronqués à32bits. Sources/brackets désormaisu64, diagnostic Resultv3 (288B)/Bracket24B ; monotonicité et indices toujours contrôlés.
- Fichier candidat version2, même framing78XYZ/512B/CRC/SHA producteur. Footer128/136 : temps sourceu64 ;88/92 : low32 recoupés. Lecteur v1/v2, version inconnue refusée, durée demandée contrôlée en unités déclarées. UTC inconnue ; interpolation interne aux records explicitement approximative.
- Profil explicite : H1N0003.TR2 gate2s/128KiB puis H1N0005.TR2/3600s/72MiB ; commande5 relit intégralement le gate via FatFs avant l'heure. Anciens paliers bypass/60s/600s et commande format refusés dans ce profil. Aucun ancien essai rejoué.
- Calcul conservateur :9600000sorties/h,57600000B utiles,123077records,63016448B avec header/footer ;72MiB=75497472B, marge calculée12481024B. Avant création : f_getfree, f_expand contigu, validation de chaque cluster, sync/close ; aucun appel FatFs pendant FIFO.
- FIR, coefficients, mapper FIFO, queue, bus et writer identiques octet pour octet à la baseline physique6fdae042 (dix fichiers comparés). File **64KiB**, SPI3 **8MHz**, SPI2 **4MHz**, FIFO64mots/SD128B/groupes1..8 ; gardes work/service/gap **1000/2500/5000µs** inchangés.
- Tests hôte ciblés C/FatFs et Python **2/2 PASS** : allocation/lecture fichier réel du modèle72MiB, DWT5400s/rollovers, temps>u32, corruption/troncature/indices/versions, disque plein/nom existant/CID,80états STOP et busy150/500ms/grappes/file saturée. Nouvelle version du lecteur relit aussi les six anciennes copies PC, dont le préfixe INCOMPLETE, sans altération des résultats.
- Cross-build canonique PASS via `bash tr2_validate.sh --cross-build-only` ; ARM candidat PASS. Modification limitée au banc, cœur scientifique inchangé : anciens150tests non rejoués. Text54192/data20/BSS546200B (réserves linker incluses), RAM budget768KiB. Empreintes/binaire/MAP/flags exacts publiés.
- Incidents d'outillage conservés : premier lancement direct du script sans bit exécutable (126), relancé avec bash ; assertion disque plein mesurée trop tôt dans le test, déplacée immédiatement avant le refus sans affaiblir l'égalité ; première analyse du gate invoquée avant fin des dumps refusée sur buffer vide, puis effectuée après retour0 sans nouvelle capture. Référence archive non suivie localement vérifiée directement par ls-remote. Aucun de ces incidents n'a déclenché de répétition physique.

## 3. E2/E3 — résultats matériels réellement observés

Préflight avant flash : UID/SN, image précédente6fdae04257448B relue exactement, WHO7B par lecture ST ciblée, WAIT/SDinactive/CShaute, puis detach. Après flash candidat : readback exact et boot WAIT sans autostart. Cmd1 : CID/CSD/OCR/capacité exacts, **FAT32 existante montée sans format**,193brackets/6576ancres, résidu calibration42µs.

| Observation | Gate nouveau2s | Capture longue |
|---|---:|---:|
| Fichier réellement copié et vérifié sur PC | H1N0003.TR2 | H1N0005.TR2 |
| Durée MCU nominale, s | 2,000162 | 3600,000743 |
| XYZ natifs datés acceptés | 52584 | 95148112 |
| XYZ natifs en attente d'ancre au STOP, non publiés | 2 | 5 |
| Sorties FIR /10 | 5259 | 9514812 |
| Octets utiles produits avant queue/TX | 31554 | 57088872 |
| Records512B acquittés / groupes | 68 /9 | 121985 /15249 |
| Pic FIFO / file | 46mots /5120B | 50mots /8192B |
| STOP / drain, µs MCU | 151 /9368 | 152 /13599 |
| Pile peinte4KiB : profondeur / plancher intact | 1112 /2984B | 1776 /2320B |

Les deux runs : stage4/error0, STOP avec contrôles ODRoff/FIFObypass/TSoff, writer terminé/SDquiescente/CShaute/débogueur détaché. Zéro OVR/FULL/source/temps/SD/queuefaute/rail/clip signalé,11sortieswarmup. **103 sorties de chaque préfixe** recalculées indépendamment avec les107coefficients ; pas une contre-vérification scientifique de toutes les95M sources. Les5XYZ finaux sans ancre future sont explicitement hors sorties, pas des octets implicitement archivés.

Sur **3764662polls** du run long :

| µs MCU nominales | Moyenne | P95 plafond1µs | P99 plafond1µs | Maximum |
|---|---:|---:|---:|---:|
| work |304,138|493|502|694,044|
| service |700,490|1187|1222|1420|
| gap |956,261|1442|1477|1676|

SDquantummax430,519µs ; marges work/service/gap305,956/1080/3324µs aux gardes inchangées.35815checks horloge/résidu48µs et3600001SysTick. Temps de service instrumenté73,253%/work31,805%/SD5,373% de la durée MCU ; le pacing est busy-wait : **pas un budget CPUidle ni un WCET industriel**.

SD :121985busy secteurs, mean1316,214µs/P95ceil2ms/P99ceil2ms/max**159929µs** ;15249busyFDstop, mean2787,515µs/P95ceil4ms/P99ceil6ms/max24506µs. Groupe write mean82946,454µs/P95ceil90ms/P99ceil110ms/max240026µs. Cette longue pause a été absorbée ; aucun résultat ne garantit des grappes arbitraires. Histogrammes complets, comptes/sommes/extrêmes complets ; raw groupes **premiers256+derniers1792 seulement**, pas15249traces intégrales.

Fichier long :147456secteurs/2304clusters, free972799clusters avant allocation ;62456320B de records acquittés,13040128B de marge préallouée après records+header/footer. Les CRC des header/footer RAM étaient conformes ; en E4, les header/footer et tous les records ont aussi été vérifiés sur les véritables fichiers lus au lecteur PC.

SHA producteur sauvegardé **avant framing/queue/TX**, désormais comparé à tous les octets utiles des fichiers PC réels :

- Gate31554B : `dc2a19be588e2d7d6006e7fd88e3c7824931f7adbcb95148a58551c1b4886a88`.
- Run long57088872B : `e99b50584536231ff881d15637fb5fd0e6b0170908fb2a3c04f6e457338c7ce1`.

Ces empreintes ne sont pas utilisées seules : les fichiers physiques entiers ont été copiés, puis chaque record et chaque octet utile ont été lus et vérifiés, ainsi que les empreintes globales carte/copie. Voir E4 ci-dessous.

## 4. Réserve temporelle — critère strict non déclaré satisfait

Lancement hôte WSL06:35:32.530364UTC, retour après dumps07:35:17.256454UTC : intervalle **3584,726090s**, incluant connexion/préallocation/STOP/dumps. Il ne faut pas annoncer3600s UTC. Les événements Windows Kernel-General1 montrent à06:46:12UTC un saut−1915,1119ms puis+1,7727/+0,1729ms. **Aucun chronomètre PC monotone indépendant n'a été enregistré.** Le MCU utilise MSI avec calibration par défaut→PLL nominale160MHz ; SysTick et DWT ont la même référence et ne se qualifient pas mutuellement en SI.

Fait :3600,000743s nominales MCU sans faute de rollover observée ; réserve : **durée physique indépendante exacte non démontrée**. Une erreur de cadence interne et/ou les ajustements de temps hôte/WSL restent à départager ; ne pas attribuer tout l'écart à−1,915s ni affirmer une fréquence MCU étalonnée. Les cadences sont également données dans ce repère : calibration initiale2643,631290sorties/s, moyenne source26430,109235Hz entre endpoints. /10 reste candidat, pasV1-05=2000Hz ni métrologie/SI/UTC.

Action recommandée après préservation/extraction : définir et enregistrer une référence monotone indépendante et les bornes début/fin de capture pour résoudre le critère d'heure physique. Une nouvelle exécution serait une reprise **justifiée par ce défaut de preuve temporelle**, avec nouveau nom, jamais une relance silencieuse de la capture actuelle. Ne pas relâcher le critère par convenance ; revue indépendante ChatGPT requise pour toute qualification.

## 5. E4 — vérification PC réelle du30/10/2026

Source de reprise : note `REPRISE_R3_1H_CONFIRMATION_CARTE_PC_20261030.md` sur main/b1c25830. Découverte réelle du lecteur **Realtek PCIE CardReader**, disque1/MBR/capacité31914983424B ; partition FAT32 saine31906594816B, clusters32768B, lettre **F:** découverte aujourd'hui et non présumée. TR2INFO.TXT343B relu et comparé à l'original. L'identité par lecteur/capacité/Info/CID enregistré et chaîne de transfert est concordante ; **aucune lecture matérielle du CID natif PC prétendue**.

`extract_hour_pc_E4.py` reprend le helper préparé, adapté uniquement au worktree actuel et à la lettre fraîchement vérifiée. Sortie réelle0, contrôles en lecture et copies exclusives sans écrasement vers `/home/lolo/dev/msm/reports/TR2-r3-1h-fat32-endurance/pc-final/`.

| Fichier PC réel | Taille complète | Records vérifiés | Sorties XYZ | Octets utiles vérifiés | Statut du fichier |
|---|---:|---:|---:|---:|---|
| H1N0003.TR2 |131072B|68|5259|31554B|COMPLETE|
| H1N0005.TR2 |75497472B|121985|9514812|57088872B|COMPLETE|

Pour chacun : CRC header/footer et **tous** les records, sessions/version2, indices, temps stricts, comptes natifs/sorties, qualité/warmup11/rails0/clips0/pending, UID/CID/configuration/coefficients et SHA firmwarea7ca102f ; payload entier SHA256 comparé au **dump indépendant du producteur RAM antérieur au writer**, pas seulement au footer.128sorties du préfixe effectivement persistées sont identiques au dump MCU ;103sorties de chaque préfixe étaient déjà recalculées par oracle indépendant, sans répéter ces acquisitions. `COMPLETE` désigne ici l'intégrité du format et du contenu, **pas une qualification horaire SI**.

SHA256 des fichiers complets copiés :

- H1N0003.TR2 : **46e71e947bbcdf7039fcaeeb1e2c283666b89ef1fe8228d01db1f7b4a116a025**.
- H1N0005.TR2 : **894d4f3bc3542694fc2a662f96a2a85b454a7c91aa176e09b85eb17b3631f601**.

Ces deux SHA ont aussi été recalculés **sur les fichiers originaux F:** et sont exactement égaux aux copies PC complètes. Les SHA des31554/57088872octets utiles sont ceux de la section3, tous conformes. Cadence /10 entre endpoints du fichier long **2643,010923652241Hz nominales MCU** (calibration initiale2643,631290455831Hz) ; première/dernière sortie75115383/3675104666µs, intervalle3599989283µs. Tous les temps `actual_duration_us` du lecteur/CSV sont des **unités MCU nominales**, jamais une durée physique externe indépendante.

Après extraction, **RUN0003/4/5.TR2 et R2N0003/4/5.TR2** re-hachés intégralement : identiques au manifeste avant campagne ; TR2INFO.TXT inchangé. Pas d'effacement, réparation, formatage ou écriture applicative sur le volume, ni copie sur un nom existant. Lecture/copie indépendantes de l'état électrique du STM32, qui reste **inconnu** à cette reprise. La carte reste dans le lecteur PC ; aucun accès cible ni commande RS-485.

### Preuves publiques et copies complètes préservées

`E4-pc/BUNDLE.json` fournit CSV de synthèse, manifeste INTEGRITY, découverte lecteur, résultats des deux fichiers,128lignes de préfixe par fichier, empreintes carte/copies et scripts réellement utilisés. Les copies PC complètes **restent inchangées** dans le répertoire externe ci-dessus.

Les fichiers préalloués contiennent une zone inutilisée non nulle :95232B pour le gate,13040128B pour le long. Ces octets ne sont pas des mesures et peuvent provenir de l'ancien contenu du média. **Le dépôt publie sans modification les vrais header+records et footers, avec tous les octets utiles ; il exclut cette zone arbitraire inutilisée.** `pc-extents/EXTENTS.json` documente tailles/offsets et SHA des fichiers PC originaux complets.

La reconstruction locale avec un trou rempli de zéros conserve exactement tous les records, footer et payload SHA ; elle est analysée COMPLETE par le même lecteur. **Ce fichier reconstruit n'est pas la copie PC originale**, et son SHA global différent est explicitement indiqué. Ne pas le présenter comme fichier intégral original ni déclarer le tail original reconstitué. La contre-vérification indépendante des records et du contenu utile est possible depuis les seuls artefacts GitHub.

### Ce qui reste à résoudre

Le besoin de transfert physique est levé. Aucun défaut de stockage ou de contenu n'est démontré en E4. La réserve de la section4 demeure : absence de référence monotone externe indépendante, UTC hôte perturbée et MSI/PLL nominale. **BLOCKED porte désormais sur ce prérequis de qualification temporelle et le cadrage de sa résolution**, conformément à la note de reprise E4 uniquement ; pas sur l'intégrité PC ni sur une indisponibilité de l'utilisateur.

Plan minimal : revue A-01 des preuves d'intégrité et de la réserve ; définir méthode/référence temporelle indépendante et bornes début/fin pour la prochaine qualification autorisée. Préserver cette capture ; aucun nouveau run ou flash déclenché dans la reprise E4, aucune relaxation implicite du critère.8h reste distinct et non lancé.

## 6. E5, Git et publication

[Audit complet et préparation8h](https://github.com/loloLR17/MSM_vibe/blob/8d4e2f3e61f1be6d87c8bee9a5ab8c73153c66ae/experiments/r3_durable_fat32/ENDURANCE_1H.md) : durée8h=28800000000µs à élargir dans session/Result/header/footer et lecteur versionné, allocation~0,5Go framed+marge, tous compteurs et produits à borner ; DWT doit être échantillonné avant un rollover(~26,84s nominales). Les sources/brackets sont déjàu64. Période raw capteur déduite de la calibration~15,043h, pas25µs/tick supposées.8h **non exécutées** ; future autonomie sans session Codex selon roadmap9e2dec54, aucune modification de cette mission. Power-loss/retrait à chaud/FRAM/RTU/multi-capteurs/métrologie/production demeurent distincts.

Dépôt historique main82169af propre préservé avec ses divergences, deux stashes6251b98/f3b829e intacts. Préparation `MSM_vibe-R3-1h` conserve ses modifications non commitées ; publication dans `MSM_vibe-R3-1h-publish`/codex/r3-1h-publication basé9e2dec54 après arrivée documentaire seulement, **sans merge/rebase/reset ni travail perdu**. Push normaux explicites vers main ; pas de force push. Les commits de documentation ultérieurs ne changent pas le code physiquea7ca102f.

Publications effectivement relues :01057a5e (69fichiers/39artefacts),01b80c5e (137/105),8d4e2f3e (153/117), puis **9919c0266b655fda1733c17d1e33be94c89b0f36** à07:40:18UTC : **190fichiers entièrement comparés,152artefacts décompressés/re-hachés**, FatFs officiel pristine. Preuves `[software,gate,audit-followup,build-provenance,physical-terminal]` sous `preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/`, chaque BUNDLE fournit SHA du brut et du zlib. Bilan de ce nouveau point de reprise encore à relire après push ; résultat fourni dans la réponse opérateur, pas un SHA autoréférent.

Journal ETAT_COURANT_TR2 actualisé : adaptations, gate, terminal sûr, compteurs ; ajout E4 réelle conforme, blocage physique levé, état électrique inconnu et réserve temporelle maintenue. Mission/anciennes archives/dettes/gels préservés ; pas d'archive DONE ni tag prématuré. **BLOCKED signifie désormais réserve de qualification temporelle indépendante, pas attente de transfert ni échec du stockage.**

**Historique avant E4 — point de reprise relu depuis GitHub :** `fe0fee1356f8d177fe3467aaeb35daf378a58b17` à `2026-10-30T07:43:33.667910+00:00`, 197 fichiers comparés intégralement et 156 artefacts décompressés/hachés, FatFs conforme. À cette date antérieure, BLOCKED concernait encore le geste physique ; ce motif est levé par la reprise E4 ci-dessus. La réserve temporelle reste ouverte, sans DONE ni essai répété. Cette preuve est publiée dans `preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/REMOTE_REREAD_fe0fee13.json`.

## Reprise Git E4 et contrôle de publication

HEAD initial de cette reprise **b1c25830f3d29c26a1d4d6a75ddd8b4ec257f143**, worktree propre neuf `MSM_vibe-R3-1h-E4`/codex/r3-1h-e4 basé sur origin/main. Dépôt historique, préparation et précédent worktree de publication préservés ; aucun merge/rebase/reset. Tous les fichiers firmware/lecteur restent inchangés ; seules preuves et documentation sont publiées. Deux stashes historiques conservés. Aucun test/build rejoué : nouvelles preuves obtenues par extraction réelle, contrôle de copie complète et contrôle des extents publiables.

Rapport et preuves E4 à publier par push normal explicite vers main puis à relire intégralement depuis origin/main récupérée. Le reçu de relecture consigne SHA/heure/fichiers/artefacts ; SHA final communiqué après publication, sans autoréférence. Aucune notification automatique ou qualification indépendante ChatGPT prétendue.
