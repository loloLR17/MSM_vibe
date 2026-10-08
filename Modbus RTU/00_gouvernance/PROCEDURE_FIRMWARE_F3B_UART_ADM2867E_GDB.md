# F3-B — Préqualification UART / commande ADM2867E avec ST-LINK/GDB

## Statut et niveau de preuve

Procédure préparée, aucune observation physique F3-B encore obtenue. Sans
adaptateur USB/RS-485 ni instrument du bus, GDB peut démontrer l'initialisation
STM32, le pilotage enregistré/senti du GPIO PG4, le lancement UART et la fin
matérielle TC traitée par l'IRQ. Il ne démontre pas la tension au contact DE du
Click, l'alimentation isolée, la forme différentielle A/B, le baud mesuré ou une
réception RS-485 correcte. Ne pas déclarer l'ADM2867E ou Modbus bout en bout
qualifiés sur la seule base de cette procédure.

HEAD de départ : `18c9871481e714e05c93ee60d982c7293f488ba3` sur main.
F1/F2/F3-A sont des modifications locales non committées ; le HEAD seul ne
représente pas ce firmware. F3-B ajoute une sonde diagnostique à ces sources.
F1/F2 ne sont pas rejouées comme qualifications ; le démarrage habituel F3
acquiert sa fenêtre, puis sert son image sans la recalculer pendant ces essais.

Références : AGENTS.md ; gels P12-F (LPUART1), P12-G (TIM6), P12-H (serveur
portable) et erratum D/G ; procédure F3-A ; plan Morpho H3h-B2.
Le schéma Click v101 utilisé pour le raccordement reste celui référencé dans
la procédure F3-A. Les données matérielles ci-dessous sont des confirmations
humaines, pas des mesures produites par GDB.

## Configuration déclarée — aucun changement de câblage demandé

- NUCLEO-U575ZI-Q, IIS3DWB raccordé et fonctionnel ; microSD retirée.
- MIKROE-3863/ADM2867E raccordé et alimenté, VCC_SEL=3,3 V.
- J7/J8 fermés ; J4/J5/J6/J9 ouverts ; aucun adaptateur de bus présent.
- PG7 CN12-67 → mikroBUS RX (entrée TXD) ; PG8 CN12-66 → mikroBUS TX (sortie RXD).
- PG4 CN12-69 → CS/DE ; J2 doit coupler DE et /RE. RST (/RE via J3) sans
  second pilotage ; PG5 non raccordé. Ce couplage n'est pas déduit de J7/J8.
- Le domaine PG4/7/8 est VDDIO2, activé par le driver ; alimentation réelle du
  domaine conforme au câblage documenté. PWM/INT non pilotés (inversion inactive).

Si le câblage ou J2 ne correspond pas à ces préconditions, ne pas improviser une
modification. Signaler le désaccord au pilote. Aucune manipulation n'est demandée
pour la préparation logicielle ; toute intervention physique reste humaine.

## Chaîne réellement implémentée

`main.c`: après la publication F2 réussie, `Iis3dwbF3_Run` appelle
`stm32_serial_transport_init_rs485`, initialise le serveur diagnostique et
`modbus_rtu_server_runtime_start` arme la réception. `Iis3dwbF3ServerReady`
marque cette réussite. Le polling reste au premier plan, sans appel au runtime
P8 ni au stockage.

`stm32_serial_transport.c`:

- `serial_init`: GPIOG, VDDIO2, PG4 sortie initialement basse, PG7/PG8 AF8,
  LPUART1 en 115200/8E1, TIM6, IRQ LPUART1/TIM6 priorité 5 ;
- `stm32_start_receive`: `HAL_UART_Receive_IT`, un octet ;
- `stm32_transmit`: copie bornée (256 octets maximum), garde busy,
  DE+/RE hauts, garde de bring-up 1 ms, `HAL_UART_Transmit_IT` ;
- échec de lancement TX : DE+/RE remis bas, busy libéré, erreur propagée ;
- `HAL_UART_TxCpltCallback`: callback sur TC après le dernier stop bit,
  DE+/RE bas, busy libéré, compteur completion incrémenté ;
