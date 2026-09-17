# P12-H3a — Audit de composition STM32 des dépendances SystemRuntime

## 1. Objet

Cette tranche prépare le branchement du serveur Modbus RTU portable P12-H dans le runtime STM32 sans introduire de faux services plateforme.

Référence de travail : branche `main`.

P12-H1/H2 a validé la chaîne portable :

```text
SerialTransport -> ModbusRtuReceiver -> RTU ADU/CRC -> addressing
                -> ModbusPduServer -> adapters B0..B7 -> RTU response
```

Le `main.c` STM32 courant ne compose encore que `SerialTransport`. Le serveur P12-H exige un `ModbusPduServerContext` adossé aux autorités applicatives, et celles-ci dépendent de `SystemRuntime`.

## 2. Dépendances obligatoires de SystemRuntime

`system_runtime_init()` refuse une composition qui ne fournit pas les six contrats suivants avec leurs opérations obligatoires :

| Dépendance | État STM32 courant | Qualification avant matériel |
|---|---|---|
| `MonotonicClock` | aucun adapter STM32 | implémentable et cross-buildable ; validation temporelle réelle reste hardware |
| `WallClock` | aucun adapter STM32 | nécessite une politique/implémentation d'horloge civile ; validation réelle hardware |
| `ResetCauseProvider` | aucun adapter STM32 | implémentable depuis les causes reset STM32 ; cross-build possible, validation des causes réelle hardware |
| `TimeContinuityEvidenceProvider` | aucun adapter STM32 | politique de preuve de continuité non définie côté STM32 ; arbitrage requis |
| `PersistentMedia` | aucun adapter STM32 | BLOQUANT pour un boot SystemRuntime complet ; backend de persistance production à arbitrer |
| `VibrationSource` | aucun adapter STM32 | backend capteur réel non composé ; interface peut être cross-buildée, validation fonctionnelle hardware |

`ConfigurationValidationEnvironment` est également requis par `SystemRuntimeDependencies`, mais il s'agit de données de validation et non d'un driver STM32. Ses valeurs de production doivent être explicitement choisies avant composition.

`selftest_executor` et `reset_trigger` existent dans `SystemRuntimeDependencies` mais ne font pas partie du prédicat minimal `dependencies_are_valid()`. Ils ne doivent pas être inventés pour H3a.

## 3. Contrainte de persistance découverte

Le layout actuel du runtime réserve notamment :

- configuration : 2 x 140 = 280 octets ;
- historique temps : 18 octets ;
- repository campagnes : 4 072 octets ;
- données campagnes : 11 552 octets ;
- journal commandes : 65 535 x 2 x 66 = **8 650 620 octets** ;
- historique diagnostic + self-test : 40 octets ;
- boot intent : 16 octets.

L'espace logique cumulé atteint donc **8 666 598 octets** avant toute marge d'implémentation du média.

Le linker STM32 courant déclare :

```text
FLASH = 2048K
RAM   = 768K
```

Conclusion : le layout de persistance V1 courant ne peut pas être mappé intégralement dans les 2 MiB de FLASH déclarés par le firmware STM32. H3 ne doit donc pas créer un faux `PersistentMedia` RAM/FLASH uniquement pour rendre `SystemRuntime` bootable.

Cette contrainte provient principalement de `TR2_COMMAND_JOURNAL_STORE_MAX_TRANSACTION_ID = UINT16_MAX` avec deux enregistrements redondants de 66 octets par transaction.

## 4. Ce qui peut être poursuivi sans NUCLEO

Les tranches suivantes restent légitimes sans matériel :

1. définir un package STM32 de contrats plateforme, sans dépendance inverse du core vers HAL ;
2. implémenter/cross-builder le `MonotonicClock` STM32 ;
3. implémenter/cross-builder le `ResetCauseProvider` STM32 ;
4. arbitrer la stratégie `WallClock` et `TimeContinuityEvidenceProvider` ;
5. arbitrer le backend `PersistentMedia` et sa capacité minimale ;
6. préparer le contrat `VibrationSource` STM32 sans prétendre valider le capteur ;
7. construire ensuite la composition `SystemRuntime -> ModbusPduServerContext -> ModbusRtuServerRuntime`.

## 5. Ce qui reste matériel

Restent explicitement HARDWARE PENDING :

- précision et continuité du temps monotone ;
- comportement RTC / horloge civile ;
- qualification réelle des causes reset ;
- preuve de continuité temporelle après reset/power loss ;
- persistance réelle, atomicité et endurance du média retenu ;
- acquisition du capteur de vibration ;
- LPUART1/PG7/PG8 et timing RTU instrumenté ;
- DE, /RE, ADM2587E et bus RS-485 ;
- intégration PC supervision S7 bout en bout.

## 6. Décision H3a

**Ne pas brancher directement P12-H dans `main.c` tant que `SystemRuntime` n'a pas de composition STM32 honnête.**

La prochaine tranche recommandée est **P12-H3b — socle plateforme STM32 minimal**, limitée aux dépendances que l'on peut implémenter et cross-builder sans matériel, en commençant par :

- `MonotonicClock` ;
- `ResetCauseProvider`.

La persistance fait l'objet d'un arbitrage séparé avant tout boot complet du `SystemRuntime`.

## 7. Invariants préservés

- aucun HAL/CMSIS dans le core portable ;
- aucun stub production silencieux ;
- aucun changement B0..B7 ;
- aucune modification des sémantiques transactionnelles B5 ;
- aucun retry automatique ;
- aucune validation hardware revendiquée ;
- P12-H1/H2 reste portable et testable indépendamment du STM32.
