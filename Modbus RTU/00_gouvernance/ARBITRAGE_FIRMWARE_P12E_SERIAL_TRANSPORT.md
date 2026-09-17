# ARBITRAGE FIRMWARE P12-E — Contrat de transport série portable

## 1. Objet

P12-E définit la frontière portable entre le cœur Modbus RTU et une future implémentation série STM32.

Le contrat doit permettre de tester la composition sans HAL et sans figer prématurément polling, IRQ ou DMA.

## 2. Constat

`main` utilise déjà pour les dépendances plateforme des contrats C à table de fonctions avec `void *context` (par exemple `VibrationSource`). Aucun contrat série existant ni politique ISR/runtime n'a été retrouvé.

## 3. Arbitrage

Le transport série portable expose uniquement des **capacités**, pas une stratégie matérielle :

- armer/démarrer la réception ;
- transmettre un buffer complet ;
- interroger/consommer les événements reçus sous forme d'octets et de silences logiques T1.5/T3.5 ;
- signaler explicitement les erreurs de transport.

Le cœur ne connaît ni HAL, ni UART handle, ni DMA, ni IRQ.

### Modèle de concurrence

Le contrat est **pull côté runtime** : l'implémentation plateforme peut collecter les événements depuis IRQ/DMA/polling, mais elle ne rappelle jamais directement le cœur métier depuis une ISR.

La méthode portable `poll_event` fournit au runtime au plus un événement à la fois. Cela impose une frontière de sérialisation claire :

```text
ISR/DMA/polling plateforme -> stockage plateforme -> poll_event() -> cœur RTU
```

La structure de stockage plateforme (ring buffer, mailbox, flags, etc.) reste un choix P12-F et n'est pas figée par P12-E.

### Transmission

`transmit` accepte un buffer ADU complet et sa longueur. Le contrat ne pilote pas DE ou `/RE` et ne définit pas encore la notion de fin physique du dernier stop bit ; ces responsabilités restent plateforme/RS-485.

Aucune relance automatique n'est autorisée dans le contrat portable.

## 4. Événements portables

Types d'événement :

- `BYTE` avec un octet ;
- `SILENCE_T1_5` ;
- `SILENCE_T3_5` ;
- `ERROR` avec un code d'erreur transport générique.

`NONE` signifie qu'aucun événement n'est actuellement disponible.

## 5. Résultats

Les opérations de contrôle (`start_receive`, `transmit`) utilisent `Tr2Result`.

`poll_event` retourne un booléen indiquant si l'appel lui-même est valide/réussi et remplit un événement, y compris `NONE`. Les erreurs série asynchrones sont transportées par l'événement `ERROR`, pas transformées silencieusement en absence de données.

## 6. Invariants

P12-E ne fixe pas :

- baudrate, parité, stop bits ;
- LPUART registers/HAL ;
- polling/IRQ/DMA ;
- ring-buffer ou taille de queue ;
- timer physique T1.5/T3.5 ;
- DE ou `/RE` ;
- politique de retry.

P12-E ne modifie pas B0–B7, B5, P12-B/C/D.

## 7. Statut

**ARBITRÉ — prêt pour contrat + fake Host P12-E.**