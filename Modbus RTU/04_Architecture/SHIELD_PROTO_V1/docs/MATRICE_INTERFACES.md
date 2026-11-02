# Matrice électrique et orientations — candidat V1.1 (liaisons identiques à V1)

Toutes les alimentations et E/S retenues sont **3,3 V**, GND logique commun. Aucune liaison à 5 V, VIN Nucleo, CN15 ou source externe n’est créée. L’alimentation actuelle du Nucleo et ses jumpers sont conservés ; le shield n’est pas un circuit de sélection de source.

| Fonction | MCU / connecteur Nucleo | Shield | Module / numéro constructeur |
|---|---|---|---|
| SPI3 CS | PC9 / CN8-4 | J2-4 → J8-21 | MKIGIBV2 JP2-21 / CS |
| SPI3 SCK | PC10 / CN8-6 | J2-6 → J8-22 | JP2-22 / SCL |
| SPI3 MISO | PC11 / CN8-8 | J2-8 → J8-24 | JP2-24 / SDO |
| SPI3 MOSI | PC12 / CN8-10 | J2-10 → J8-23 | JP2-23 / SDA |
| VDDIO / VDD | CN8-7 / +3V3 | J8-1 et J8-2 | JP1-1 et JP1-2 |
| GND capteur | CN8-11 et CN8-13 | J8-13 | extrémité basse droite JP2 vue dessus |
| SPI2 SCK | PB13 / CN7-5 | J1-5 → J7-3 | Adafruit4682 CLK |
| SPI2 MISO | PD3 / CN9-10 | J3-10 → J7-4 | SO / D0 |
| SPI2 MOSI | PD4 / CN9-8 | J3-8 → J7-5 | SI / CMD |
| SPI2 CS | PF12 / CN7-20 | J1-20 → J7-6 | CS / D3 |
| SD alimentation | +3V3 / GND | J7-1 / J7-2 | 3V / GND |
| SPI1 SCK | PA5 / CN7-10 | J1-10 → J6-6 | Adafruit4718 SCK |
| SPI1 MISO | PA6 / CN7-12 | J1-12 → J6-5 | MISO |
| SPI1 MOSI | PA7 / CN7-14 | J1-14 → J6-4 | MOSI |
| SPI1 CS | PD14 / CN7-16 | J1-16 → J6-3 | CS |
| FRAM alimentation | +3V3 / GND | J6-9 / J6-7 | **VIN** / GND |
| LPUART TX | PG7 / CN10-14 | J4-14 → J9-13 | Click TX, entrée transceiver |
| LPUART RX | PG8 / CN10-16 | J4-16 → J9-14 | Click RX, sortie transceiver |
| Direction RS485 | PG4 / CN12-69 | J5-1 → J9-3 | Click PWM / DE |
| Click alimentation | +3V3 / GND | J9-7 / J9-8 et 9 | VCCSEL sur 3V3 requis |

J5-1 est le **numéro local du connecteur shield** correspondant à CN12-69 ; J5-2 correspond à CN12-70/PG6 et est NC. Ne pas lire J5-1 comme CN12-1. CN12/Morpho retenu uniquement pour PG4 ; PC9 reste sur Zio CN8 pour ne pas dépendre de SB22.

J6 : 1 HOLD, 2 WP, 3 CS, 4 MOSI, 5 MISO, 6 SCK, 7 GND, 8 sortie régulateur 3V3, 9 VIN. **J6-8 reste non connecté.** L’ancien libellé de banc « FRAM 3V3 » ne permet pas de conclure quel point avait été utilisé : la proposition respecte l’entrée VIN du fabricant et ne relie pas sa sortie régulateur au rail Nucleo. Ne pas transposer automatiquement une ancienne consigne ambiguë.

J7 : 1 3V, 2 GND, 3 CLK, 4 SO/D0, 5 SI/CMD, 6 CS/D3, 7 D1, 8 D2, 9 détection. 7/8/9 sont NC ; **aucun SDMMC**. Repérage issu du PCB Adafruit, pas d’une ancienne carte de brochage SDMMC2.

J8 : JP1 à gauche, 1 à 12 de haut en bas ; JP2 à droite, **24 à 13 de haut en bas**. L’utilisateur avait appelé le dernier emplacement « JP2-12 » en comptant les douze positions : sa description finale situe GND en bas à droite, cohérente avec **13 global**, sans mesure de continuité pendant cette mission. Entraxe mesuré rapporté : 15,2 mm au pied à coulisse ; implantation nominale 15,24 mm au pas 2,54 mm. Aucun signal INT raccordé.

J9 : 1 NC, 2 /RE externe NC, 3 DE, 4–6 NC, 7 +3V3, 8 GND, 9 GND, 10 +5V NC, 11–12 NC, 13 TX, 14 RX, 15 INVD NC, 16 INVR NC. Cette variante exige le **pont J2 du Click reliant DE et /RE**, cohérent avec le banc documenté ; ne pas le confondre avec JP2/T_NRST du Nucleo. INVD/INVR conservent leurs résistances embarquées. Les borniers A/B/GND2 restent sur le Click déporté hors contour. **GND2 n’existe pas dans le schéma du shield.** Le dessin v101 utilisé pour la géométrie et le schéma électrique v102 sont identifiés séparément : leur concordance mécanique reste à vérifier avant fabrication.

R1/R2/R3 : rappels CS au +3V3 de 10 kΩ ; R4 : rappel DE au GND de 10 kΩ. Les rappels embarqués compatibles restent en place. C1 100 nF, C2 10 µF/≥10 V ; polarité + côté pad1 si condensateur polarisé. Chaque module conserve son découplage propre. TP1=3V3, TP2=GND, TP3=ACC_SCK, TP4=SD_CS ; ne pas introduire une sonde longue sur l’horloge en campagne.

Références de code lues, inchangées : `05_Firmware/platform/stm32/main.c` (PD14/FRAM, PA5–7/SPI1, PC9–12/SPI3) et `experiments/spi2_microsd_isolated/src/main.c` / `experiments/r3_durable_fat32/` pour le banc SPI2 de la campagne CN0. Le firmware CN0 réellement qualifié ne pilote pas nécessairement simultanément FRAM et RS485 : leur présence sur le shield **ne qualifie pas la coexistence logicielle**.

Retenus : Nucleo MB1549-U575ZIQ-C05, Adafruit4718/MB85RS2MTA, Adafruit4682, MKIGIBV2 + MKI208V1, RS485 Isolator2 Click MIKROE3863/ADM2867E. Références confirmées par l’utilisateur et gel matériel historique (précision suffixe MTA provenant du gel Adafruit4718, non d’une lecture de boîtier actuelle). Non présents dans ce candidat : anciennes EVAL RS485, Ethernet/PoE, SDMMC, nouveau MCU. Optionnel : boîtier futur et fixation de tête, sans ajout de fonctions électriques.
