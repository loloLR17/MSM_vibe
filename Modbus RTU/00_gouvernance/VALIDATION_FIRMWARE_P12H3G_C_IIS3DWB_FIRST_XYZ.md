# P12-H3g-C — Première acquisition physique brute IIS3DWB

## Objet

Qualifier la première acquisition physique X/Y/Z du IIS3DWB après H3g-B (WHO_AM_I = 0x7B).

## Configuration

- interface : SPI3 STM32U575 ;
- pleine échelle : ±2 g ;
- ODR : 26,667 kHz ;
- BDU activé ;
- IF_INC activé pour les lectures multi-octets ;
- acquisition polling minimale ;
- conversion nominale utilisée pour interprétation : 0,061 mg/LSB.

## Incident de bring-up

La première version écrivait CTRL3_C = 0x40 : BDU était activé mais IF_INC restait désactivé.

Résultat physique :
- HAL_OK ;
- data ready ;
- X = Y = Z = 7710 (0x1E1E).

Le motif identique sur les trois axes a permis d'identifier une lecture répétée de la même adresse lors du burst.

Correction :
- CTRL3_C = 0x44 ;
- BDU = 1 ;
- IF_INC = 1.

Commit correctif : `9bc0112ea1e5de08f84973390a6f88600c356ca6`.

## Résultat physique après correction

Capteur immobile :

- config_status = HAL_OK ;
- sample_status = HAL_OK ;
- data_ready = 1 ;
- X = -861 LSB ;
- Y = -1060 LSB ;
- Z = +16483 LSB.

À 0,061 mg/LSB, valeurs indicatives :

- X ≈ -52,5 mg ;
- Y ≈ -64,7 mg ;
- Z ≈ +1005,5 mg ;
- norme ≈ 1009 mg.

Le vecteur mesuré est donc cohérent avec un capteur immobile soumis principalement à la gravité sur +Z.

## Statut

Première acquisition brute X/Y/Z physiquement démontrée.

Avant gel H3g-C, une vérification d'orientation indépendante doit encore démontrer que le vecteur gravité change d'axe ou de signe conformément au déplacement physique.

## Invariants

- aucune modification SPI1/FRAM ;
- aucune nouvelle transaction FRAM de qualification ;
- pas encore de revendication métrologique/calibration ;
- pas encore de gel du VibrationSource complet.
