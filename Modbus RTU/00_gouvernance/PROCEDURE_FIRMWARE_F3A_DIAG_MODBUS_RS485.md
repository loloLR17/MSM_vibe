# F3-A — Préparation Modbus RTU / RS-485 des indicateurs IIS3DWB diagnostiques

## Statut et portée

Firmware diagnostique F3-A, qualification physique RS-485 encore à réaliser.
La boucle STM32 compose explicitement le serveur après publication F2 réussie.
L'identité B0 a été attribuée au banc par le pilote, sans provisioning production.

HEAD de départ : `18c9871481e714e05c93ee60d982c7293f488ba3`.
Les modifications F1/F2-DIAG préexistantes ne sont pas encore committées ; le HEAD
seul ne décrit donc pas le firmware. Relever diff, SHA256 BIN/ELF et résultat de
validation du firmware final avant tout essai. Ne jamais utiliser un artefact E4.

F3-A vise uniquement FC03 B0 (0–20) et B3 diagnostique (3000–3047), après
acquisition réelle des 4096 échantillons F2. Pas de CampaignService, checkpoint
simulé, publication P8, reprise de campagne, FRAM ou accès microSD. Une seule
fenêtre par démarrage contrôlé ; le capteur est ensuite arrêté. Le calcul inclut
la composante statique/gravitatoire. Aucune cadence 26,667 kHz n'est démontrée.

## Références

- `../01_Specification_source/bloc0.md`, `bloc1.md`, `bloc2.md`, `bloc3.md`
  et `charte_typage.md` : mappings V1 et atomicité des lectures.
- `FREEZE_FIRMWARE_P12F_STM32_LPUART1.md` : PG7/PG8, 115200/8E1, IRQ.
- `FREEZE_FIRMWARE_P12G_STM32_RTU_TIMING.md` et
  `ERRATUM_FIRMWARE_P12D_P12G_RTU_TIMING_COMPOSITION.md` : TIM6, T1.5/T3.5.
- `FREEZE_FIRMWARE_P12H_PORTABLE_RTU_SERVER.md` : CRC/ADU/PDU/adressage.
- `CONCEPTION_FIRMWARE_P12H3H_B2_NUCLEO_PHYSICAL_OCCUPANCY.md` : Morpho et
  candidats PG4/PG5 dans VDDIO2.
- [Schéma fabricant MIKROE-3863, v101](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v101.pdf),
  examiné visuellement : ADM2867E, alimentation, J2/J3, J7/J8 et borniers.

## Identité et composition

`iis3dwb_diag_modbus_init` reçoit explicitement l'IdentitySnapshot du banc,
le SerialTransport et la fenêtre diagnostique déjà publiée. Il réutilise
`modbus_project_b0` et `ModbusRtuServerRuntime`, avec B3 pointant exclusivement
vers l'image F2. La composition conserve B0 et emprunte B3/transport ; tous ces
objets doivent vivre pendant le service. Aucun backend de production n'est créé.

L'adresse 1 est un choix local du banc, pas un changement V1 ni une politique de
provisionnement production. Ne connecter qu'un serveur d'adresse 1 au bus d'essai.
B0 requiert une identité unique/persistante explicitement attribuée au banc.
Ne pas déduire le device_id d'une fixture, du campaign_id, d'une valeur arbitraire
ou d'un hash matériel supposé unique. Attribution approuvée par le pilote pour le banc de développement n°1 :

- device_id = `0x54520001` ; serial_number = `TR2-DEV-0001` ; manufacturer = `MSM` ;
- hardware_version = `1`, code de banc pour `DEV_NUCLEO-U575` ;
- firmware_version = `0.3.0`, version de banc F3-A ; protocol_version = `1` ;
- capabilities = `0x000D` : acquisition, supervision, diagnostic ; pas de SD.

Ces constantes sont immuables dans ce firmware, réservées à ce seul banc et
persistées dans son image Flash. Elles ne définissent ni l'allocation industrielle
des identités ni la future identité logique FRAM D1/D3. Ne pas dupliquer ce BIN
sur un second banc avec la même identité.

B1/B2 et les autres sources/services ne sont pas composés : une lecture d'une
adresse valide sans source doit produire l'exception 04, pas un état fictif.
Les écritures RO sont rejetées selon V1 ; les services RW ne sont pas fournis.
Cette composition n'est donc pas un endpoint complet du runtime de production.
La supervision générale qui interroge B1/B5 ne doit pas interpréter leurs erreurs
comme une défaillance du calcul B3 : commencer par deux lectures FC03 ciblées.

## Raccordement prévu — intervention humaine, carte hors tension

Table fondée sur le Morpho documenté et le schéma v101. Vérifier les sérigraphies
et la révision réelle du Click avant branchement. TX/RX du connecteur mikroBUS
ne désignent pas directement les noms TXD/RXD du circuit ADM2867E.

