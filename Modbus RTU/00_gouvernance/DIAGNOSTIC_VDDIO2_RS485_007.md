# Mission 007 — VDDIO2 et décision avant essai RX

11 octobre 2026. **Analyse logicielle établie ; COMPILÉ STM32 ; NON QUALIFIÉ
électrique ; NON QUALIFIÉ RX. Essai physique NON PRÊT.** Aucune commande cible,
programmation, écriture de registre, émission ou manipulation exécutée par Codex.
Ce document complète la préparation 006 sans modifier les freezes.

## Conclusion et décision

PG7/PG8 font partie de PG2..PG15 alimentées par VDDIO2. Leur isolation doit être
levée avant usage, une fois cette alimentation présente. Le code courant
n'effectue pas cette levée : **initialisation incomplète pour un démarrage depuis
reset avec IO2SV=0**, même si l'UART s'initialise et `tr2_rtu_rx_ready` passe à 1.
Le correctif logique nécessaire est identifié ; sa sûreté électrique sur ce
montage n'est pas encore établie. La compilation ne résout pas ce défaut.

**Choix autorisé par le contrat §4 : procédure de décision, sans patch actif.**
Pas d'EnableVddIO2 inconditionnel, pas de direction GPIO supposée. Mettre IO2SV à
1 affecte tout le domaine PG2..PG15 ; PG7 déjà configurée AF push-pull peut alors
piloter TXD. Un DE flottant/non établi interdit d'assimiler ceci à une opération
purement passive. Le moniteur de tension seul ne vérifie ni le bon niveau
logique, ni la direction, ni le câblage.

## IO2SV : sources officielles et portée de la lecture