- `HAL_UART_RxCpltCallback`: compteur RX, événement portable, TIM6 réarmé,
  nouvelle réception un octet ;
- `HAL_UART_ErrorCallback`: compteur et code HAL conservés avant réarmement RX,
  événement d'erreur portable, arrêt de la fenêtre TIM6.

`stm32u5xx_it.c` route `LPUART1_IRQHandler` vers
`stm32_serial_transport_irq_handler` / `HAL_UART_IRQHandler`, et TIM6 vers le
handler HAL timer. Le startup CMSIS STM32U575 référence ces vecteurs.
Le HAL CubeU5 `UART_EndTransmit_IT` appelle le callback TX depuis le traitement
TC, et non depuis TXE. Cela doit être vérifié aussi dans l'ELF final.

## Sonde explicitement commandée en RAM

Une absence de requête externe laisse F3-A silencieux. F3-B ajoute :

- `tr2_iis3dwb_f3_probe_request`, valeur de reset 0 ;
- une écriture GDB de **1** demande une émission unique ;
- la boucle remet immédiatement la demande à 0 puis appelle
  `iis3dwb_diag_modbus_transmit_probe` ;
- ce helper réutilise le PDU server existant pour une lecture B0 0/21,
  l'encodeur ADU/CRC existant et `SerialTransport.transmit` ;
- sortie : une **réponse B0 non sollicitée**, adresse 1, FC03, 42 octets de
  registres, **47 octets ADU au total**. Ce n'est pas une requête reçue sur RX ;
- résultat et compteur de demandes conservés ; aucun retry ;
- toute valeur non nulle différente de 1 est consommée et rejetée sans TX.

Ne pas activer cette sonde avec un maître connecté. Elle n'ajoute ni registre,
ni commande B5, ni nouvelle sémantique Modbus de production. Hors demande GDB,
le serveur F3-A continue son fonctionnement normal. Aucun accès FRAM/microSD,
aucun effet CampaignService et aucun checkpoint contourné.

## Observations exposées dans l'ELF

Tous les scalaires ci-dessous sont `volatile uint32_t`, accessibles par cast si
les types DWARF ne sont pas présents.

| Symbole | Interprétation |
|---|---|
| tr2_iis3dwb_f3_ready / result | serveur initialisé et RX armé / Tr2Result |
| tr2_iis3dwb_f3_probe_request | commande RAM consommée, 1=sonde unique |
| tr2_iis3dwb_f3_probe_result / count | Tr2Result de la sonde / nombre de demandes consommées |
| tr2_serial_uart_init_result | dernier HAL_UART_Init, UINT32_MAX avant appel |
| tr2_serial_rx_start_result | HAL_UART_Receive_IT initial, UINT32_MAX avant appel |
| tr2_serial_tx_launch_result | HAL_UART_Transmit_IT, UINT32_MAX avant appel |
| tr2_serial_tx_length | longueur de l'émission acceptée par le driver |
| tr2_serial_tx_launch_count | lancements HAL TX réussis |
| tr2_serial_tx_complete_count | callbacks TX après TC |
| tr2_serial_rx_complete_count | callbacks RX ; zéro ne prouve aucune réception |
| tr2_serial_uart_error_count / code | erreurs UART / dernier masque HAL capturé |
| tr2_serial_gpio_base / uart_base | adresses réellement utilisées par ce BSP |

Les compteurs saturent à UINT32_MAX. Les codes HAL ne sont pas des Tr2Result :
HAL_OK=0, HAL_ERROR=1, HAL_BUSY=2, HAL_TIMEOUT=3 dans CubeU5 courant.

Marqueurs GDB ciblés :

| Fonction | Point exact du flot |
|---|---|
| Iis3dwbF3ServerReady | init complète et réception armée |
| Stm32Rs485DeAsserted | PG4 écrit haut, avant garde et appel HAL TX |
| Stm32Rs485TxLaunched | HAL TX OK et compteur launch incrémenté |
| Stm32Rs485TxCompleted | callback TC, PG4 écrit bas et compteur completion incrémenté |
| HAL_UART_ErrorCallback | entrée du diagnostic d'erreur ; lire aussi ISR |

## Build et identification avant tout flash

Depuis la racine :

