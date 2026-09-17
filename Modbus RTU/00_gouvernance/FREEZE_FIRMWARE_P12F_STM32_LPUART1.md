# FREEZE FIRMWARE P12-F — Transport STM32 LPUART1

Date : 2026-09-17

## Statut

**GELÉ — validation locale utilisateur acquise après correction de la dépendance HAL UARTEx.**

Niveaux de preuve :

- HOST : VALIDATED ;
- STM32 Cortex-M33 cross-build : VALIDATED ;
- NUCLEO-U575ZI-Q réel : HARDWARE PENDING ;
- RS-485 / ADM2587E réel : HARDWARE PENDING.

## Baseline d'entrée

P12-E gelée :

`9e1218d6d44447468cd9628cf73251916020d471` — `Firmware: freeze P12-E serial transport`

## Périmètre P12-F gelé

P12-F implémente derrière `SerialTransport` la première adaptation STM32 du transport série :

- cible STM32U575 ;
- périphérique `LPUART1` ;
- TX `PG7` ;
- RX `PG8` ;
- profil `115200 / 8E1` ;
- réception par interruption UART, octet par octet ;
- transmission par interruption UART ;
- pas de DMA ;
- stockage d'événements borné côté plateforme ;
- callbacks HAL confinés à `platform/stm32` ;
- erreurs UART traduites vers le contrat portable P12-E ;
- IRQ LPUART1 routée vers l'adaptateur STM32.

Le cœur portable n'est jamais rappelé directement depuis une ISR.

## Commits

- `0b509ece905c65d318c536fab2a49a91f037ae36` — `Firmware: arbitrate P12-F0 UART profile`
- `e63d87dc00cb1f7dc22b8f0882c5c7a1cbd9a379` — `Firmware: add P12-F STM32 serial adapter contract`
- `11274a11c84390fea17bc08326515b07167645da` — `Firmware: implement P12-F LPUART1 interrupt transport`
- `c63e44b95c8726592d6efc023b3d00e02cc6d8a3` — `Firmware: enable HAL UART for P12-F`
- `cb499692794b2762ea94bf11e0ed8756d2897f74` — `Firmware: route LPUART1 IRQ to P12-F transport`
- `2e1e3d7f506170861e710f628d763f7c315d469c` — `Firmware: initialize P12-F LPUART1 transport`
- `641d4dd954c66c55db6ae9d4b783dc48b7918eca` — `Firmware: wire P12-F HAL UART into STM32 cross-build`
- `d3b60a01297d373efc0e116112f163019fa1a44a` — `Firmware: link HAL UARTEx for P12-F`

## Validation observée

Premier cycle après implémentation :

- Host : vert ;
- compilation ARM : réussie jusqu'au link ;
- link : échec sur symboles `HAL_UARTEx_*`.

Cause : `stm32u5xx_hal_uart.c` était linké mais pas son complément `stm32u5xx_hal_uart_ex.c`.

Correction minimale : ajout de `stm32u5xx_hal_uart_ex.c` à la vérification des dépendances STM32CubeU5 et aux sources du target STM32.

Après cette correction, l'utilisateur a relancé le cycle racine et rapporté l'ensemble vert. P12-F est donc validée au niveau Host + cross-build uniquement.

## Invariants / exclusions

P12-F ne fournit toujours pas :

- timer physique T1.5/T3.5 ;
- DE ;
- stratégie `/RE` ;
- pilotage du transceiver ADM2587E ;
- terminaison/polarisation RS-485 ;
- validation du mapping physique EVAL-ADM2587EARDZ ↔ Nucleo ;
- preuve de baudrate réel ;
- preuve d'IRQ réelle sur matériel ;
- communication Modbus RTU réelle.

Aucun changement n'est apporté à B0–B7, B5, P12-B, P12-C, P12-D ou P12-E.

## Revue de périmètre

La comparaison entre la freeze P12-E et la fin fonctionnelle P12-F ne contient que :

- l'arbitrage P12-F0 ;
- les fichiers de l'adaptateur STM32 ;
- les raccordements `main.c`, IRQ, HAL config et CMake nécessaires à P12-F.

Aucune refonte du cœur portable n'est incluse.

## Suite

La prochaine tranche doit traiter le **timing RTU matériel T1.5/T3.5** avant toute composition complète ADU → PDU → réponse sur le transport réel.

Le pilotage DE `/RE` reste différé jusqu'au gel du câblage et de la stratégie RS-485.