[ST AN5373 Rev7 §2.1.2, p11](https://www.st.com/resource/en/application_note/an5373-getting-started-with-stm32u5-mcu-hardware-development-stmicroelectronics.pdf)
décrit le rail indépendant 1,08..3,6 V et l'isolation après reset. Lever IO2SV
requiert VDDIO2 déjà présente. Ce bit est une commande logicielle d'isolation,
pas une tension mesurée. Le moniteur IO2VM compare vers 0,9 V : un état ready ne
prouve pas un rail 3,3 V ni toutes les conditions d'usage. AN5373 écrit « SVMR »
dans ce paragraphe ; le header STM32U575 et la HAL vérifient **SVMCR** pour ce MCU.

Sources ST locales effectivement lues, SDK officiel CubeU5 **v1.9.0** :

- `Drivers/CMSIS/Device/ST/STM32U5xx/Include/stm32u575xx.h` : `PWR_TypeDef.SVMCR`
  offset0x10 ; `PWR_SVMCR_IO2SV` bit29 (`0x20000000`) ; IO2VMEN bit25 ;
  `PWR_SVMSR_VDDIO2RDY` bit25 dans SVMSR (offset0x3C), registre distinct.
- `Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr_ex.c` :
  `HAL_PWREx_EnableVddIO2()` fait `SET_BIT(PWR->SVMCR, PWR_SVMCR_IO2SV)`.
  Aucun contrôle de tension, attente ou alimentation physique créé par cet appel.
  `HAL_PWREx_EnableIO2VM()` active le moniteur vers0,9 V ; ce n'est pas IO2SV.
- Exemple ST `Projects/NUCLEO-U575ZI-Q/Applications/USBX/Ux_Device_CDC_ACM/Core/Src/stm32u5xx_hal_msp.c` :
  activation PWR puis EnableVddIO2 dans HAL_MspInit. Exemple adapté à sa carte,
  pas preuve de l'état électrique de notre prototype.

La [documentation ST](https://www.st.com/en/microcontrollers-microprocessors/stm32u575-585/documentation.html)
pointe RM0456. Son PDF n'a pas pu être récupéré par l'outil web ni le téléchargement
local (timeout) : aucune citation de contenu RM0456 non lu, aucune règle empruntée
à RM0487/STM32U3. Les conclusions ci-dessus reposent sur AN5373, datasheet et HAL/CMSIS U575.

| Lecture / contexte | Conclusion permise | Ce qu'elle ne prouve pas |
|---|---|---|
| Utilisateur : `0x46020810 → 0x00000000` | IO2SV lu à0 dans cette vue ; si accès valide à ce registre, isolation non levée | Tension VDDIO2, défaut du câble, état futur après flash/boot |
| Alias non sécurisé | CMSIS : PWR_NS=0x46020800, +0x10=SVMCR | TrustZone désactivée, protection/horloge PWR de firmware inconnu |
| Alias sécurisé | CMSIS : PWR_S=0x56020800 | Droit de lire/écrire cette vue depuis une session non sécurisée |
| Après reset | Isolation rétablie, init de chaque firmware à examiner | Persistance d'un ancien essai ou d'une écriture GDB |

La valeur0 est compatible avec le défaut local, mais **ne démontre pas que le
firmware chargé contient ce code**. Un accès refusé, un alias/contexte inadéquat
ou une horloge périphérique non établie doit empêcher de conclure sur le seul
nombre affiché. Ne pas essayer un autre alias ou changer les protections à
l'aveugle. Le projet active l'horloge PWR dans HAL_MspInit ; SystemPower_Config
configure SMPS/UCPD, sans lever IO2SV. Le mapping PWR dépend de `__ARM_FEATURE_CMSE`
dans les headers ; le firmware inconnu et la session utilisateur ne doivent pas
être assimilés au contexte du build local. Aucun Option Byte modifié.

## Pinout et état du code

[DS13737 Rev10, table28 p147](https://www.st.com/resource/en/datasheet/stm32u575vg.pdf)
confirme AF8 : PG7=LPUART1_TX, PG8=LPUART1_RX. PG5 AF8 est CTS ; PG6 AF8 est
RTS/DE. **PG4/PG5 candidats ne sont pas une affectation matérielle automatique de
DE UART**. PG6 CN12-70 n'est pas retenue/câblée par cette mission.

[UM2861 Rev10, tables9/19 p24/38](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf)
et [plan d'occupation](CONCEPTION_FIRMWARE_P12H3H_B2_NUCLEO_PHYSICAL_OCCUPANCY.md)
concordent : PG8 CN12-66, PG7 CN12-67, PG5 CN12-68, PG4 CN12-69.
La documentation NUCLEO prévoit SB28 ON/SB29 OFF pour VDDIO2 suivant VDD_MCU,
ou SB28 OFF/SB29 ON pour3V3 fixe ; JP4 sélectionne VDD. **Ce sont des possibilités
documentées, pas une inspection des ponts du banc.** Les deux SB ne doivent pas
être pontés à l'aveugle. Tension ST-LINK≈3,288 V ne mesure pas le rail VDDIO2.

Code effectivement inspecté :

- `stm32_serial_transport.c` : GPIOG clock, LPUART1 clock, PG7/PG8 AF8 push-pull,
  sans pull ; UART115200, mot9bits avec parité paire →8E1, IRQ LPUART1 et TIM6.
  Aucun EnableVddIO2, aucun PG4/PG5, aucun HAL_RS485Ex_Init ou gestion DE//RE.
- `main.c` : HAL_MspInit active PWR seulement ; SystemPower_Config ne traite pas
  IO2SV ; diagnostic RX et compteurs006 conservés. Production binding par défaut
  NOT_AVAILABLE ; RX ne traite pas de PDU et ne répond pas.
- Les autres `GPIO_PIN_4/5` du harness sont sur GPIOD/GPIOA, sans affectation
  implicite à GPIOG. Aucun autre écrivain SVMCR/IO2SV trouvé dans la plateforme.

## Direction, niveaux et mesures minimales localisées

[Schéma Click v102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf)
identifie U1 ADM2867EBRNZ, RXD8, /RE9, DE10, TXD11, VIO7, VCC4 et GND1.
Le texte extrait n'établit pas toutes les connexions graphiques VIO/VCCSEL ;
aucune liaison d'alimentation réelle n'en est déduite. La révision physique reste
inconnue. [MIKROE](https://www.mikroe.com/rs485-isolator-2-click) annonce /RE sur
RST, DE sur CS, contrôle commun JP2 et indépendant J3 ; leur état actuel est à
constater. A↔Y et B↔Z mesurés hors tension par l'utilisateur ne prouvent pas ces contrôles.

[ADI RevD, tables1/11 et §VIO](https://www.analog.com/media/en/technical-documentation/data-sheets/adm2867e_adm2865e_adm2863e_adm2861e.pdf) :
DE bas désactive le driver, /RE bas active le récepteur. VIO alimente la logique
et RXD ; VCC alimente le convertisseur. Seuil bas≤0,3×VIO, haut≥0,7×VIO.
VIO5 V demanderait≥3,5 V pour une entrée haute ; une sortie MCU3,3 V ne garantit
pas cela. Ne pas déduire de « tolérant5 V » une compatibilité universelle avec
rail absent/isolation active. GND1 logique et GND2 bus restent distincts.

**Un seul relevé de prérequis**, sous future autorisation de l'utilisateur :

| Point réellement identifié (préférer pad accessible relié, éviter court-circuit de pattes fines) | Référence / critère pour le banc3V3 visé |
|---|---|
| Sortie des ponts SB28/SB29 réellement reliée à VDDIO2 MCU | GND NUCLEO ; rail stable voisin3,3 V, dans limites ST, alimentation/ponts identifiés |
| VIO U1-7 et VCC U1-4 | GND1 Click ; VIO cohérent avec domaine3V3 ; VCC dans plage3..5,5 V, sélection connue |
| DE U1-10 et /RE U1-9 | GND1 ; tous deux proches0 V et≤0,3×VIO, état établi sans entrée flottante |
| RXD U1-8 et connexion PG8 CN12-66 | Même référence logique établie ; repos haut compatible avec VIH STM32 et limites d'entrée, continuité réelle connue |

Vérifier identité/révision et routes de ces points avant de poser les sondes.
Un seul relevé au repos ne prouve pas le maintien des niveaux au boot/en réception.
Risque ou référence inconnue : arrêter, consigner le point manquant ; aucune
émission, modification de jumper ou écriture GDB en guise de test. Ne pas relier
GND1 à GND2 pour faciliter une mesure et ne pas rebrancher le fil rouge5 V.
Pas de nouveau recâblage du bornier demandé. INVR/INVD doivent avoir un état
connu pour interpréter la polarité ; ne pas les basculer au hasard.

## Procédure de décision pour le correctif — non appliquée

Si rails, références logiques et direction sûre **y compris au boot** sont
établis et documentés, corriger le seul init plateforme **avant configuration
AF de PG7/PG8** par les API réellement présentes :

```c
/* RECETTE FUTURE, NON INTÉGRÉE : alimentation/direction qualifiées requises. */
__HAL_RCC_PWR_CLK_ENABLE();
HAL_PWREx_EnableVddIO2();
```

Cela ne provisionne pas le rail et n'ajoute pas DE//RE. Si rail indépendant/
power-sequencing non garanti, définir d'abord un contrôle de présence avec délai
borné et politique d'échec ; IO2VM ready vers0,9 V ne suffit pas aux exigences3V3.
Si PG4/PG5 doivent piloter DE//RE, confirmer fils/affectation/états de démarrage
avant tout code ; aucun choix physique ou interface de production inventé.
Les deux lignes seules ne sont **pas** un correctif physiquement validé.

Après cette décision, nouvelle tranche logicielle autorisée, cross-build et
nouveau couple BIN/ELF/empreintes requis. **Ne pas flasher BIN006 inchangé en
pensant que la mission007 a réparé IO2SV.** Aucun write GDB temporaire ne doit
remplacer cette initialisation reproductible au boot.

## Prochaine séance : quatre étapes et portes d'arrêt

**Toutes les commandes cible/gestes ci-dessous restent NON EXÉCUTÉS en007.**

1. **Régler l'état de session avant toute autre opération.** L'utilisateur a été
   invité à `monitor resume`, `detach`, `quit`, sans confirmation d'exécution.
   Reprendre la session existante sous autorisation, confirmer sortie/reprise
   dans son journal. Si elle a disparu, état CPU/serveur inconnu : inspection
   autorisée de l'état, pas de reset/second serveur/flash pour deviner.
2. **Faire le relevé électrique unique ci-dessus.** Logement microSD vide
   confirmé dans le contrat007 : ne pas demander de retrait. Maintenir cet état
   avant tout futur boot ; harness E1/E4 peut écrire si carte réintroduite.
   Si un prérequis manque, s'arrêter et utiliser la procédure de décision.
3. **Identifier/programmer seulement le firmware approuvé.** La lecture utilisateur
   de157816 octets SHA256 `8869e369664da06a707d8d73633fd9fcf1e7940e985b963004c9da73d7b0f480`
   diffère du BIN006 `f9356f279d45cb411bc5b98107e0fd291e2d1ab903c216ff052083b7abfd103d`.
   La copie `/tmp/tr2-rx006-loaded.bin` retrouvée a été hashée hors cible par
   Codex : même taille/empreinte que le contrat, provenance matérielle rapportée.
   Aucun compteur006 interprétable maintenant. Après correctif/revue et permission
   flash distincte, sauvegarde Flash, programmation standard vérifiée puis
   relecture au nouveau BIN sur sa longueur entière. Le nouveau SHA/longueur
   remplacent les constantes006 ; aucune supposition à partir d'un simple boot.
4. **Essai unique seulement si tous les jalons passent.** Relire IO2SV dans le
   contexte établi : bit29 attendu1 après init ; confirmer RXready=1 et polling
   actif sur ELF correspondant. HP : exécution hors ligne du script006 d'abord
   (politique Windows non contournée). Puis AVANT→resume→un seul `-Transmit`
   COM7→attente1 s→APRÈS→resume, selon préparation006. Octets+8/trames+1/valides+1,
   erreur+0, dernière ADU exacte `01 03 03 E8 00 01 04 7A`. Pas de halt durant
   émission/T3.5, aucun retry automatique. Réponse absente compatible avec RX seul.

La voie OpenOCD/GDB WSL est désormais **observée par l'utilisateur** pour attach,
halt et lecture ; elle n'est pas qualifiée pour programmation ni reset/reprise
reproductible. La voie ST Windows qualifiée le10 octobre est décrite dans
`00_Environnement_developpement/T1000/REPRISE_QUOTIDIENNE.md` et le rapport physique
local sous `/home/lolo/dev/msm/reports/2026-10-10-cloture/historique-TR2/`.
Les commandes qualifiées utilisent ST-LINK SN `003500463235510B37333439`, SWD8000,
lecture HOTPLUG et programmation BIN à0x08000000 avec vérification/reset.
Relire cette procédure avec le **nouvel artefact** et noms de sauvegarde nouveaux.
Elle suppose ST-LINK disponible côté Windows ; actuellement attaché à WSL,
son changement de propriétaire doit être explicite après fermeture des sessions.
Pas de transfert usbipd, arrêt forcé serveur, reset, flash OpenOCD `program` ou
GDB `load` implicites. Option Bytes, mass erase et protections restent exclus.

Le diagnostic documentalement établi n'est pas une qualification électrique ni
RX. Le protocole préparatoire est complet ; les mesures humaines et le correctif
conditionné restent les prochaines portes de décision. E4 reste ouverte.
