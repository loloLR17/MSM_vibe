---
mission_id: TR2-20261029-R3-1H-FAT32-ENDURANCE
status: BLOCKED
base_ref: main
base_sha: c9d7333e5ea1747bd5744daabd0c87a247608b09
initial_head: d6bd8e45ba43a5736d6f7642ad1113cf49042d6d
result_sha: a7ca102f857310e139d26f52c6332032ca8e9381
created_at_utc: 2026-10-30T07:42:40.088546+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-1H — capture terminée, reprise au transfert physique vers PC

**Point de reprise, pas une clôture DONE.** Le seul essai long a terminé sans faute signalée. Les deux nouveaux fichiers ne sont **pas encore extraits ni vérifiés sur PC** : le transfert physique demandé à l'utilisateur reste à confirmer. C'est le blocage externe immédiat selon AGENTS.md §9. La même mission reprend après ce geste, sans refaire les essais acquis.

Une réserve temporelle distincte reste ouverte : **3600,000743 s en unités MCU nominales** observées, mais **3600 s physiques indépendamment mesurées non établies**. Ne pas transformer cette réserve en succès du critère strict, ni en panne de stockage démontrée.

## 1. Référence, périmètre et préservation

Code réellement évalué **a7ca102f857310e139d26f52c6332032ca8e9381**. Image BIN54212B SHA256 **e6168e5c999e27d38cc718bdc6cdf98c05b23766da0270f226b7c42b6d9b9239**, ELF **e9991679d9df514477b6a48709a9c4e9dc8f175209fad585e070857f414057e6** ; GCC ARM14.2.1/CubeU5v1.9.0, profil `R3_ENDURANCE_1H=ON`, Cortex-M33/O2, paramètres exacts archivés. Un seul flash normal : programmation ciblée à0x08000000, vérification outil puis readback de **tous les54212octets du BIN**, pas une empreinte de l'intégralité2MiB Flash. Aucun mass erase/OptionBytes/RDP ni reset logiciel supplémentaire. Backup initial2MiB et images R1/R2/R3 vérifiés/réutilisés, non multipliés.

Cible exclusive TR2-002 : ST-LINK **0032001F3235510C37333439**, UID **001B0046/38425018/20333157**, WHO7B. Câblage acquis SPI3 PC9/10/11/12 ; SPI2 PB13/PD3/PD4/PF12,3,3V/GND CN8-7/11, USB natif débranché demandé, réinsertion sous ST-LINK confirmée par l'utilisateur. Aucun changement de câblage demandé dans ce lot.

MicroSD test seule : CID **0353445344333247858bf0923c01927b**, CSD **400e00325b590000edc87f800a4040c3**, OCR C0FF8000,62333952×512B. Avant transfert sur banc : véritable lecteur PC Realtek/FAT32,31876808704B libres ; **six anciens RUN/R2N re-hachés intégralement identiques** et TR2INFO.TXT343B SHA71e626c7e69644ff7aec1047fbb0417bc9ff80b6ec21a47659d68e70fbd11553 exact. Les deux noms H1N absents. Recontrôle après nouvelles écritures encore à faire. Aucun formatage/effacement/nom réutilisé.

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
| Fichier prévu sur carte, pas encore copié PC | H1N0003.TR2 | H1N0005.TR2 |
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

Fichier long :147456secteurs/2304clusters, free972799clusters avant allocation ;62456320B de records acquittés,13040128B de marge préallouée après records+header/footer. Les CRC des header/footer **RAM** sont conformes ; ce n'est pas la lecture du média.

SHA producteur sauvegardé **avant framing/queue/TX**, à comparer au fichier réel :

- Gate31554B : `dc2a19be588e2d7d6006e7fd88e3c7824931f7adbcb95148a58551c1b4886a88`.
- Run long57088872B : `e99b50584536231ff881d15637fb5fd0e6b0170908fb2a3c04f6e457338c7ce1`.

**Aucune de ces empreintes seules ne prouve la conservation du contenu sur la carte.**

## 4. Réserve temporelle — critère strict non déclaré satisfait

Lancement hôte WSL06:35:32.530364UTC, retour après dumps07:35:17.256454UTC : intervalle **3584,726090s**, incluant connexion/préallocation/STOP/dumps. Il ne faut pas annoncer3600s UTC. Les événements Windows Kernel-General1 montrent à06:46:12UTC un saut−1915,1119ms puis+1,7727/+0,1729ms. **Aucun chronomètre PC monotone indépendant n'a été enregistré.** Le MCU utilise MSI avec calibration par défaut→PLL nominale160MHz ; SysTick et DWT ont la même référence et ne se qualifient pas mutuellement en SI.

