# P12-H3g-D — Qualification physique du VibrationSource IIS3DWB STM32

## Objet

Qualifier sur STM32U575 et IIS3DWB physiques le driver dédié
`stm32_iis3dwb_vibration_source` derrière le contrat portable
`VibrationSource`.

Cette tranche succède à H3g-C, qui avait démontré l'acquisition brute
X/Y/Z et la réponse à un changement d'orientation.

## Configuration de qualification

- interface physique : SPI3 ;
- fréquence contractuelle : 26667 Hz ;
- axes : X + Y + Z (`axes_enable_mask = 0x0007`) ;
- pleine échelle : code 0 = ±2 g ;
- BDU et IF_INC activés ;
- conversion délivrée par `read_sample()` : mg entiers.

## Chaîne exercée

Le test physique passe par le driver dédié et l'interface portable :

1. `stm32_iis3dwb_vibration_source_init()` ;
2. contrôle WHO_AM_I ;
3. `VibrationSource.configure()` ;
4. `VibrationSource.start()` ;
5. `VibrationSource.read_sample()` ;
6. lecture de `VibrationSample` en mg ;
7. `VibrationSource.stop()`.

Les anciens helpers de bring-up IIS3DWB dans `main.c` ont été retirés
avant cette qualification.

## Résultats physiques

Valeurs observées par GDB après flash du firmware :

- SPI init OK = 1 ;
- WHO_AM_I status = TR2_OK ;
- WHO_AM_I = 0x7B ;
- WHO_AM_I matches = 1 ;
- configure result = TR2_OK ;
- sample result = TR2_OK ;
- sample valid = 1 ;
- X = +2 mg ;
- Y = -95 mg ;
- Z = +1009 mg.

Norme indicative du vecteur : environ 1013 mg.

Le capteur était immobile. Le vecteur observé est cohérent avec la gravité
portée principalement par +Z. Cette norme est un contrôle de cohérence et
ne constitue pas une qualification métrologique ou une calibration.

## Conclusion

**H3g-D : driver STM32 IIS3DWB / VibrationSource physiquement démontré
pour la configuration 26667 Hz, XYZ, ±2 g.**

La preuve couvre le chemin fonctionnel `configure/start/read_sample/stop`
et la conversion en mg sur matériel réel.

Elle ne qualifie pas encore :

- les autres pleines échelles par mesure physique ;
- les masques d'axes individuels ;
- la cadence soutenue à 26,667 kHz ;
- FIFO/DMA/interruptions ;
- performance de campagne vibration ;
- calibration métrologique.

Ces sujets ne doivent pas être déduits de cette preuve.

## Invariants

- aucune nouvelle qualification destructive FRAM ;
- baseline FRAM H3d3-D3-C préservée ;
- pas de nouvelle sémantique de persistance ;
- H3g-D ne constitue pas encore le gel final de la composition SystemRuntime.
