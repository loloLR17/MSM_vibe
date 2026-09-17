# FREEZE — Firmware P12-G — STM32 RTU timing

## 1. Objet

Ce document gèle la tranche P12-G du firmware TR2 : matérialisation côté STM32 des événements temporels Modbus RTU nécessaires au récepteur portable P12-D.

Le gel porte sur l'état de `main` validé localement après le durcissement P12-G2.

## 2. État de validation

- Host firmware : **VALIDATED**
- STM32 Cortex-M33 cross-build : **VALIDATED**
- NUCLEO-U575ZI-Q runtime : **HARDWARE PENDING**
- RS-485 / ADM2587E : **HARDWARE PENDING**

Aucune validation matérielle de la précision temporelle, du baud réel, des IRQ réelles ou du bus RS-485 n'est revendiquée par ce gel.

## 3. Baseline temporelle gelée

Pour le profil série TR2 gelé en P12-F0, `115200 / 8E1`, la couche STM32 applique les délais fixes Modbus RTU retenus par l'arbitrage P12-G0 :

- T1.5 : **750 µs**
- T3.5 : **1750 µs**
- timer dédié : **TIM6**
- base nominale : **1 MHz**, soit 1 µs par tick

Le prescaler TIM6 est calculé à partir de l'horloge effective APB1/TIM6 fournie par RCC ; il n'est pas codé en dur.

## 4. Séquence gelée

À chaque octet reçu par LPUART1 :

1. un événement portable `BYTE` est placé dans la file plateforme ;
2. TIM6 est redémarré depuis zéro ;
3. après 750 µs sans nouvel octet, `SILENCE_T1_5` est placé dans la file ;
4. une seconde période nominale de 1000 µs est armée ;
5. après cette seconde période sans nouvel octet, `SILENCE_T3_5` est placé dans la file ;
6. TIM6 est arrêté jusqu'au prochain octet.

Un nouvel octet reçu avant la fin de la fenêtre redémarre la séquence temporelle depuis cet octet.

## 5. Concurrence et frontière ISR

Les callbacks UART et TIM6 ne doivent jamais appeler directement le cœur portable.

La frontière reste :

```text
LPUART1 / TIM6 IRQ
        |
        v
stockage plateforme borné
        |
        v
SerialTransport.poll_event()
        |
        v
cœur portable RTU
```

Le séquencement TIM6 après son initialisation HAL utilise les primitives timer nécessaires pour arrêter/recharger/réactiver le compteur sans repasser par une séquence HAL Start/Stop depuis le callback de période.

## 6. Saturation de la file d'événements

La file plateforme reste bornée.

Une saturation ne doit pas être silencieuse :

- un latch `event_overflow_pending` mémorise la perte ;
- `poll_event()` traite cet état avant les événements ordinaires ;
- la file devenue incohérente est purgée sous courte section critique ;
- un événement `SERIAL_TRANSPORT_EVENT_ERROR / SERIAL_TRANSPORT_ERROR_OVERRUN` est exposé au consommateur.

Cette règle évite qu'une séquence ayant perdu un octet puisse ensuite être consommée comme une trame temporellement intacte.

## 7. Erreurs UART

Les erreurs UART restent représentées par les événements d'erreur définis en P12-E.

Une erreur UART arrête la fenêtre TIM6 courante et remet l'état temporel en attente d'un nouvel octet.

## 8. Dépendances STM32 cross-build

Le target STM32 inclut les dépendances HAL nécessaires au transport UART et au timer :

- HAL UART / UARTEx ;
- HAL TIM / TIMEx ;
- HAL DMA comme dépendance de link du HAL UART/TIM, sans adoption d'une stratégie DMA pour le transport TR2.

Le transport TR2 reste en réception et émission UART par interruption, sans DMA applicatif.

## 9. Hors périmètre du gel

P12-G ne gèle pas :

- DE RS-485 ;
- /RE RS-485 ;
- pilotage ADM2587E ;
- mapping physique EVAL-ADM2587EARDZ ↔ NUCLEO ;
- terminaison ou polarisation du bus ;
- adresse esclave Modbus ;
- composition runtime complète ADU → PDU → réponse ;
- mesure instrumentée réelle de T1.5/T3.5 ;
- validation du comportement sur NUCLEO réel.

Ces éléments restent explicitement **PENDING**.

## 10. Invariants préservés

P12-G ne modifie pas :

- le mapping B0–B7 ;
- la sémantique transactionnelle B5 ;
- les transaction IDs ;
- la persistance/recovery ;
- les adapters de registres ;
- le codec RTU portable P12-B ;
- le serveur PDU P12-C ;
- la machine de réception portable P12-D ;
- le contrat de transport portable P12-E.

## 11. Critère de sortie

La tranche P12-G est considérée gelée sur la base des validations acquises :

```text
HOST VALIDATED
CROSS-BUILD VALIDATED
HARDWARE PENDING
```

Toute affirmation concernant le fonctionnement réel sur NUCLEO-U575ZI-Q ou sur bus RS-485 devra être établie par une validation matérielle ultérieure.
