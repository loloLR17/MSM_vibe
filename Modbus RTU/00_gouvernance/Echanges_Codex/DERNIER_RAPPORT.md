---
mission_id: TR2-20261117-SHIELD-DUAL-BRINGUP
status: BLOCKED
base_ref: main
base_sha: f019dfbaa4e2cf9cf181031d0534013b89d989e1
initial_head: d5d3da95697607ad21cc57f95f48534e49ed3bb6
result_sha: 41c55205a61eb26d406edf52bfed68f7de145fc9
created_at_utc: 2026-11-17T19:22:15Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Deux shields — résultats TR2-002 et attente de mesures physiques

**Mission non clôturée.** Sur TR2-002, la FRAM réussit l'identification, la double lecture et le test de 64 octets avec restauration intégrale contrôlée. Le capteur renvoie `WHO_AM_I=0x00` et la SD échoue à l'initialisation. Les causes ne sont pas établies. Les mesures et observations physiques demandées sont nécessaires pour poursuivre ; aucun essai identique n'est relancé en boucle. TR2-001 et RS485 restent non testés.

**Actualisation alimentation :** l'utilisateur rapporte **2,3 V entre TP1/TP2 et aux alimentations des breakouts**, contre **3,3 V entre J2-7/J2-11**. Le premier repérage J2-9 a été explicitement corrigé ; aucune conclusion sur le rail5V n'en est tirée. Ces mesures révèlent un écart à localiser sur l'alimentation ou le retour de masse, sans preuve d'un composant défectueux. Essais matériels suspendus ; coupure ST-LINK et contrôles de continuité hors tension demandés, pas encore confirmés.

**Dernier relevé Ω utilisateur : J2-11↔TP2=35Ω ; J2-13↔TP2=35Ω ; J2-11↔J2-13=0,1Ω.** Les deux contacts de masse sont reliés entre eux mais présentent une résistance anormale versTP2. La lecture précédente≈26kΩ n’est pas reproduite et reste archivée. La liaison commune vers la masse du shield ou la prise de mesure est à localiser ; aucune soudure précise identifiée. Les tests restent suspendus hors tension.

**Complément : TP2↔GND du lecteurSD=0,1Ω**, rapporté par l'utilisateur ; la pastilleJ2-11 est inaccessible sur le montage assemblé. Cette liaison locale est continue. Il faut maintenant isoler le shield de la Nucleo hors tension pour distinguer broches, soudures et cuivre ; démontage demandé, pas encore confirmé. Aucun module ou microSD à retirer, aucune remise sous tension autorisée à ce stade.

## Périmètre et confirmations utilisateur

L'utilisateur a explicitement choisi de tester **002 puis 001**, en précisant que « premier TR2 » désignait le premier ensemble testé. L'identité de la Nucleo reste fondée sur son UID/SN, indépendamment des modules éventuellement permutés.

Confirmations rapportées : montage correct après demande d'inspection C2/soudures/orientation/absence de contrainte, TR2-002 alimenté par ST-LINK avec JP6 sur 5V_STLINK, CN15 débranché, TR2-001 hors tension et débranché ; microSD de test annoncée 32 Go insérée. Multimètre indisponible initialement, mais proposé en cas de problème avéré. Click déclaré être celui déjà modifié/testé avec pontage **J2**. Résistance 3V3/GND, tensions VDD/VDDIO/lecteur et positions VCCSEL/J7/J8/TERM/BIAS non mesurées ou relevées à ce stade. Aucun changement physique effectué par Codex.

## Résultats observés

| Ensemble / interface | Statut et preuve | Limite |
|---|---|---|
|002 / Nucleo|PASS identification : SN `0032001F3235510C37333439`, UID `001B0046 / 38425018 / 20333157`|Ne donne pas l'identité des périphériques|
|002 / FRAM SPI1|PASS borné : RDID `04 7F 48 03`, double lecture de 262144 octets identique, motif 64 octets écrit/relu puis restauré, comparaison intégrale : zéro écart|Pas de provisioning, de numéro de série module ou d'endurance qualifiée|
|002 / IIS3DWB SPI3|FAIL identification : WHO `00` au lieu de `7B`, une fois à 8 MHz puis un essai différentiel à 2 MHz ; HAL_OK et réception complète|Aucune configuration/XYZ après refus ; cause non établie|
|002 / SD SPI2|FAIL initialisation : timeout/error2, dernière commande0/R1FF, aucune réponse reconnue, aucune écriture|CID/CSD/capacité non établis ; FAT et fichier temporaire non atteints|
|002 / RS485 et Modbus|NON_TESTÉ, gates physiques en attente|J2 seul confirmé ; J5/DE, configuration Click et bus à établir|
|001 / toutes interfaces|NON_TESTÉ, hors tension|Aucun accès debug ou flash ; transfert de la seule 32 Go à effectuer hors tension par l'utilisateur|

