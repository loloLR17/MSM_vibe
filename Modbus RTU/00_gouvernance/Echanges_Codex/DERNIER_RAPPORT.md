---
mission_id: TR2-20261014-P12H3H-SDMMC-DAT-PERMUTATION-REVIEW
status: IN_PROGRESS
base_ref: main
base_sha: 240a83a86e0f93e1ef647f945853e396111ba971
initial_head: 163255a6b67b9260c2d213f8ef500268da9f89bd
result_sha: null
created_at_utc: 2026-10-14T16:27:26.740222+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Revue DAT : corruption silencieuse possible, premier CRC SSR non expliqué

**Une permutation idéale et constante de DAT1–DAT3 peut corrompre les octets sans erreur CRC, et être masquée par une écriture/relecture sur le même câblage. Elle ne produit pas, à elle seule, le premier DCRCFAIL du SSR observé dans FIRSTFAULT B.** Une anomalie supplémentaire, électrique, temporelle, de mode ou d'implémentation, serait nécessaire.

Les preuves qui changent de chemin — write4bits/read1bit, writeSTM32/readPC et write1bit/read4bits — excluent les permutations non identiques constantes aux périodes concernées. Elles ne certifient pas le montage actuel après ses recâblages successifs. Statut IN_PROGRESS uniquement en attente de publication et relecture de cette revue, sans blocage technique.

**Aucun nouvel essai matériel, flash, debug, reset, connexion à une cible, commande SD, accès au lecteur PC ou changement de firmware.** Le travail est documentaire et hors ligne.

## 1. Sources et recherche antérieure

HEAD initial `163255a6b67b9260c2d213f8ef500268da9f89bd`. Contrat, AGENTS, gouvernance, protocole, journal intégral, mission précédente et rapports historiques examinés. [Catalogue de18 contrats/rapports](preuves/TR2-20261014-SDMMC-DAT-PERMUTATION/historical-contract-report-catalogue.json), [inventaire des preuves](preuves/TR2-20261014-SDMMC-DAT-PERMUTATION/historical-proof-inventory.json). Les neuf dossiers requis (huit historiques et FIRSTFAULT) ont **560 entrées de manifest vérifiées** ; GPIO ajoute102 entrées, soit **662**. Leur intégrité démontre la conservation des fichiers, pas une reproduction indépendante des essais.

