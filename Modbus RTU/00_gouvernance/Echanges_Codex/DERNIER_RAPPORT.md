---
mission_id: TR2-20261117-SHIELD-DUAL-BRINGUP
status: BLOCKED
base_ref: main
base_sha: f019dfbaa4e2cf9cf181031d0534013b89d989e1
initial_head: d5d3da95697607ad21cc57f95f48534e49ed3bb6
result_sha: 2e39e2c61b5e9e4960a306aed1a47f517cb70dbf
created_at_utc: 2026-11-17T20:44:11Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Défaut de brochage SPI3 confirmé — qualification suspendue, acquis SD/FRAM préservés

**Mission non clôturée.** TR2-001 a réussi le test SD/FAT32 et le test FRAM avec restauration à 250 kHz. La FRAM reste non qualifiée à 1 MHz et le capteur n'est pas identifié. TR2-002 est démonté et hors tension pour reprise des soudures de masse. Le brochage SPI3 erroné impose maintenant une décision corrective avant toute réalimentation des shields ; aucune émission RS485 ou communication Modbus n'est validée.

**Constat nouveau prioritaire : erreur de brochage SPI3 du shield confirmée.** Le PCB raccorde SCK au contact22, qui est **SDO/MISO** sur MKIGIBV2 ; l'utilisateur mesure J2-6→TP3=0,3Ω puis TP3→SDO=0,3Ω, contre≈600kΩ versSCL. Le schéma ST impose CS19/SCL20/SDO22/SDA21, alors que notre PCB utilise21/22/24/23. Ce blocage est une erreur du dossier de conception, pas une preuve de quatre soudures défectueuses. **Aucun nouvel essai alimenté, aucun décalage du module ou pontage à exécuter.** Proposition corrective logique préparée, CAD/CAM inchangés ; décision de périmètre requise.

## Résultats par ensemble

| Interface | TR2-002 | TR2-001 |
|---|---|---|
| Identification Nucleo | SN0032001F3235510C37333439 ; UID001B0046/38425018/20333157 | SN003500463235510B37333439 ; UID004A003D/38425018/20333157 |
| FRAM | PASS borné à 1 MHz : RDID, double lecture intégrale, motif64 octets et restauration intégrale sans différence | FAIL stabilité à 1 MHz :9 différences ; **PASS borné à 250 kHz**, double lecture, motif64 octets et restauration intégrale sans différence |
| IIS3DWB | FAIL WHO00 au lieu7B, à8 puis2 MHz configurés ; aucun XYZ | FAIL WHO00 conservé sous CN0, puis unique essai à2 MHz ; aucun XYZ |
| microSD/FAT32 | FAIL initialisation/timeout, CID non lu ; aucune écriture | **PASS court** : CID/CSD/capacité/FAT32, fichier neuf4096 octets relu intégralement/CRC puis supprimé |
| RS485/Modbus | NON_TESTÉ, gates matériels en attente | NON_TESTÉ, gates matériels en attente |

Les périphériques ont pu être permutés. Aucune association module déduite du numéro de Nucleo, aucun numéro de série FRAM ou capteur inventé. Les PASS ci-dessus sont limités aux essais observés ; aucune endurance, tenue mécanique, qualification industrielle, coexistence générale ou mesure vibratoire qualifiée.

## Conditions physiques rapportées par l'utilisateur

002 a été testé en premier sur demande explicite, sous ST-LINK/JP6STLK, CN15 débranché, montage annoncé correct. Après les mesures de masse anormales, l'utilisateur confirme002 entièrement débranché, shield séparé et prêt pour reprise des soudures. Cette reprise n'est pas encore confirmée terminée.

Pour001 : ST-LINK/JP6STLK, CN15 débranché, même carte de test transférée, RS485 connecté et pont J2 du Click annoncés. L'utilisateur confirme inspection hors tension de C2, modules et soudures sans pont ou contrainte, et **3,3 V aux alimentations des breakouts**. La résistance J2-11/TP2, non disponible au gate initial, a ensuite été rapportée à **0,1 Ω hors tension**. Les réponses intermédiaires ambiguës sur le banc démonté ont été clarifiées comme concernant002.

Les indications CLI de3,28 V sur002 et3,29 V sur001 portent sur la cible MCU et ne remplacent pas les mesures aux modules. Aucun changement physique réalisé par Codex ; un seul banc alimenté selon les confirmations utilisateur. Aucun accès à la512 Go.