## Identification, sauvegardes et outils

Chaîne Windows historiquement qualifiée dans `QUALIFICATION_M015_L_BANC_ST.md`, versions fraîches conformes : CubeProgrammer 2.23.0, GDBserver 7.14.0, GNU GDB15.2.90/CubeCLT1.22, ST-LINK V3J15M7. Source Linux et SDK CubeU5 v1.9.0 aux chemins prescrits ; aucune migration d'environnement.

L'inventaire WMI initial ne listait que COM5. L'inventaire PnP complet voit aussi **COM4 FTDI BG04KSSW** : une liste WMI incomplète ne prouve pas une déconnexion. Un seul ST-LINK présent et aucun processus Windows concurrent GDB/STLINK/TR2/dotnet/python dans cet inventaire. Aucun port COM ouvert ni maître Modbus créé par cette mission ; aucun accès au HP historique.

Avant toute modification, Flash 2 MiB et SRAM 768 KiB sauvegardées via GDB avec UID vérifié. Flash SHA256 `ca275a54998a0730d9067f4107b06830aacc59e3e5789d6e25df579e91731a7e`, RAM SHA256 `76839fdf2f691cab553b4d2d9176cc61dde1a1f8aa1bd877adc63444509f4be7`. Image initiale identifiée **OBE2h**, source `dd2f27ca3d886742b0ab37ab378cd524012a0370`, 53032 octets, SHA256 `20785017e36617d5b17c5fa1598cc51254388984bb293951fbca16fa484a9692`, comparée intégralement au préfixe Flash. Les octets de fin non exécutés ne sont pas utilisés pour identifier le firmware courant.

L'essai initial a utilisé cette image, sans flash/reset : garde FIFO/writer/commande au repos, unique commande RAM `r3_command=0x44335201`, puis détachement. Résultat stage99/error91/WHO00, une trace reg0F/len1/HAL_OK/error0/RX2 complet ; arrêt avant SD/calibration, zéro écriture et aucun XYZ. Trace et payload préservés ; aucun rejeu de cette commande.

## Banc indépendant et validations logicielles

Nouveau projet isolé `experiments/shield_dual_bringup/` : boot WAIT, commandes RAM indépendantes, trois CS hautes et PG4/DE bas, aucun accès aux données persistantes au boot, aucun UART/Modbus, formatage ou provisioning. Il réutilise les drivers et le HAL instrumenté avec son contrôle d'empreinte, sans modifier le cœur partagé, les sources historiques, la CAO ou le protocole V1. README avec commandes et garde-fous ; écritures désarmées par défaut.

Tests natifs des refus UID/carte/armement/zone FRAM : **CTest 1/1 PASS**, assertions actives. Cross-build du banc réellement utilisé : **réussi** avec `-Wall -Wextra -Werror`. Erreurs initiales corrigées : collision du nom `bench_uid` avec le symbole exporté de la plateforme ; ajout du chemin réel de configuration FatFs situé dans `r3_durable_fat32/src`. Avertissements linker nosys `_close/_lseek/_read/_write` documentés, aucun appel filesystem newlib utilisé ; FatFs passe par ses callbacks disque.

Validation ciblée justifiée : nouveau banc isolé, aucun cœur ou driver partagé modifié ; la validation globale ne construirait pas ce projet autonome. Aucun test acquis sur code inchangé répété. Compilation et tests hôte ne constituent pas la qualification physique des modules.

## Flash et état de démarrage

