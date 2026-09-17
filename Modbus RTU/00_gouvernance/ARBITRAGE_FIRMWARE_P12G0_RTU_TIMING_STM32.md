# Projet MSM — Capteur de vibration TR2

## P12-G0 — Timing matériel Modbus RTU sur STM32

Date : 2026-09-17

Statut : **ARBITRÉ**

## 1. Baseline

- P12-D définit les événements logiques `SILENCE_T1_5` et `SILENCE_T3_5` sans imposer leur source matérielle.
- P12-E transporte ces événements via `SerialTransport::poll_event()`.
- P12-F fournit LPUART1 / PG7 / PG8, profil 115200 / 8E1, RX/TX par IRQ.
- Aucun timer RTU matériel n'est actuellement implémenté dans `main`.

## 2. Référence normative Modbus

Le projet suit **MODBUS over Serial Line Specification and Implementation Guide V1.02**.

Pour les débits supérieurs à 19200 bit/s, le guide recommande des valeurs fixes :

- inter-character timeout `t1.5` : **750 µs** ;
- inter-frame delay `t3.5` : **1750 µs**.

Le profil TR2 étant fixé à 115200 bit/s, P12-G utilise ces valeurs fixes. Le firmware ne recalcule donc pas 1,5 et 3,5 temps-caractère à partir de 115200 / 8E1.

## 3. Source temporelle STM32

P12-G utilise **TIM6** comme timer matériel dédié au timing de réception Modbus RTU.

Raisons de conception :

- timer général simple, sans dépendance au cœur portable ;
- résolution microseconde facilement obtenue ;
- aucune fonction applicative TR2 actuelle ne l'utilise ;
- confinement intégral dans `platform/stm32` ;
- possibilité de vérifier la configuration au cross-build avant essai matériel.

TIM6 est réservé au timing RTU tant que cette architecture est gelée.

## 4. Base de temps

Le timer est configuré pour fournir un compteur à **1 MHz**, soit un tick nominal de **1 µs**.

La valeur de prescaler doit être calculée à partir de l'horloge TIM6 réellement fournie par RCC et ne doit pas reposer sur une constante implicite non vérifiée.

Les échéances nominales sont donc :

```text
T1.5 =  750 ticks à 1 MHz
T3.5 = 1750 ticks à 1 MHz
```

Le cross-build prouve la cohérence des symboles et types HAL ; la fréquence réelle reste une preuve matérielle ultérieure.

## 5. Machine temporelle

À chaque octet reçu :

1. l'octet est déposé comme événement `BYTE` par l'adaptateur série P12-F ;
2. le timing RTU est redémarré depuis zéro ;
3. première échéance à 750 µs.

À 750 µs sans nouvel octet :

- un événement `SILENCE_T1_5` est déposé une seule fois ;
- le timer poursuit la même fenêtre jusqu'à l'échéance absolue 1750 µs depuis le dernier octet.

À 1750 µs sans nouvel octet :

- un événement `SILENCE_T3_5` est déposé une seule fois ;
- le timer est arrêté jusqu'au prochain octet.

Tout nouvel octet reçu avant T3.5 annule la fenêtre en cours et redémarre les échéances depuis cet octet.

## 6. Ordonnancement des événements

Les événements issus des IRQ sont consommés par le cœur via la file plateforme et `poll_event()`.

La couche STM32 ne rappelle jamais directement P12-D depuis une ISR.

Pour une réception normale, l'ordre observable est :

```text
BYTE ... BYTE -> SILENCE_T1_5 -> SILENCE_T3_5
```

La file doit préserver cet ordre.

Si la file plateforme est saturée, P12-G ne doit pas masquer silencieusement la perte d'un événement temporel. La politique de débordement existante de l'adaptateur doit être revue/étendue de manière à rendre cette perte observable avant gel P12-G.

## 7. Erreurs UART

Une erreur UART reçue pendant une fenêtre temporelle reste un événement `ERROR` P12-E. P12-G ne transforme pas une erreur UART en silence valide et ne publie pas artificiellement une trame.

La décision de remise à zéro du receiver portable après erreur appartient à la future composition runtime et non à l'ISR STM32.

## 8. Hors périmètre

P12-G ne fixe ni n'implémente :

- DE ;
- `/RE` ;
- ADM2587E ;
- terminaison/polarisation ;
- adresse esclave ;
- dispatch ADU/PDU complet dans `main` ;
- réponse Modbus sur bus réel.

## 9. Validation

Avant matériel :

- Host : régression complète ;
- Cross-build : TIM6 HAL, IRQ, configuration et raccordement à l'adaptateur ;
- Hardware : PENDING.

La mesure réelle de 750 µs / 1750 µs et du comportement sous trafic devra être faite sur NUCLEO.

## 10. Suite

P12-G1 peut implémenter le timer TIM6, les deux échéances et leur injection ordonnée dans le transport série STM32, sans toucher au cœur portable ni au pilotage RS-485.