## Firmware, sauvegardes et chaîne utilisée

GDB et programmation Windows qualifiés : CubeProgrammer2.23.0, GDBserver7.14.0, GNU GDB15.2.90/CubeCLT1.22, ST-LINKV3J15M7 ; SDK Linux CubeU5v1.9.0. Qualification002 documentée dans `QUALIFICATION_M015_L_BANC_ST.md` ; procédure001 relue dans les preuves versionnées CN0 du02/11. Pas d'inférence de qualification depuis la seule disponibilité d'un outil.

Avant modification de chaque cible : UID frais, Flash2 MiB et RAM768 KiB sauvegardées ; l'exécutable est comparé intégralement à la Flash. Les dumps complets restent locaux, leurs empreintes sont publiées.

| Image initiale | Source / empreinte |
|---|---|
|002 : OBE2h,53032 octets | Source `dd2f27ca3d886742b0ab37ab378cd524012a0370`, BIN20785017… ; Flash2 MiB SHA `ca275a54998a0730d9067f4107b06830aacc59e3e5789d6e25df579e91731a7e` |
|001 : CN0,53340 octets | Source `6633b0154bf67b4c5437f1d259732fc1b1f21e4a`, BINd3186380… ; Flash2 MiB SHA `65f4ab9f828ee358e30a1df1b07f21d03848bdaab837b5ac130e311be333fcf5` |

Un flash du banc indépendant initial sur chaque cible, puis un second flash sur001 pour le seul profil FRAM250 kHz. Procédure standard : NORMAL/SWD/SN explicite/freq8000, adresse08000000, verify/reset ; uniquement pages0..4, pas de mass erase, Option Bytes ou RDP. Readback entier : image exacte et tail après40960 octets inchangée depuis l'original. Reset GDB après chaque dernier CLI selon le diagnostic NVIC historique, aucun HOTPLUG.

| Profil évalué | Code / binaire |
|---|---|
| Initial, FRAM1 MHz | `41c55205a61eb26d406edf52bfed68f7de145fc9` ; SHA256 `631b05dbc27843a7cdc780818418790057e193ed454fdd7e26d78dc7390782d7` |
| Comparatif FRAM250 kHz, actuellement sur001 | `2e39e2c61b5e9e4960a306aed1a47f517cb70dbf` ; SHA256 `019569f9d58d09189f12b167be7e71b87ee4fcbbc5f95e3f354fee1ffd0795fd` |

Boot WAIT observé, aucune commande média ni UART au démarrage, trois CS relâchées, PG4/ODR0. Ce dernier relevé ne prouve pas le niveau réel au Click ni la continuité J5/DE. Les images initiales restent récupérables ; aucune restauration inutile vers un firmware qui relancerait son initialisation de périphériques. Aucun code partagé de production ni contrôle normatif modifié.

## FRAM : échec préservé et comparaison contrôlée

RDID des modules lus : **04 7F 48 03**. Test002 à1 MHz : double lecture262144 octets identique, export vérifié, motif64 octets à0x3FFC0 puis restauration et comparaison complète sans écart. CRC32 de la sauvegarde `e20eea22`, SHA256 `8a39d2abd3999ab73c34db2476849cddf303ce389b35826850f9a700589b4a90`.

Sur001 à1 MHz, la comparaison complète signale **9 octets différents**. Première lecture conservée avec CRC1351935669 ; export hôte exact, mais **pas une sauvegarde stable qualifiée**. Aucune écriture n'a été armée dans ce profil. La seconde lecture n'a pas été conservée intégralement : ses neuf adresses différentes ne sont pas prétendues connues.

Un seul comparatif à250 kHz a été préparé puis exécuté, sans déplacer le montage. Ajout minimal de `BENCH_FRAM_DIVIDER` dans `experiments/shield_dual_bringup/` ; défaut16 inchangé,64 pour250 kHz, autres valeurs refusées. Deux cross-builds réussis ; le binaire par défaut reste **byte exact** à l'initial. La variante ne diffère que de deux octets ; différence archivée. Configuration32 explicitement refusée. Gardes, seuil zéro différence et armements inchangés.

À250 kHz : deux lectures intégrales identiques, export hôte dont CRC32 **92b9b0f5** égale le MCU, SHA256 **`f442f68373b5c41ff25408c47fbcf4dd032f0ee1257a806741668c3e55225f2e`**. Seulement après cet export vérifié, armement RAM et test64 octets, motif relu, restauration puis comparaison des262144 octets : **zéro différence**, error0. La zone n'est pas déclarée libre : méthode sauvegarde/restauration expressément autorisée par le contrat. Aucun effacement ni provisioning.