Code évalué **`41c55205a61eb26d406edf52bfed68f7de145fc9`**. Un seul flash standard sur002 après nouvelle garde UID/repos et contrôle des sauvegardes : NORMAL/SWD/SN explicite/freq8000, adresse08000000, verify/reset. Pages0..4 seulement ; aucun mass erase, Option Bytes ou changement de sécurité. Reset GDB après le dernier CLI conformément au diagnostic NVIC historique, pas de lecture HOTPLUG.

Binaire publié `shield_bringup-41c55205.bin`, SHA256 `631b05dbc27843a7cdc780818418790057e193ed454fdd7e26d78dc7390782d7`. Relecture Flash entière : image exacte et toute la suite après40960 octets inchangée. ELF/map conservés localement, empreintes dans `probe-software-validation.json`. Boot observé stage1/sequence0/commande0/arm0, zéro commande SD, CS relâchées, DE bas, CFSR/HFSR0. Le CLI indique **3,28 V côté cible** ; cela ne mesure pas VDD/VDDIO ni le lecteur SD.

Le banc expérimental est laissé en attente après refus ; pas de restauration inutile vers OBE. La sauvegarde permet la récupération des octets, sans prouver le fonctionnement applicatif de l'ancien firmware. Une coupure/redémarrage de ce banc ne lance aucun essai ni écriture automatiquement.

## FRAM : préservation vérifiée

RDID puis première lecture intégrale, seconde lecture comparée octet par octet, puis export hôte contrôlé : 262144 octets, CRC32 hôte/MCU **e20eea22**, SHA256 **`8a39d2abd3999ab73c34db2476849cddf303ce389b35826850f9a700589b4a90`**. Original conservé hors Git, manifeste publié.

La zone `0x3FFC0..0x3FFFF` n'est **pas déclarée libre** : méthode sauvegarde/restauration expressément autorisée par le contrat. Après vérification de l'export et armement RAM one-shot : motif de64 octets écrit puis relu, restauration de l'original, comparaison des262144 octets au snapshot ; résultat error0/restore_mismatches0. Aucun effacement, reprovisionnement d'identité ou diffusion du contenu complet de la FRAM. Cette preuve est bornée au test effectué, pas à l'endurance ou à la coexistence des fonctions.

## SD et capteur : refus conservés, limites

SD : une seule initialisation, errorSD2/timeout, probe_error202, last_cmd0/last_r1FF, commands0/reads0/writes0, pas de faute de transfert HAL enregistrée. **Correction d'une première formulation en conversation :** `commands=0` compte les réponses reconnues, car son incrément intervient après réception deR1. Cela ne prouve pas que CMD0 n'a pas été envoyé. Sans trace complète de ces octets, impossible de distinguer le délai précommande de disponibilité du délai R1 après trame. Ni alimentation, lecteur, carte ou soudure ne sont identifiés comme cause. Aucun fichier créé/supprimé ou formatage ; le filtre CID32Go préparé n'est **pas une identité actuellement lue**.

Capteur : un seul essai différentiel WHO dans le nouveau banc à **2 MHz/mode3 configurés**, après premier refus à8 MHz ; baisse ×4 justifiée pour vérifier si elle rétablit l'identification sans affaiblir le contrôle7B. Résultat00 inchangé, trace unique reg0F/len1/HAL_OK/error0/remaining0, préclose RXoffset2/valid1, payload00. Aucun write capteur ni XYZ. Fréquences configurées et snapshots GPIO/SPI, pas mesures aux fils ; pas de conclusion causale sur capteur/câblage/alimentation/MCU.

Le premier script de capture s'est interrompu sur le nom erroné `bench_traces` ; le symbole réel est `bench_trace`. Échec brut conservé, symboles réinspectés dans source/ELF, nouvelle capture en lecture seule : **aucun nouveau WHO, reset ou flash** pour corriger l'observation.

## Shield et RS485

Netlist V2.3 relue et liaisons archivées dans `shield-v23-interface-nets.json`, cohérentes avec SPI1 PA5/6/7+PD14, SPI2 PB13/PD3/PD4+PF12, SPI3 PC10/11/12+PC9 et J5local5/PG4. Cela ne mesure pas les soudures de l'assemblage.

