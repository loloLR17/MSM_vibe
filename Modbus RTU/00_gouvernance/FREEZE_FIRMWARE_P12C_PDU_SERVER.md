# FREEZE FIRMWARE P12-C — Serveur PDU Modbus FC03 / FC10

## Statut

**GELÉ — validation locale utilisateur acquise.**

Niveaux de preuve :

- HOST : VALIDATED
- STM32 Cortex-M33 cross-build : VALIDATED
- NUCLEO-U575ZI-Q / RS-485 réel : HARDWARE PENDING

## Périmètre gelé

P12-C ajoute le serveur PDU portable au-dessus des adapters Modbus existants :

- FC03 — Read Holding Registers ;
- FC10 — Write Multiple Registers ;
- autres Function Codes : Illegal Function (`0x01`) ;
- erreurs d'adresse/droits : Illegal Data Address (`0x02`) ;
- erreurs de forme/quantité/byte-count : Illegal Data Value (`0x03`) ;
- indisponibilité interne d'un service requis : Slave Device Failure (`0x04`).

Le serveur :

- sérialise/désérialise les champs PDU 16 bits en big-endian ;
- respecte les limites FC03 = 125 registres et FC10 = 123 registres ;
- délègue les lectures à `modbus_read_adapter_read` ;
- délègue les écritures aux adapters B2/B4/B5/B6 existants ;
- ne contourne pas le Register Model ;
- conserve le chemin transactionnel B5 via `CommandRequestMailbox` ;
- n'introduit aucune allocation dynamique requise par le chemin PDU.

## Commits fonctionnels

- `8f6f2d742d315f9acd1f624977024fa05bfd33e6` — `Firmware: add P12-C PDU server contract`
- `369ee5c33dde736fec5c8c6fff09f483b3fdba0e` — `Firmware: implement P12-C FC03 FC10 PDU server`
- `3d2dfeca144ab4e0c414638fba0487143004f988` — `Firmware: fix P12-C B5 submit initialization`
- `90eb227f7267faaaf42d168e98bdf1b86cb89c57` — `Firmware: add P12-C PDU server tests`
- `42e10197ea995a766b600430a3efe44cf1f90d20` — `Firmware: wire P12-C PDU server into validation`

Arbitrage préalable :

- `c2527b8b26dd919f3a6d7017806d4442302341e0` — `Firmware: arbitrate P12-C Modbus function codes`

## Validation

La validation locale complète via le script racine `tr2_validate.sh` a été rapportée verte après raccordement du serveur PDU et de son test à la chaîne CMake/CTest.

Aucun nombre de tests n'est figé ici faute de sortie CTest complète consignée dans cette discussion ; seule la réussite du cycle complet est revendiquée.

## Invariants inchangés

P12-C ne modifie pas :

- le mapping B0–B7 ;
- les droits RO/RW/RESERVED ;
- la sémantique transactionnelle B5 ;
- transaction IDs, persistance ou recovery ;
- la distinction V1/V1.1 ;
- le pinout STM32 P11-A ;
- DE ou `/RE` ;
- UART, IRQ, DMA ou timing RTU T1.5/T3.5.

## Limite du gel

Ce gel prouve le serveur **PDU portable**, pas encore la délimitation temporelle d'une trame RTU ni le transport série physique.

La tranche suivante est P12-D : réception RTU et délimitation de trame indépendante du HAL.