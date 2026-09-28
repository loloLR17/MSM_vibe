# P12-H3h-B — Sélection SDMMC2 et pinout microSD

## Objet

Choisir un bus SD natif et un jeu de broches pour le breakout microSD Adafruit
4682 sur NUCLEO-U575ZI-Q MB1549-U575ZIQ-C05, sans collision avec les ressources
déjà qualifiées du TR2.

## Références constructeur

- STM32U575xx datasheet, alternate functions SDMMC1/SDMMC2.
- Schéma ST MB1549-U575ZIQ-C05.
- Documentation Adafruit 4682 : alimentation 3.3 V, CLK, CMD et D0..D3 pour SDIO.

## Ressources déjà occupées

- FRAM : SPI1 PA5/PA6/PA7 + CS PD14.
- IIS3DWB : SPI3 PC10/PC11/PC12 + CS PC9.
- RS-485 : LPUART1 PG7/PG8.
- RTC : LSE PC14/PC15.

## Rejet de SDMMC1

Le jeu SDMMC1 4-bit naturel comprend :

- PC8 = SDMMC1_D0 ;
- PC9 = SDMMC1_D1 ;
- PC10 = SDMMC1_D2 ;
- PC11 = SDMMC1_D3 ;
- PC12 = SDMMC1_CK ;
- PD2 = SDMMC1_CMD.

Il entre directement en collision avec le bus SPI3 IIS3DWB sur PC9..PC12.
SDMMC1 n'est donc pas retenu.

## Sélection SDMMC2

Le STM32U575 offre un jeu SDMMC2 4-bit sans collision avec les ressources TR2 :

- PD6 = SDMMC2_CK, AF11 ;
- PD7 = SDMMC2_CMD, AF11 ;
- PB14 = SDMMC2_D0, AF12 ;
- PB15 = SDMMC2_D1, AF12 ;
- PB3 = SDMMC2_D2, AF12 ;
- PB4 = SDMMC2_D3, AF12.

Le manuel UM2861 Rev 10 et le schéma MB1549-U575ZIQ-C05 donnent le repérage
physique exact sur les connecteurs ST Morpho :

| Signal microSD | STM32 | Connecteur |
|---|---|---|
| CLK | PD6 | CN11 pin 43 |
| CMD | PD7 | CN11 pin 45 |
| D0 | PB14 | CN12 pin 28 |
| D1 | PB15 | CN12 pin 26 |
| DAT2 | PB3 | CN12 pin 31 |
| D3 | PB4 | CN12 pin 27 |
| 3V | 3V3 | CN11 pin 16 |
| GND | GND | CN11 pin 19 ou 20 |

Les solder bridges SB19/SB22 documentés pour la qualité des signaux SDMMC
concernent PC8/PC9 et donc le chemin SDMMC1/Zio CN8 ; ils ne font pas partie du
jeu SDMMC2 retenu ici.

## Raccordement logique breakout

- Adafruit CLK  <- PD6 / SDMMC2_CK
- Adafruit CMD  <-> PD7 / SDMMC2_CMD
- Adafruit D0   <-> PB14 / SDMMC2_D0
- Adafruit D1   <-> PB15 / SDMMC2_D1
- Adafruit DAT2 <-> PB3 / SDMMC2_D2
- Adafruit D3   <-> PB4 / SDMMC2_D3
- Adafruit 3V   <- 3V3
- Adafruit GND  <- GND

Le pin DET n'est pas requis pour le premier bring-up. Il pourra être ajouté
ultérieurement si le runtime a besoin d'une détection d'insertion dédiée.

## Décision

**SDMMC2 en bus 4 bits est retenu pour H3h-C.**

Cette décision évite toute modification du SPI3 IIS3DWB et du SPI1 FRAM.
Elle ne constitue pas encore une preuve électrique ni fonctionnelle microSD.

## Prochaine frontière

Avant tout câblage :

1. vérifier les exigences électriques du breakout Adafruit 4682 en mode SDIO,
   notamment alimentation et pull-ups ;
2. confirmer qu'aucun strap supplémentaire n'est requis côté breakout ;
3. câbler selon le tableau Morpho ci-dessus ;
4. seulement ensuite implémenter un bring-up SDMMC2 minimal.

Aucun filesystem n'est introduit dans H3h-B.