Fait :3600,000743s nominales MCU sans faute de rollover observée ; réserve : **durée physique indépendante exacte non démontrée**. Une erreur de cadence interne et/ou les ajustements de temps hôte/WSL restent à départager ; ne pas attribuer tout l'écart à−1,915s ni affirmer une fréquence MCU étalonnée. Les cadences sont également données dans ce repère : calibration initiale2643,631290sorties/s, moyenne source26430,109235Hz entre endpoints. /10 reste candidat, pasV1-05=2000Hz ni métrologie/SI/UTC.

Action recommandée après préservation/extraction : définir et enregistrer une référence monotone indépendante et les bornes début/fin de capture pour résoudre le critère d'heure physique. Une nouvelle exécution serait une reprise **justifiée par ce défaut de preuve temporelle**, avec nouveau nom, jamais une relance silencieuse de la capture actuelle. Ne pas relâcher le critère par convenance ; revue indépendante ChatGPT requise pour toute qualification.

## 5. Blocage immédiat et reprise exacte E4

L'utilisateur a reçu après STOP/detach : couper **uniquement TR2-002/ST-LINK**, transférer sa carte au lecteur natif T1000 et laisser le banc hors tension. **Retour encore attendu à ce point**. État électrique et présenceF: non présumés. Aucun nouveau diagnostic/flash/reset/write depuis le terminal.

Après confirmation «carte dans le lecteur PC, TR2-002 hors tension», exécuter :

`python3 /home/lolo/dev/msm/reports/TR2-r3-1h-fat32-endurance/extract_hour_pc.py`

Le script vérifie lecteur/volume/capacité/Info, copie **sans écraser** H1N0003 etH1N0005 entiers vers `pc-final`, lit chaque record/CRC/index/temps/compte/session/footer/identité/version, compare tous les octets utiles au SHA producteur RAM ci-dessus, compare128sortiescapturées du préfixe, écrit CSV/manifeste et re-hache les six anciens fichiers. Si un contrôle échoue, conserver copies/logs et diagnostiquer ; aucun faux COMPLETE. Aucun fichier `pc-final` créé à ce checkpoint.

Puis publier preuves PC, résultats réellement obtenus, actualiser journal et verdict strict en gardant la réserve temporelle si non résolue ; ne pas clore DONE au seul succès des écritures ou de la copie.

## 6. E5, Git et publication

[Audit complet et préparation8h](https://github.com/loloLR17/MSM_vibe/blob/8d4e2f3e61f1be6d87c8bee9a5ab8c73153c66ae/experiments/r3_durable_fat32/ENDURANCE_1H.md) : durée8h=28800000000µs à élargir dans session/Result/header/footer et lecteur versionné, allocation~0,5Go framed+marge, tous compteurs et produits à borner ; DWT doit être échantillonné avant un rollover(~26,84s nominales). Les sources/brackets sont déjàu64. Période raw capteur déduite de la calibration~15,043h, pas25µs/tick supposées.8h **non exécutées** ; future autonomie sans session Codex selon roadmap9e2dec54, aucune modification de cette mission. Power-loss/retrait à chaud/FRAM/RTU/multi-capteurs/métrologie/production demeurent distincts.

Dépôt historique main82169af propre préservé avec ses divergences, deux stashes6251b98/f3b829e intacts. Préparation `MSM_vibe-R3-1h` conserve ses modifications non commitées ; publication dans `MSM_vibe-R3-1h-publish`/codex/r3-1h-publication basé9e2dec54 après arrivée documentaire seulement, **sans merge/rebase/reset ni travail perdu**. Push normaux explicites vers main ; pas de force push. Les commits de documentation ultérieurs ne changent pas le code physiquea7ca102f.

Publications effectivement relues :01057a5e (69fichiers/39artefacts),01b80c5e (137/105),8d4e2f3e (153/117), puis **9919c0266b655fda1733c17d1e33be94c89b0f36** à07:40:18UTC : **190fichiers entièrement comparés,152artefacts décompressés/re-hachés**, FatFs officiel pristine. Preuves `[software,gate,audit-followup,build-provenance,physical-terminal]` sous `preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/`, chaque BUNDLE fournit SHA du brut et du zlib. Bilan de ce nouveau point de reprise encore à relire après push ; résultat fourni dans la réponse opérateur, pas un SHA autoréférent.

Journal ETAT_COURANT_TR2 actualisé : adaptations, gate, terminal sûr, compteurs, réserve temporelle, attente retraitPC. Mission/anciennes archives/dettes/gels préservés ; pas d'archive DONE ni tag prématuré. **BLOCKED signifie attente de l'intervention physique indispensable, pas échec du stockage ni clôture de la mission.**

**Point de reprise effectivement relu depuis GitHub :** `fe0fee1356f8d177fe3467aaeb35daf378a58b17` à `2026-10-30T07:43:33.667910+00:00`, 197 fichiers comparés intégralement et 156 artefacts décompressés/hachés, FatFs conforme. Statut BLOCKED au seul geste physique immédiat conservé ; réserve temporelle ouverte, aucun DONE ni essai répété. Cette preuve est publiée dans `preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/REMOTE_REREAD_fe0fee13.json`.
