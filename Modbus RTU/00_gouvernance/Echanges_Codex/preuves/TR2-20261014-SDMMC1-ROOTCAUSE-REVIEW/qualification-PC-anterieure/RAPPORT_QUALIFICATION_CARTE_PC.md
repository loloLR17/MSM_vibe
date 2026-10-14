# Qualification PC bornée de la microSD de test — 14 octobre 2026

**Résultat : carte fonctionnelle et performances compatibles avec les seuils A1 dans les essais réalisés.** Pas de certification normative A1, ni de qualification de toute la capacité, d'endurance ou du stockage TR2. Aucun défaut de données ou d'I/O observé dans les zones vérifiées.

Demande directe utilisateur : qualifier la carte de test dans le lecteur natif du PC. Autorisation antérieure explicite de sacrifier tout le contenu de cette carte de test réutilisée pour des motifs contrôlés ; aucun effacement global/formatage ici. Cette intervention PC est distincte de la mission COLDSTART10CM déjà clôturée. Pas de modification du dépôt, de MISSION_EN_COURS ou de DERNIER_RAPPORT ; pas de commit/push sous l'ancien contrat. Résultats locaux ici, pas présentés comme une publication GitHub.

## Identification et lecture préservée

Lecteur **Realtek PCIE CardReader**, modèle WindowsSDHC, PNP natif RTS5208LUN0, disque3. Capacité annoncée **31914983424 octets**, cohérente avec le média précédent62333952secteurs512 (~29,72GiB). PartitionMBR numéro1, offset1048576, longueur17179869184, lettreF. **Aucun accès au SSD du PC, disque ST-LINK ou STM32 SD Card** par les probes de données ; garde strict lecteur/capacité/partition avant toute écriture.

Les **131072 octets du jeuB précédent**, àcarteLBA506304/partitionLBA504256, sont entièrement conformes au motif connu : SHA **c8a5f1e01ab61ebcace803eb63d5cf360035b17a8aaaae309369c3922a611df4** ; comparaisonPython zéro différence. Cela soutient l'identité du média de test et prouve un accès natif réussi à ces données, contrairement à NoMedia sur Nucleo. **Le CID n'est pas exposé par cette méthode Windows** ; pas de nouvelle authentification matérielle SanDisk ou de référenceSKU exacte déduite de la capacité seule.

Partition reconnue dans MBR commeFAT32 mais volume sans filesystem lisible, conformément aux données brutes des campagnes précédentes ; ce constat ne démontre pas un défaut de carte. Aucun formatage/réparation Windows.

## Écriture/relecture séquentielle d'un Gio

Méthode `CardProbe.cs` : motif pseudo-aléatoire dépendant de l'adresse, non constant, zone **partition[1073741824,2147483648[** ⇒ carte[1074790400,2148532224[. **16384chunks×65536**, VirtualAlloc aligné, NO_BUFFERING+WRITE_THROUGH, flush, fermeture puis nouvelle ouverturelecture, comparaison de chaque octet et SHA256 de chaque flux. Région distincte des anciennes preuves PoC et records de boot.

- Écrits et relus intégralement : **1073741824octets**, **0différence**, flushOK.
- SHAexpected/read identiques : **efc54c222e733c30126fc20ec275c5406a9ea2fcc1a633f0061efdbb0107cdce**.
- Write+flush **86091.8919ms**, **12.472Mo/s**.
- Read+oracle/hash **16102.1629ms**, **66.683Mo/s**.
- Latences APIWriteFile64KiB : P50 **1.6903ms**, P95 **3.5608ms**, P99 **140.1152ms**, max **1077.7948ms** ; **44/16384** supérieures au temps d'arrivéeTR2 de64KiB (~153,6ms).
- Lecture API64KiB P99 **1.0327ms**, max **2.6955ms**.

Débit applicatif inclut génération/oracle/hash ; NO_BUFFERING évite le cacheWindows des données, WRITE_THROUGH exige confirmation des writes, flush final inclus. Durées API et globales peuvent inclure scheduling/lecteur/driver ; elles ne sont pas une isolation du temps interne de la carte.

## Accès aléatoires4KiB et effet de WRITE_THROUGH