Sources primaires : [Click3863 fabricant](https://www.mikroe.com/rs485-isolator-2-click) et [schéma v102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf), téléchargé et rendu pour inspection. **J2 DE/RE**, confirmé utilisateur, est distinct de **J7 A/Y et J8 B/Z half-duplex**. VCCSEL3V3, J3/RE, TERMJ5/J9 et BIASJ4/J6 restent à relever ; le tableau commercial utilise les appellations JP1/JP2 pour VCC/DE. GND2 isolé ne rejoint pas le GND logique du shield. Polarité A/B du convertisseur USB non présumée.

J5 femelle sous shield, PG4/CN12-69→J5local5→ClickDE : présence réelle et commande DE non établies. Aucun RX/TX/Modbus revendiqué, aucune émission forcée ; si DE manque, ce sera explicitement une limite du montage.

## Blocage, état sûr et suite

Mesures demandées après les échecs avérés : **VDD/GND et VDDIO/GND du kit IIS3DWB, puis 3V/GND du lecteur Adafruit**, attendues≈3,3 V ; contrôle visuel de nappe/contacts, J5 et VCCSEL sans manipulation sous tension. Les3,28 V du programmateur ne lèvent pas ce contrôle. Les observations Click J7/J8/TERM/BIAS restent aussi nécessaires. Ces interventions relèvent d'AGENTS.md §9 ; aucun accord logiciel supplémentaire demandé.

Après retour physique : correction hors tension si anomalie, puis seul essai ciblé justifié ; sinon continuités/orientation à vérifier selon schémas, sans oscilloscope supposé disponible. Pas d'initialisation SD en boucle ni formatage ; CID/FAT/fichier scratch seulement après init conforme. RS485 conditionnel aux gates électriques et DE. Ensuite, un seul banc alimenté, transfert utilisateur de la seule32Go vers001 et identification fraîche de chaque module. Les deux ensembles ne sont pas qualifiés.

Dernier état observé002 : stage99/error301/sequence4, commandes et armement0, done_mask34 (FRAM lecture+restauration), aucun capteur configuré/FIFO, writer inactif/non incertain, SD zéro écriture, CS relâchées, GDB détaché. TR2-001 jamais accédé. Le rapport est `BLOCKED` sur cette dépendance physique réelle, pas sur un build ou une publication.

## Preuves et publication

Preuves versionnées : `Echanges_Codex/preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/` — scripts, invocations, logs bruts, manifests Flash/FRAM, payloads WHO et synthèse `002-independent-results.json`. Originaux et sauvegardes complètes : `/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/`. Aucune redistribution inutile des données complètes de FRAM/Flash/RAM. Logs Windows conservés byte exact (CRLF/espaces du fabricant) : warnings whitespace documentés ; contrôles documents/scripts hors logs bruts réussis sans altérer les preuves.

Git : worktree isolé `MSM_vibe-SHIELD-DUAL-BRINGUP`, branche `codex/shield-dual-bringup`, cible de publication main. Racine historique propre82169af7 et deux stashes historiques préservés ; aucun reset/clean/merge/rebase/force push. Mission archivée sans modifier le contrat humain. Commits publiés et relus intégralement depuis GitHub : `1cb1ced71d6392305c717cdd162d2e2f5971f52d` (31 blobs) puis `41c55205a61eb26d406edf52bfed68f7de145fc9` (7 blobs). Manifests de publication joints ; SHA du présent checkpoint communiqué après push normal et relecture distante.

`ETAT_COURANT_TR2.md` actualisé : image expérimentale en place, FRAM PASS, refus SD/WHO, mesures et gates restants. À transmettre à ChatGPT : qualification incomplète, cause non établie, aucune validation industrielle/endurance/mécanique ni du Modbus bidirectionnel.

## Mesures d'alimentation rapportées — reprise après fc1d2cb7

Le checkpoint `fc1d2cb768dcf13cd3b7b21b6a5c296e92faa808` a été publié et64blobs complets comparés à la référence récupérée deGitHub. Après cette publication, l'utilisateur a relevé2,3V aux alimentations des breakouts etentreTP1/TP2 ;3,3V au connecteurJ2, d'abord indiqué9/11 puis **corrigé7/11**. Points exacts sur chaque breakout, méthode et précision du multimètre non précisés ; valeurs conservées comme observations utilisateur. Elles ne sont pas des mesures effectuées parCodex et ne datent pas rétroactivement la tension pendant le testFRAM.

Netlist officielle ST C05 réutilisée depuis les fichiers de fabrication déjà archivés localement :CN8-7=3V3,CN8-9=5V,CN8-11/13=GND. Le shield J2 reprend ces numéros ; son net+3V3 relie directementJ2-7/TP1, son GND relieJ2-11/13/TP2. Pas de régulateur, diode ou résistance en série prévu entre ces points. Source, SHA256 et lignes consignés dans `ST-C05-CN8-power-pins.json`, observations dans `002-operator-power-discrepancy.json`. La source constructeur reste inchangée.

**Hypothèses à discriminer, aucune cause acquise :** résistance/contact/soudure sur le chemin+3V3, retourGND dégradé, surcharge ou autre anomalie d'assemblage ; le seul écart des deux mesures ne les distingue pas. Ne pas shunter le rail ni alimenter un module séparément pour masquer le problème. Aucun nouveau flash/reset/debug/ordreSPI/SD/Modbus après l'annonce de ces tensions.

**Intervention demandée selonAGENTS.md§9 :** débrancherST-LINK, conserverCN15débranché et001hors tension ; vérifier d'abord enDC queTP1/TP2 est près de0V. Seulement après mise hors tension, mesurer la résistance des pointes court-circuitées, puisJ2-7↔TP1 etJ2-11↔TP2, sans démontage ni contrainte. Rapporter lesohms etnonseulement le bip. En présence d'une tension résiduelle, nepas utiliser l'ohmmètre. Coupure etrésultats nonencoreconfirmés àla publication. La reprise matérielle reste bloquée surce contrôle électrique, aucune permissionlogicielle supplémentaire requise.

Journal actualisé avec cetécart rapporté etle repérage corrigé. La validation de cette tranche est documentaire :lecture croisée netlistST/shield, conservation des valeurs utilisateur etdiff ; aucunbuild/test acquis répété.

## Contrôle du retour de masse — reprise après 58d139dd

Après la consigne de mise hors tension, l'utilisateur rapporte :pointes court-circuitées0,1Ω ;J2-7/TP1 0,3Ω ;J2-11/TP2 initialement instable puis stabilisé vers26kΩ. L'état débranché etla valeurDCprès0V n'ont pas été confirmés séparément ; ces conditions ont été rappelées comme préalables avant toute nouvellemesureΩ. Mesures utilisateur, précision/contacts nonvérifiés indépendamment.

Le chemin positif testé est proche de la résistance despointes (excès apparent≈0,2Ω, pas une mesure milliohm qualifiée). La résistance élevée côtéGND est incompatible avec la liaisondirecte basseimpédance prévue surle schéma. **Hypothèse principale : défaut du retour de masse**, cohérent avec l'écart de tension rapporté ; cela ne localise pas encore la coupure ni ne démontre que c'est l'unique cause desrefusSD/WHO. Unchemin àtravers descomposants peut expliquer une lecture instable ouélevée ; nepas en déduire un courant de fonctionnement par une simple divisionV/R.

SourceSTetnetlistshield recontrôlées :J2-11 etJ2-13 sont toutesdeuxGND, reliées àTP2 par conception. Demande ciblée sans démontage ni remise sous tension :résistanceJ2-13↔TP2 etJ2-11↔J2-13, aprèsvérificationDCprès0V. Cesmesures doivent départager un contactisolé de11, une rupture commune versleGNDshield ou un problème de prise de mesure ; aucune soudure, pose defil ouinjection d'alimentation prescrite àce stade. Preuve `002-operator-ground-resistance.json`.

Aucun nouvel accès cible, flash/reset, SPI/SD ouModbus ; FRAM sauvegardée/restaurée inchangée depuis son test. Contrôle documentaire uniquement, pas de build acquis relancé. Mission resteBLOCKED surla vérification électrique physique. Publication précédente58d139ddea13e69c9a6a698d3427ff8b74c31536 relue intégralement(4blobs).

## Localisation de la masse — dernier relevé 35 Ω

L'utilisateur rapporte désormais **J2-11↔TP2=35Ω, J2-13↔TP2=35Ω et J2-11↔J2-13=0,1Ω**. Cette dernière valeur est celle des pointes court-circuitées lors du relevé antérieur. La valeur de26kΩ initiale n'est pas reproduite : elle est conservée dans sa preuve, sans moyenne ni remplacement silencieux. Origine de cette variation et qualité de contact des pointes non établies.

**Déduction bornée :** les deux contacts11/13 sont reliés entre eux, mais leur liaison versTP2 ne présente pas la faible résistance attendue. Le défaut n'est donc pas attribuable à11seul sur ces mesures ; un segment commun vers la masse du shield ou la prise de contact de mesure reste suspect. La continuité11↔13 peut aussi passer par la Nucleo toujours assemblée : cette mesure ne prouve pas à elle seule les deux soudures du shield. Le chemin positif antérieurement mesuré0,3Ω ne présente pas le même symptôme. Aucun courant en fonctionnement n'est déduit des35Ω mesurés hors tension dans un circuit comportant des composants.

Inspection statique duPCB candidat V2.3 :J2-11,J2-13 etTP2 affectésGND, zonesGND remplies présentes surF.Cu etB.Cu ; aucune résistance35Ω prévue dans ce chemin. Cela ne démontre pas la continuité du PCB fabriqué et assemblé. La cause exacte(contact,soudure,piste,point de mesure) etson lien causal exclusif auxrefusWHO/SD ne sont pas établis.

**Prochain contrôle utilisateur, toujours hors tension et sans déplacer lesmodules :** TP2↔GND marqué du lecteurSD Adafruit ; puis TP2↔anneau métallique de la pastilleJ2-11 sur lePCB du shield, en distinguant ce cuivre de la tige déjàmesurée35Ω. Si la pastille n'est pas accessible sans démontage, nepas forcer etle signaler. Les demandes de coupureST-LINK/CN15 etvérificationDCprès0V restent applicables ; pas de nouvelordre de remise sous tension, shunt ou retouche de soudure.

Preuve `002-ground-localization-35ohm.json`. Aucun build,testlogiciel,flash/debug,SPI/SD ouModbus ; sauvegardes etfirmware inchangés. Journal mis àjour etmissionBLOCKEDsurcette localisation physique. Publication antérieureb16f8f8344a955de9b9fdbd81bd47d76464f3fc1 relue(3blobs).

## Continuité locale TP2 / lecteurSD et nécessité d'isoler le shield

Utilisateur :anneauJ2-11 inaccessible ;**TP2↔GND du lecteurAdafruit=0,1Ω**, identique àla résistance despointes rapportée. Ce contrôle valide seulement la continuité locale entre cesdeuxpoints. Associé aux35Ω depuis11 et13, il oriente vers la liaison entre lescontacts de masse côtéNucleo etle réseau de masse du shield ; il ne distingue pas encore un contact de connecteur, une soudure ou un défaut du cuivre. Ne pas présenter l'ensemble du planGND ou les modules comme qualifiés.

La mesure11↔13 réalisée enassemblage peut emprunter le GND de la Nucleo. Puisque lescouronnes de soudure ne sontpas accessibles, unnouveau contrôle identique enassemblage n'apporteraitpas cette discrimination. **Action physique demandée selonAGENTS.md§9 :** débrancherST-LINK/CN15 etl'USB de l'adaptateurRS485, vérifierTP1/TP2près0V ; séparer doucement le shield de la Nucleo enprenant lesbords, sans tirer surlesmodules, sansflexion/forçage, sansdéplacer lescavaliers, enconservantmodules etSD. Si résistance mécanique anormale, arrêter.

Shield isolé :résistances tigeJ2-11↔TP2, tigeJ2-13↔TP2 etcuivre de pastilleJ2-11↔TP2 siaccessible ; inspectionvisuelle de11/13(fissure,mouillage), sansretouche préalable. Ce changement de configuration est techniquement nécessaire pour éliminer leschemins parallèles de la Nucleo, pasunrejeu d'un testvalidé. Aucune remise sous tension demandée. Retrait du shield etrésultats nonencoreconfirmés.

Preuve `002-local-SD-ground-continuity.json`. Aucun accèsdebug, flash/reset, commandeSPI/SD ouModbus ; code etNVM préservés. Journal actualisé. Publicationprécédentef1bf6329d974958dfb6081f7e5ab5412a60c486a relue intégralement(3blobs). La mission demeureBLOCKEDsurcetaccès physique ; aucune modificationCAO ou réparation déclarée.
