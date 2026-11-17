# Bring-up isolé des shields — NON-PRODUCTION

Mission TR2-20261117-SHIELD-DUAL-BRINGUP. Aucun démarrage automatique, acquisition
longue, service de provisioning ou UART/Modbus. Boot `probe.stage=1`, commandes RAM
indépendantes, consommées une fois. Trois CS hautes, PG4/DE bas. Le banc ne remplace
pas le firmware de production. Cœur partagé et drivers historiques inchangés.

## Construire

Projet CMake autonome ; outil ARM qualifié du dépôt, SDK CubeU5 v1.9.0 ; configurer
avec `CMAKE_TOOLCHAIN_FILE` vers `05_Firmware/platform/stm32/cmake/arm-none-eabi-gcc.cmake`
et `STM32CUBE_U5_ROOT`. Cible `shield_bringup.elf/.bin`. Tests natifs séparés avec
`-DBRINGUP_HOST_TESTS=ON`, puis build et CTest. Instrumentation HAL existante avec
contrôle d'empreinte conservé. SPI1 1 MHz / SPI2 250 kHz puis 4 MHz / SPI3 2 MHz mode3
sont des fréquences configurées, pas des mesures aux fils.

Comparaison diagnostique FRAM : `-DBENCH_FRAM_DIVIDER=64` sélectionne 250 kHz.
La valeur par défaut16 conserve 1 MHz ; seules ces deux valeurs sont acceptées.
Les comparaisons intégrales, l'exigence de zéro écart et les armements ne changent
pas. Préserver toute lecture instable avant de changer de profil ; ne pas la
présenter comme une sauvegarde fiable ni écrire sur cette base. Un succès à
250 kHz ne qualifie pas 1 MHz et ne prouve pas l'absence de défaut de soudure.

## Commandes et garde-fous

L'opérateur doit vérifier SN/UID, Flash sauvegardée et image exacte avant flash
standard qualifié à 0x08000000. Lire `probe` après chaque commande ; ne pas chaîner
sur une erreur ni répéter une commande pour masquer un refus. `probe_command=N`
seulement après lecture de l'état d'attente/repos. Aucune commande physique implicite.

| N | Effet | Condition supplémentaire |
|---|---|---|
|1|RDID FRAM, sauvegarde 262144 octets et seconde lecture comparée|Lecture seule ; RDID 04 7F 48 03 obligatoire|
|2|CID/CSD SD, secteur0 et montage FAT32|Lecture seule ; CID entier et 62333952 secteurs de la 32 Go exacts|
|3|WHO_AM_I IIS3DWB|Essai distinct explicitement justifié, jamais automatique|
|4|16 XYZ en ±8 g avec restauration CTRL1/CTRL3|WHO conforme ; observation bornée, pas qualification métrologique|
|5|Motif FRAM 64 octets puis restauration et comparaison intégrale|1 réussi + export hôte vérifié + armement explicite|
|6|Création fichier neuf 4096 octets, sync/close/relecture intégrale/CRC|2 réussi + armement explicite + nom inexistant|
|7|Suppression du seul fichier scratch vérifié, contrôle d'absence|6 réussi + armement explicite|

Écritures : `probe_arm=0x53414645` avant la seule commande autorisée ; effacé dès
consommation. Pour5 : exporter `probe_fram_snapshot` sur l'hôte, vérifier taille et
CRC32/SHA256 puis seulement positionner `probe_export_confirmed=1`. La zone
0x3FFC0..0x3FFFF n'est **pas déclarée libre** : méthode sauvegarde/restauration
explicitement autorisée par le contrat. Tout échec de restauration impose arrêt
et préservation des deux copies ; ne pas effacer/provisionner la FRAM.

Pour6 : noms 8.3 stricts `0:/T1700201.TMP` ou `0:/T1700101.TMP`, CREATE_NEW, aucun
écrasement/formatage. Carte autorisée CID0353445344333247858bf0923c01927b seulement.
Writer CMD24 historique conservateur, refus latched, pas de retry après incertitude.
Un fichier incomplet reste disponible pour diagnostic. Horodatage FAT de test fixe,
pas une datation réelle. Aucune carte 512 Go acceptée. Ne pas utiliser7 sur un fichier
préexistant. Vérifier writer inactif, CS relâchée, absence d'incertitude avant transfert
hors tension demandé à l'utilisateur. Aucun accès au lecteur Windows dans ce firmware.

Les tests hôte portent sur les refus UID/carte/armement/zone FRAM ; ils ne prouvent
ni les tensions, ni le routage du shield, ni le fonctionnement des modules.