```bash
STM32CUBE_U5_ROOT=/mnt/c/Users/Lolo/Desktop/STM32/STM32CubeU5 ./tr2_validate.sh
sha256sum 'Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.bin' 'Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.elf'
```

Utiliser le **BIN F3-B** et l'ELF correspondant, pas le BIN historique F3-A
identifié par `5be24ba3...`, ni F2, ni E4. Relever aussi le diff local exact.
Le test `iis3dwb_diag_modbus` vérifie la réponse de sonde B0 entière, longueur
47, adresse/CRC corrects, absence de mutation B3 et erreur TX sans retry.
Cela reste une preuve logicielle sur transport fake, pas une transmission réelle.

## Exécution future — seulement après autorisation de flash/debug

Aucune commande de cette section n'a été exécutée pendant la préparation.
Configuration attendue : carte alimentée, RESET physique relâché, microSD
retirée. Programmer sous reset ST-LINK, jamais en maintenant le bouton RESET.

Depuis PowerShell Windows, après comparaison SHA256 du BIN :

```powershell
& "D:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe" `
  -c port=SWD sn=003500463235510B37333439 freq=8000 mode=UR `
  -halt -w "C:\Users\Lolo\Desktop\MSM_Vibe\MSM_vibe\MSM_vibe\Modbus RTU\05_Firmware\build-stm32-p11c\tr2_stm32_p11c.bin" 0x08000000 `
  -v -halt -score
```

Exiger `Download verified successfully` et core halted. Aucun -rst final,
mass erase ou Option Bytes. Puis serveur attach qualifié :

```powershell
& "D:\ST\STM32CubeCLT_1.22.0\STLink-gdb-server\bin\ST-LINK_gdbserver.exe" `
  -d -i "003500463235510B37333439" --frequency 8000 `
  -cp "D:\ST\STM32CubeCLT_1.22.0\STM32CubeProgrammer\bin" --attach