La première lecture instable à1 MHz et la lecture stable à250 kHz diffèrent sur neuf octets entre0x1000 et0x103D ; détails hors ligne publiés. Cela ne localise pas automatiquement les écarts de la seconde lecture à1 MHz. Aucun ordre d'écriture entre profils. Le défaut n'est pas observé dans l'essai à250 kHz ; **cause non établie et 1 MHz non qualifié**. Un succès à fréquence réduite ne démontre pas l'absence de défaut de soudure ou de marge électrique. La restauration est prouvée par rapport à la référence stable250 kHz, pas à un état initial dont la première lecture était instable.

## SD001 : test physique court réussi

Carte autorisée identifiée par **CID0353445344333247858bf0923c01927b**, CSD400e00325b590000edc87f800a4040c3, **62333952 secteurs, 31914983424 octets**, FAT32,64 secteurs/cluster et deux FAT. CRC transport conservés, aucune erreur de lecture. Seule carte adressée par le banc, un seul volume FatFs ; aucun accès à un lecteur Windows ou autre média pour écrire.

Fichier **`0:/T1700101.TMP`**, nom absent vérifié et CREATE_NEW.4096 octets synthétiques écrits après armement one-shot, sync/close, réouverture/relecture complète/EOF, comparaison MCU puis export et comparaison hôte octet par octet. **CRC32c0e65e2b** identique MCU/hôte, SHA dans le manifeste. Suppression par commande séparée armée, absence `FR_NO_FILE` et repos SD contrôlés.

**15 secteurs confirmés au total**, métadonnées FAT incluses, zéro CRC/timeout, writer inactif et non incertain. Aucun formatage ou écrasement d'un fichier existant. Les autres fichiers ne sont pas déclarés intégralement relus/hachés après ce test. Le PASS concerne ce fichier court et le profil initial41c55205, pas une endurance. Aucun rejeu après le changement du seul diviseur SPI1.

Sur002, timeout d'initialisation et aucune écriture ; CID/FAT non lus. Correction conservée : `commands=0` compte les réponses reconnues et ne prouve pas que CMD0 n'a pas été émis ; l'attente précommande et le délai R1 ne sont pas distingués par cette trace.

## Capteurs, défaut de masse002 et erreurs d'observation

Sur001, la RAM initiale CN0 contenait déjà une trace WHO00 valideHAL/RX complet. Extraction hors ligne depuis le dump préservé et l'ELF exact ; aucun nouvel ordre SPI pour cette observation. Puis unique essai du banc à2 MHz/mode3 : WHO00, trace valide, réception2 octets complète. Sur002, premier WHO00 à8 MHz puis unique comparaison2 MHz, même résultat. Aucune configuration/XYZ après refus ; pas de répétition aveugle. Les changements de firmware, reset et temps depuis alimentation empêchent d'attribuer isolément le résultat à la fréquence. Les GPIO/SPI instantanés ne sont pas des mesures aux fils.

002 : utilisateur mesure2,3 V aux breakouts/TP1-TP2 contre3,3 V J2-7/J2-11. Le premier repérage9 a été corrigé7, sans conclusion sur le rail5 V. Netlist ST C05 : CN8-7=3V3,9=5V,11/13=GND ; shield J2-7/TP1 et11/13/TP2 directement reliés par conception.

Pointes0,1 Ω ; chemin positif0,3 Ω ; masse d'abord≈26 kΩ, valeur non reproduite puis35 Ω de11 et13 versTP2, contre0,1 Ω entre11/13 et entreTP2/GND lecteur. Les deux contacts peuvent être reliés via la Nucleo assemblée. Retour de masse suspect, défaut exact non établi ; aucune intensité sous tension déduite par simple V/R. Soudures11/13 jugées défectueuses par l'utilisateur, reprise annoncée,002 démonté et débranché. Après reprise : continuités de masse, absence de court-circuit/pont et rails avant qualification ciblée. Aucun accès002 pendant cette intervention.

