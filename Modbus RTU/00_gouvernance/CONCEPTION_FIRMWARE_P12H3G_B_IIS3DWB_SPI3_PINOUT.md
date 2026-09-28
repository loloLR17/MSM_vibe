# P12-H3g-B — Pinout SPI3 IIS3DWB / NUCLEO-U575ZI-Q

## 1. Décision

Le bring-up IIS3DWB utilise **SPI3** sur un groupe contigu de GPIO du STM32U575 :

- PC10 = SPI3_SCK / AF6 ;
- PC11 = SPI3_MISO / AF6 ;
- PC12 = SPI3_MOSI / AF6 ;
- PC9 = CS logiciel GPIO.

Cette solution laisse inchangé SPI1 de la FRAM (PA5/PA6/PA7 + PD14) et évite PB14/PB15, associés à des fonctions USB/UCPD sur la NUCLEO.

## 2. Broches NUCLEO

D'après UM2861, connecteur Zio CN8 :

- CN8 pin 4 = D44 = PC9 ;
- CN8 pin 6 = D45 = PC10 ;
- CN8 pin 8 = D46 = PC11 ;
- CN8 pin 10 = D47 = PC12 ;
- CN8 pin 7 = 3V3 ;
- CN8 pin 11 ou 13 = GND.

Les quatre signaux SPI/CS sont donc physiquement regroupés sur CN8.

## 3. Broches STEVAL-MKIGIBV2

D'après le schéma officiel STEVAL-MKI208V1K / STEVAL-MKIGIBV2 :

- JP1 pin 1 = VDDIO ;
- JP1 pin 2 = VDD ;
- JP2 pin 24 = SDO-SA0 ;
- JP2 pin 23 = SDA ;
- JP2 pin 22 = SCL ;
- JP2 pin 21 = CS ;
- GND = masse commune de l'adaptateur.

En SPI 4 fils IIS3DWB :

- SCL = SCK ;
- SDA = SDI = MOSI vers le capteur ;
- SDO-SA0 = SDO = MISO depuis le capteur ;
- CS = chip select actif bas.

## 4. Câblage retenu

| STEVAL-MKIGIBV2 | Fonction | NUCLEO-U575ZI-Q |
|---|---|---|
| VDD | alimentation capteur | 3V3 |
| VDDIO | alimentation I/O | 3V3 |
| GND | masse | GND |
| CS | chip select | D44 / PC9 / CN8-4 |
| SCL | SPI SCK | D45 / PC10 / CN8-6 |
| SDO-SA0 | SPI MISO | D46 / PC11 / CN8-8 |
| SDA | SPI MOSI | D47 / PC12 / CN8-10 |

INT1 et INT2 restent non connectées pour H3g-B.

## 5. Bring-up logiciel prévu

Le premier firmware H3g-B doit rester non destructif côté capteur :

- SPI3 maître 4 fils ;
- GPIO CS PC9, haut au repos ;
- 8 bits, MSB first ;
- mode SPI compatible datasheet IIS3DWB ;
- fréquence volontairement conservatrice ;
- lecture seule de WHO_AM_I (0x0F) ;
- valeur attendue : 0x7B ;
- exposition du statut HAL et de l'octet lu par sondes GDB.

Aucune configuration d'acquisition et aucune utilisation FIFO/interruptions dans ce premier test.

## 6. Invariants

- SPI1/FRAM inchangé ;
- LPUART1/RS-485 inchangé ;
- INT1/INT2 non câblées à ce stade ;
- alimentation capteur exclusivement 3V3 pour ce bring-up ;
- VDD **et** VDDIO doivent être alimentés ;
- pas de 5 V sur VDD/VDDIO ;
- aucun gel avant lecture physique WHO_AM_I = 0x7B.
