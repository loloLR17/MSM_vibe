# P11-A — Gel BSP / pinout / périphériques STM32

## 1. Objet

Ce document clôt la tranche **P11-A — fermeture BSP / pinout / périphériques** du prototype TR2 sur **NUCLEO-U575ZI-Q / STM32U575ZIT6Q**.

P11-A ne modifie ni la spécification Modbus RTU V1, ni le core portable, ni les contrats génériques `tr2/platform`.

Le but est uniquement de vérifier qu'un ensemble cohérent de périphériques STM32 peut supporter simultanément le prototype physique retenu avant le premier code cible.

## 2. Autorité et classification

Les décisions de ce document sont des décisions **FW_POLICY / BSP P11**.

Elles ne créent aucune nouvelle sémantique Modbus V1.

Restent applicables les invariants déjà gelés :

- le core portable reste indépendant de STM32 HAL/CMSIS/BSP ;
- `PersistentMedia` reste réservé à la persistance critique ;
- le stockage campagne bulk reste séparé via `CampaignDataStore` ;
- le prototype P11 commence exclusivement par RS-485 ;
- aucune décision V1.1 n'est importée pour combler une lacune V1.

## 3. Matériel P11 concerné

- MCU : `NUCLEO-U575ZI-Q`, `STM32U575ZIT6Q` ;
- MEMS : `IIS3DWB`, prototype `STEVAL-MKI208V1K` ;
- hot store : `MB85RS2MTA`, prototype Adafruit 4718 ;
- cold lifetime store : `W25Q64JV`, prototype Adafruit 5636 ;
- stockage bulk : carte SD/microSD standard 128 Go ;
- RS-485 : `ADM2587E`, prototype `EVAL-ADM2587EARDZ`.

## 4. Pinout et périphériques candidats gelés pour P11

| Fonction | Périphérique STM32 | Broches / ressources candidates | DMA | IRQ | Conflits / précautions | Décision P11-A |
|---|---|---|---|---|---|---|
| IIS3DWB SPI | SPI1 | PA5 SCK, PA6 MISO, PA7 MOSI, PD14 CS | GPDMA requis pour l'acquisition | SPI1 + EXTI DRDY | aucun conflit retenu avec SWD, LSE ou SDMMC1 | retenu |
| IIS3DWB DRDY | GPIO / EXTI | PE11 candidat | non | EXTI | choix purement BSP, remplaçable avant câblage définitif | retenu comme candidat |
| FRAM MB85RS2MTA | SPI2 partagé | PD1 SCK, PD3 MISO, PD4 MOSI + CS dédié | disponible, non requis au premier bring-up | SPI2 si mode IRQ retenu | partage volontaire avec NOR | retenu |
| NOR W25Q64JV | SPI2 partagé | PD1 SCK, PD3 MISO, PD4 MOSI + second CS dédié | disponible | SPI2 si mode IRQ retenu | partage volontaire avec FRAM | retenu |
| SD 128 Go | SDMMC1 4-bit | PC8 D0, PC9 D1, PC10 D2, PC11 D3, PC12 CK, PD2 CMD | requis | SDMMC1 | surveiller les stubs / solder bridges Nucleo associés à PC8-PC9 | retenu |
| RS-485 TX/RX | LPUART1 | PG7 TX, PG8 RX | disponible et candidat | LPUART1 | conserve USART1 VCP ST-LINK séparé | retenu |
| RS-485 DE | GPIO | GPIO BSP dédié à déterminer avec câblage final EVAL-ADM2587EARDZ | non | non | ne pas inventer le mapping J5 | principe retenu, broche non gelée |
| RS-485 /RE | GPIO ou couplage avec DE | à déterminer avec câblage final EVAL-ADM2587EARDZ | non | non | stratégie exacte NOT_DEFINED V1 | principe retenu, broche non gelée |
| RTC | RTC + LSE | PC14 OSC32_IN, PC15 OSC32_OUT | non | RTC selon usage | LSE Nucleo déjà présent ; ne pas réutiliser comme GPIO | retenu |
| VBAT | domaine backup | VBAT Nucleo | non | non | aucun | retenu |
| Tick / scheduling | TIM6 candidat | interne | non requis | TIM6 | choix final dépend du runtime cible | candidat retenu |
| Acquisition timing | timer interne | aucune broche imposée par P11-A | selon implémentation | selon timer | ne pas sur-spécifier avant driver IIS3DWB | à fermer ultérieurement |
| Watchdog | IWDG | interne | non | aucun requis | aucun | retenu |
| Debug | SWD / STLINK-V3E | PA13 SWDIO, PA14 SWCLK, SWO éventuel, NRST | non | debug | PA13/PA14 réservées au debug | préservé |

## 5. Architecture physique résultante

```text
IIS3DWB
   ↓
SPI1 + GPDMA

MB85RS2MTA ─┐
             ├─ SPI2 partagé, CS séparés
W25Q64JV ────┘

SD 128 Go
   ↓
SDMMC1 4-bit + DMA

ADM2587E
   ↓
LPUART1 + GPIO DE//RE

RTC
   ↓
LSE onboard + domaine VBAT

Debug
   ↓
STLINK-V3E / SWD conservé
```

## 6. Invariants de séparation

Le port STM32 doit conserver :

```text
tr2_core
    ↓
contrats génériques include/tr2/platform
    ↓
platform/stm32
    ↓
STM32 HAL / LL / CMSIS / BSP / périphériques physiques
```

Aucun header STM32 ne doit être inclus depuis le core portable ou depuis les contrats génériques `include/tr2/platform`.

La SD bulk ne doit pas être incorporée artificiellement dans `PersistentMedia`.

## 7. DMA

P11-A gèle uniquement le besoin fonctionnel suivant :

- SPI1 / IIS3DWB : DMA requis ;
- SDMMC1 : DMA requis ;
- LPUART1 : DMA disponible et candidat ;
- SPI2 / FRAM-NOR : DMA disponible mais non requis au premier bring-up.

Les numéros précis de channels / requests GPDMA ne sont **pas** gelés par P11-A. Ils devront être validés dans la configuration STM32 réelle.

## 8. Point explicitement non gelé

Le mapping exact des lignes `DE` et `/RE` de `EVAL-ADM2587EARDZ` vers les GPIO Nucleo n'est pas déduit par intuition.

Il sera fermé sur schéma / câblage physique vérifié avant activation du transceiver.

Cela ne remet pas en cause le choix de `LPUART1` pour TX/RX.

## 9. Références matérielles utilisées pour P11-A

Documentation officielle consultée :

- STMicroelectronics — NUCLEO-U575ZI-Q user manual / MB1549 ;
- STMicroelectronics — STM32U575 datasheet et alternate-function mapping ;
- STMicroelectronics — MB1549 schematic pack ;
- STMicroelectronics — STEVAL-MKI208V1K documentation ;
- STMicroelectronics — STM32CubeU5 / CMSIS Device U5 ;
- Analog Devices — EVAL-ADM2587EARDZ user guide ;
- documentation fabricant des breakouts prototype FRAM et NOR retenus.

## 10. Statut

**P11-A validée et gelée.**

La tranche suivante est :

**P11-B — squelette `platform/stm32` et build cible minimal NUCLEO-U575ZI-Q, sans raccordement métier ni périphérique physique.**
