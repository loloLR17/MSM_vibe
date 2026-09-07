# Projet MSM — Capteur de vibration TR2

## P9-N4a — Arbitrage du périmètre SELFTEST standard V1

## 1. Statut

Ce document arbitre le contenu exécutable du `SELFTEST` standard avant son raccordement au `SystemRuntime`.

Il ne modifie pas la spécification Modbus RTU V1. Il distingue explicitement ce qui est normatif, ce qui relève de l'architecture gelée et ce qui reste `NOT_DEFINED V1`.

Baseline d'entrée :

- `428f295df946c61d1f044ed6647b98e2357becdf` — `Firmware: wire P9-N3 refresh indicators and B1` ;
- validation locale utilisateur : `66/66` tests réussis.

## 2. Exigences V1 existantes

La V1 définit :

- la commande B5 code `5` : lancer autotest ;
- `cmd_result_code = 11` : échec autotest ;
- `cmd_result_detail` peut porter un détail spécifique, par exemple l'identifiant d'un sous-test en échec ;
- B7 expose `selftest_status`, `selftest_result_code` et `selftest_detail` ;
- `selftest_status` vaut `0` jamais exécuté, `1` en cours, `2` OK, `3` échec.

La V1 ne définit pas :

- un catalogue normatif de sous-tests ;
- les codes normatifs de `selftest_result_code` ;
- les codes normatifs de `selftest_detail` ;
- une séquence exhaustive d'essais matériels ;
- l'obligation de démarrer une acquisition ou d'écrire des données de campagne pendant l'autotest.

L'extension autotest multi-niveaux est explicitement une réserve future de B7.

## 3. Architecture et firmware déjà gelés

P9-G/P9-H ont déjà gelé les invariants suivants :

- `SelfTestService` est l'autorité runtime du cycle de vie autotest ;
- `RUNNING` est volatile et n'est jamais persisté ;
- seul un résultat terminal complètement achevé `PASSED` ou `FAILED` est durable ;
- un autotest interrompu par reboot n'est pas transformé en échec ;
- aucun replay automatique de SELFTEST au boot ;
- le mode V1 actuellement supporté est le mode standard `param1=param2=param3=0` ;
- `SelfTestExecutor` est le seam d'exécution fourni par la plateforme/runtime ;
- aucun faux autotest toujours PASS n'est autorisé.

## 4. Inventaire des autorités réellement disponibles

### 4.1 Stockage persistant

Le runtime possède des stores et repositories déjà initialisés/recoverés : configuration, historique temps, journal de commandes, campagnes, données de campagne, historique diagnostic et BootIntent.

Leur recovery constitue une preuve de disponibilité/intégrité des autorités persistantes correspondantes.

Un SELFTEST ne doit toutefois pas écrire, effacer ou altérer ces autorités uniquement pour se tester.

### 4.2 Configuration

`ConfigurationService` sait indiquer si une `ActiveConfigurationSnapshot` valide existe.

L'absence d'une configuration active est un état métier V1 valide et ne constitue donc pas, à elle seule, un échec d'autotest matériel.

### 4.3 Chaîne vibration

`VibrationSource` fournit les opérations `configure/start/read/stop` utilisées par l'acquisition réelle.

Mais la V1 ne définit pas un mode de diagnostic non destructif du MEMS, et l'architecture interdit de fabriquer implicitement une acquisition/campagne.

Un SELFTEST standard ne doit donc pas appeler `start/read/stop` sur la chaîne vibration tant qu'un contrat HAL de diagnostic explicitement non destructif n'existe pas.

### 4.4 Temps

`TimeService`, `WallClock` et l'historique de synchronisation existent, mais une heure non synchronisée ou une continuité non prouvée sont des états métier possibles et ne constituent pas automatiquement une panne matérielle.

Le SELFTEST ne doit pas transformer l'état de synchronisation courant en verdict PASS/FAIL inventé.

### 4.5 Diagnostic

