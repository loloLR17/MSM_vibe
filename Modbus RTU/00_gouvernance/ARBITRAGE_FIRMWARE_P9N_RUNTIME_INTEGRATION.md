# Projet MSM — Capteur de vibration TR2

## P9-N — Cadrage d'intégration runtime des commandes B5 restantes

## 1. Statut

Ce document cadre l'intégration runtime P9-N à partir de l'état réel de `main` après validation de P9-L.

Il ne modifie pas la spécification Modbus RTU V1 et ne transforme aucune zone `NOT_DEFINED V1` en exigence normative.

Baseline d'entrée :

- `ed02deff101f4d085d0eb88c0e9dd659f0686d08`
- validation locale utilisateur : `66/66` tests réussis.

## 2. Constat d'intégration

Les tranches P9-B à P9-K ont construit les autorités et comportements unitaires nécessaires aux commandes 5 à 10, mais le `SystemRuntime` courant reste essentiellement celui gelé à P8.

En particulier, le runtime courant ne compose pas encore explicitement :

- `DiagnosticService` ;
- `DiagnosticHistoryStore` ;
- `SelfTestService` ;
- `MaintenanceService` ;
- `SystemStateAggregator` ;
- `BootIntentStore` ;
- `PlatformResetTrigger`.

Le chemin runtime existant raccorde explicitement les commandes d'acquisition 3/4, mais il n'existe pas encore de dispatch runtime transversal pour les commandes 5 à 10.

La projection Modbus courante expose B0 à B6. La projection B7 diagnostic n'est pas encore implémentée dans le firmware courant.

B1 dispose déjà d'un codec/projection depuis `SystemStateSnapshot`, mais le runtime P8 ne compose pas encore l'agrégateur P9 comme autorité de reconstruction B1.

## 3. Invariant de P9-N

P9-N ne doit pas créer un second moteur de commandes.

Le chemin reste :

```text
B5 mailbox
→ CommandRequest immutable
→ CommandEngine admission
→ autorité métier P9 concernée
→ CommandJournal
→ CommandSnapshot
→ B5 projection
```

Les services P9 restent les autorités métier ; le runtime ne fait que les composer et les orchestrer.

## 4. Commandes raccordables

Les commandes raccordables à P9-N sont :

- 5 `SELFTEST` ;
- 6 `ACKNOWLEDGE_FAULT` ;
- 7 `REFRESH_INDICATORS` ;
- 8 `ENTER_MAINTENANCE` ;
- 9 `EXIT_MAINTENANCE` ;
- 10 `SOFTWARE_RESET`.

La commande 11 `RESET_STATISTICS` reste explicitement non raccordée conformément à `ARBITRAGE_FIRMWARE_P9L_STATISTICS_SCOPE.md`.

Un code de commande connu mais sans autorité métier raccordable ne doit jamais être transformé en succès no-op.

## 5. Découpage P9-N

Afin d'éviter un raccordement big-bang, P9-N est découpée en sous-tranches :

### P9-N1 — composition des autorités P9

Composer dans `SystemRuntime` les autorités diagnostique, maintenance, selftest et reset nécessaires, avec recovery des faits durables existants.

Aucune nouvelle sémantique métier.

### P9-N2 — dispatch runtime commandes 6/8/9

Raccorder :

- `ACKNOWLEDGE_FAULT` ;
- `ENTER_MAINTENANCE` ;
- `EXIT_MAINTENANCE`.

Ces commandes n'exigent pas de nouvelle persistance B5.

### P9-N3 — `REFRESH_INDICATORS` + B1

Construire le `SystemStateRefreshSource` depuis les autorités runtime réellement disponibles, puis reconstruire B1 via `SystemStateAggregator`.

Interdiction de fabriquer des données d'acquisition ou de reconstruire B3 depuis l'historique bulk.

### P9-N4 — `SELFTEST`

Raccorder la commande 5 au `SelfTestService` et à un `SelfTestExecutor` explicitement fourni par la plateforme/runtime.

Aucun faux autotest toujours PASS.

### P9-N5 — `SOFTWARE_RESET`

Raccorder `BootIntentStore`, `PlatformResetTrigger` et le recovery reset au boot.

Préserver l'ordre durable :

```text
RESERVED
→ recovery context durable
→ BootIntent durable
→ STARTED durable
→ reset HAL
```

### P9-N6 — projection B7 et cohérence B1/B5/B7

Ajouter la projection B7 depuis les autorités diagnostiques gelées, puis vérifier la cohérence transverse des projections.

## 6. Politique B5 maintenance

`maintenance_active` ne peut être projeté qu'à partir de `MaintenanceService`.

Aucun booléen runtime indépendant ne peut devenir une seconde autorité du mode maintenance.

## 7. Recovery

Le boot P9-N doit respecter :

- aucune reprise automatique de SELFTEST ;
- aucune reprise automatique d'acquisition ;
- aucune restauration d'un mode maintenance volatile ;
- last fault et dernier selftest terminal restaurés uniquement depuis `DiagnosticHistoryStore` ;
- `BootIntent` utilisé uniquement comme preuve corrélée et jamais comme remplacement d'une cause reset matérielle contradictoire ;
- transaction `STARTED` sans preuve causale suffisante → `INDETERMINATE`.

## 8. Commande 11

`RESET_STATISTICS` reste hors implémentation P9-N.

P9-N ne crée ni `StatisticsService`, ni `StatisticsStore`, ni succès no-op pour satisfaire artificiellement la présence normative du code 11.

## 9. Critère de clôture

P9-N ne sera considérée terminée qu'après :

- composition runtime réelle des autorités 5 à 10 ;
- tests d'intégration du dispatch ;
- reconstruction cohérente B1/B5/B7 ;
- tests boot/recovery associés ;
- maintien des invariants P7/P8 ;
- validation canonique locale par `./tr2_validate.sh`.