| NUCLEO | Click / fonction |
|---|---|
| PG7 / CN12-67, LPUART1 TX | mikroBUS RX, net TX, entrée TXD du transceiver |
| PG8 / CN12-66, LPUART1 RX | mikroBUS TX, net RX, sortie RXD du transceiver |
| PG4 / CN12-69 | mikroBUS CS, net DE ; J2 couple DE à /RE |
| PG5 / CN12-68 | non raccordé ; ne pas piloter un second GPIO sur la liaison J2 |
| 3V3 / CN11-16 | +3.3V Click ; J1 sélectionné 3V3 |
| GND / CN11-20 | GND logique Click (GND1) |

Le driver opt-in `stm32_serial_transport_init_rs485` active VDDIO2 et utilise
PG4 bas pour DE=0,/RE=0 (réception), haut pour DE=1,/RE=1 (émission, sans écho).
Il active le driver avant HAL UART TX IT avec une garde conservatrice de 1 ms,
puis revient à réception dans le callback HAL sur **TC**, après le dernier stop
bit. Un échec de lancement TX restaure la réception. Le chemin UART historique
sans RS-485 reste disponible. La durée de garde n'est pas une qualification
instrumentée du bus ou de ses latences.

Avant mise sous tension : contrôler alimentation VDDIO2 à 3,3 V, J1=3V3,
continuité DE↔/RE via J2, absence de second pilotage sur RST(/RE via le pont J3). J2/J3 sont représentés
comme ponts 0 ohm sur le schéma : contrôler leur présence réelle, ne pas les
confondre avec un sélecteur d'inversion. AN n'est pas la ligne /RE.
Laisser PWM(INVR) et INT(INVD) non raccordés ; les résistances R15/R16 du Click
les maintiennent au niveau non inversé. Vérifier leur présence sur la révision
réelle. Ne pas relier une logique 5V
aux GPIO. La configuration réelle de ces jumpers n'est pas supposée acquise.

Pour un bus deux fils, J7/J8 doivent réaliser A↔Y et B↔Z (half-duplex).
Utiliser CN1 A/B et référence isolée GND2 vers le côté bus de l'adaptateur selon
sa documentation. Ne pas ponter GND1 et GND2 par commodité. La correspondance
A/B de l'adaptateur PC reste à vérifier sur sa notice ; elle n'est pas inventée.
Terminaison uniquement aux deux extrémités, sans doublon ; vérifier les jumpers
Term/Bias et les résistances déjà présentes sur l'adaptateur. Ces vérifications
et toute soudure/déplacement de jumper restent humains. IIS3DWB conserve le
câblage F1 ; microSD retirée.

## Validation logicielle avant essai

Depuis la racine :

```bash
STM32CUBE_U5_ROOT=/mnt/c/Users/Lolo/Desktop/STM32/STM32CubeU5 ./tr2_validate.sh
```

La validation complète est nécessaire : nouvelle composition et test ADU→B3,
pas seulement harness HAL. `iis3dwb_diag_modbus` utilise des échantillons de
fixture pour une preuve logicielle uniquement. Il vérifie FC03/CRC, encodage
intégral B0/B3, MSW/LSW, absence de source B1/B2, RO, silence broadcast lecture,
adresse étrangère, CRC erroné, trame interrompue et échec TX sans retry.

## Essai physique ultérieur — procédure à appliquer au firmware F3-A final

1. Valider et relever BIN et
   ELF `Modbus RTU/05_Firmware/build-stm32-p11c/tr2_stm32_p11c.{bin,elf}`, SHA256,
   diff exact et symboles réellement présents. Ne pas utiliser un ancien BIN F2/E4.
2. Raccordement hors tension et contrôles humains ci-dessus. Pas de microSD.
3. Après autorisation explicite, appliquer le flash standard qualifié sous reset
   ST-LINK `mode=UR`, `-halt`, écriture à `0x08000000`, `-v`, `-halt`, sans `-rst`.
   ST-LINK attendu `003500463235510B37333439`. Vérifier le succès effectif.
4. GDB attach au bon ELF sans reprise ; reset contrôlé, arrêt Reset_Handler,
   flush register-cache et contrôle PC/SP. Observer F2 ready/result/WHO_AM_I,
   puis vérifier le serveur F3 démarré. Commandes avec les symboles de l'ELF final :

   ```gdb
   hbreak *Reset_Handler
   monitor reset hardware
   monitor halt
   maintenance flush register-cache
   info registers pc sp
   info breakpoints
   ```

   Après PC=Reset_Handler, supprimer uniquement ce breakpoint par son numéro
   constaté. Armer `hbreak Iis3dwbF3ServerReady`, puis `continue`. A cet arrêt :

   ```gdb
   p/x *(unsigned char *)&tr2_iis3dwb_whoami
   p/u *(unsigned int *)&tr2_iis3dwb_f2_ready
   p/u *(unsigned int *)&tr2_iis3dwb_f2_result
   p/u *(unsigned int *)&tr2_iis3dwb_f3_ready
   p/u *(unsigned int *)&tr2_iis3dwb_f3_result
   x/48uh &tr2_iis3dwb_f2_b3_registers
   ```

   Attendu WHO_AM_I=0x7B, F2/F3 ready=1 et result=0. Supprimer le breakpoint
   F3 par son numéro, puis `continue` pour servir Modbus. Ne pas laisser d'autre
   breakpoint F2 armé pendant l'essai. `tr2_iis3dwb_f3_poll_errors` compte les
   résultats de polling non OK, sans retry ; les erreurs UART sont aussi visibles
   dans les événements de transport conformément au contrat P12-E/H.
   Ne pas laisser de breakpoint arrêté
   pendant la communication : le CPU doit exécuter le polling/IRQ.