Erreurs de capture conservées : nom `bench_traces` corrigé en `bench_trace` sur002 ; symbole `r3_command` absent du CN0 autonome001, supprimé du script de lecture. Corrections sans nouvelle commande SPI/reset/flash pour refaire l'observation. Avant le second flash001, GDB a averti que le source local avait changé ; variables interprétées avec l'ELF exact de l'image initiale, structure inchangée. Après flash250 kHz, nouvel ELF exact utilisé. Pas d'erreur masquée pour annoncer un PASS.

## RS485 : réception et émission encore non testées

Pont J2 DE/RE confirmé par utilisateur sur les deux montages testés ; cela ne démontre ni J5/PG4/DE ni J7/J8 half-duplex. Sources primaires : [Click3863](https://www.mikroe.com/rs485-isolator-2-click) et [schéma v102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf). J7 relie A/Y et J8 B/Z ; VCCSEL doit être3V3. J3/RE, TERMJ5/J9, BIASJ4/J6 et montage réel à relever. GND2 reste séparé du GND logique ; polarité du convertisseur USB non présumée.

PG4/CN12-69→J5local5→ClickDE : présence/continuité non établies. Aucun port COM ouvert ni émission par Codex, pas de double maître introduit. Inventaire initial PnP : COM4 FTDI BG04KSSW etCOM5ST-LINK ; WMI initial incomplet non interprété comme déconnexion. Aucun accès au HP historique. Pas de PASS bidirectionnel déduit du seul branchement RS485.

## État final et prochaine intervention

001 : profil250 kHz en place, stage3/command5/sequence2/done_mask34, error0, commande/armement0 ; writer inactif/non incertain, CS relâchées, PG4/ODR0, GDB détaché. Les compteurs SD sont nuls **depuis ce changement de profil**, ce qui n'annule pas les15 secteurs du test précédent. La SD était au repos avant le flash, aucun ordre SD ensuite. Aucun essai ne démarre automatiquement au prochain boot.

**Actions précédentes sur001 supersédées par le constat SPI3 ci-dessous** : les correspondances21/22/24/23 que Codex utilisait étaient erronées. Garder001 hors tension ; aucune nouvelle requête WHO, aucun décalage de module ni pont improvisé. Les mesures TP3/SDO et la lecture du schéma constructeur établissent maintenant un défaut de brochage du PCB.

**Pour002** : la reprise des masses annoncée reste distincte et peut être terminée hors tension, mais ne pas réalimenter après cette seule réparation ; le même fichier de conception SPI3 s'applique aux deux shields. RepriseGND non encore confirmée, aucune continuitéSPI3 mesurée sur002 revendiquée.

La suite nécessite une décision corrective sur le routage fabriqué, ainsi que les autres contrôles physiques/RS485. Statut **BLOCKED** ; aucune qualification générale des deux ensembles et aucun changement normatif implicite.

## Validation, publication et journal

Modification limitée au banc isolé et à son paramètre SPI1. Cœur, drivers partagés, HAL, PCB/Gerber et Modbus V1 inchangés. Cross-builds des deux nouveaux profils réussis, configuration interdite refusée ; tests hôte des garde-fous1/1PASS conservés, pas rejoués car logique inchangée. Le validateur général ne construit pas ce projet autonome ; le cross-build ciblé est la preuve pertinente. Avertissements linker nosys conservés, aucun filesystem newlib utilisé. Un build ne vaut pas validation physique.

Worktree `MSM_vibe-SHIELD-DUAL-BRINGUP`, branche `codex/shield-dual-bringup`, publication normale sur main. Racine historique82169af7 et deux stashes conservés ; aucun reset/clean/merge/rebase/force push. Code évalué **2e39e2c61b5e9e4960a306aed1a47f517cb70dbf**, 96 blobs relus après publication ; dernier checkpoint documentaire **21f7ea4fda4fbc504309da2bf314b27343844fa5**, 38 blobs relus intégralement. Le présent rapport reçoit son propre SHA après commit/push/relecture, communiqué dans la réponse finale.

Preuves : `Echanges_Codex/preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/`, notamment `001-final-results-fram250-and-SD.json`, scripts/logs/invocations, manifestes et données synthétiques. Originaux et copies complètes Flash/RAM/FRAM dans `/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/`, non redistribués. Logs Windows conservés byte exact ; whitespace contrôlé hors logs bruts, pas de normalisation des preuves.

`ETAT_COURANT_TR2.md` actualisé avec les résultats, les états matériels et les actions restantes. Historique conservé dans Git et les preuves. À transmettre à ChatGPT : défaut de brochage SPI3 confirmé, correction de conception à décider, PASS limités SD001 et FRAM001 à 250 kHz préservés, échec FRAM à 1 MHz, capteurs non identifiés et RS485 en attente ; pas de clôture ni de qualification générale.

## Écart de brochage SPI3 confirmé après les mesures utilisateur

Sur001, l'utilisateur rapporte : masseJ2-11/TP2=0,1Ω ;CS314kΩ,SCL576kΩ,SDO580kΩ,SDA270kΩ depuis les contactsJ2 attendus. Puis J2-6/TP3=0,3Ω etTP3/SCL≈600kΩ. L'adaptateur est confirmé **MKIGIBV2**, la mentionV3 étant une faute de frappe. Enfin **TP3/SDO=0,3Ω** : le tronçon d'horloge est physiquement raccordé au mauvais signal. Aucun nouvel accès MCU, SPI ou alimentation pour ces constats.

Le document ST DB4012 Rev3, page2 figure2, récupéré via miroir Xon après échec de téléchargement direct ST, a été rendu et inspecté. Le brochage réel y est CS19,SCL20,SDO22,SDA21 ;23/24NC. Le lien du [schéma officiel ST](https://www.st.com/resource/en/schematic_pack/steval-mki208v1k_schematic.pdf) et le texte constructeur ont été recoupés ; URL du miroir, hash et rendu local sont distingués dans la preuve. Lecture native KiCad du PCB V2.3 conservé :CS21,SCK22,MISO24,MOSI23 et19/20NC. Quatre écarts documentaires ; erreur de la ligne d'horloge aussi confirmée sur le montage001. Le même fichier de conception sert aux deux shields, sans prétendre avoir mesuré cette continuité sur002.

Cela explique structurellement l'impossibilité d'identifier le capteur avec ces raccordements ; son état propre reste à qualifier après correction. Les numéros de broche que Codex utilisait étaient erronés ; le diagnostic imputant possiblement ces quatre liaisons aux soudures a été explicitement corrigé auprès de l'utilisateur. Les PASS ERC/DRC antérieurs ne couvraient pas la validité du brochage constructeur. Aucun problème de ce type n'est déduit pour laSD ou laFRAM, dont les résultats bornés restent distincts.

Proposition préparée dans la [note corrective](preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/ECART_SPI3_MKIGIBV2_ET_REPRISE_PROPOSEE.md) : variante séparée, mêmes broches MCU et interfaces V1, quatre affectations corrigées. Une topologie de reprise théorique comporte deux anciens nets à isoler et quatre liaisons à ajouter ; **aucun lieu de découpe qualifié et aucune instruction d'exécution donnée**. Pas de modification des fichiers PCB/CAM, firmware ou tests gelés. Ne pas ajouter de ponts sans isolement des anciens nets ; ne pas décaler le module. Garder les deux bancs hors tension, y compris002 après la seule reprise des masses.

AGENTS.md §8 prévoit l'escalade lorsque les faits contredisent la spécification ou qu'une modification hors périmètre devient nécessaire. La correction d'un routage fabriqué et le choix reprise/nouvellefabrication nécessitent une décision structurante dans le contrat de bring-up actuel («aucune refonte»). La proposition est publiée pour décision ; statutBLOCKED sur cette incohérence de conception désormais établie, en plus des autres gates physiques.

Preuves :`001-confirmed-SPI3-pinout-mismatch.json`, lecture nativePCB etscriptread-only, note corrective. Journal actualisé ; validation documentaire etnativeKiCad ciblée, pas de rerunERC/DRC oubuild sans pertinence pour ce constat. Dernièrepublication21f7ea4fda4fbc504309da2bf314b27343844fa5 relue intégralement(38blobs).

Le contrôle de cohérence hors ligne (`001-spi3-evidence-consistency.json`) vérifie les empreintes du PCB, du document miroir et du rendu, les quatre nets extraits, les liens des errata et l'absence de changement firmware/CAO/CAM. Il ne remplace ni la lecture visuelle du schéma ST ni une qualification après correction.

Deux avertissements documentaires ont aussi été ajoutés en tête de `SHIELD_PROTO_V1/docs/MATRICE_INTERFACES.md` et `V2_3_ARNACO/README.md` pour empêcher la réutilisation des anciens numéros comme consigne. Les tableaux et sources historiques ne sont pas silencieusement réécrits ; aucune modification KiCad/CAM. Ces notices renforcent l'arrêt déjà justifié par le constat et ne libèrent aucune correction.
