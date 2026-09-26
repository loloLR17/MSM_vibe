# Freeze P12-H3d3-B — NUCLEO STM32 physical bring-up

## Statut

**GELÉ — validation matérielle réelle**

Baseline d'entrée :
- P12-H3d2 gelé : `9e08ae03d086adc8fa8336e8827ac2150bff1644`
- préparation H3d3-0 : `06b373ace0da3bbec08d4d59bd016a0906d46008`

Baseline firmware physiquement validée :
- `aa9559c7659c6080e182c4af561ace0cd881f83d`
- `Firmware: enable PWR clock during STM32 HAL MSP init`

Cette tranche qualifie uniquement le bring-up de la NUCLEO. Elle ne qualifie encore ni SPI, ni mémoire NVM, ni PhysicalStorage H3d3-D.

## 1. Matériel qualifié

Carte :
- ST NUCLEO-U575ZI-Q ;
- carte MB1549C ;
- MCU STM32U575ZIT6Q ;
- STLINK-V3E intégré ;
- programmation/debug via CN1 / STLK Micro-USB et SWD.

Identification physique observée avec STM32CubeProgrammer :
- ST-LINK SN : `0032001F3235510C37333439` ;
- ST-LINK FW : `V3J15M7` ;
- tension observée : environ 3.27–3.28 V ;
- Device ID : `0x482` ;
- Revision ID : Rev U ;
- device : STM32U575/STM32U585 ;
- NVM annoncée : 2 MiB ;
- CPU : Cortex-M33 ;
- bootloader : `0x93`.

## 2. Flash, SWD et reset

Validé sur matériel réel :
- connexion STLINK-V3E ;
- accès SWD ;
- lecture de la Flash interne ;
- programmation du binaire à `0x08000000` ;
- debug GDB via ST-LINK ;
- reset matériel B2 ;
- démarrage autonome reproductible après reset.

Le binaire validé est :
`build-stm32-p11c/tr2_stm32_p11c.bin`.

## 3. Défaut de bring-up découvert et corrigé

Le premier firmware programmé ne faisait pas clignoter LD1.

Le debug physique a établi :
- entrée dans `main()` ;
- passage dans `SystemClock_Config()` ;
- retour `HAL_TIMEOUT (0x03)` de `HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)` ;
- timeout sur l'attente `PWR->VOSR.VOSRDY` ;
- base PWR Non-Secure compilée : `0x46020800` ;
- écriture CPU de `0x00070000` vers `PWR->VOSR` à `0x4602080c`, avec relecture immédiate à zéro ;
- `RCC_AHB3ENR = 0x80000000` ;
- `RCC_AHB3ENR.PWREN`, bit 2 / masque `0x00000004`, non positionné.

Cause racine qualifiée : l'horloge du périphérique PWR n'était pas activée avant l'utilisation du service HAL PWR.

Correctif minimal :
```c
void HAL_MspInit(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
}
```

Aucune modification de l'architecture portable, de H3d2 ou du transport transactionnel n'a été nécessaire.

## 4. Validation physique après correctif

Après compilation et programmation du commit `aa9559c7659c6080e182c4af561ace0cd881f83d` :
- LD1 / PC7 clignote avec la temporisation prévue de 250 ms ;
- la boucle principale est donc atteinte ;
- `SystemClock_Config()` est franchi ;
- `SystemPower_Config()` est franchi ;
- `BringupLed_Init()` est franchi ;
- `stm32_serial_transport_init()` est franchi ;
- `serial_transport_start_receive()` est franchi ;
- après un nouveau B2 RESET, LD1 repart spontanément : démarrage autonome reproductible validé.

## 5. Build STM32 validé

Cross-build :
- GNU Arm Embedded GCC 12.2.1 ;
- 85/85 étapes de build ;
- ELF final : `tr2_stm32_p11c.elf`.

Résumé :
```text
text     data    bss     dec     hex
32604      20   3284   35908    8c44
```

## 6. Budget mémoire réel du binaire H3d3-B

Mesure `arm-none-eabi-size -A` :
```text
.isr_vector           568
.text               32028
.ARM.exidx              8
.init_array             4
.fini_array             4
.data                  12
.bss                  720
._user_heap_stack    2564
```

Réservation SRAM visible :
- `.data` : 12 B ;
- `.bss` : 720 B ;
- `._user_heap_stack` : 2564 B ;
- total statique/réservé correspondant : 3296 B.

Sur les 768 KiB déclarés par le linker, ce socle représente environ 0.42 %.

Attention : `._user_heap_stack` est une réservation linker, pas une mesure de stack high-water.

Le buffer candidat H3d2 de 51 818 B n'est pas encore alloué dans ce binaire. Son ajout ultérieur reste plausible en capacité brute, mais devra être requalifié avec le runtime physique complet.

## 7. Conclusion du gel

P12-H3d3-B est validée et gelée pour :
- identification de la NUCLEO/MCU ;
- programmation Flash ;
- SWD/debug ;
- reset matériel ;
- démarrage autonome ;
- bring-up HAL/PWR/clock ;
- franchissement de l'initialisation série existante ;
- budget mémoire du binaire de bring-up.

Ce gel ne vaut pas validation de :
- SPI ;
- câblage d'une NVM ;
- protocole d'une mémoire physique ;
- PhysicalStorage brut ;
- H3d2 sur stockage réel ;
- coupures d'alimentation réelles.

La tranche suivante est P12-H3d3-C — SPI électrique minimal, conformément à H3d3-0.
