# P12-H3g-A — Audit interface matérielle IIS3DWB / STEVAL-MKI208V1K

## 1. Objet

Arbitrer l'interface matérielle du futur `VibrationSource` STM32 à partir des documents constructeur ST, avant tout câblage ou driver.

Baseline projet au démarrage : `43c3ed73e6905f51a66c79dbd307eba3754775ca`.

Matériel cible :

- NUCLEO-U575ZI-Q / STM32U575ZIT6Q ;
- STEVAL-MKI208V1K ;
- capteur IIS3DWB.

## 2. Interface capteur retenue

Le IIS3DWB supporte SPI et I2C sur les mêmes broches, mais la documentation ST indique que seul SPI permet d'exploiter toutes les capacités du composant ; I2C est limité par son débit et n'est pas recommandé pour l'usage complet.

Décision : **SPI 4 fils**.

Signaux nécessaires :

- CS ;
- SPC/SCL = SCK ;
- SDI/SDO/SDA = MOSI en SPI 4 fils ;
- SDO/SA0 = MISO en SPI 4 fils ;
- VDD ;
- VDDIO ;
- GND.

INT1/INT2 ne sont pas nécessaires pour le premier bring-up WHO_AM_I. Leur usage sera arbitré ultérieurement si FIFO/DRDY l'exige.

## 3. Caractéristiques fonctionnelles pertinentes

Référence IIS3DWB :

- accéléromètre numérique 3 axes ;
- pleine échelle sélectionnable ±2 / ±4 / ±8 / ±16 g ;
- ODR nominal fixe 26,667 kHz lorsque l'accéléromètre est actif ;
- bande plate annoncée jusqu'à 6 kHz (point ±3 dB) ;
- SPI modes 0 et 3 selon la datasheet ;
- WHO_AM_I : registre 0x0F, valeur attendue 0x7B ;
- sorties accélération : 0x28 à 0x2D.

Le premier bring-up doit rester en lecture seule : alimentation, SPI, lecture WHO_AM_I. Aucune configuration dynamique du capteur n'est nécessaire pour prouver le bus.

## 4. STEVAL-MKI208V1K

Le schéma ST montre sur la carte capteur :

- VDD et VDDIO séparément exposés ;
- GND ;
- CS, SCL, SDA et SDO ;
- résistances série 33 ohms sur les lignes numériques CS/SCL/SDA/SDO ;
- découplage local déjà présent sur VDD/VDDIO.

Le kit comprend également l'adaptateur STEVAL-MKIGIBV2 qui rend le pinout accessible au format DIL24.

Avant câblage physique, les numéros de broches du connecteur réellement utilisé seront vérifiés sur le schéma ST et confrontés visuellement au module présent.

## 5. Coexistence avec la FRAM

La FRAM H3d3 utilise déjà SPI1 :

- PA5 = SCK ;
- PA6 = MISO ;
- PA7 = MOSI ;
- PD14 = CS FRAM.

Il n'est pas nécessaire, à ce stade, de modifier cette interface gelée.

Deux architectures restent techniquement possibles :

1. partager SPI1 avec un CS distinct pour IIS3DWB ;
2. utiliser un second contrôleur SPI STM32.

Pour la rigueur du bring-up H3g, la préférence est donnée à **un second contrôleur SPI**, afin de ne pas perturber la chaîne FRAM physiquement qualifiée et de permettre au capteur d'avoir sa propre configuration SPI/timing.

Le choix exact SPIx + GPIO + connecteur NUCLEO doit être validé contre le pinout officiel STM32U575/MB1549 avant câblage. Aucun pin n'est figé par le présent audit sans cette vérification.

## 6. Stratégie de bring-up

H3g-B devra être strictement minimal :

1. choisir et documenter SPIx/SCK/MISO/MOSI/CS libres ;
2. alimenter VDD et VDDIO en 3,3 V et relier GND ;
3. configurer SPI maître 4 fils, MSB first, fréquence volontairement conservatrice ;
4. maintenir CS haut au repos ;
5. lire uniquement WHO_AM_I 0x0F ;
6. exiger 0x7B ;
7. exposer résultat HAL + octet lu par sondes GDB ;
8. aucune écriture de configuration capteur à cette étape.

Le mode SPI exact choisi (0 ou 3) et la fréquence de qualification seront explicitement figés dans H3g-B à partir de la datasheet et des contraintes STM32.

## 7. Suite après WHO_AM_I

Après preuve physique du bus :

- adapter bas niveau IIS3DWB ;
- configuration `VibrationSourceConfiguration` ;
- validation des full scales réellement supportées ;
- acquisition X/Y/Z cohérente ;
- conversion brute signée vers mg selon la sensibilité documentée ;
- comportement start/read/stop ;
- décision polling vs FIFO/interruptions en fonction du débit réel requis ;
- qualification physique ;
- gel du `VibrationSource`.

## 8. Invariants

- aucune modification de la baseline FRAM H3d3 ;
- aucune nouvelle écriture FRAM de qualification ;
- aucun faux `VibrationSource` ;
- aucune dépendance HAL dans le core portable ;
- pas de câblage basé sur une supposition de pinout ;
- documentation ST officielle prioritaire.
