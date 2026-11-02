# B02 — chaîne autonome CN15 + SD : minute COMPLETE relue sur PC

Conditions user : CN15secteur5V seul/JP6USB/JP2retiré/STLINKoff/SD512insérée/WAIT.
Un appuiRESET accidentel est rapporté AVANT USER ; retourWAIT puis départUSERlong,
vertfixe. Ce reset antérieur au départ n'est pas un échec pendant l'acquisition.
Aucun START, reset ou flashagent pendant la capture/récupération. FirmwareB02
source3f803d77393c6853ed76d796367df443a0eb9eb9/BIN53312B exact au relevé.

## Résultat embarqué et récupération

Snapshot67488B CRC50fbba78 conforme/SHA25d940c9aee32c56b11c2184420c25873cb3a0361ea704700c5dfb6bd0adefcd.
Capturé4/error0/run5/mode1,60000254µsMCU,1585192natifsacceptés/158520sorties,
597checks/résidumax28µs≤100, FIFO0/STOP134µs/stoppedtrue, rails/clips/flags0,
255groupesSDterminés/1040896Btransférés, filevide/peak5120B, uncertainfalse,
needs_power_cyclefalse. Maximaµs service1322,85625/consumer720,71875/gap1582/
quantumSD456,39375 tous sous les gardes. sd_write_max85133µs est la durée complète
d'un groupe coopératif, pas le blocage d'un poll ; ne pas confondre ces métriques.

ReconnexionSTLINK suivie d'un nouveauboot enlectureFAT : snapshotpréservé, codeactuel
ne représente pas le résultatcapturé. Attente de fin du scanlectureseule ; arrêt
surpréflightFR_EXIST(8) car B02déjàprésent. Relevé05 : FIFO/writeroff/incertitudefalse/
needs_cyclefalse/SDwrites0/CShaute, GDBdétaché. La lecture en cours n'est pas un second
START et le refus de nom existant est la protection attendue. Usertransfère ensuite
la512GoauT1000,001restehorstension ; aucune002/FRAM/COM4/format/réparation.

## Vérification complète PC

Disque512Go/MBR/partitionoffset1048576/longueur511867584512/FAT32cluster32768,
CIDarchive03534453443531328562e43cf80195eb, UID001/source3f/FS16/488µgLSB/v3.
Copieexclusive2097152B B020005.TR2, hashes source/destinationidentiques :
**d49dbd86954125c37563f0418e5af4744ec2851b5a4acf8c1af7766f35b680d4**.
LecteurstrictCOMPLETE,2033records/158520XYZ/951120Bpayload, CRCheader/records/footer,
séquence/session/qualité/durée60000254µsMCU/source/profil vérifiés. QualitéOR1 = warmup,
pas rail/clip. Digest de TOUS les XYZ strictementégal au producteurRAM :
**d8afd2a04abc4a987577d8806f90e45896ab8c8bf1b49d44bda051737e2b0d69**.
Parcoursindépendant desoctets reproduit2033CRC/séquences/comptes/digest. Neufcaptures
A1A..H etB01 SHAinchangés. Pas de test ancien répété : nouveau fichier réel vérifié.

Archivebrute2MiB/copieintégrale etXYZ restent localementT1000, pasGitHub. Preuvescompactes
publiées : header/footer exacts, snapshotcompressé, sortieslecteur/copie et auditparrecord
compressé. Un recontrôleexhaustifindépendant nécessite la copiebrute locale identifiée
ci-dessus ; les seules empreintes publiées ne reconstituent pas les XYZ.

## Portée et suite

PASS physique borné60s de chaîne capteur/FIR16g/SD512 surCN15 seul, jusqu'à relecturePC
intégrale. Ni endurance8h, powerbank, tenueCEM/mécanique enmachine ou duréeSI/UTC qualifiées.
Les accélérationsDC sont conservées dans ce format expérimental ; aucune évolutionnormative.

Palier10min préparé par nouveau buildARM ciblé de la même chaîne normale001,
optionsdiag/comparateurOFF, durée600000000µs/préfixeT10/nouveauT100005.TR2,
SPI3 8MHz/SD4MHz/FS16/gardesinchangés. Source d7bd4df001576d5fef9b0af034b1cb96c505859a,
artefactsT10-image.json/compiler-flags exacts, aucunmodification du code etaucunsuitehôte
inchangée relancée. Préallocation640MiB conservée du profil ; libre etnomneuf contrôlés
surPC, gardeCID/freespace/CREATE_NEW surcible avantécriture. Pasencoreflashé niSTART10min.
001hors tension,512GoauPC. PourchargerT10 : carte restePC, alimentationCN15d'abord,
STLINKensuite/lecteur001vide/JP6USB-JP2off, aucunUSER ; manipulationuser requise.
