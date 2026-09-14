# FREEZE — Firmware P12-A — STM32 core cross-build

## Statut

**P12-A VALIDÉE ET GELÉE**

Cette tranche établit que le cœur portable `tr2_core` du firmware TR2 peut être compilé pour la cible Cortex-M33 / STM32U575 et lié dans le bring-up STM32 existant, sans introduire de dépendance STM32 HAL/CMSIS dans le cœur portable.

## Baseline validée

HEAD `main` validé localement :

```text
a72687e1a3550bf2e572b9519240209b4cbd72da
Firmware: restore register model and make validation ARM-safe
```

La tranche P12-A s'appuie notamment sur :

```text
b16ec6b20f9f9fa5f12df58d185030ac733c0098
Firmware: cross-build portable core on STM32
```

Le script racine de validation versionné est :

```text
tr2_validate.sh
```

## Validation locale obtenue

Résultat communiqué après exécution du script racine :

```text
HOST VALIDATED
CROSS-BUILD VALIDATED
HARDWARE PENDING: no NUCLEO-U575ZI-Q runtime or RS-485 hardware claim is made.
```

## Ce que P12-A prouve

- le firmware Host continue de compiler et ses tests CTest passent ;
- les sources portables de `tr2_core` compilent avec la toolchain ARM `arm-none-eabi-gcc` pour Cortex-M33 ;
- `tr2_core` est lié au target STM32 de bring-up ;
- les artefacts STM32 ELF/BIN/MAP sont générés ;
- le cœur portable reste séparé des includes STM32 HAL/CMSIS.

## Écart ARM découvert et corrigé

Le cross-build P12-A a révélé dans `src/modbus/register_model.c` des comparaisons de bornes sur enums rejetées par GCC ARM avec `-Werror=type-limits`.

La correction conserve la règle métier et remplace les comparaisons dépendantes de la représentation de l'enum par une validation exhaustive des valeurs autorisées. Aucun mapping B0 à B7 n'a été modifié.

## Hors périmètre / non prouvé

P12-A ne prouve pas :

- l'exécution sur NUCLEO-U575ZI-Q ;
- le démarrage matériel ;
- LPUART1 en fonctionnement réel ;
- PG7/PG8 en fonctionnement réel ;
- le protocole Modbus RTU physique ;
- le timing inter-caractères ou T3.5 ;
- le pilotage DE ;
- la stratégie `/RE` ;
- le fonctionnement de l'ADM2587E ;
- la liaison avec la supervision S7 sur un bus RS-485 réel.

Ces points restent explicitement **HARDWARE PENDING** ou appartiennent aux tranches P12 suivantes.

## Invariants conservés

- le mapping Modbus V1 B0 à B7 reste inchangé ;
- les politiques B5 transactionnelles restent inchangées ;
- la persistance et le recovery restent inchangés ;
- aucune propriété électrique RS-485 n'est inventée ;
- aucune dépendance HAL/CMSIS n'est ajoutée dans `tr2_core` ;
- l'architecture reste :

```text
tr2_core -> include/tr2/platform -> platform/stm32 -> STM32 HAL/LL/CMSIS/BSP
```

## Suite autorisée

La tranche suivante peut être engagée :

**P12-B — socle Modbus RTU portable : CRC16 + ADU bornée.**

P12-B doit rester indépendante du HAL, de l'UART, du GPIO DE et du matériel RS-485, et être validable intégralement sur Host avant cross-build.