`DiagnosticService` expose les conditions diagnostiques courantes, mais les flags V1 ne constituent pas un catalogue normatif des sous-tests SELFTEST.

Le SELFTEST ne doit pas simplement recopier `system_health_status` pour se déclarer réussi ou échoué : cela transformerait une projection d'état en procédure d'essai sans base normative.

## 5. Arbitrage P9-N4a

### 5.1 Périmètre standard V1 exécutable aujourd'hui

Avec les contrats HAL actuellement présents, il n'existe **aucun test actif matériel non destructif suffisamment défini** pour constituer à lui seul un SELFTEST standard réel.

En particulier sont interdits :

- `return PASS` inconditionnel ;
- démarrer artificiellement une acquisition vibration ;
- créer/fermer une campagne de test ;
- effectuer une écriture destructive de stockage ;
- déclarer FAIL uniquement parce qu'aucune configuration active n'existe ;
- déclarer FAIL uniquement parce que l'heure n'est pas synchronisée ;
- inventer un catalogue de sous-tests ou de codes résultat/detail.

### 5.2 Contrat nécessaire

Le raccordement P9-N4b doit introduire un **contrat plateforme explicite de SELFTEST standard**, distinct des opérations métier normales.

Ce contrat doit :

- être non destructif vis-à-vis des campagnes, configuration, identité, journal de commandes et historique diagnostic ;
- ne pas démarrer implicitement l'acquisition métier ;
- retourner explicitement `passed`, `result_code` et `detail` via le `SelfTestExecutor` déjà gelé ;
- laisser `result_code/detail` opaques tant qu'aucun catalogue protocolaire n'est défini ;
- permettre à une plateforme qui ne possède pas encore d'implémentation matérielle réelle de retourner `TR2_ERROR_NOT_AVAILABLE`, jamais un PASS synthétique.

### 5.3 Plateforme host

La plateforme host peut fournir une implémentation déterministe **injectable pour les tests d'intégration**, mais cette implémentation est un test double et ne constitue jamais la définition métier du SELFTEST embarqué.

Le comportement par défaut d'une plateforme sans executor SELFTEST réel est :

```text
SELFTEST demandé
→ executor absent/non disponible
→ aucun résultat terminal inventé
→ pas de PASS synthétique
```

La manière exacte de convertir une indisponibilité technique après la barrière `STARTED` reste celle déjà gelée en P9-H : la transaction reste conservatrice et ne publie pas de faux résultat terminal.

## 6. Conséquence pour P9-N4b

P9-N4b peut maintenant raccorder la commande 5 au runtime à condition de :

1. ajouter `SelfTestExecutor` aux dépendances explicites de `SystemRuntime` ;
2. ne jamais construire un executor PASS par défaut dans `SystemRuntime` ;
3. faire fournir l'executor réel par la plateforme ;
4. utiliser un test double host injecté uniquement dans les tests ;
5. conserver l'idempotence P7 et le recovery P9-H ;
6. ne pas encore inventer de projection B7 supplémentaire — celle-ci reste P9-N6.

## 7. Classification

| Sujet | Classification |
|---|---|
| commande 5 SELFTEST | V1 normative |
| états B7 selftest 0..3 | V1 normative |
| SelfTestService / persistance du dernier terminal | architecture/FW_POLICY gelée |
| SelfTestExecutor | architecture d'implémentation gelée P9-H |
| contenu exhaustif du SELFTEST matériel | NOT_DEFINED V1 |
| catalogue result_code/detail | NOT_DEFINED V1 |
| faux PASS par défaut | interdit |
| acquisition/campagne artificielle pendant SELFTEST | interdit sans nouveau contrat explicite |
| executor plateforme injecté | IMPLEMENTATION |

## 8. Critère de clôture

P9-N4a est close lorsque cet arbitrage est validé par la validation canonique du dépôt.

La tranche suivante est P9-N4b : raccordement runtime de SELFTEST par dépendance `SelfTestExecutor` explicite, sans faux autotest et sans nouvelle sémantique métier.
