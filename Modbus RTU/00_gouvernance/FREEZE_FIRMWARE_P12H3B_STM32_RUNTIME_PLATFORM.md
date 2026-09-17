# Freeze — P12-H3b — socle runtime plateforme STM32

## Statut

P12-H3b est gelé après validation complète locale :

- Host : VALIDATED.
- STM32 Cortex-M33 cross-build : VALIDATED.
- Hardware : PENDING.

## Contenu

La couche `platform/stm32` fournit désormais deux adapters vers les contrats portables :

- `MonotonicClock` basé sur `HAL_GetTick()` ;
- `ResetCauseProvider` basé sur les flags de reset RCC capturés au démarrage puis mémorisés avant effacement.

Aucun HAL/CMSIS n'est introduit dans le core portable.

## Limites de validation

Le cross-build prouve la cohérence de compilation et de linkage pour STM32U575. Il ne valide pas encore :

- exactitude temporelle réelle de `HAL_GetTick()` ;
- continuité et comportement lors des resets ;
- qualification réelle de chaque flag RCC sur la NUCLEO.

Ces points restent HARDWARE PENDING.

## Dépendances SystemRuntime encore non composées

- `WallClock` ;
- `TimeContinuityEvidenceProvider` ;
- `PersistentMedia` ;
- `VibrationSource` ;
- valeurs de production de `ConfigurationValidationEnvironment`.

La persistance reste un arbitrage architectural préalable au boot complet du `SystemRuntime`.

## Invariants

- pas de stub production silencieux ;
- pas de modification B0..B7 ;
- pas de modification B5 ;
- aucun retry automatique ajouté ;
- aucune validation hardware revendiquée.
