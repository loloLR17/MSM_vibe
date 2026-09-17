# Projet MSM — Capteur de vibration TR2

## P12-F0 — Profil série UART du transport Modbus RTU

Date : 2026-09-17

Statut : **ARBITRÉ**

Ce document fixe les paramètres nécessaires à l'implémentation P12-F du transport série STM32. Il complète P12-E sans modifier la spécification des registres B0–B7 ni les sémantiques transactionnelles B5.

## 1. Interface matérielle gelée

Cible : NUCLEO-U575ZI-Q / STM32U575ZIT6Q.

- périphérique : `LPUART1` ;
- TX : `PG7` ;
- RX : `PG8` ;
- STLINK-V3E / SWD : préservé.

## 2. Profil série TR2 V1

Le profil de communication externe retenu pour le prototype TR2 est :

```text
115200 bit/s
8 bits de données
parité paire
1 bit de stop
```

Notation : **115200 / 8E1**.

Cette valeur de baudrate est une décision `FW_POLICY` du projet TR2, pas une valeur héritée d'une exigence antérieure.

La supervision PC doit être configurée avec exactement le même profil lorsqu'elle communique avec le capteur réel.

## 3. Stratégie P12-F

P12-F utilise le HAL UART STM32 derrière le contrat portable P12-E.

Politique retenue :

- réception pilotée par interruption UART, octet par octet ;
- transmission pilotée par interruption UART ;
- pas de DMA en P12-F ;
- les callbacks HAL/IRQ restent confinés à la couche `platform/stm32` ;
- le cœur portable n'est jamais appelé directement depuis une ISR ;
- les octets et erreurs reçus sont déposés dans un stockage plateforme borné puis consommés via `poll_event()` conformément à P12-E.

Le choix IRQ sans DMA est volontaire pour la première intégration : les ADU RTU sont bornées à 256 octets et le contrat portable permet une évolution ultérieure de l'implémentation plateforme sans modifier le cœur.

## 4. Timing RTU

P12-F ne matérialise pas encore T1.5/T3.5 par un timer STM32. Les événements de silence définis par P12-D/P12-E restent une obligation du transport complet mais leur source matérielle sera arbitrée dans une tranche dédiée avant activation du serveur RTU réel.

Aucun faux événement T1.5/T3.5 n'est synthétisé par le driver UART P12-F.

## 5. Adresse Modbus

P12-F ne code aucune adresse esclave en dur.

L'adresse reste distincte du profil UART et devra être fournie par l'autorité de configuration/provisioning prévue par l'architecture. L'état courant de la supervision représente l'adresse par un octet et n'impose pas encore de plage métier additionnelle.

## 6. RS-485

Le transport physique final est half-duplex RS-485, mais P12-F reste volontairement limité à LPUART1 TX/RX.

Restent hors P12-F :

- GPIO DE ;
- stratégie `/RE` ;
- bascule RX/TX du transceiver ADM2587E ;
- terminaison et polarisation ;
- validation électrique ;
- mapping physique EVAL-ADM2587EARDZ ↔ Nucleo.

Ces éléments ne seront gelés qu'après vérification matérielle appropriée.

## 7. Gestion des erreurs

Les erreurs UART suivantes doivent être traduites vers le contrat P12-E :

- overrun → `SERIAL_TRANSPORT_ERROR_OVERRUN` ;
- framing → `SERIAL_TRANSPORT_ERROR_FRAMING` ;
- parity → `SERIAL_TRANSPORT_ERROR_PARITY` ;
- noise → `SERIAL_TRANSPORT_ERROR_NOISE` ;
- autre erreur HAL UART → `SERIAL_TRANSPORT_ERROR_UNSPECIFIED`.

Aucune erreur n'est silencieusement transformée en absence de données.

## 8. Niveaux de validation autorisés

Sans carte NUCLEO disponible :

- Host : régression du cœur portable ;
- STM32 cross-build : compilation et link du driver HAL, configuration LPUART1/PG7/PG8 et symboles IRQ ;
- Hardware : **PENDING**.

Le cross-build ne constitue pas une preuve de fonctionnement électrique, de baudrate réel, d'IRQ réelle ou de timing RTU.

## 9. Suite

P12-F peut maintenant implémenter le driver STM32 LPUART1/PG7/PG8 derrière `SerialTransport`, avec 115200 / 8E1 et IRQ RX/TX, sans DE `/RE` ni timer T1.5/T3.5.