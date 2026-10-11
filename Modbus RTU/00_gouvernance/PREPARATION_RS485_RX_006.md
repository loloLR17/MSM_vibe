# Mission 006 — premier essai RX HP COM7 → TR2

11 octobre 2026. **PRÉPARÉ logiciel ; NON PRÊT pour exécution physique immédiate.**
Le contrat 006 interdit flash, debug cible et émission. Toutes les commandes
matérielles ci-dessous sont préparées pour une autorisation humaine distincte.
Aucune n'a été exécutée. Aucun changement de câblage demandé.

## État établi et conditions avant essai

| Élément | Preuve disponible | Condition restant à établir |
|---|---|---|
| SWD | Utilisateur : OpenOCD 0.12.0, STLINK V3J15M7, stm32u5x.cpu, 3,288 V | SN attendu `003500463235510B37333439` à recontrôler ; détection ≠ firmware identifié |
| HP | Utilisateur : FT232R, COM7 | Exécution hors ligne du script sur HP ; port libre ; émission jamais observée |
| Bus | Utilisateur : blanc A+→A, vert B−→B, noir GND→GND, rouge +5 V isolé ; hors tension A↔Y et B↔Z, A/B ≈110 Ω | Mesures ne prouvent ni polarité fonctionnelle, ni niveaux, ni RX |
| Carte | Utilisateur : MIKROE-3863 ADM2867E ; fabricant concordant | Révision réelle vs schéma v102, VIO/VCC et références de masse |
| MCU | Code : LPUART1 PG7 TX / PG8 RX, AF8, 115200 8E1, IRQ LPUART1/TIM6 | PG8 CN12-66 relié à RXD sortie Click ; PG7 CN12-67 à TXD entrée Click |
| Direction | PG5 CN12-68 / PG4 CN12-69 **candidats uniquement**, aucun pilotage dans le code | À U1 : DE bas, /RE bas pour recevoir sans émettre ; niveaux et ponts J2/J3 réels |
| VDDIO2 | Code transport : aucun `HAL_PWREx_EnableVddIO2` ; SDK : IO2SV bit29 PWR_SVMCR | Alimentation réelle compatible 3,3 V et domaine disponible ; ne pas écrire IO2SV sans vérifier l'alimentation |
| Firmware | Nouveau ELF contient tous les symboles RX, sans dépendre de DWARF | Flash relue identique au BIN correspondant avant toute interprétation de RAM |
| Boot | Harness contient des écritures de qualification microSD E1/E4 | Situation microSD actuelle à confirmer avant tout flash/reset ; absence du 10 octobre non transposable |

Minimum électrique, après autorisation humaine : contrôler les alimentations
logiques VIO/VDDIO2 et le niveau RXD au repos avec leur bonne référence logique,
puis DE et /RE à U1 (ou points réellement reliés). Une tension hors domaine
compatible interdit l'émission. Ne pas ajouter une masse qui court-circuite
l'isolation ; le GND du bornier bus ne prouve pas une référence logique commune.
DE=0 et /RE=0 sont les états de réception attendus ; des GPIO non configurés ne
les garantissent pas. Si alimentation/configuration VDDIO2 absente ou direction
indéterminée : arrêter l'essai, consigner la mesure et traiter le correctif sous
mission adaptée. Aucun changement silencieux de câblage, jumpers ou direction.

