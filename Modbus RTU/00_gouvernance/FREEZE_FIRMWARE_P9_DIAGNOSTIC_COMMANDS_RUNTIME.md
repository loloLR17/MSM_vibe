# Projet MSM — Capteur de vibration TR2

## Gel firmware P9 — Commandes diagnostic, maintenance, SELFTEST, reset et cohérence B1/B5/B7

## 1. Statut

La tranche firmware **P9 — commandes B5 restantes raccordables et intégration runtime diagnostic** est considérée comme implémentée et validée sur la branche `main`.

Baseline de validation fonctionnelle avant le présent gel :

- commit : `05beff4b23889dc21c5dc406c58fb576ad8af309` ;
- commit : `Firmware: wire P9-N6c B7 read path` ;
- validation locale utilisateur : `./tr2_validate.sh` ;
- résultat : **66/66 tests réussis** ;
- runtime Host : `TR2 P0 host runtime ready` ;
- validation canonique : `TR2 validation complete.`

Le présent document est un constat de gel d'implémentation. Il ne modifie pas la spécification Modbus RTU V1, ne remplace pas les documents d'architecture gelés et ne transforme aucune zone `NOT_DEFINED V1`, décision `FW_POLICY` ou extension V1.1 en exigence normative V1.

---

## 2. Références

P9 s'appuie notamment sur :

- `FREEZE_FIRMWARE_P7_COMMAND_ENGINE_B5.md` ;
- `FREEZE_FIRMWARE_P8_FG_RUNTIME_INTEGRATION.md` ;
- `ARCHITECTURE_FIRMWARE_SERVICES_MODBUS_V1.md` ;
- `ARCHITECTURE_FIRMWARE_BOOT_PERSISTENCE_RECOVERY.md` ;
- `RECOVERY_FAULT_INJECTION_MATRIX.md` ;
- `ARBITRAGE_FIRMWARE_P9L_STATISTICS_SCOPE.md` ;
- `ARBITRAGE_FIRMWARE_P9N_RUNTIME_INTEGRATION.md` ;
- `ARBITRAGE_FIRMWARE_P9N4_SELFTEST_STANDARD_SCOPE.md` ;
- `ARBITRAGE_FIRMWARE_P9N5B_BOOT_INTENT_CONSUMPTION.md` ;
- la baseline normative Modbus RTU V1, notamment B1, B5 et B7.

Les invariants transactionnels P7 et les invariants acquisition/campagne P8 restent applicables intégralement.

---

## 3. Périmètre P9 gelé

P9 raccorde réellement les commandes B5 suivantes :

- 5 `SELFTEST` ;
- 6 `ACKNOWLEDGE_FAULT` ;
- 7 `REFRESH_INDICATORS` ;
- 8 `ENTER_MAINTENANCE` ;
- 9 `EXIT_MAINTENANCE` ;
- 10 `SOFTWARE_RESET`.

Avec P7/P8, les commandes réellement raccordées deviennent donc :

```text
1  APPLY_CONFIG
2  SYNC_TIME
3  START_ACQUISITION
4  STOP_ACQUISITION
5  SELFTEST
6  ACKNOWLEDGE_FAULT
7  REFRESH_INDICATORS
8  ENTER_MAINTENANCE
9  EXIT_MAINTENANCE
10 SOFTWARE_RESET
```

La commande 11 `RESET_STATISTICS` reste volontairement **non raccordée**, conformément à P9-L. Elle ne reçoit aucun succès no-op et aucune autorité statistique fictive n'est créée.

---

## 4. Autorités métier et chaîne runtime

P9 compose dans `SystemRuntime` les autorités nécessaires :

```text
DiagnosticHistoryStore
        ↓
DiagnosticService
   ├── SelfTestService
   └── SystemStateAggregator

MaintenanceService

BootIntentStore
        ↓
PlatformResetTrigger
```

Le runtime ne devient pas une seconde autorité métier.

