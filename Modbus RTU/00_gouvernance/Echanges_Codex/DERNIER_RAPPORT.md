---
mission_id: TR2-20261117-SHIELD-DUAL-BRINGUP
status: IN_PROGRESS
base_ref: main
base_sha: f019dfbaa4e2cf9cf181031d0534013b89d989e1
initial_head: d5d3da95697607ad21cc57f95f48534e49ed3bb6
result_sha: dd2f27ca3d886742b0ab37ab378cd524012a0370
created_at_utc: 2026-11-17T18:34:57Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Deux shields — checkpoint1 :identification et premier refus capteur

**Mission en cours. TR2-002 est identifié ; premier WHO_AM_I reçu00 au lieu7B. Aucun flash, reset, ordre Modbus, commandeSD ou accèsFRAM effectué à ce stade.** L'utilisateur a explicitement choisi de tester le montage002 en premier, puis001, contrairement à l'ordre initial du contrat. Pas de permutation d'identité des Nucleo ; l'identité des périphériques reste indépendante et à établir.

## État et autorisations physiques

Utilisateur :montage correct annoncé (réponse au contrôle orientationC2/soudures/absence de contrainte), alimentation002 parST-LINK, JP6sur5V_STLINK, microSD32Go de test insérée ; pas de multimètre immédiatement disponible. TR2-001 hors tension/débranché et CN15de002débranché ensuite explicitement confirmés. Click déclaré être le module déjà modifié/testé avec pontageJ2. **Résistance3V3/GND, tensions3V3/VDD/VDDIO, positionVCCSEL et autres ponts non mesurées/relevées** ; aucune assimilation à une validation électrique instrumentée.

L'inventaireWMI initial ne listait queCOM5. L'inventairePnP complet suivant voit **COM4 FTDI BG04KSSW** etCOM5ST-LINK ; ne pas conclure à une déconnexion matérielle depuis la seule listeWMI. Aucun port COM ouvert, processus GDB/STLINK/TR2/dotnet/python Windows concurrent détecté dans cet inventaire. Pas de second maîtreModbus créé. Un seulST-LINK présent :0032001F3235510C37333439/V3J15M7,NUCLEO-U575ZI-Q. Aucun accès auHP historique.

## Traces effectivement acquises sur002

- Chaîne secondecible qualifiée historiquement :`QUALIFICATION_M015_L_BANC_ST.md`. Outils frais :CubeProgrammer2.23.0, GDBserver7.14.0, GDBGNU15.2.90/CubeCLT1.22 ; mêmechaîne Windows locale. LeCLI n'a servi qu'à`--version`/`-l stlink`, **aucune connexionMCU CLI/HOTPLUG**. GDBseul, SNexplicite, UID vérifié, attach/read/detach, aucunecommande reset/flash/OptionBytes.
- UIDfrais **001B0046 /38425018 /20333157** ; PC080026f0,VTOR08000000,CFSR/HFSR0. Flash2MiB etRAM768KiB conservées localement avant changement. FlashSHA `ca275a54998a0730d9067f4107b06830aacc59e3e5789d6e25df579e91731a7e`. Pas de sauvegarde répétée inutile.
- Imageprésente **OBE2h**,53 032octets, SHA `20785017e36617d5b17c5fa1598cc51254388984bb293951fbca16fa484a9692`, source `dd2f27ca3d886742b0ab37ab378cd524012a0370`. Tous ses octets comparés à la Flashfraîche, ELF exact retrouvé dans `/home/lolo/dev/msm/reports/TR2-002-2H-OBE-20261102/arm/`. Le SHA du checkpoint documentaire à venir n'est pas le code exécuté.
- Étatinitial :stage2,run0,commande0, FIFOarrêté, writer inactif/non incertain/sansbesoin de cycle, carte non initialisée, aucun ordreSD. Instruction RAM documentée unique `r3_command=0x44335201`, après gardes de repos, puis debugger détaché. Cette entrée fait WHO puisSD/calibration seulement siWHOconforme ; aucunformatage/fichier/acquisitionlongue demandé.
- Résultat :stage99,error91,WHO00,who_ok0,run0. Traceunique reg0F,len1,lecture, HAL_OK/error0,Tx/Rxrestants0, RXcomplet2octets en transportduplex8MHz/mode3 configurés ; payloadWHO00. La captureauHALpreclose est préservée, pas une mesure des signaux au fil. GPIO/registreSPI instantanés ne prouvent pas les niveaux analogiques.
- Garde respecté :SDcommands0/writes0/durable.io_writes0 ; FRAM non accédée ; aucunXYZproduit. Le WHO n'a pas été rejoué. Une lectureGDB ultérieure a seulement conservé la trace et les registres, sans nouvelle requêteSPI. FIFO/writer demeurent inactifs etSDnonaccédée.

| Ensemble | IdentificationNucleo | IIS3DWB | FRAM | microSD | RS485/Modbus |
|---|---|---|---|---|---|
|002|PASS UID/SN/image frais|**FAIL WHO00**, cause non établie|NON_TESTÉ, aucune écriture|NON_TESTÉ, init non atteinte ; CID non lu|NON_TESTÉ, gates matériels non levés|
|001|NON_TESTÉ, hors tension utilisateur|NON_TESTÉ|NON_TESTÉ|NON_TESTÉ|NON_TESTÉ|