La [spécification officielle SD6.00](https://www.sdcard.org/cms/wp-content/themes/sdcard-org/dl.php?f=Part1_Physical_Layer_Simplified_Specification_Ver6.00.pdf), §3.6.1 et CRC16/Fig4-21/p78, confirme le CRC séparé par ligne et le polynôme `x^16+x^12+x^5+1`. Le [format SDR ST, diapositive15](https://www.st.com/content/ccc/resource/training/technical/product_training/group0/06/8b/84/d3/16/f6/4b/95/STM32H7-Peripheral-SDMMC_interface_SDMMC/files/STM32H7-Peripheral-SDMMC_interface_SDMMC.pdf/_jcr_content/translations/en.STM32H7-Peripheral-SDMMC_interface_SDMMC.pdf) confirme la répartition des bits, les start/end/CRC16 par ligne et le statutCRC d'écriture sur DAT0.

**Limites d'accès explicites :** paragraphes et diagrammes indexés officiels consultés ; PDF SD direct inexploitable par outil, téléchargement local403. RM0456 intégral inaccessible (taille outil, timeout local) ; aucune citation d'un paragraphe non lu, aucune prétention d'audit RTL/errata U5 complet. La présentation ST H7 corrobore le format, sans être présentée comme un manuel U5 interchangeable. Les axiomes SD nécessaires sont disponibles ; la preuve formelle reste conditionnelle à leur application. Headers/HAL U5 et captures réels sont examinés en complément. [Accès et références](preuves/TR2-20261014-SDMMC-DAT-PERMUTATION/source-access.json).

[Recherche antérieure](preuves/TR2-20261014-SDMMC-DAT-PERMUTATION/prior-reasoning-search.json) : git grep sur les fichiers versionnés auSHA240a83a, puis recherche ciblée dans toutes les zones SDMMC. Aucune démonstration antérieure CRC/par ligne/inverse du round-trip ou calcul des24 permutations retrouvé. M016 REFERENCE-PHYSICAL/RAPPORT.md:L61 disait seulement « sans preuve d'un contact inversé ». Les permutations de mots Modbus sont un autre sujet. L'absence dans Git ne prouve pas l'absence dans une conversation non versionnée ; aucune analyse publiée retrouvée n'a été refaite.

## 2. Démonstration CRC et effet sur les données

**Hypothèses H :** liaison SDR4bits bijective, constante pendant trame et opérations comparées, réciproque dans les deux sens ; largeur carte/contrôleur concordante ; CMD, CLK et alimentation correctement séparés ; aucun bruit, skew, retard d'échantillonnage, inversion logique, contact instable, ligne ouverte ou duplication ; contrôleur conforme au CRC par ligne. Une permutation qui modifie aussi les conditions électriques sort de H.

Noter `p[h]` la ligne carte connectée à la ligne hôte h. En lecture, h reçoit le flux `s[p[h]]` **et son propre CRC**, puis recalcule ce même CRC : l'égalité est conservée pour toute permutation. Le CRC n'encode pas l'identité de la ligne. Start0 et end1 identiques sur les quatre lignes restent identiques. Il ne s'agit pas d'une collision probabiliste de checksum : données et checksum sont déplacés ensemble.

La reconstruction des octets change toutefois : DAT3 porte b7/b3, DAT2 b6/b2, DAT1 b5/b1, DAT0 b4/b0, avec nibble haut puis bas. DAT1↔DAT2 échange donc b1↔b2 et b5↔b6. FIFO/RAM reçoivent ces octets permutés, sans pouvoir détecter leur sens applicatif.

CMD transporte séparément commandes/réponses et CRC7. Une permutation pure DAT1–3 ne modifie pas directement R1/CMD13 de readiness. Elle peut altérer les **données** des registres lus sur DAT, dont ACMD13/SSR et ACMD51/SCR. ACMD13/SSR n'est pas CMD13 de readiness. Une permutation peut ainsi changer des champs sémantiques de SSR tout en laissant le CRC accepté.

DAT0 a des rôles particuliers : payload en1bit, busy et tokenCRC d'écriture. Déplacer DAT0 peut faire échouer ces phases, même si l'algèbre CRC d'une trame4bits entièrement reçue reste vraie. On ne déduit donc pas24 round-trips matériels garantis. Les six permutations fixant DAT0 évitent cette difficulté dans H. Si le contrôleur ou le timing ne respecte pas H, DCRCFAIL devient possible ; cela ne démontre pas qu'une permutation pure l'aurait provoqué.

## 3. Round-trip, oracles et signature55AA

Écriture : `W_p(x)[p[h]]=x[h]`. Lecture : `R_p(y)[h]=y[p[h]]`. Donc **`R_p(W_p(x))=x`** pour chaque nibble : la carte peut stocker une version permutée, puis la liaison annule la permutation au retour. C'est vrai aussi pour les cycles de trois lignes. Utiliser p² au lieu de l'inverse réciproque serait une erreur pour ces cycles.

Un SHA exact après un round-trip sur le même chemin ne garantit donc pas les octets stockés vus par un autre hôte. L'annulation suppose même permutation et même mode pour write/read, write réellement acceptée et contrôleDAT0 opérationnel. Elle disparaît si l'un des chemins est1bit avec DAT0 correcte, passe par un lecteur indépendant ou si le câblage change. SSR est créé par la carte, pas écrit par le test : pas d'annulation write/read possible pour ce registre.

00/FF et autres motifs symétriques ne discriminent pas l'ordre. **Octets55 AA : DAT1↔DAT3 les conserve**, contrairement à DAT1↔DAT2 (33CC) et DAT2↔DAT3 (9966) en lecture. La signatureMBR seule ne suffit pas ; sa valeur ici est un calcul, pas une nouvelle lecture du MBR. Une signature écrite puis relue sur le même chemin peut aussi subir l'annulation.

Un SSR valide déclarant width2, pris seul, laisse DAT1↔DAT2 invisible dans le champ de largeur ; d'autres champs peuvent être altérés. FIRSTFAULT B refuse le SSR : sa variable width0 initialisée ne prouve pas une largeur1bit ou un mapping. RESP1 est une réponse CMD, pas le payloadSSR.

## 4. Preuves primaires et séparation des bancs

Les chemins ci-dessous sont relatifs à `preuves/`, avec le dossier explicitement indiqué. Les contrats/rapports correspondants et leurs hashes sont dans le catalogue.

| Mission et banc | Preuve primaire examinée | Interprétation |
|---|---|---|
|M016 REFERENCE-BENCH, SDMMC2|TR2-20261013-M016-SDMMC-REFERENCE, provenance/builds/manifest ; [rapport](archives/TR2-20261013-M016-SDMMC-REFERENCE-BENCH/RAPPORT.md)|Préparation sans matériel : aucune exclusion de câblage.|
|M016 REFERENCE-PHYSICAL, SDMMC2|TR2-20261013-M016-SDMMC-PHYSICAL : `readonly-4MHz-actual-divider.log`, `readonly-4bit-4MHz-rising-clean.log`, `readonly-differential-analysis.json`, `readonly-1bit-4MHz-commanded-rx.bin.hex`|CRC4bits24/4MHz sur fronts descendant/montant : permutation pure insuffisante. Write4bits campagne2, puis read1bit4MHz après reset,4096octets exacts : le read ne peut annuler une permutation de write.|
|M016 GPIO, ancien SDMMC2|TR2-20261013-M016-GPIO-DAT123 : `user-measurements-summary.json`, `gpio-phase1-observed.log`, `gpio-phase3-observed.log`, `gpio-phase5-observed.log`|Singlehot3,3/0/0 puis0/3,3/0 puis0/0/3,3 au breakout, contrôlesMCU. Indice fort d'ordre correct aux contacts identifiés par l'utilisateur ; pas mesure ohmique jusqu'au socket, ni preuve pour le recâblageSDMMC1 ultérieur.|
|M016 LOAD1bit, SDMMC2|TR2-20261013-M016-1BIT-LOAD : `run-analysis.json`, `integral-prefix-verification.json`|1310720octets exacts/error0 ; overflow après4,164454s. DAT1–3 inutilisées : leur permutation pure n'explique pas le déficit de débit.|
|M017, SDMMC2|TR2-20261014-M017-SDMMC-1BIT : `run-01-result.json`, `run-01-prefix-verification.json`, `prefix-after-reinit-verification.json`|RUN1overflow après0,402315s ;65536octets1bit exacts avant/après réinit ; diagnosticsDMA/polling16×64KiB conformes. Préfixe1bit fournit un oracle à PoC ; pas qualification4bits.|
|PoC20cm, SDMMC1|TR2-20261014-SDMMC1-REFERENCE-POC : `release-four-bit-card-and-read.log`, `release-first-read-verification.json`, `sdmmc1_release_read.py`|Read4bits8192 à2048 exacts du préfixeM0171bit **avant toute writePoC** : contrôle indépendant du mapping de lecture à cet instant. Deux jeux128KiBwrite/read4bits seuls auraient pu masquer une permutation.|
|PoC jeuB puis lecteurPC|TR2-20261014-SDMMC1-ROOTCAUSE-REVIEW/qualification-PC-anterieure : `read-probe-result.json` ; PoC : `native-aligned-recovered-verification.json`|Write4bitsSTM32 au506304 puis lectureRealtek native128KiB exacte, SHAc8a5f1e… ; changement de chemin retire le masquage. Pas de write intermédiaire documentée sur cette zone. CID non exposé par le lecteurPC : identité étayée par capacité/zone/payload, pas authentifiée à nouveau.|
|CABLE10CM, SDMMC1|TR2-20261014-SDMMC1-CABLE10CM-COMPARISON : `host-before-debug.json`, `initial-identity-image-state.log`, `after-initial-recovery-preflight.log`|NoMedia avantdebug, erreurs3005→2005, aucun cycle. OR accumulés sans première faute : pas diagnostic de permutation. Changement20→10cm déclaré, pas présomption de mapping inchangé éternellement.|
|COLDSTART10CM, SDMMC1|TR2-20261014-SDMMC1-COLDSTART10CM : `host-before-debug.json`, `initial-identity-image-state.log`|NoMedia avantGDB,1007 ensuite, aucun cycle/resetCodex. Pas de payload valide à inverser.|
|ROOTCAUSE-REVIEW|TR2-20261014-SDMMC1-ROOTCAUSE-REVIEW : `source-review-provenance.json`, `adafruit-netlist-review.json`, `PROTOCOLE_LECTURE_PROPOSE.md`|Pullups et recovery/NoMedia étayés ; aucune ancienne démonstration formelle de permutation retrouvée.|
|FIRSTFAULT, SDMMC1|TR2-20261014-SDMMC1-FIRSTFAULT : `RUN-A-decoded.json`, `RUN-B-decoded.json`, `SSR-firstfault-source-excerpts.json` ; [rapport archivé](archives/TR2-20261014-P12H3H-SDMMC1-READONLY-FIRSTFAULT/RAPPORT.md)|A1bit512/8192 exacts du jeuB ; B premierSTA2 surSSR64/ACMD13 avantCMD17/CMD18. Permutation pure incompatible comme cause suffisante ; mapping actuel non établi par un payloadB valide.|

Les essaisPC1GiB/8MiB étaient dans d'autres zones. La source des oracles et l'absence documentée d'écriture intermédiaire sont des conditions de l'inférence : une opération non enregistrée n'est pas rendue impossible par ce rapport.

**Calcul nouveau hors ligne :** dumpM0164096 SHA9ec45ab0… et FIRSTFAULTA8192 SHA2ac5183f… relus depuisGit, comparés intégralement aux motifs. DumpPoC8192 local `/home/lolo/dev/msm/reports/TR2-SDMMC1-POC-20261014/release-original-read-8192.bin` relu et SHA486984c9… vérifié contre le résultat versionné, puis comparé au motifM017. Chacun contient les256 valeurs d'octet ; seule l'identité est compatible. Fichiers et hashes complets : [résultats](preuves/TR2-20261014-SDMMC-DAT-PERMUTATION/permutations.json). Les reads4bits refusés, avec RAM partielle/stale, ne sont pas utilisés pour exclure un mapping.

## 5. Matrice des permutations

p=(destination carte deH0,H1,H2,H3). SousH, chaque mapping conserve CRC et start/end. Diff=contrôles M016write4→read1 / jeuBwrite4→FIRSTFAULTread1 / M017write1→PoCread4.

|p|CRC idéal|Octets modifiés/256|55AA lu|SSRwidth2 isolé devient|Diff4096/8192/8192|Statut|
|---|---|---:|---|---:|---|---|
|(0,1,2,3)|accepté|0|55AA|2|0/0/0|Identité non exclue ; ne cause pas unCRC seule.|
|(0,2,1,3), D1↔D2|accepté|192|33CC|2|3072/6144/6144|Exclusion conditionnelleE ci-dessous.|
|(0,3,2,1), D1↔D3|accepté|192|55AA|0|3072/6144/6144|E ; signature55AA non discriminante.|
|(0,1,3,2), D2↔D3|accepté|192|9966|1|3072/6144/6144|E.|
|(0,2,3,1), cycle3|accepté|240|33CC|1|3840/7680/7680|E ; round-trip algébrique identique.|
|(0,3,1,2), cycle inverse|accepté|240|9966|0|3840/7680/7680|E ; round-trip algébrique identique.|
|18 cas déplaçantD0|accepté si trame4bits entière reçue|192–252|variable|variable|nonzero pour tous|Exclus dansH par1bit exact avec seuleDAT0 active ; contrôle busy/CRCwrite non garanti.|
|Permutation +skew/contact/bruit ou changement pendant trame|refus possible|non prédit|variable|invalide possible|non prédit|Combinaison non exclue, non démontrée ; hors permutation pure.|
|CroisementCMD/CLK/alimentation, ligne ouverte/court|hors modèle|sans objet|sans objet|sans objet|sans objet|Init/identité/read1bit défavorables à un croisement permanent grossier ; autre défaut matériel indéterminé.|

**E :** permutation pure constante exclue dans chacun des contrôles concernés et comme cause suffisante du CRC B. Une permutation actuelle combinée à un autre défaut reste indéterminée, après recâblages. « Exclue » est une incompatibilité déterministe sousH/provenance, pas une certification de chaque fil aujourd'hui. Les24 cas individuels sont calculés danspermutations.json ; pas de succès opérationnel supposé pour les18 déplaçantDAT0.

## 6. Premier DCRCFAIL SSR

RecordFIRSTFAULTB : phase4, overlayHALligne3401, kind4, `STA=2`, `RESPCMD=13`, `RESP1=920`, `ARG=0`, `CMD=10D`, `DCTRL=62`, `DLEN=64`, `DCOUNT=0`, `CLKCR=4002`, `IPSR=0`. Capture dansSD_SendSDStatus avantclear, HALerror encore0 ; ensuiteErrorCode2/terminal99. RXOVR20, timeout8, CCRC1 absents **de cette capture**. DCOUNT0 n'est pas une preuve d'intégrité.

SousH, le flux SSR et son CRC se déplacent ensemble, comme une lecture de blocs : la permutation seule ne produit pasSTA2. Il faut une variable horsH ou un défaut d'implémentation pour expliquer l'événement. Le CRC n'est pas calculé à partir du champ sémantique DataBusWidth. Pas de payloadSSR64 brut valide dans les preuves du RUN B ; buffers512/8192 inutilisés etRESP1 ne le remplacent pas.

La HAL originale envoie encoreCMD16/512 aprèsclear, avantretour ; RESP16 terminal ne contredit pas la première captureACMD13. Ce nettoyage interne et la limite littérale « aucune commande après erreur » restent documentés, sans réécriture historique. Aucun CMD13/récupération/retry ajouté par le probe. USB/MSC etCMD18 n'étaient pas nécessaires à ce premier défaut, sans exclure leur rôle dans d'autres épisodes historiques.

## 7. Mapping documentaire

[ST UM2861 Rev10, tables19/20](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf), [MB1549-C05](https://www.st.com/resource/en/schematic_pack/mb1549-u575ziq-c05-schematic.pdf) et MSP réels relus :

|Signal|SDMMC2 historique|SDMMC1 récent|
|---|---|---|
|CLK|PD6 AF11 /CN11-43|PC12 AF12 /CN11-3|
|CMD|PD7 AF11 /CN11-45|PD2 AF12 /CN11-4|
|D0|PB14 AF12 /CN12-28|PC8 AF12 /CN12-2 viaSB19|
|D1|PB15 AF12 /CN12-26|PC9 AF12 /CN12-1 viaSB22|
|D2|PB3 AF12 /CN12-31|PC10 AF12 /CN11-1|
|D3|PB4 AF12 /CN12-27|PC11 AF12 /CN11-2|

CN11-16=3V3, CN11-18=5V, CN11-20=GND. Le repérage actuel déclaré est aubergine16/bleu20, confirmé hors tension dans la mission précédente ; aucune nouvelle mesure ou remise sous tension demandée. SB19/SB22 fermés ajoutent des stubs selonST, sans constituer un échange de lignes. SDMMC2/DMA/HALCube1.9.0/24 ou4MHz et SDMMC1/polling/HALtiers1.6.2/12MHz ne sont pas des configurations interchangeables.

[Adafruit pinouts](https://learn.adafruit.com/adafruit-microsd-spi-sdio/pinouts) : labels D0/D1/DAT2/D3 distincts deCMD, alimentation3,3V. [Netlist épinglée](preuves/TR2-20261014-SDMMC1-ROOTCAUSE-REVIEW/adafruit-netlist-review.json), Eagle95979247… relue : DO→D0, DI→CMD, CS→D3, DAT1/DAT2 distincts ; pas de permutation dans la conception. Les aliasSPI ne doivent pas être confondus avec des DAT. Pullups47kΩ et rails prévus ne prouvent pas population/câblage actuels.

## 8. Vérifications encore utiles, sans nouvel essai

Aucune vérification supplémentaire nécessaire pour réfuter la permutation pure comme **cause suffisante** du CRC sousH. Les preuves croisées et le calcul suffisent. Pour trancher un mapping actuel, examiner d'abord toute photo/netlist datée déjà disponible et la chronologie exacte des zones write/read ; un autre ancien payloadSSR valide, s'il existe, serait exploitable. Les fichiers consultés ne fournissent pas ce payload duRUN B.

Certifier le point-à-point actuel jusqu'au socket demanderait éventuellement une vérification physique hors tension, à encadrer par une autre mission si nécessaire ; elle n'est ni effectuée ni demandée ici. Les contrôlesGPIO antérieurs ne valent pas cette certification après recâblage. Attribuer le CRC à signal/rail/mode exige d'autres preuves, sans campagne automatique ni recâblage aveugle. Ne pas rejouerA/B ou corriger recovery à partir de cette revue.

## 9. Préservation, validation et publication

FIRSTFAULT archivée intacte depuis240a83a : contrat **BLOCKED**, rapport **FAILED**, résolution conditionnelle du repérage et limiteCMD16 conservées. Cette contradiction administrative est signalée, pas corrigée rétroactivement. Autres archives, preuves, firmware, schémas, gouvernance, gels et detteA04 inchangés.

Dépôt historique propre82169af7e76797731358852bec83bfb0ceb54ccf ; worktreeisolé `MSM_vibe-SDMMC-DAT-permutation-review`, branche `codex/p12h3h-dat-permutation-review`, basé163255a. Stashes6251b988e8b20f97997a8f98fd26d4d35a45a1b2 etf3b829e6188f491920edf74a5f9ed0f559d08b87 préservés. Aucun merge/rebase/reset/clean ou écrasement.

Validation proportionnée : documents/diff, manifests/archives et [script hors ligne](preuves/TR2-20261014-SDMMC-DAT-PERMUTATION/analyse.py),24 permutations×256octets, inverse dans lesdeux sens, CRC indépendantstdlib,3 jeux antérieurs intégralement comparés. Commande exécutée depuis le worktree : `python3 'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261014-SDMMC-DAT-PERMUTATION/analyse.py' --repo "$PWD" --out 'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261014-SDMMC-DAT-PERMUTATION'`. Résultat24cas/3datasets/662empreintes/0matériel. Aucun échec de calcul/manifest masqué. Pas de compilation ou tests de production sur code inchangé. `result_sha:null` avant commit : aucun firmware nouvellement évalué. Le SHA du script de modèle effectivement exécuté sera indiqué après publication, sans l’attribuer à un nouveau runSTM32.

État matériel **hérité et non relu** : secondeNucleo probeB SHA fdb7fa151bd69051c839cf11a1f1e3eb134d6a821a07ab0b827eef1d91bdf829 au terminal99/trigger0/run1/error2, débogueur détaché ; carte insérée/STLINKseul/CN15off selon dernier utilisateur. Principal/FRAM/microSD512Go/Modbus/protections jamais accédés dans cette mission. Aucun geste physique nécessaire pour cette revue.

Journal intégral relu et conservé sans modification : le contrat limite la publication au rapport et aux preuves documentaires nécessaires (exception de périmètre à l’actualisation normale AGENTS§18). Conclusions, preuves croisées, limite55AA et mapping actuel non certifié sont transmis ici à ChatGPT ; aucun changement de règle ou de jalon. Publications et relecture distante à effectuer avant DONE ; SHA final et étatGit annoncés après vérification. Contrôle indépendant ChatGPT distinct de cette clôture documentaire.