`CardRandom.cs` : zone contrôlée8MiB àpartitionoffset3221225472 (carte3222274048),2048blocs, préremplissage déterministe dépendant index/version, accèspseudo-aléatoires5s lecture puis5swrite, flushfinal et close/reopen. Chaque read aléatoire comparé àl'oracle ; toutesles2048pages relues/vérifiées après writes. Même méthode/zone pour deux variantes ; **seule différence code0xa0000000→0x20000000 aux ouvertures write**, explicitement conservée dans les sources. Pas de suppression du premier résultat défavorable.

| Optionwrite | RandomreadIOPS | RandomwriteIOPS, flushfinal inclus | Intégrité |
|---|---:|---:|---|
| NO_BUFFERING+WRITE_THROUGH |2346.74|7.58|0différence,2048blocs finaux vérifiés|
| NO_BUFFERING seul+flushfinal |2311.16|554.76|0différence,2048blocs finaux vérifiés|

La confirmation immédiate a donc **un effet majeur sur ce chemin PC**. Cela ne prouve pas si son coût se situe dans le driver/lecteur, le protocole ou la mémoireflash. Sans cette option, les mesures dépassent **1500readIOPS/500writeIOPS**. Le cache interne de la carte n'est ni contrôlé ni instrumenté ; un flushfinal réussi ne qualifie pas le comportement en coupure d'alimentation.

## Références et réponse à « conforme ? »

[SD Association — A1](https://www.sdcard.org/developers/sd-standard-overview/application-performance-class/) : minima1500readIOPS/500writeIOPS pour commandes4KB, séquentiel10MB/s **dans les conditions de la spécification**. [SpeedClass/U1](https://www.sdcard.org/developers/sd-standard-overview/speed-class/) : classes de vitesse d'écriture minimale, pas maxima de pause. [SanDisk Ultra — fiche A1](https://documents.sandisk.com/content/dam/asset-library/en_us/assets/public/sandisk/product/memory-cards/ultra-uhs-i-microsd/data-sheet-ultra-uhs-i-microsd.pdf) ; gamme et générations variables, référence commerciale exacte inconnue, aucun débit maximal98/120/150MB/s attribué sansSKU et lecteur compatible.

**Conforme aux essais fonctionnels/performance bornés réalisés**, et compatible avec les seuilsA1 moyens mesurés sans WRITE_THROUGH ; **pas une certification du labelA1** : protocole officiel, préconditionnement complet, plage/randommix et conditionslecteur ne sont pas reproduits. Les IOPS obtenus dans8MiB ne prouvent pas ceux de toutesles32Go. Aucun signe de corruption dans leszones testées ; pas de preuve d'authenticité/capacitéphysique entière/absence de secteursdéfectueux surle reste du média.

PourTR2 : débit moyenwrite≈29fois426672B/s danscecheminPC, mais despausesjusqu'à1,08s restentcompatibles avec ceslabels. ÀcadenceTR2 une tellepause correspond à≈459865octets entrants (~449KiB). **Ne pas conclure àune margeSTM32 acquise ou tamponminimal garanti** : schedulingPC et drive/hôtesdiffèrent, reset/USB SDMMC1 nonqualifiés. Le blocageSTM32 précédent nepeutplus être attribué àune indisponibilité générale du média ; origineCRC/Nucleo resteàisoler.

## Limites matérielles et état laissé

L'accèsread `\\.\PhysicalDrive3` est **refusé parWindows** avec droitsactuels. AccèsauhandlepartitionF autorisé et utilisé. **Capacité entière31,9Go nonvérifiée**, seulement1GiB séquentiel et8MiB random horsde cettezone ; la partitionvisible ne couvre que16GiB. Une épreuve decapacité entière avecmotifunique surchaqueadresse et relecture exige accèsadministrateur au disquephysique, puis procédurestructurée dédiée ; aucune élévation/politique/ACL automatique ici.

Carte restée danslecteurPC, dernièreinspection **Online/31914983424**. Régionsbornées désormais motifsdetest, partitiontable/boot/ancienneszonesPoC conservées ; aucun nouveauformat. Nucleo, FRAM, Modbus, Flash, reset, OptionBytes nonaccédés. Touteslespreuves précédentes préservées. Pasdetestsrépétés pourconfort ; deuxièmevariantesrandom justifiée par effetWRITE_THROUGH nonmaîtrisé danslepremier résultat.

Fichiersbruts JSON/logs/scripts dans cedossier ; manifestSHA256. Réponse limitée auxobservations réellementobtenues, pasàunepublicationGitHub niàunequalificationindépendanteChatGPT.
