---
mission_id: TR2-20261011-RS485-FLASH-RX-010
status: BLOCKED
base_ref: main
base_sha: null
initial_head: 53a99ba3cf7abee830cd95dd541fd578677a65f8
mission_sha: 92af7a40ed6dbb0fd4143c9380a17cdea48e1a06
result_sha: 0d4dde6cf31508ab54911f976f51f45c2f819152
created_at_utc: 2026-10-11T12:59:44.603088+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Mission010 — sauvegarde et flash008 vérifiés, essai HP restant

**BACKUP RÉUSSI / FLASH RÉUSSI / VERIFY RÉUSSI / DEBUG RX PRÊT OBSERVÉ / RX PHYSIQUE NON QUALIFIÉ. BLOCKED pour envoi COM7 sur HP indisponible depuis T1000 et contrôles électriques Click encore non observés.** Aucun envoi effectué, aucun retry à réaliser automatiquement.

## Références et garde-fous

HEAD initial propre main `53a99ba3cf7abee830cd95dd541fd578677a65f8`, fetch/pull fast-forward vers contrat `92af7a40ed6dbb0fd4143c9380a17cdea48e1a06`. AGENTS.md actualisé et autorisation utilisateur11octobre, protocoleV2-B3, rapport009, préparation009, bring-up008, journal intégral et procédure physique Windows T1000 inspectés. Mission READY inchangée ; base_sha/date absents du contrat signalés sans fabrication ; référence effective consignée. Archives009 exactes copiées depuis HEAD initial. Deux stashes005/002 préservés.

MicroSD absente, montage inchangé, PG4DE, PG5 et /RE sans fil, pont J2 soudé : observations utilisateur reprises du contrat010, pas nouvelles mesures Codex. Rail3,31V/SB28 documenté008. Révision réelle/J2continuité/VIO Click non mesurées ; aucun risque électrique sérieux nouveau constaté, mais cette absence de mesure ne qualifie pas la sûreté électrique. Flash ciblé avec DE bas préparé008, sans TX ajouté ; pas de modification firmware dans010. Image et voie qualifiées vérifiées avant exécution.

## BACKUP — réellement exécuté

USBIPD bus2-3 Shared, sonde disponible Windows ; inventaire CubeProgrammer `-l st-link-only` retrouve SN003500463235510B37333439, V3J15M7, NUCLEO-U575ZI-Q. Aucun OpenOCD/GDB/serveurST concurrent détecté par inventaire Windows. CLI CubeProgrammer2.23.0 :

```text
-c port=SWD sn=003500463235510B37333439 mode=HOTPLUG -u 0x08000000 0x200000 <flash-before.bin>
```

Retour0, Data read successfully ; **2097152 octets** vérifiés. Sauvegarde unique hors Git `/home/lolo/dev/msm/backups/TR2-mission010-20261011T125638Z/flash-before.bin`, SHA256 **4e49d49ac35d0e2db184ae8dc66dac5ac62b25a89d197a3749beaff12a8e6153**. Cible observée STM32U575/U585 ID0x482 RevU Cortex-M33 Flash2Mio,3,29V,SWD8000kHz. Aucune déprotection requise. Sauvegarde et logs via chemins Windows UNC `wslpath -w`, interop réellement fonctionnelle.

Restauration possible PRÉPARÉE, non exécutée : recontrôler SN et hash sauvegarde ; extraire les seuls secteurs0..19 affectés (163840 octets,20×8192) puis programmation/vérification à0x08000000 suivant procédure qualifiée sous permission de récupération distincte ; comparer les zones sauvegardées. Aucune restauration automatique/mass erase/OptionBytes. Flash au-delà de ces pages non ciblée par la programmation observée ; aucune sauvegarde FRAM/microSD revendiquée.

## FLASH / VERIFY — réellement exécutés

BIN local157944 octets SHA256 **02ed2cf64d4d78225a4781b823c9d27e96a8ea693679d8a834d230568383593b** et ELF **6546629b85dba6ba5ead029d208d9a95e67a13e0e244d477d01828b07ab4c23e**, tous deux rehashés avant programmation, concordent contrat008/010.

```text
-c port=SWD sn=003500463235510B37333439 -w <BIN008> 0x08000000 -v
-c port=SWD sn=003500463235510B37333439 mode=HOTPLUG -u 0x08000000 157944 <flash-after.bin>
-c port=SWD sn=003500463235510B37333439 -rst
```

Chaque commande retour0. Effacement standard observé **secteurs0..19**, téléchargement et **Download verified successfully**. Relecture indépendante `/home/lolo/dev/msm/backups/TR2-mission010-20261011T125638Z/flash-after.bin`,157944 octets : égalité byte-à-byte BIN008 et même SHA256. Reset logiciel/démarrage contrôlé, explicitement autorisé par cycle flash/debug010 et procédure ST qualifiée ; pas de coupure physique. Code réellement évalué `0d4dde6cf31508ab54911f976f51f45c2f819152`, sources firmware inchangées depuis008. Aucun build refait afin de préserver l'image demandée.

## DEBUG — observations réelles avec ELF008 exact