Le chemin B5 reste :

```text
B5 mailbox
→ CommandRequest immutable
→ CommandEngine admission
→ autorité métier concernée
→ CommandJournal
→ CommandSnapshot
→ B5 projection
```

`CommandJournal` reste uniquement l'autorité transactionnelle/idempotence. Il ne devient jamais l'autorité de l'état diagnostique, maintenance, SELFTEST, reset ou campagne.

---

## 5. Diagnostic et historique

`DiagnosticService` est l'autorité runtime des faits diagnostiques.

`DiagnosticHistoryStore` conserve uniquement les faits durables explicitement définis par l'architecture/P9 :

- dernier défaut historique et son timestamp lorsque disponible ;
- dernier résultat SELFTEST terminal complètement achevé.

Les conditions actives sont réévaluées au boot ; une ancienne projection B7 n'est jamais restaurée comme autorité.

L'état d'acquittement des défauts est une politique runtime volatile P9. Une disparition puis réapparition d'un défaut ne réutilise pas silencieusement un acquittement antérieur.

---

## 6. SELFTEST

La commande 5 respecte le cycle transactionnel P7 :

```text
RESERVED durable
→ STARTED durable
→ SelfTestService RUNNING
→ SelfTestExecutor plateforme
→ résultat terminal durable
→ COMPLETED B5
```

Règles gelées :

- seul le SELFTEST standard V1 sans catalogue inventé est raccordé ;
- aucun sous-test non défini n'est inventé ;
- aucun faux SELFTEST toujours PASS ;
- la plateforme doit fournir explicitement un `SelfTestExecutor` ;
- absence d'executor → indisponibilité technique avant consommation transactionnelle ;
- RUNNING est volatile ;
- seuls PASSED/FAILED terminaux sont durables ;
- une interruption par reboot n'est pas transformée automatiquement en échec ;
- aucun replay SELFTEST au boot ;
- result/detail restent opaques tant qu'aucun catalogue normatif n'existe.

---

## 7. Acquittement des défauts

`ACKNOWLEDGE_FAULT` modifie uniquement l'état d'acquittement d'un défaut acquittable ; il ne supprime jamais une cause encore active.

P9 gèle deux formes d'appel conformes au handler implémenté :

- acquittement unitaire ;
- acquittement global des défauts actuellement acquittables.

Les paramètres invalides, cibles absentes et défauts non acquittables sont refusés avant effet métier significatif.

Aucune persistance additionnelle B5 n'est introduite pour cette commande.

---

## 8. Maintenance

`MaintenanceService` est l'unique autorité du mode maintenance.

Politique P9 gelée :

- état NORMAL au boot ;
- maintenance volatile par boot ;
- entrée refusée si acquisition active ;
- entrée déjà active refusée ;
- sortie depuis NORMAL refusée ;
- aucune persistance ou auto-restauration du mode maintenance.

Le flag B5 `maintenance_active` est projeté exclusivement depuis `MaintenanceService`. Aucun booléen runtime indépendant ne constitue une seconde autorité.

---

## 9. REFRESH_INDICATORS et état système

La commande 7 déclenche une reconstruction depuis les autorités runtime réellement disponibles.

Chaîne :

```text
autorités runtime courantes
→ DiagnosticService
→ SystemStateAggregator
→ DiagnosticSnapshot + SystemStateSnapshot
→ B1 + B7
```

Règles :

- aucune acquisition artificielle ;
- aucune reconstruction B3 depuis les données bulk ou l'historique ;
- aucun producteur CPU/mémoire/température/tension inventé ;
- les valeurs sans producteur réel restent neutres/non disponibles selon leur contrat ;
- RETRY transactionnel ne redispatche pas le refresh métier.

---

## 10. SOFTWARE_RESET, BootIntent et recovery

La commande 10 conserve la barrière transactionnelle suivante :

