# P12-H3f — Audit de préparation à la composition complète SystemRuntime STM32

## 1. Objet

Réévaluer, après les gels H3d3 (persistance FRAM) et H3e (WallClock / continuité RTC), les dépendances réelles qui séparent encore le firmware STM32 d'une composition honnête de `SystemRuntime`.

Référence auditée : branche `main` au HEAD `bbed850ddfa538890b67332afca8876078693eae`.

## 2. Évolution depuis l'audit H3a

L'audit H3a était correct à sa date mais deux blocages majeurs qu'il décrivait ont depuis été levés :

- la persistance logique a été ramenée à `TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE = 51818` octets et portée par le média transactionnel H3d2 sur la FRAM physique qualifiée H3d3 ;
- `WallClock` et `TimeContinuityEvidenceProvider` disposent maintenant d'adapters STM32 RTC/LSE qualifiés au niveau A par H3e.

Le constat historique H3a d'un layout de plusieurs Mio n'est donc plus représentatif de la baseline actuelle.

## 3. Dépendances SystemRuntime sur la baseline actuelle

`dependencies_are_valid()` exige :

| Dépendance | État courant |
|---|---|
| `MonotonicClock` | adapter STM32 présent, H3b |
| `WallClock` | adapter STM32 présent, H3e |
| `ResetCauseProvider` | adapter STM32 présent, H3b |
| `TimeContinuityEvidenceProvider` | adapter STM32 présent, H3e |
| `PersistentMedia` | interface portable fournie par `TransactionalImageMedia`; backend FRAM STM32 physiquement qualifié |
| `ConfigurationValidationEnvironment` | données de composition à fixer explicitement |
| `VibrationSource` | **aucun adapter STM32 réel présent** |

`selftest_executor` et `reset_trigger` restent optionnels vis-à-vis du prédicat minimal de `system_runtime_init()` et ne doivent pas être inventés pour franchir ce jalon.

## 4. Persistance : composition désormais possible

La chaîne existante permet une composition sans nouveau protocole de stockage :

```text
Stm32FramStorage
    -> TransactionalImagePhysicalStorage
    -> TransactionalImageMedia
    -> PersistentMedia
    -> SystemRuntimeDependencies.persistent_media
```

La géométrie de qualification H3d2/H3d3 fournit exactement 51 818 octets logiques, soit la capacité attendue par la baseline transactionnelle gelée.

La composition doit réutiliser cette chaîne. Elle ne doit ni créer un second format FRAM, ni contourner `TransactionalImageMedia`, ni reformater automatiquement un média non EMPTY.

La baseline physique FRAM à préserver reste :

`VALID / génération 10 / image B / payload[0] = 0xAC`.

## 5. Bloquant principal restant : VibrationSource

Le repository ne contient actuellement qu'un contrat portable `vibration_source.h` :

- `configure()` ;
- `start()` ;
- `read_sample()` ;
- `stop()`.

Aucun driver STM32/IIS3DWB n'est présent dans la baseline auditée.

Il serait contraire aux invariants du projet d'injecter un faux `VibrationSource` uniquement pour rendre `SystemRuntime` bootable.

Le prochain travail matériel légitime est donc l'adapter réel du capteur **IIS3DWB** utilisé sur le module STEVAL-MKI208V1K, avant composition finale de `SystemRuntime`.

## 6. ConfigurationValidationEnvironment

`SystemRuntimeDependencies` exige également un `ConfigurationValidationEnvironment`.

La structure actuelle contient :

- `storage_capacity_known` ;
- `usable_storage_capacity_mb`.

Ces valeurs doivent être fixées à partir de la capacité de stockage réellement destinée aux campagnes et des règles de validation de configuration. Elles ne doivent pas être déduites de la FRAM transactionnelle de 256 Kio si le stockage campagne de production repose sur un autre média.

L'arbitrage doit donc être fait avant la composition finale, en cohérence avec l'architecture de stockage campagne retenue.

## 7. Tranche suivante recommandée

La prochaine tranche est **P12-H3g — VibrationSource STM32 / IIS3DWB**, découpée de manière progressive :

1. audit officiel IIS3DWB + STEVAL-MKI208V1K et pinout NUCLEO ;
2. choix du bus et des broches sans conflit avec FRAM, UART/RS-485 et ressources déjà gelées ;
3. bring-up minimal et lecture identité capteur ;
4. configuration bornée correspondant au contrat `VibrationSourceConfiguration` ;
5. lecture physique d'échantillons et conversion documentée vers mg ;
6. qualification start/read/stop ;
7. adapter `VibrationSource` de production et gel.

Seulement ensuite : composition `SystemRuntime -> ModbusPduServerContext -> ModbusRtuServerRuntime` sur STM32.

## 8. Invariants

- aucune modification des sémantiques B0..B7 ;
- aucune réimplémentation H3d2 dans le driver STM32 ;
- aucune nouvelle campagne destructive FRAM ;
- aucune fausse source vibratoire ;
- aucun HAL/CMSIS dans le core portable ;
- pas de gel avant preuve matérielle requise ;
- documentation constructeur officielle prioritaire pour le câblage et les caractéristiques IIS3DWB.

## 9. Décision

La composition complète de `SystemRuntime` est désormais débloquée côté temps et persistance.

Le **seul driver plateforme obligatoire encore absent** est `VibrationSource`.

La suite doit donc traiter le capteur IIS3DWB réel avant de brancher le runtime complet dans `main.c`.