Références : [pinout projet](CONCEPTION_FIRMWARE_P12H3H_B2_NUCLEO_PHYSICAL_OCCUPANCY.md),
[préparation 005](PREPARATION_RTU_PRODUCTION_005.md),
[MIKROE](https://www.mikroe.com/rs485-isolator-2-click),
[schéma v102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf),
[ADI ADM2867E](https://www.analog.com/en/products/adm2867e.html).
VCCSEL et ponts du schéma ne sont pas présumés identiques au montage.

## Trame et outil HP

Dans `05_Firmware/platform/host/rs485_rx_probe/`, copier le script sur HP.
Dépendances : Windows PowerShell 5.1, .NET Framework `System.IO.Ports`, pilote
FTDI opérationnel. Aucun module Python ni application Modbus requis.

```powershell
# Hors ligne : ne crée pas de port série et n'ouvre pas COM7.
powershell.exe -NoProfile -File .\Send-Tr2RxProbe.ps1
# UNIQUEMENT pendant le futur essai autorisé : un seul envoi, aucun retry.
powershell.exe -NoProfile -File .\Send-Tr2RxProbe.ps1 -Transmit
```

Trame exacte : **01 03 03 E8 00 01 04 7A**. FC03, B1 `system_status`,
adresse registre 1000 (`0x03E8`), quantité 1 ; CRC `0x7A04`, octet faible d'abord.
Source effective : `platform/host/rtu_request.c` → codec de production et
mapping B1 du dépôt. L'unité 1 est un marqueur de test, **pas** une adresse
provisionnée supposée. Le diagnostic RX ne filtre pas l'adresse et n'exécute
aucun PDU. Le script calcule le CRC et contrôle la constante, le résidu nul et
un bit corrompu avant de pouvoir ouvrir le port.

Validation indépendante hors ligne dans Linux :

```bash
'Modbus RTU/05_Firmware/build-host-validation/tr2_rtu_request' 1 b1
# attendu : 010303e80001047a
# Écrire ces 8 octets dans un fichier binaire puis :
'Modbus RTU/05_Firmware/build-host-validation/tr2_rtu_request' --decode /tmp/tr2-mission006-request.bin
# attendu : unit=1 pdu=0303e80001
```

Windows local T1000 refuse actuellement `-File` par politique d'exécution.
Aucun contournement ni changement de politique effectué. Analyse syntaxique AST
réussie, mais **exécution PowerShell complète NON VÉRIFIÉE**. Sur HP, si le même
refus apparaît, une décision humaine sur l'exécution du script est requise avant
l'essai ; ne pas ajouter `Bypass` ni modifier la politique silencieusement.

COM7 : 115200, 8 données, parité paire, 1 stop, handshake None, DTR/RTS false.
Écriture binaire unique de 8 octets, aucun CR/LF. Silence préalable 100 ms ;
attente PC environ 1 s, sans retransmission. Données présentes avant émission :
arrêt sans envoi. Les octets reçus par le PC peuvent être un écho : conserver
l'hex, ne pas l'appeler réponse applicative. Direction automatique du câble
FT232R non démontrée par son identité ; aucun pilotage RTS supposé.

Firmware : T1.5=750 µs, T3.5=1750 µs à ce débit. 8E1 donne 11 bits/caractère,
soit environ 95,5 µs/octet et 764 µs pour 8 octets. L'écriture unique réduit les
coupures logicielles mais Windows/USB ne garantit pas le délai interoctet sur
le fil. Aucun halt/breakpoint pendant l'émission ou la clôture T3.5.

## Identification et future mise en service — ne pas exécuter en 006

Les symboles de l'ELF local ne disent rien du firmware chargé. Une lecture seule
Flash en session autorisée doit être comparée au BIN **entier sur sa longueur**.
Si elle diffère : aucune adresse de compteur de cet ELF ne doit être utilisée.
La comparaison ne qualifie pas le reste de la Flash ni les protections.

Build 006 : `build-stm32-p11c/tr2_stm32_p11c.{elf,bin}` sous `05_Firmware`.
BIN : 157816 octets, SHA256
`f9356f279d45cb411bc5b98107e0fd291e2d1ab903c216ff052083b7abfd103d`.
ELF : SHA256
`5ac5f0c66645692ff78579d792227ceb834f331baef9b8ed3a64d0ea55c293c3`.
Préserver ce couple et ses empreintes hors build avant toute recompilation.
Pas de symboles copiés d'un ancien ELF ; `arm-none-eabi-nm -S` confirme les
compteurs 32 bits et le tableau de 256 octets. La macro GDB utilise des casts
explicites et ne requiert pas les types/lignes DWARF. Son chargement seul ne
connecte ni ne modifie la cible.

**Voie WSL proposée**, ST-LINK actuellement attaché à WSL : OpenOCD SWD a été
observé par l'utilisateur ; la session GDB de cette voie n'est pas encore
qualifiée. Elle nécessite une autorisation humaine ponctuelle. La chaîne
Windows ST qualifiée le 10 octobre est décrite dans
`00_Environnement_developpement/T1000/REPRISE_QUOTIDIENNE.md` et son rapport
physique local ; ne pas déplacer implicitement le ST-LINK entre Windows/WSL.

Après autorisation, identité/protections et prérequis de boot vérifiés :

```bash
# Terminal WSL 1 : serveur, sans programmation ni reset explicite.
openocd -f interface/stlink.cfg -f target/stm32u5x.cfg
# Terminal WSL 2, depuis 05_Firmware :
gdb-multiarch -nx -q build-stm32-p11c/tr2_stm32_p11c.elf
```

Dans GDB, **future lecture initiale seulement**, après vérification du serveur
local/cible :

```gdb
target extended-remote localhost:3333
monitor halt
dump binary memory /tmp/tr2-rx006-loaded.bin 0x08000000 0x08026878
monitor resume
```

Comparer hors cible : `cmp /tmp/tr2-rx006-loaded.bin
'Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.bin'` et `sha256sum`.
Une absence de match rend le firmware **non identifié** (elle ne prouve pas
quel firmware est chargé). Une sauvegarde entière peut être préparée selon la
procédure qualifiée, sans effacement. Si flash nécessaire : nouvelle permission
`allow_flash: true`, artefact/SN/adresse `0x08000000` vérifiés, sauvegarde et
procédure ST déjà qualifiée ; commandes Windows exactes dans le rapport physique
local cité. **Ne pas utiliser `load`, `program`, reset ou effacement comme moyen
d'identification.** La voie OpenOCD de programmation n'est pas qualifiée ici.

Lecture VDDIO2 supplémentaire préparée : `x/wx 0x46020810` = PWR_SVMCR non sécurisé
STM32U575, bit29 `0x20000000` IO2SV, dérivé des headers CubeU5 v1.9.0. Ne vaut que
si contexte non sécurisé effectivement confirmé ; ne pas écrire ce registre.
IO2SV seul ne prouve pas la tension physique. Si accès refusé, ne pas changer
les protections ni essayer une écriture. DE//RE à U1 nécessitent une mesure
réelle ; lire PG4/PG5 ne prouverait pas un câblage qui n'est pas établi.

## Essai très court, seulement après levée des conditions

1. T1000, firmware identifié, session autorisée : charger `source
   platform/host/rs485_rx_probe/rx_snapshot.gdb`, `monitor halt`,
   `tr2-rx-snapshot`, conserver **AVANT**, puis `monitor resume`.
   Exiger `tr2_rtu_rx_ready=1` et binding `TR2_ERROR_NOT_AVAILABLE=3`.
2. HP : lancer **une fois** `Send-Tr2RxProbe.ps1 -Transmit` ; attendre sa fin
   (~1 s). STM32 reste en marche pendant toute cette étape.
3. T1000 : `monitor halt`, `tr2-rx-snapshot`, conserver **APRÈS**, puis
   `monitor resume`. Aucun reset ni remise à zéro de compteurs.

Deltas attendus modulo 2³² : octets +8, trames +1, ADU valides +1, ADU invalides
+0, CRC erreurs +0, transport erreurs +0 ; polling progresse. Dernière longueur
8, decode 0, huit premiers octets exactement `01 03 03 E8 00 01 04 7A`.
Compteurs = événements consommés en boucle, pas mesures électriques ni compteurs
bruts de tous les caractères présents sur le fil. Les octets après la longueur
courante dans le tableau sont anciens. Arrêter le cœur au milieu de sa copie
peut produire un snapshot transitoire : prendre APRÈS le silence et traitement,
puis reprendre et refaire uniquement la lecture si le polling n'a pas abouti.
Un second appareil/bruit peut remplacer la dernière ADU : preuve ambiguë,
ne pas conclure au succès sur les seuls totaux.

| Observation | Conclusion permise / limite |
|---|---|
| ready=0 ou polling sans progression | Diagnostic non démarré/bloqué ou mauvais firmware ; aucun verdict électrique |
| Aucun octet, polling actif | RX non établi : alimentation/direction/VDDIO2/pinout/émission câble à diagnostiquer ; ne pas inverser A/B au hasard |
| Octets mais erreurs transport | Relever last_error : 1 overrun, 2 framing, 3 parity, 4 noise ; queue pleine peut aussi signaler overrun |
| Octets sans ADU terminée | Silence/interoctets, rejet récepteur, overflow ou snapshot trop tôt ; invalid_adu ne compte pas tous les rejets de réception |
| ADU invalide / decode=4 | CRC incorrect (compteur CRC +1) ; autres decode : longueur/arguments, pas forcément CRC |
| Deltas exacts et ADU exacte valide | Réception UART et ADU/CRC de cette trame démontrées sur cet essai uniquement |
| Aucune réponse HP | Compatible avec diagnostic RX ; aucun échec serveur/TX déduit |
| Octets HP reçus | Écho possible ; ne prouve pas un serveur applicatif ni la direction TX TR2 |

Serveur Modbus/PDU, adresse provisionnée, réponse TX, turnaround, débit et E4
microSD/power-loss restent **NON QUALIFIÉS**. L'instrumentation ne modifie ni
le binding désactivé, ni le cœur, ni la configuration UART, ni DE//RE.
