# FREEZE FIRMWARE P12-E — Contrat de transport série portable

## Statut

**GELÉ — validation locale utilisateur acquise.**

Niveaux de preuve :

- HOST : VALIDATED
- STM32 Cortex-M33 cross-build : VALIDATED
- NUCLEO-U575ZI-Q / RS-485 réel : HARDWARE PENDING

## Périmètre gelé

P12-E définit la frontière portable entre le cœur Modbus RTU et la future implémentation série STM32.

Contrat gelé :

- `start_receive` ;
- `transmit` d'un buffer complet ;
- `poll_event` côté runtime ;
- événements `BYTE`, `SILENCE_T1_5`, `SILENCE_T3_5`, `ERROR`, `NONE` ;
- erreurs génériques `OVERRUN`, `FRAMING`, `PARITY`, `NOISE`.

Politique de concurrence gelée :

```text
ISR/DMA/polling plateforme -> stockage plateforme -> poll_event() -> cœur RTU
```

Le cœur portable n'est jamais rappelé directement depuis une ISR.

## Commits P12-E

- `5673cf74675e7aa2bcf9ac383b133c5f13956995` — `Firmware: arbitrate P12-E serial transport contract`
- `0f532ad98040c4eb7b8ab06b8240ac7627685022` — `Firmware: add P12-E serial transport contract`
- `a847d5ad591910eb493565d149c0a5604b2427f4` — `Firmware: implement P12-E serial transport guards`
- `790d2ae1029ef8784bb8d8373e3df751a178e71e` — `Firmware: test P12-E serial transport contract`
- `16273299453efd44fe2c9aee5af5a0f5fa71f561` — `Firmware: wire P12-E serial transport into validation`

## Validation

Le cycle racine `tr2_validate.sh` a été rapporté intégralement vert par l'utilisateur après raccordement P12-E : Host et cross-build STM32 réussis. Le matériel réel reste hors preuve.

## Invariants inchangés

P12-E ne fixe ni baudrate, ni parité, ni stop bits, ni timer matériel, ni polling/IRQ/DMA, ni structure de queue, ni DE `/RE`. Il n'introduit aucun retry automatique et ne modifie ni B0–B7, ni B5, ni P12-B/C/D.

## Suite autorisée

P12-F — implémentation STM32 LPUART1 / PG7 / PG8 derrière le contrat portable, sans DE `/RE`.