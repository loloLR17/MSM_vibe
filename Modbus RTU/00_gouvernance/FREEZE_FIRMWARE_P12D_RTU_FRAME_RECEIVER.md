# FREEZE FIRMWARE P12-D — Réception et délimitation de trame RTU portable

## Statut

**GELÉ — validation locale utilisateur acquise.**

Niveaux de preuve :

- HOST : VALIDATED
- STM32 Cortex-M33 cross-build : VALIDATED
- NUCLEO-U575ZI-Q / RS-485 réel : HARDWARE PENDING

## Périmètre gelé

P12-D ajoute une machine d'état portable de réception RTU :

- `IDLE` ;
- `RECEIVING` ;
- `INVALID`.

Événements d'entrée :

- réception d'un octet ;
- silence logique T1.5 ;
- silence logique T3.5.

Règles gelées :

- un octet en IDLE démarre une trame ;
- T3.5 en RECEIVING publie la trame brute délimitée ;
- T1.5 en RECEIVING invalide la trame ;
- overflow au-delà de `MODBUS_RTU_ADU_MAX_SIZE` invalide la trame ;
- T3.5 en INVALID abandonne la trame et revient à IDLE ;
- aucune validation CRC/PDU/adresse n'est dupliquée dans le receiver.

## Commits P12-D

- `cb5311e798ca86a3382a371f6cfee511268c4330` — `Firmware: arbitrate P12-D RTU frame boundary`
- `b9ab40371fb2da64f898f6b06a5bfa94b197c6ea` — `Firmware: add P12-D RTU receiver contract`
- `20dcaac6a7d53995c7bfc653f8b02d54c80e3a36` — `Firmware: implement P12-D RTU frame receiver`
- `33a234b91c4816d5d71bd10ac469a9df907ed985` — `Firmware: test P12-D RTU frame receiver`
- `3a7b5c884cade0d2756a922fbc9bbbfb90f22d5f` — `Firmware: wire P12-D RTU receiver into validation`

## Validation

Le cycle racine `tr2_validate.sh` a été rapporté intégralement vert par l'utilisateur après raccordement P12-D : Host et cross-build STM32 réussis. Le matériel réel reste explicitement hors preuve.

## Invariants inchangés

P12-D ne fixe ni baudrate, ni parité, ni stop bits, ni timer STM32, ni polling/IRQ/DMA, ni synchronisation ISR/runtime. Il ne modifie ni B0–B7, ni FC03/FC10, ni P12-B, ni P12-C, ni B5, ni LPUART1 PG7/PG8, ni DE `/RE`.

## Suite autorisée

P12-E — contrat de transport série portable et politique de concurrence.