## RS485 :préparation documentaire, pas PASS physique

Sources primaires consultées :[Click3863 fabricant](https://www.mikroe.com/rs485-isolator-2-click) et [schéma électriquev102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf). Schéma téléchargé et rendu localement pour lecture des connexions, SHA dans `002-synthese-initiale.json`.

Le schéma distingue **J2 DE/RE** (confirmé par utilisateur), **J7/J8 halfduplex**, J1/VCCSEL3V3 et J3/RE ; le tableau commercial utilise les nomsJP1/JP2 pour les fonctionsVCC/DE. J7 relie les branchesA/Y, J8 les branchesB/Z ; les deux transceivers/paires ne doivent pas être confondus. BIASJ4/J6 et TERMJ5/J9 à relever réellement, sans supposer la polarité d'un convertisseurUSB étiquetéA/B. GND2 isolé ne rejoint pas le GNDlogique du shield.

La matrice du shield confirme PG4/CN12-69→J5local5→RTU_DE→ClickDE, RXPG8 etTXPG7. **La femelleJ5 réelle et la commandeDE ne sont pas encore confirmées** ; siJ5 absent, aucune transmissionModbus ne sera déclarée valide. Contrôle des jumpers et du bus demandé à l'utilisateur ; aucune modification de cavalier sous tension ni émission avant gate.

## Travail restant / reprise

1. Obtenir les tensionsVDD/VDDIO et le contrôlevisuelnappe après ce refus réellement observé ; aucune cause capteur/câble/alimentation/transport affirmée. Pas de répétitionaveugle duWHO.
2. Préparer des tests indépendants FRAM/SD pour éviter qu'un refuscapteur masque les autres interfaces :bootattente, zéroécritureautomatique, RDID/sauvegardeFRAM avant touttestrestore, CID/CSD/FAT avant fichiertemporaire unique et relectureCRC, aucunformatage par défaut.
3. Qualifier les interfaces de002 dans les possibilités réelles, puis transférer la seule32Go et basculer vers001 uniquement par interventionutilisateur hors tension avec étatSDquiescent établi. Relever chaque périphérique séparément, sans lui inventer un numérodesérie.
4. PoursuivreRS485 uniquement après relevéJ5/DE etClick/bus ; réceptionpassive si faisable, TX/Modbus conditionnels.

Preuves versionnées :`Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261117-SHIELD-DUAL-BRINGUP/` ; rawGDB/scripts/invocations, versions, inventaireWindows, synthèseWHO. Originaux etbinairesFlash/RAM conservés dans `/home/lolo/dev/msm/reports/TR2-shield-dual-bringup-20261117/`, leursSHA publiés sans redistribuer inutilement les imagescomplètes. Toute lectureGDB est une observation, pas une qualificationphysique globale.

## Git et journal

Départd5d3da95, worktree neuf `/home/lolo/dev/msm/projects/MSM_vibe-SHIELD-DUAL-BRINGUP`, branche`codex/shield-dual-bringup`. Racinehistorique82169af7 propre/divergencepréservée, deuxstashes historiques intacts et120worktrees sanssuppression. Mission archivée, contrat/gouvernance/PCB/Gerber/firmwareinchangés. `ETAT_COURANT_TR2.md` complété avec observations et gates restants. Aucunbuild/testhôte/cross-build nécessaire à ces seules lectures du firmware existant ; pas de validationlogicielle ou physique générale déduite.

Ce checkpoint est une tranche de diagnostic vérifiée, **pas une clôture**. Commit/push et relecture seront consignés après publication. Prochaine intervention physique requise :mesurescapteur et relevéjumpers, pas une validation intermédiaire du travaillogiciel autorisé. Mission resteIN_PROGRESS.

## Checkpoint2 — banc indépendant préparé, pas encore flashé

Premier checkpoint `1cb1ced71d6392305c717cdd162d2e2f5971f52d` publié sur main et31blobs distants relus intégralement. Les logs bruts Windows conservent CRLF/espaces du fabricant : le lint whitespace global signale ces octets ; le contrôle des documents/scripts hors logs bruts passe, sans altération des preuves.

`experiments/shield_dual_bringup/` ajoute un banc isolé WAIT à commandes indépendantes. Test hôte des gates 1/1PASS et cross-build spécifique réussi ; deux erreurs initiales de nom/configuration d'include corrigées sans toucher aux dépendances. Avertissements nosys du linker documentés dans `probe-software-validation.json`. Pas de tests globaux relancés : aucun cœur ou driver partagé modifié ; le cross-build du banc réellement utilisé est pertinent. Sauvegarde FRAM puis export hôte/armement requis avant test restauré64octets ; SD exactCID/32Go/FAT avant tout fichier neuf4096octets, relecture complète CRC puis suppression. Aucun formatage, UART/Modbus ou auto-init périphérique au boot. Ce code est préparé/compilé, **pas encore physiquement qualifié**. Mesures capteur et observationsJ5/Click toujours attendues.
