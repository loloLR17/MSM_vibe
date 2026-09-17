# ARBITRAGE FIRMWARE P12-D — Délimitation de trame RTU portable

## 1. Objet

P12-D introduit la réception d'octets Modbus RTU et la délimitation d'une ADU complète **sans dépendance HAL, UART, IRQ, DMA ou GPIO**.

Cette tranche doit être testable intégralement sur Host et cross-buildable Cortex-M33.

## 2. Constat dans `main`

Aucune politique gelée n'a été retrouvée pour :

- baudrate ;
- parité / stop bits ;
- source du timer de silence ;
- stratégie polling / IRQ / DMA ;
- seuil temporel calculé en microsecondes ;
- gestion STM32 des erreurs UART.

Ces choix restent donc hors P12-D.

## 3. Arbitrage

P12-D travaille en **temps logique de caractères RTU**, fourni par l'appelant.

Le cœur portable ne calcule pas T1.5/T3.5 depuis un baudrate. Il reçoit uniquement des événements :

- `BYTE(byte)` : nouvel octet reçu ;
- `SILENCE_T1_5` : silence d'au moins 1,5 temps caractère détecté par la couche supérieure ;
- `SILENCE_T3_5` : silence d'au moins 3,5 temps caractère détecté par la couche supérieure.

La future couche STM32 sera responsable de convertir son horloge/UART en ces événements.

## 4. Machine d'état P12-D

États logiques :

- `IDLE` : aucun octet de trame en cours ;
- `RECEIVING` : au moins un octet reçu ;
- `INVALID` : trame en cours invalidée (silence inter-caractère >= T1.5 avant T3.5 ou overflow).

Règles :

1. `BYTE` en `IDLE` démarre une trame.
2. `BYTE` en `RECEIVING` est ajouté tant que la capacité ADU n'est pas dépassée.
3. Un dépassement de `MODBUS_RTU_MAX_ADU_SIZE` invalide la trame courante ; aucun octet supplémentaire n'est stocké.
4. `SILENCE_T1_5` en `RECEIVING` invalide la trame courante mais ne la publie pas.
5. `SILENCE_T3_5` en `RECEIVING` termine la trame et la rend disponible au consommateur.
6. `SILENCE_T3_5` en `INVALID` abandonne la trame invalide et revient à `IDLE` sans publication.
7. Les silences en `IDLE` sont sans effet.
8. Après publication d'une trame, le buffer interne redevient disponible pour la trame suivante.

## 5. Validation de l'ADU

P12-D est responsable de la **délimitation**, pas de réimplémenter le codec P12-B.

À la fermeture T3.5, une trame délimitée est publiée avec ses octets bruts. La validation longueur minimale / CRC et le décodage ADU restent délégués au codec RTU P12-B par la couche de composition ultérieure.

Ainsi :

- P12-D ne duplique pas CRC16 ;
- une trame de longueur physique courte peut être délimitée puis rejetée ensuite par P12-B ;
- P12-D ne connaît pas encore l'adresse esclave ni les Function Codes.

## 6. Propriété de concurrence

P12-D n'introduit aucune hypothèse ISR/thread.

Son API est une machine d'état séquentielle portable. La politique de synchronisation entre ISR UART et runtime appartient à P12-E/P12-F et devra être arbitrée séparément.

## 7. Invariants

P12-D ne modifie pas :

- B0–B7 ;
- FC03/FC10 ;
- P12-B codec ADU/CRC ;
- P12-C serveur PDU ;
- B5 transactionnel ;
- STM32 LPUART1 PG7/PG8 ;
- DE `/RE` ;
- baudrate/parité/stop bits ;
- stratégie IRQ/DMA/polling.

## 8. Statut

**ARBITRÉ — prêt pour implémentation P12-D Host-testable.**