```text
RESERVED durable
→ CommandRecoveryContext durable
→ BootIntent SOFTWARE_RESET(txid) durable
→ STARTED durable
→ PlatformResetTrigger.software_reset()
```

L'effet métier significatif est le déclenchement réel du reset plateforme, pas la simple écriture du `BootIntent`.

Le `BootIntent` est une preuve préparatoire corrélée, jamais une autorité de cause reset.

Un succès causal après reboot exige les preuves compatibles nécessaires, notamment :

- transaction STARTED ;
- recovery context correspondant ;
- BootIntent valide et correspondant ;
- cause reset matérielle normalisée compatible SOFTWARE.

Un `BootIntent SOFTWARE_RESET` suivi d'un POWER_ON, BROWNOUT, WATCHDOG ou autre cause contradictoire ne transforme jamais cette cause en SOFTWARE_RESET.

En cas de preuve insuffisante, la transaction reste `INDETERMINATE` et n'est jamais rejouée automatiquement.

---

## 11. Consommation one-shot du BootIntent

Après exploitation du BootIntent pendant le recovery :

```text
capture cause reset
→ recover BootIntent
→ recover CommandJournal
→ calcul reconciliation
→ restore incomplete state
→ clear durable du BootIntent VALID
→ READY Modbus
```

Règles gelées :

- seul un BootIntent `VALID` est consommé ;
- le verdict déjà calculé n'est pas modifié par sa consommation ;
- l'échec du clear durable fait échouer le boot de manière conservative ;
- le système ne devient pas Modbus-ready tant que cette consommation requise n'est pas prouvée ;
- après clear réussi, un boot ultérieur ne peut pas réutiliser l'ancien BootIntent comme preuve.

La fault injection P9 valide explicitement cette propriété one-shot.

---

## 12. Projection B7

P9 implémente la projection Modbus B7 normative sur `7000..7015`.

B7 est une projection de `DiagnosticSnapshot` complétée par :

- uptime issu du `MonotonicClock` ;
- cause reset normalisée issue du `BootContext`.

Les bits de défaut B7 sont mappés explicitement depuis les conditions diagnostiques définies. Les registres réservés restent à zéro.

Les mesures `internal_temp_dC` et `supply_voltage_mV` ne sont pas fabriquées en l'absence de producteurs réels.

Le read adapter sert B7 depuis l'image runtime déjà construite, comme B5/B6, sans créer une nouvelle autorité ni effectuer une reprojection opportuniste lors de la lecture Modbus.

---

## 13. Cohérence B1 / B5 / B7

P9 gèle les règles transversales suivantes :

- B1 et B7 sont construits au boot avant passage Modbus-ready ;
- B1 et B7 utilisent le même uptime runtime et la même cause reset normalisée ;
- les faits diagnostiques de B1/B7 proviennent des mêmes autorités runtime ;
- SELFTEST met à jour l'état diagnostique puis les projections B1/B7 ;
- REFRESH_INDICATORS reconstruit puis projette B1/B7 de manière cohérente ;
- B5 reste la projection du moteur transactionnel et de ses flags ;
- `maintenance_active` B5 provient uniquement de `MaintenanceService` ;
- B1 ne reçoit aucun bit maintenance inventé ;
- B3 reste indépendant et n'est jamais reconstruit depuis l'historique.

Le mapping de cause reset est explicite afin de ne pas dépendre de l'ordre interne des enums plateforme. Les valeurs B1/B7 restent celles définies par la V1.

---

## 14. Boot P9 gelé

L'ordre fonctionnel pertinent devient :

```text
persistent storage
→ capture / normalisation reset cause
→ recover Time
→ recover Configuration
→ recover CampaignRepository / CampaignDataStore
→ compose F/G runtime
→ compose autorités P9
→ recover BootIntent
→ recover CommandJournal et reconciliation
→ consume BootIntent VALID après exploitation
→ rebuild B4/B5/B6
→ build B1/B7 depuis autorités courantes
→ READY Modbus
```