```

Depuis WSL, racine main :

```bash
gdb-multiarch -q 'Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.elf'
```

Connexion et **un seul** reset contrôlé de démarrage :

```gdb
set pagination off
target extended-remote 172.17.0.1:61234
hbreak *Reset_Handler
monitor reset hardware
monitor halt
maintenance flush register-cache
info registers pc sp
info breakpoints
```

Vérifier PC=Reset_Handler et SP au sommet RAM du linker. Supprimer uniquement
le breakpoint Reset_Handler par son numéro constaté. Puis :

```gdb
hbreak Iis3dwbF3ServerReady
continue
```

### 1. Init complète, aucune émission encore demandée

Au marqueur serveur prêt :

```gdb
p/u *(unsigned int *)&tr2_iis3dwb_f3_ready
p/u *(unsigned int *)&tr2_iis3dwb_f3_result
p/u *(unsigned int *)&tr2_serial_uart_init_result
p/u *(unsigned int *)&tr2_serial_rx_start_result
p/u *(unsigned int *)&tr2_serial_tx_launch_count
p/u *(unsigned int *)&tr2_serial_tx_complete_count
p/u *(unsigned int *)&tr2_serial_uart_error_count
set $gpio = *(unsigned int *)&tr2_serial_gpio_base
set $uart = *(unsigned int *)&tr2_serial_uart_base
p/x $gpio
p/x $uart
x/4wx $gpio
x/2wx ($gpio + 0x20)
x/4wx $uart
p/x *(unsigned int *)($uart + 0x1c)
p/u ((*(unsigned int *)($gpio + 0x14) >> 4) & 1)
p/u ((*(unsigned int *)($gpio + 0x10) >> 4) & 1)
```

Attendu ready=1, Tr2Result=0, HAL init/RX=0, launch=completion=error=0 et
PG4 ODR/IDR bas. Contrôler MODER PG4=sortie (bits 8..9=01), PG7/PG8=AF
(bits 14..17=1010), AFR PG7/PG8=8. LPUART CR1 : UE/RE/TE/PCE/M0 activés,
PS=0 et M1=0 (9 bits avec parité = 8 bits de données). CR2 STOP=0 (1 stop),
BRR cohérent avec l'horloge configurée ; pas preuve de baud mesuré. ISR TEACK
bit21 et REACK bit22 indiquent l'activation reconnue par le périphérique.
Ne jamais lire RDR pour « observer » des octets : cela pourrait les consommer.

Les offsets proviennent des `GPIO_TypeDef` et `USART_TypeDef` CMSIS
`stm32u575xx.h` du CubeU5 utilisé : IDR=0x10, ODR=0x14, AFR=0x20/0x24,
CR1/CR2/CR3/BRR=0/4/8/12, ISR=0x1c. Les bases sont exportées par le driver,
pas supposées à partir d'une autre famille STM32.

### 2. Sonde unique, observation DE puis lancement

Supprimer le breakpoint ServerReady par son numéro. Armer :

```gdb
hbreak Stm32Rs485DeAsserted
hbreak Stm32Rs485TxLaunched
hbreak Stm32Rs485TxCompleted
hbreak HAL_UART_ErrorCallback
set variable *(unsigned int *)&tr2_iis3dwb_f3_probe_request = 1
continue
```

A **DeAsserted**, relever PC et PG4 ODR/IDR avec les commandes ci-dessus :
attendu hauts. `probe_request=0`, `tx_length=47`, launch/completion encore 0.
Le résultat de sonde et le résultat de lancement restent UINT32_MAX tant que
les appels correspondants ne sont pas revenus. ODR haut prouve le latch,
IDR haut le niveau logique lu au pad STM32 ; aucun des deux ne mesure le
contact DE de l'ADM2867E ou ses sorties A/B.

```gdb
info registers pc
p/u *(unsigned int *)&tr2_iis3dwb_f3_probe_request
p/u *(unsigned int *)&tr2_serial_tx_length
p/u ((*(unsigned int *)($gpio + 0x14) >> 4) & 1)
p/u ((*(unsigned int *)($gpio + 0x10) >> 4) & 1)
continue
```

A **TxLaunched** :

```gdb
info registers pc
p/u *(unsigned int *)&tr2_serial_tx_launch_result
p/u *(unsigned int *)&tr2_serial_tx_launch_count
p/u *(unsigned int *)&tr2_serial_tx_complete_count
p/u *(unsigned int *)&tr2_serial_uart_error_count
p/u ((*(unsigned int *)($gpio + 0x14) >> 4) & 1)
continue
```

Attendu HAL TX=0, launch=1. Le résultat de sonde n'est stocké que lorsque le
transport revient au premier plan ; ne pas exiger prématurément probe_result=0
à ce marqueur. Les interruptions/periphériques peuvent évoluer autour des
breakpoints : conserver les valeurs et leur ordre exact, pas une supposition.

### 3. Fin matérielle TC et retour réception

A **TxCompleted**, le CPU reste arrêté :

```gdb
info registers pc xpsr
p/u *(unsigned int *)&tr2_serial_tx_complete_count
p/u *(unsigned int *)&tr2_serial_tx_launch_count
p/u *(unsigned int *)&tr2_iis3dwb_f3_probe_result
p/u *(unsigned int *)&tr2_iis3dwb_f3_probe_count
p/u *(unsigned int *)&tr2_serial_uart_error_count
p/x *(unsigned int *)&tr2_serial_uart_error_code
p/u ((*(unsigned int *)($gpio + 0x14) >> 4) & 1)
p/u ((*(unsigned int *)($gpio + 0x10) >> 4) & 1)
p/x *(unsigned int *)($uart + 0x1c)
p/u *(unsigned int *)&tr2_serial_rx_complete_count
```

Attendu launch=completion=1, probe_result=0/probe_count=1, erreur=0,
PG4 ODR/IDR bas, ISR.TC bit6=1. Le callback a libéré tx_busy. La RX initiale
reste armée, mais RX count=0 ne valide pas la réception. Une entrée dans
ErrorCallback arrête l'essai : conserver PC, ISR et dernier code latched,
qui peut n'être actualisé qu'après reprise du callback. Ne pas reprendre en
boucle, changer un jumper ou reflasher pour masquer l'erreur.

Les arrêts GDB pendant TX peuvent créer de longs trous entre octets : cette
première passe n'est **pas** une validation du timing ou d'une ADU RTU intacte
sur le fil. Aucun maître ne doit être connecté pendant ce diagnostic.

Option pour une seule émission supplémentaire sans arrêt intermédiaire :
supprimer DeAsserted/TxLaunched, conserver uniquement TxCompleted et le
breakpoint d'erreur, écrire probe_request=1 puis continuer. Attendre completion=2
et PG4 bas. Aucun reset, nouvelle fenêtre acquisition, retry ou changement de
câblage n'est nécessaire. Cette passe ne prouve toujours pas le signal A/B reçu.

### Arrêt et critères

Conserver le CPU arrêté après la dernière observation. Clôture sur demande :
`disconnect`, `quit` (pas `detach`), arrêt du serveur et contrôle des processus,
sans reprise ou reset. Le serveur ne doit pas rester attaché après la session.

PASS limité côté STM32 : init et RX armement OK, transition PG4 bas→haut→bas
observée, TX HAL réussi, longueur47, callback matériel TC complet, aucune erreur.
FAIL côté STM32 : erreur init/lancement, GPIO incohérent ou erreur UART démontrée.
INCONCLUSIVE : breakpoint non atteint ou compteur incohérent sans cause établie,
absence d'observation nécessaire. En cas d'attente anormale, interrompre GDB
pour observer puis arrêter le diagnostic ; pas de reset/retry automatique.

**ADM2867E différentiel, réception, baud mesuré et Modbus physique restent
NON DÉMONTRÉS** par GDB seul. Leur validation nécessite le futur adaptateur
(et, pour certaines propriétés électriques/timing, un instrument approprié),
avec réponse à une véritable requête puis lecture B0/B3 comparée à l'image.
Ne pas simuler une IRQ RX ou injecter une file RX RAM comme preuve de réception.

| Observation | Valeur constatée / preuve |
|---|---|
| HEAD/diff, SHA256 BIN/ELF | — |
| Configuration matérielle déclarée et J2 | — |
| Reset_Handler / PC / SP | — |
| init / RX armée / bases / CR1 / BRR | — |
| PG4 au repos / DeAsserted / TxCompleted | — |
| HAL TX / longueur / launch / completion | — |
| probe_result / compteur / erreur UART | — |
| RX réellement reçue | Non démontrée par cette procédure |
| Signal différentiel réellement mesuré | Non démontré par cette procédure |
| Verdict limité et limites | — |

## Firmware préparé et contrôles logiciels — 2026-10-08

Validation complète exécutée, exit 0 : **104/104 tests hôte réussis**,
cross-build STM32 réussi. Taille link : text=94472, data=144, bss=71656 octets.
Le test ciblé `iis3dwb_diag_modbus` réussit aussi séparément. Diff relu,
`git diff --check` propre ; code SDMMC2/E4 identique au HEAD. Aucun flash,
connexion matérielle GDB, changement de câblage ou opération microSD effectué.
Le fichier ELF a été inspecté **hors cible**, sans connexion remote.

- BIN : `Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.bin`
- BIN SHA256 : `65b734933d1f3947b30f82c3371eb10e557a34d8d893202de23ef5be82d56321`
- ELF : même répertoire, `tr2_stm32_p11c.elf`
- ELF SHA256 : `a8e4040c35548a1e8a0cb962082c2e2582d7943e09b2596c611481317a3c9810`

Adresses de breakpoint sélectionnées par GDB sur cet ELF (inspection offline) :

| Symbole | Breakpoint symbolique résolu |
|---|---|
| Iis3dwbF3ServerReady | 0x08002084 |
| Stm32Rs485DeAsserted | 0x08003DCA |
| Stm32Rs485TxLaunched | 0x08003DD8 |
| Stm32Rs485TxCompleted | 0x08003DE6 |
| HAL_UART_ErrorCallback | 0x0800463C |

`Reset_Handler` a son entrée à `0x08004DD4`. Les IRQ LPUART1 et les marqueurs
sont présents dans l'ELF. Employer `hbreak` sur cible, pas les breakpoints
logiciels de l'inspection offline ; aucune écriture Flash de breakpoint requise.
Revérifier empreintes et symboles après tout rebuild avant d'utiliser les
adresses numériques. La preuve physique F3-B reste à obtenir après autorisation.