5. Identifier le port réellement attribué à l'adaptateur PC (aucun COM fictif).
   Configurer 115200, 8 bits, parité paire, 1 stop, adresse 1, timeout adapté,
   sans retry caché. L'adaptateur doit gérer sa direction deux fils.
6. FC03 adresse protocolaire 0, quantité 21 : vérifier l'identité attribuée,
   version protocole et réservé=0. Ne pas ajouter une base 40001 aux adresses ADU.
7. FC03 adresse 3000, quantité 48 : comparer tous les mots au miroir GDB de la
   même fenêtre ; données uint32 MSW/LSW, unités mg. Ne pas utiliser une
   interprétation float, vitesse, FFT ou données simulées.
8. Lire plusieurs fois : image de mesure stable pour ce boot, CRC valide,
   réponses cohérentes. Age/temps restent non disponibles si leurs flags ne sont
   pas établis, et séquence=1 ne prouve pas une acquisition continue.
9. Tester adresse étrangère/CRC erroné sans réponse, lecture broadcast silencieuse,
   écriture RO avec exception, sans changer la configuration ni B5.
10. Pour une seconde fenêtre, manipulation humaine du capteur puis reset contrôlé
    explicitement autorisé ; comparer B3 via Modbus et GDB. Ne pas revendiquer
    une campagne durable ou de la supervision de production.

PASS exige le firmware final traçable, WHO_AM_I=0x7B, F2 ready=1/result=0,
4096 mesures physiques, B0 conforme à l'attribution et B3 Modbus identique au
miroir GDB de chaque fenêtre, avec variation physique et réponses RTU correctes.
FAIL : mesure/calcul/CRC/adressage incohérent, écriture RO acceptée, écho incorrect
ou direction incorrecte démontrée. INCONCLUSIVE : adaptateur absent, câblage ou
identité non confirmés, données GDB insuffisantes, timeout sans cause établie.

| Élément | Observation réelle à consigner |
|---|---|
| Diff/HEAD et SHA256 BIN/ELF | — |
| Révisions Click/NUCLEO, jumpers, câblage | — |
| Adaptateur/port/paramètres | — |
| Flash et reset contrôlé | — |
| WHO_AM_I / ready / result / durée / compteur | — |
| B0 brut et identité attendue | — |
| B3 brut / RMS / Peak, fenêtre 1 | — |
| B3 brut / RMS / Peak, fenêtre 2 | — |
| CRC/adressage/RO/direction/timing | — |
| Conclusion et limites | — |

Aucune observation physique F3-A n'est enregistrée comme déjà acquise.

## Validation logicielle du firmware préparé — 2026-10-08

Commande complète ci-dessus exécutée : exit 0, **104/104 tests hôte réussis**,
cross-build STM32 réussi. Aucune validation physique F3-A à cette date.

Artefacts du build courant (le HEAD seul ne suffit pas : diff F1/F2/F3 local) :

- BIN SHA256 : `5be24ba354ea4186b94117b5156abde3ecad9ec4e2db1a239dd03126a29e42e8` ;
- ELF SHA256 : `5ca1a6ee9e92d1e964813f7f34180c4e340d9a68e228373f87cf148025696b0c` ;
- taille link : text 93856, data 128, bss 71616 octets ;
- symbole `Iis3dwbF3ServerReady` à `0x0800207E` (entrée de fonction) ;
- `stm32_serial_transport_init_rs485` et
  `modbus_rtu_server_runtime_poll_once` présents dans l'ELF ;
- diagnostics F3 ready/result/poll_errors présents.

Ces empreintes doivent être revérifiées avant flash. Un nouveau build ou une
modification invalide cette référence d'artefact jusqu'à nouvelle comparaison.
Le pilotage GPIO/IRQ, le timing réel et le bus restent à qualifier physiquement.

## Suite F3-B

La préparation F3-B ajoute une sonde TX explicitement commandée en RAM et des
observations UART/DE/TC, sans modifier B0/B3 ou P8. Le BIN du build courant
est désormais celui identifié dans `PROCEDURE_FIRMWARE_F3B_UART_ADM2867E_GDB.md`.
Les empreintes de la section F3-A ci-dessus restent des références historiques
au firmware A, pas des empreintes du firmware B courant. Ne pas confondre les
deux pour le prochain flash. Aucune qualification physique RS-485 n'est déduite
de cette évolution logicielle.