Le boot ne :

- reprend pas automatiquement l'acquisition ;
- reprend pas automatiquement un SELFTEST ;
- restaure pas le mode maintenance ;
- rejoue pas une commande STARTED ;
- restaure pas B3 depuis la NVM ;
- restaure pas une ancienne projection B7 comme autorité.

---

## 15. RESET_STATISTICS reste non raccordé

L'arbitrage P9-L reste applicable intégralement.

Le périmètre statistique métier implémentable reste :

```text
EMPTY
```

Par conséquent P9 ne crée :

- ni `StatisticsService` factice ;
- ni `StatisticsStore` vide ;
- ni succès no-op pour la commande 11 ;
- ni remise à zéro de l'uptime, des générations, des campagnes, du diagnostic, du journal transactionnel ou de compteurs HAL/test.

Une future implémentation de la commande 11 exige d'abord un arbitrage explicite définissant une véritable autorité statistique et son périmètre.

---

## 16. Fault injection et non-régression

P9 ajoute ou consolide notamment les scénarios suivants :

- recovery de l'historique diagnostic ;
- SELFTEST terminal durable et interruption non rejouée ;
- SOFTWARE_RESET avec barrières avant reset ;
- BootIntent absent, corrompu ou incompatible ;
- cause reset contradictoire ;
- BootIntent stale avec journal RESERVED ;
- consommation one-shot du BootIntent ;
- échec du clear BootIntent empêchant READY ;
- boot B1/B7 ;
- lecture Modbus B7 complète, partielle, indisponible et hors plage ;
- non-redispatch sur RETRY des commandes P9.

Les invariants P7/P8 restent couverts par la validation canonique complète.

---

## 17. Validation

La baseline immédiatement avant le présent gel a été validée localement par l'utilisateur :

```text
100% tests passed, 0 tests failed out of 66
TR2 P0 host runtime ready
TR2 validation complete.
```

Cette validation couvre P1 à P9 et constitue la baseline fonctionnelle de clôture de P9 avant le commit documentaire de gel.

---

## 18. Points volontairement ouverts

P9 ne définit pas silencieusement :

- un catalogue de sous-tests SELFTEST ;
- un catalogue normatif supplémentaire de `selftest_result_code` / `selftest_detail` ;
- de nouveaux producteurs de température interne ou tension d'alimentation ;
- une persistance du mode maintenance ;
- une persistance de l'acquittement des défauts ;
- un périmètre de statistiques de service ;
- une commande 11 exécutable sans autorité statistique réelle ;
- de nouvelles commandes B5 ;
- les extensions transactionnelles V1.1 au-delà des décisions déjà séparément gelées ;
- un replay automatique d'une commande après reboot.

Toute évolution sur ces points exige une tranche ou un arbitrage explicite.

---

## 19. Décision de gel

À compter de ce gel :

- P9 constitue la baseline firmware validée des commandes diagnostic/maintenance/reset raccordables de la V1 ;
- les commandes B5 1 à 10 disposent d'un chemin métier réel selon leur périmètre gelé ;
- la commande 11 reste volontairement non raccordée pour absence d'autorité statistique définie ;
- `DiagnosticService`, `SelfTestService`, `MaintenanceService`, `SystemStateAggregator`, `DiagnosticHistoryStore` et `BootIntentStore` conservent leurs responsabilités séparées ;
- B1/B5/B7 restent des projections et ne deviennent jamais des autorités métier ;
- `CommandJournal` reste une autorité transactionnelle uniquement ;
- BootIntent reste une preuve corrélée one-shot et ne remplace jamais la cause reset matérielle ;
- les zones `NOT_DEFINED V1`, `FW_POLICY` et V1.1 restent explicitement séparées de la baseline normative V1.

La prochaine tranche firmware devra être définie à partir de cette baseline gelée et de l'état réel de `main`.