Serveur ST-LINK Windows7.14.0, `-d -g -i <SN> --frequency 8000 -cp <CubeProgrammer/bin>` (attach au firmware démarré), GDB ARM Windows exactCLT, localhost61234. Deux sessions batch attach/halt/lecture/detach, aucun `load`, breakpoint logiciel, écriture RAM/registre ou émission. Lecture de symboles seulement après concordance Flash/BIN/ELF. Serveurs lancés par Codex terminés normalement après detach.

| Observation | Première lecture | Deuxième lecture |
|---|---:|---:|
| RX ready | 1 | 1 |
| Poll count | 2545194 | 15458093 |
| RX octets / trames / ADU valides | 0 / 0 / 0 | 0 / 0 / 0 |
| Erreurs UART / CRC / ADU invalides | 0 / 0 / 0 | 0 / 0 / 0 |
| Binding production | 3 NOT_AVAILABLE | 3 NOT_AVAILABLE |

PC observés RtuReceive_Poll puis stm32_poll_event ; progression du polling entre détachements démontre reprise entre les sessions. Après seconde session detach réussi, serveur terminé ; aucune sonde laissée sous serveur de debug. SD stage5/init_status1 conforme absence carte, pas validation microSD ; CFSR/HFSR0 lors des relevés.

Adresses contrôlées dans CMSISU575 officiel : PWR_SVMCR NS0x46020810 lu **0x20000000**, IO2SVbit29 actif. GPIOG NS0x42021800 MODER **0xfffebdff** : PG4output1, PG7/8AF2 ; PUPDR **0x200** : PG4pulldown2 ; IDR **0x180** : PG4bit4=0 ; ODR0 : latchDEbas. Ce sont des **observations digitales MCU**, pas des mesures de tension DE ou /RE sur U1, ni preuve J2. LPUART1 NS0x46002400 : CR1=0x152d, CR2=0,CR3=1,BRR=0x56ce4, ISR=0x006000d0 (pas de flags d'erreur bits0..3, REACK/TEACK présents). Initialisation RX active, aucune réception effective prouvée.

## RX — blocage réel et action minimale

Inventaire `Get-CimInstance Win32_SerialPort` T1000 ne montre que **COM3 ST-LINK**, aucun COM7/FT232R. COM7 décrit par contrat appartient au HP ; aucun accès distant HP établi dans les procédures inspectées. Ne pas utiliser COM3 pour RS485, ne pas simuler l'envoi, ne pas créer de liaison distante non autorisée. **Zéro émission COM7 et zéro trame observée.**

Action utilisateur minimale restante : **sur HP**, après vérification physique VCC/VIO~3,31V et DE//RE franchement bas/J2 conforme, lancer une fois le script006 `Send-Tr2RxProbe.ps1 -Transmit` (1152008E1, `01 03 03 E8 00 01 04 7A`) et communiquer résultat/mesures ; le test offline préalable sans `-Transmit` doit réussir suivant procédure008/006. Ne pas contourner une politique Windows si refus ; aucun retry d'émission automatique. Cible laissée en exécution, baseline deuxième snapshot ci-dessus ; Codex pourra ensuite prendre APRÈS et comparer +8/+1 ADU valide/zéro erreur. Réception reste NON QUALIFIÉE avant cette preuve. Aucune réponse attendue du diagnostic.

## Livrables, validation et publication

Journal actualisé : backup/flash/verify, identité exacte008, observations digitales, limites électriques et blocage HP. Archives009 immuables. Logs et preuves hors Git `/home/lolo/dev/msm/backups/TR2-mission010-20261011T125638Z` : backup.log,backup-proof.json,flash-verify.log,readback.log,start-reset.log,snapshot.gdb,snapshot.log,snapshot-second.log,gdb-server*.log,manifest.json. Aucune donnée binaire Flash publiée dans dépôt public.

Validation proportionnée : contrôles physiques/opérations réelles décrits, inspection contenu/diff/index ; aucun code modifié donc pas de compilation/tests hôte/.NET répétés. Permissions exercées : accèsSWD/backup/flashvérifié/reset/diagnosticlecture/reprise, documentation/commit/push main. Pas de mass erase, OptionBytes,RDP,déprotection,câblage, microSD destructif,TXTR2, émissionHP ou opérationGit destructive. Travaux et freezes/E4 conservés. Champs de permissions vrais ne qualifient pas une opération absente.

Push normal main réussi ; HEAD=origin/main=`8591e8177563405bfa9e04bcff26c815412fee16` après fetch. Relecture intégrale des quatre livrables modifiés et du contrat010 inchangé depuis cette référence distante, comparaison byte-à-byte avec fichiers locaux : concordance complète ; identifiant et result_sha contrôlés. Commit `Report mission 010 verified backup and BIN008 flash with HP RX pending`. Arbre propre, stashes005/002 préservés. Mise à jour de cette preuve publiée et relue une dernière fois ; SHA final communiqué dans la réponse Codex. Statut BLOCKED conservé pour preuve RX manquante même après publication ; les étapes backup/flash/verify sont acquises. ChatGPT doit contrôler le rapport et les limites indépendamment.
