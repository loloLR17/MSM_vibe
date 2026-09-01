# Architecture firmware TR2 — Services, autorités métier et adaptation Modbus V1

## 1. Statut du document

Ce document formalise la passe d’architecture A→J validée après le gel Modbus RTU V1. Il complète `ARCHITECTURE_FIRMWARE_BOOT_PERSISTENCE_RECOVERY.md` et `RECOVERY_FAULT_INJECTION_MATRIX.md` sans modifier la spécification normative V1.

Références de lecture :

- **V1** : exigence normative existante ;
- **FW_POLICY** : politique firmware TR2 retenue ;
- **IMPLEMENTATION** : mécanisme technique volontairement non figé ;
- **NOT_DEFINED V1** : comportement que la V1 ne définit pas et qui ne doit pas être inventé.

Le principe directeur reste :

```text
L4 Modbus RTU Transport / Server
        ↓
L3 Register Model B0..B7
        ↓
L2 Modbus Adaptation Layer
        ↓
L1 TR2 Domain Services
        ↓
L0 HAL / Platform
```

Les services métier TR2 ne dépendent jamais de Modbus. B0…B7 sont des projections protocolaires et ne constituent jamais une autorité métier ni persistante.

---

## 2. A — Platform / Runtime

### 2.1 Responsabilités

- `MonotonicClock` : autorité unique des durées monotones runtime ; ne dépend jamais de `WallClock`.
- `ResetCauseProvider` : capture les faits matériels bruts avant toute initialisation susceptible de les altérer.
- `BootContext` : porte la cause de reset TR2 normalisée pour le boot courant et reste immuable pendant ce runtime.
- `SystemRuntime` : orchestre l’ordre logique de boot et les barrières de readiness sans posséder d’état métier.

`BootIntent` peut raffiner une cause compatible pour le boot attendu suivant ; son mécanisme physique reste `IMPLEMENTATION`.

### 2.2 Invariants

- **A-PLAT-01** — `MonotonicClock` est la seule autorité de durée monotone runtime.
- **A-PLAT-02** — l’uptime appartient au boot courant et n’est jamais restauré.
- **A-RESET-01** — les faits matériels de reset sont capturés avant altération.
- **A-RESET-02** — faits bruts, cause normalisée et encodage Modbus restent distincts.
- **A-RESET-03** — la cause normalisée du `BootContext` est immuable.
- **A-RESET-04** — une ambiguïté non résolue n’est jamais transformée en cause supposée.
- **A-RUN-01** — `SystemRuntime` orchestre sans devenir un god object.
- **A-RUN-02** — l’initialisation runtime ne déclenche aucune action métier implicite.
- **A-RUN-03** — `SYSTEM_READY_FOR_MODBUS` n’est atteint qu’après recovery et snapshots cohérents.
- **A-RUN-04** — disponibilité Modbus et acceptation de nouvelles commandes B5 sont deux capacités distinctes.

---

## 3. B — Persistance générique

### 3.1 Architecture

```text
Domain Services
    ↓
Domain Stores / Repositories
    ↓
PersistentStorageCore
    ↓
Storage Media Abstractions
    ↓
HAL / physical media
```

`PersistentStorageCore` fournit uniquement des mécanismes génériques : lecture, écriture, commit durable, intégrité technique, atomicité d’une unité persistante, état du média. Les Stores/Repositories connaissent les versions et la sémantique métier.

### 3.2 Règles centrales

- `commit SUCCESS` signifie qu’après coupure immédiate la représentation engagée appartient aux états techniquement récupérables autorisés, sous hypothèse d’intégrité physique du média.
- `commit SUCCESS` ne rend pas l’objet automatiquement autoritatif au niveau métier.
- `EMPTY`, `CORRUPTED`, `UNAVAILABLE` et `UNSUPPORTED` restent distincts.
- intégrité technique et validité métier restent distinctes.
- remplacement/GC : copy-before-retire ; jamais détruire la dernière copie exploitable avant commit sûr d’une remplaçante.
- aucune structure RAM native ne constitue implicitement un format NVM.

### 3.3 Invariants

- **B-STOR-01** — le Core ne contient aucune règle métier ou Modbus.
- **B-STOR-02** — aucun service/store métier n’accède directement au média physique.
- **B-STOR-03** — écriture terminée et commit durable sont distincts.
- **B-STOR-04** — une unité atomique récupère l’ancien état valide ou le nouveau complet, jamais un hybride.
- **B-STOR-05** — les quatre états `EMPTY/CORRUPTED/UNAVAILABLE/UNSUPPORTED` ne sont jamais confondus silencieusement.
- **B-STOR-06** — le Core ne choisit jamais l’autorité métier parmi des candidats techniquement valides.
- **B-STOR-07** — récupération technique du stockage et récupération sémantique d’un Store sont deux étapes distinctes.
- **B-STOR-08** — le stockage ne dépend pas du `WallClock`.

---

## 4. C — Identity

`IdentityService` est l’autorité runtime de l’identité logique ; `IdentityStore` ne persiste que l’identité provisionnée non reconstructible.

Concepts séparés :

```text
ProvisionedIdentity
  - device_id
  - serial_number if not provided by immutable hardware source

ProductIdentity
  - manufacturer
  - hardware_version
  - firmware_version
  - protocol_version
  - capabilities

IdentitySnapshot
  - coherent runtime composition of the above
```

### Invariants

- **C-ID-01** — B0 est exclusivement une projection de l’identité runtime.
- **C-ID-02** — identité provisionnée persistante et informations reconstructibles restent distinctes.
- **C-ID-03** — `device_id` est stable, unique, persistant et jamais régénéré silencieusement au boot.
- **C-ID-04** — absence/corruption/indisponibilité d’identité ne conduit jamais à inventer automatiquement une nouvelle identité.
- **C-ID-05** — `device_id` et `serial_number` sont deux identifiants distincts.
- **C-ID-06** — firmware/protocol version décrivent l’implémentation réellement exécutée.
- **C-ID-07** — les capabilities décrivent le support statique, pas la disponibilité instantanée.
- **C-ID-08** — B0 n’est jamais une interface de provisioning.

Restent ouverts : attribution initiale du `device_id`, reprovisioning, sources exactes HW/serial/manufacturer, comportement global si identité invalide.

---

## 5. D — Configuration

Chaîne d’autorité :

```text
PreparedConfiguration (volatile)
        ↓
ConfigurationValidator
        ↓
ValidatedConfiguration (volatile)
        ↓ B5 APPLY
ConfigurationService
        ↓
ConfigurationStore
        ↓ durable commit
ActiveConfigurationSnapshot (immutable runtime authority)
        ↓
consumers + B4 projection
```

Prepared, validated, active durable et active runtime sont quatre états distincts.

### Invariants

- **D-CONF-01** — le prepared est volatile, mutable et sans effet immédiat.
- **D-CONF-02** — toute modification du prepared invalide la validation précédente.
- **D-CONF-03** — les valeurs RW représentables mais hors domaine sont stockables ; la validation métier les rejette ultérieurement.
- **D-CONF-04** — contrôle CRC préparé et validation sémantique sont distincts.
- **D-CONF-05** — le CRC B4 porte sur la représentation normative B4, jamais sur le record persistant.
- **D-CONF-06** — l’activation ne passe que par B5.
- **D-CONF-07** — aucune nouvelle configuration active n’est publiée avant commit durable.
- **D-CONF-08** — échec de validation/application laisse l’ancienne active inchangée.
- **D-CONF-09** — `ActiveConfigurationSnapshot` est cohérent, immuable et publié atomiquement.
- **D-CONF-10** — aucune configuration active par défaut n’est inventée lorsqu’aucune autorité récupérable n’existe.
- **D-CONF-11** — une nouvelle configuration active ne déclenche pas implicitement une campagne.
- **D-CONF-12** — une campagne historique n’est jamais reconstruite depuis l’active actuelle.
- **D-CONF-13** — L'absence d'`ActiveConfigurationSnapshot` constitue un état runtime explicite ; une projection B4 neutralisée ne constitue jamais une `ActiveConfiguration`.
- **D-CONF-14** — Le commit durable du `ConfigurationStore` constitue la frontière d'existence d'une nouvelle `ActiveConfiguration` ; les snapshots runtime et B4 sont des projections postérieures à cette frontière.
- **D-CONF-15** — `ActiveConfiguration` et `config_revision_counter` appartiennent au même commit logique par `FW_POLICY`.

---

## 6. E — Time

Séparation fondamentale :

```text
MonotonicClock ≠ WallClock
```

`TimeService` est l’autorité métier runtime de l’heure civile TR2. `WallClock` fournit seulement une abstraction de lecture/écriture du temps civil. `TimeHistoryStore` conserve uniquement les faits historiques utiles de synchronisation.

### Chaîne de synchronisation

```text
PreparedTime (volatile)
   ↓
B5 SYNCHRONIZE
   ↓
TimeService
   ↓
WallClock.set
   ↓ confirmed success
update runtime sync facts
   ↓
durable TimeHistory
   ↓
TimeSnapshot
   ↓
B2 projection
```

### Invariants

- **E-TIME-01** — le prepared time est volatile et sans effet immédiat.
- **E-TIME-02** — synchroniser le `WallClock` ne modifie jamais le `MonotonicClock`.
- **E-TIME-03** — la synchronisation effective n’est reconnue qu’après application confirmée au `WallClock`.
- **E-TIME-04** — `last_sync_time` n’est mis à jour qu’après succès effectif.
- **E-TIME-05** — `TimeHistoryStore` ne contient pas de copies périodiques de l’heure courante.
- **E-TIME-06** — `time_status` et `time_flags` sont des projections de faits et non des autorités.
- **E-TIME-07** — aucun automate exhaustif de `time_status` n’est inventé au-delà de la V1.
- **E-TIME-08** — un reboot n’invalide pas automatiquement une RTC conservée ; la validité dépend des faits et d’une policy explicite.
- **E-TIME-09** — un `WallClock` invalide ne bloque pas le boot global.
- **E-TIME-10** — aucun timestamp historique manquant n’est reconstruit depuis l’heure courante postboot.

---

## 7. F — Acquisition / Supervision

Séparation :

```text
ActiveConfigurationSnapshot
       ↓
AcquisitionService
       ↓
AcquisitionWindow
       ↓
SupervisionService
       ↓
SupervisionSnapshot
       ↓
B3 projection
```

`AcquisitionService` possède le processus runtime de capture. `SupervisionService` possède le calcul des indicateurs, la qualification de validité/fraîcheur et l’évaluation des seuils. B3 reste une projection.

### Invariants

- **F-ACQ-01** — acquisition, supervision et B3 sont trois responsabilités distinctes.
- **F-ACQ-02** — une fenêtre utilise une seule génération cohérente d’`ActiveConfigurationSnapshot`.
- **F-ACQ-03** — aucun changement de configuration ne rétroagit sur une fenêtre déjà calculée.
- **F-ACQ-04** — données brutes, résultats de supervision et projection B3 restent distincts.
- **F-SUP-01** — les indicateurs V1 sont calculés selon les conventions normatives B3.
- **F-SUP-02** — les seuils appliqués viennent du contexte de configuration associé à la fenêtre.
- **F-SUP-03** — valeur, validité, fraîcheur et qualité sont des notions distinctes.
- **F-SUP-04** — la dernière valeur valide peut être conservée en cas d’indisponibilité temporaire, mais sa fraîcheur/validité reflète les faits.
- **F-SUP-05** — `SupervisionSnapshot` est construit complètement puis publié atomiquement.
- **F-SUP-06** — les âges/durées runtime utilisent un repère monotone lorsque possible ; les timestamps civils viennent de `TimeService`.
- **F-SUP-07** — les snapshots live ne sont pas restaurés depuis NVM comme autorité après reboot.
- **F-RUN-01** — l’initialisation acquisition/supervision au boot ne lance aucune campagne.
- **F-RUN-02** — `AcquisitionService` ne peut démarrer sans `ActiveConfigurationSnapshot` autoritatif.
- **F-RUN-03** — `AcquisitionService` ne reconstruit jamais sa configuration depuis la projection Modbus B4.

---

## 8. G — Campaign / Storage

Séparation :

```text
CampaignService
   ├── orchestrates AcquisitionService
   └── CampaignRepository
          ├── CampaignMetadata
          └── CampaignDataStore
                 ↓
          PersistentStorageCore
```

`CampaignService` possède le cycle de vie runtime et l’identité de la campagne. `CampaignRepository` possède l’autorité historique durable. `AcquisitionService` produit les données mais ne possède ni l’identité de campagne ni son histoire durable.

### Chaîne START

```text
B5 START
→ preconditions
→ allocate campaign_id
→ freeze historical context
→ DURABLE OPEN CAMPAIGN
→ start AcquisitionService
→ append durable chunks
```

### Recovery

Une campagne ouverte avant reset n’est jamais reprise automatiquement. Le recovery conserve son `campaign_id`, son contexte historique et seulement le dernier préfixe durable valide. Aucun `end_timestamp` ni `duration_s` ne sont inventés.

### Invariants

- **G-CAMP-01** — identité/contexte de campagne durable avant premier enregistrement durable de données de cette campagne.
- **G-CAMP-02** — `campaign_id` ne vient ni de B6 ni d’un nom de fichier.
- **G-CAMP-03** — chaque campagne possède son propre contexte historique durable.
- **G-DATA-01** — stockage bulk en append/checkpoint permettant le recovery du dernier préfixe valide.
- **G-DATA-02** — un chunk partiel/corrompu ne contamine pas les chunks précédemment démontrés valides.
- **G-REC-01** — aucune auto-reprise de campagne au boot.
- **G-REC-02** — aucun fait historique n’est deviné ou réparé depuis l’état courant.
- **G-REC-03** — recovery idempotent face aux resets répétés.
- **G-INV-01** — B6 est une projection de `CampaignRepository`.
- **G-INV-02** — `selected_campaign_index` est un curseur protocolaire volatile, distinct de `campaign_id`.
- **G-STORE-01** — limite métier de campagne et capacité physique du média sont distinctes.
- **G-CMD-01** — le `CommandJournal` ne devient jamais l’autorité sur l’existence ou l’état d’une campagne.

---

## 9. H — B5 Command Engine

Architecture :

```text
B5 request registers
      ↓
CommandRequestMailbox (volatile)
      ↓ submit rising edge
immutable CommandRequest
      ↓
CommandEngine
      ├── CommandJournal (persistent)
      ├── CommandRecoveryContext when required
      └── dispatch to Domain Services
      ↓
CommandSnapshot
      ↓
B5 projection
```

### 9.1 Séparation des responsabilités

Les notions suivantes restent strictement distinctes :

```text
transaction_id
      ↓
identifie la transaction

CommandRequest identity
      ↓
identifie le contenu B5 soumis

CommandRecoveryContext
      ↓
relie éventuellement la transaction à son objet métier

Domain Authority
      ↓
établit les faits métier réellement produits
```

Le `CommandJournal` fournit l'autorité transactionnelle et les preuves nécessaires à l'idempotence. Il ne devient jamais l'autorité de l'état métier.

Le `CommandRecoveryContext`, lorsqu'il existe, ne contient que l'identité minimale permettant de corréler une transaction à son objet ou effet métier. Il ne constitue jamais une copie de l'autorité métier.

### 9.2 Admission transactionnelle

Une simple écriture des registres B5 ne crée aucune transaction. Seul le front montant de `submit` capture une `CommandRequest` complète et cohérente.

Séquence logique :

```text
submit rising edge
↓
capture immutable CommandRequest
↓
validate transaction_id
↓
lookup transaction_id in CommandJournal
├── known + same request identity
│       → retry
├── known + different request identity
│       → collision
└── new valid transaction_id
        → durable RESERVED
```

Une écriture rejetée au niveau Modbus ne crée aucune transaction. Un `transaction_id = 0` soumis reste invalide selon la V1 et ne crée aucune identité persistante d'idempotence.

Pour un `transaction_id` valide et nouveau, l'identité transactionnelle durable est créée avant toute évaluation ou exécution pouvant conduire à un résultat transactionnel idempotent.

### 9.3 Identité canonique de requête

Le `transaction_id` identifie la transaction mais ne fait pas partie de l'identité de contenu de la requête.

Pour la V1, l'identité canonique candidate couvre au minimum les valeurs effectivement capturées de :

```text
command_code
param1
param2
param3
confirm_key
```

Les bits `submit`, `cancel_request` et `clear_request_fields` n'appartiennent pas à cette identité.

Même `transaction_id` + même identité de requête :

```text
retry
→ aucune nouvelle exécution
→ restitution du résultat précédent
```

Même `transaction_id` + identité différente :

```text
collision transactionnelle
→ aucun redispatch métier
```

La réponse protocolaire exacte à cette collision reste `NOT_DEFINED V1`.

La représentation physique de l'identité — champs complets, fingerprint, hash ou combinaison — reste `IMPLEMENTATION`.

### 9.4 Cycle de vie transactionnel

Les états internes conceptuels retenus sont :

```text
RESERVED
STARTED
COMPLETED
```

Ils ne constituent pas de nouveaux états Modbus.

Sémantique :

```text
RESERVED
→ identité transactionnelle durable
→ aucun effet métier significatif autorisé à commencer

STARTED
→ frontière autorisant un effet métier franchie
→ effet possible mais non prouvé

COMPLETED
→ résultat transactionnel final durable disponible
```

Une transaction refusée ou finalisée sans effet métier significatif peut évoluer directement de `RESERVED` vers `COMPLETED`.

### 9.5 Barrières de durabilité

Ordre logique :

```text
capture immutable CommandRequest
↓
RESERVED power-loss-safe
↓
CommandRecoveryContext power-loss-safe when required
↓
STARTED power-loss-safe
↓
first significant business effect
↓
COMPLETED + final result power-loss-safe
↓
final B5 publication
```

`RESERVED`, `STARTED` et `COMPLETED` sont des barrières logiques de durabilité ; elles n'imposent pas un nombre particulier d'écritures physiques.

L'échec de persistance de `RESERVED` interdit le dispatch.

L'échec de persistance d'un `CommandRecoveryContext` requis interdit le passage à `STARTED`.

L'échec de persistance de `STARTED` interdit le début de tout effet métier significatif.

L'échec de persistance de `COMPLETED` interdit la publication d'un résultat final B5 prétendument durable.

Le contrat de persistance doit fournir une barrière réellement power-loss-safe ; une acceptation en RAM ou dans un cache non durable ne suffit pas.

### 9.6 Idempotence et rétention

Un `transaction_id` déjà traité n'est jamais redispatché.

Succès, refus et échecs finaux appartiennent tous à l'histoire d'idempotence lorsqu'une transaction valide a été admise.

Pour toute transaction finalisée encore considérée comme déjà traitée, l'information durable doit permettre :

- de reconnaître son `transaction_id` ;
- de comparer l'identité de la requête ;
- de restituer son résultat précédent sans redispatch.

La profondeur visible B5 de la dernière commande terminée n'est pas la profondeur de l'historique interne d'idempotence.

La politique firmware V1 retenue est `lifetime strict` :

```text
once admitted
→ transaction_id permanently known
→ no implicit reuse
```

Cette politique est une `FW_POLICY` de confinement. Elle ne définit pas un cycle de vie normatif général du `transaction_id`.

L'espace transactionnel disponible est donc fini :

```text
1..65535
```

Après admission des 65535 identifiants, aucun ancien identifiant n'est automatiquement évincé ou rendu libre.

La réponse protocolaire explicite à l'épuisement du namespace reste `NOT_DEFINED V1`.

La politique V1 de réutilisation après disparition de l'historique reste elle-même `NOT_DEFINED V1`.

### 9.7 Recovery d'une transaction RESERVED

Un `RESERVED` valide retrouvé au boot prouve que l'identité transactionnelle a été durablement admise mais que la barrière `STARTED` n'a pas été franchie.

Il fournit donc une preuve d'absence d'effet métier significatif de cette transaction.

Une transaction `RESERVED` :

- conserve définitivement son `transaction_id` ;
- n'est jamais restaurée comme requête de mailbox ;
- n'est jamais automatiquement redispatchée ;
- converge vers une finalisation interne durable d'interruption avant effet.

Le mapping exact de cette finalisation vers `cmd_status`, `cmd_result_code` et `cmd_result_detail` reste `NOT_DEFINED V1`.

Le moteur ne réévalue pas la commande après reboot pour deviner le résultat qu'elle aurait hypothétiquement produit.

### 9.8 Recovery d'une transaction STARTED

Un `STARTED` valide prouve seulement que la frontière autorisant un effet significatif a été franchie.

Il ne prouve ni que l'effet a eu lieu ni que la commande a réussi.

La réconciliation confronte :

```text
transaction evidence
+
recovered domain evidence
↓
reconciliation outcome
```

Outcomes conceptuels :

```text
TERMINAL_EFFECT_PROVEN
ABSENCE_PROVEN
INDETERMINATE
```

`TERMINAL_EFFECT_PROVEN` exige que l'effet terminal pertinent soit durablement démontré avec une causalité suffisante pour être attribué à la transaction considérée.

Un état métier courant simplement compatible avec la commande ne constitue pas une preuve suffisante.

La preuve d'un effet partiel ne constitue pas une preuve de succès terminal.

`ABSENCE_PROVEN` exige une preuve que cette transaction n'a pas produit l'effet significatif considéré. Cette conclusion n'autorise jamais le replay.

Toute situation où ni l'effet terminal ni son absence ne peuvent être prouvés est classée `INDETERMINATE`.

`INDETERMINATE` est un résultat sûr de recovery. Il ne rend jamais le `transaction_id` libre et n'autorise jamais un redispatch.

Le mapping B5 exact des conclusions de recovery non représentées explicitement par la V1 reste `NOT_DEFINED V1`.

### 9.9 CommandRecoveryContext

L'identité canonique de requête utilisée pour l'idempotence B5 et le contexte nécessaire à la réconciliation métier sont deux notions distinctes.

Pour la politique firmware V1 :

```text
REQUIRED
  APPLY_CONFIG
  SYNC_TIME
  START_ACQUISITION
  STOP_ACQUISITION
  SOFTWARE_RESET

OPTIONAL
  SELFTEST

NO ADDITIONAL B5 RECOVERY PERSISTENCE REQUIRED
  ACK_FAULT
  REFRESH_INDICATORS
  ENTER_MAINTENANCE
  EXIT_MAINTENANCE
  RESET_STATISTICS
```

Cette classification est une `FW_POLICY`.

Contextes conceptuels minimaux possibles :

```text
APPLY_CONFIG       → identity of candidate configuration
SYNC_TIME          → identity of prepared synchronization
START_ACQUISITION  → allocated campaign_id
STOP_ACQUISITION   → target campaign_id
SOFTWARE_RESET     → BootIntent correlation
```

Les formes physiques restent `IMPLEMENTATION`.

Pour `START_ACQUISITION`, l'allocation/réservation d'un `campaign_id` doit rester distincte de l'ouverture métier autoritative de la campagne afin qu'aucun effet significatif n'apparaisse avant `STARTED`.

### 9.10 Recovery d'une transaction COMPLETED

Un `COMPLETED` valide constitue l'autorité transactionnelle du résultat final historique.

Il n'est jamais revalidé à partir de l'état métier courant et n'est jamais redispatché.

Un retry cohérent restitue le résultat durable précédent.

Une évolution ultérieure de l'état métier ne modifie jamais rétroactivement le résultat de la transaction.

Après `COMPLETED`, les informations spécifiques de recovery peuvent être compactées si restent durablement disponibles :

- l'identité transactionnelle ;
- l'identité de requête ;
- le résultat final nécessaire à l'idempotence.

### 9.11 Projection B5

B5 reste une projection du moteur transactionnel.

`cmd_last_*` représente la dernière transaction logiquement terminée, quel que soit son résultat final.

L'ordre de terminaison n'est jamais déduit de la valeur numérique du `transaction_id`.

Le `CommandJournal`, son ordre durable ou une métadonnée reconstructible doivent permettre d'identifier la dernière transaction terminée.

Une copie persistante éventuelle de `cmd_last_*` reste un cache dérivé, jamais une seconde autorité.

Un timestamp original durable est restitué sans modification. Un retry ou un reboot ne crée jamais un nouveau timestamp de terminaison.

Lorsqu'aucun timestamp valide n'existait lors de la terminaison, le recovery n'en invente aucun. La représentation B5 exacte de cette absence reste `NOT_DEFINED V1`.

### 9.12 Corruption

Une preuve transactionnelle corrompue ou indéterminée n'est jamais interprétée comme une identité libre.

Si l'identité d'une transaction reste démontrable mais que son résultat final est perdu ou corrompu :

```text
transaction remains protected
→ never redispatch
→ result becomes indeterminate
```

La présence d'une preuve plus récente corrompue interdit de considérer silencieusement un record valide plus ancien comme étant nécessairement la dernière commande terminée.

Les index et métadonnées d'optimisation reconstructibles ne sont jamais l'autorité transactionnelle.

### 9.13 Endurance et représentation physique

Le coût informationnel conservateur d'une transaction complète est de l'ordre de 24 à 32 octets avant choix final du format persistant.

Conserver une preuve complète pour 65535 identifiants représente conceptuellement environ 1,5 à 2 MiB.

Cette estimation ne préjuge pas de la technologie NVM.

Capacité, amplification d'écriture, endurance, distribution des écritures et atomicité power-loss doivent être évaluées séparément lors de l'architecture d'implémentation.

L'architecture n'impose jamais une concentration inutile des écritures sur une cellule ou une métadonnée unique.

### 9.14 Limitations V1 identifiées par K1

Restent explicitement `NOT_DEFINED V1` :

- cycle de vie normatif du `transaction_id` ;
- wrap / réutilisation après disparition de l'historique ;
- comportement protocolaire explicite en cas d'épuisement des 65535 identifiants ;
- réponse exacte à même txid + requête différente ;
- représentation B5 d'une transaction interrompue avant effet ;
- représentation B5 d'un résultat post-crash `INDETERMINATE` ;
- représentation d'un timestamp final indisponible.

Ces points sont candidats V1.1 et ne sont pas complétés silencieusement par la politique firmware V1.

### Invariants

- **H-CMD-01** — mailbox, moteur, journal et autorités métier restent distincts.
- **H-CMD-02** — la mailbox est volatile et n’est jamais restaurée/resoumise.
- **H-CMD-03** — le front montant `submit` capture une requête complète cohérente.
- **H-IDEM-01** — toute transaction déjà traitée réutilise son résultat durable précédent sans redispatch métier.
- **H-IDEM-02** — refus et échecs finaux font partie de l’histoire d’idempotence.
- **H-JRN-01** — une transaction à effet non trivial est durablement identifiée avant son premier effet métier significatif.
- **H-JRN-02** — un résultat final B5 n’est publié qu’après durabilité du résultat transactionnel correspondant.
- **H-JRN-03** — le journal fournit une preuve transactionnelle, jamais une autorité métier.
- **H-EXEC-01** — une seule commande active ; aucune file générale V1 n’est introduite.
- **H-EXEC-02** — le moteur possède les règles transactionnelles génériques ; les préconditions métier restent dans les services compétents.
- **H-REC-01** — aucune entrée `STARTED` n’autorise un replay aveugle.
- **H-REC-02** — la réconciliation s’effectue contre les autorités métier récupérées.
- **H-RST-01** — un reset logiciel commandé laisse sa trace durable et son `BootIntent` avant reset.
- **H-PUB-01** — B5 est exposé depuis un `CommandSnapshot` cohérent.
- **H-ADM-01** — une requête valide avec txid nouveau est durablement admise avant son résultat transactionnel.
- **H-REQ-01** — identité transactionnelle et identité de contenu de requête restent distinctes.
- **H-IDEM-03** — un même txid associé à une requête différente est une collision et n'autorise aucun redispatch.
- **H-STATE-01** — `RESERVED`, `STARTED` et `COMPLETED` sont des barrières logiques de durabilité.
- **H-STATE-02** — `RESERVED` prouve l'absence d'effet métier significatif de la transaction.
- **H-STATE-03** — `STARTED` prouve seulement qu'un effet a pu commencer.
- **H-STATE-04** — `COMPLETED` prouve l'existence d'un résultat transactionnel final durable.
- **H-REC-03** — un état métier compatible ne constitue pas à lui seul une preuve causale de la transaction.
- **H-REC-04** — un effet partiel ne constitue pas une preuve de succès terminal.
- **H-REC-05** — `INDETERMINATE` reste protégé contre tout replay.
- **H-RCTX-01** — le `CommandRecoveryContext` est une preuve de corrélation minimale et jamais une autorité métier.
- **H-RCTX-02** — tout contexte requis pour attribuer un effet est durable avant cet effet.
- **H-LIFE-01** — la politique firmware V1 ne rend jamais volontairement libre un txid déjà admis.
- **H-LIFE-02** — le renouvellement du namespace des txid reste une limite normative ouverte.
- **H-CPL-01** — un résultat `COMPLETED` valide est historique et immuable.
- **H-CPL-02** — l'ordre numérique des txid ne définit jamais l'ordre de terminaison.
- **H-CORR-01** — corruption ou indétermination d'une preuve ne transforme jamais une identité connue en identité libre.

Restent ouverts au niveau d'implémentation : représentation physique du journal, technologie NVM, format du fingerprint, indexation, stratégie d'endurance et emplacement physique des contextes de recovery.

Restent `NOT_DEFINED V1` les limitations listées en 9.14.

---

## 10. I — Diagnostic / System State

Architecture :

```text
Diagnostic fact / metric producers
        ↓
DiagnosticService
   ├── DiagnosticHistoryStore
   └── SelfTestService
        ↓
DiagnosticSnapshot
        ↓
SystemStateAggregator
        ↓
SystemStateSnapshot
        ↓
B1 / B5 flags / B7 projections
```

Les conditions actives, l’historique du dernier défaut, l’autotest et la santé globale sont quatre notions distinctes.

### Invariants

- **I-DIAG-01** — les conditions actives sont réévaluées depuis les autorités/producteurs ; elles ne sont jamais restaurées depuis une image B7.
- **I-DIAG-02** — `last_fault_code` est historique et indépendant des défauts actifs.
- **I-DIAG-03** — code + timestamp du dernier défaut forment un record persistant cohérent.
- **I-DIAG-04** — aucun timestamp de défaut n’est inventé si le temps civil est indisponible.
- **I-SELF-01** — `selftest running` est volatile.
- **I-SELF-02** — un autotest interrompu par reboot n’est pas automatiquement déclaré en échec.
- **I-SELF-03** — seul le dernier autotest complètement terminé peut être restauré.
- **I-SYS-01** — `system_health_status` est un agrégat dérivé, jamais une autorité métier unique.
- **I-SYS-02** — `SystemStateAggregator` agrège des faits sans déclencher d’action métier.
- **I-TIME-01** — `uptime_s` vient exclusivement du `MonotonicClock` du boot courant.
- **I-RESET-01** — `reset_cause` vient du `BootContext` normalisé.
- **I-RESET-02** — reset cause et last fault restent indépendants.
- **I-METRIC-01** — métrique brute et condition diagnostic sont distinctes.
- **I-ACK-01** — acquitter un défaut ne supprime jamais sa cause active.
- **I-STAT-01** — les statistiques de service restent distinctes des historiques critiques.
- **I-PUB-01** — B7 est construit depuis un snapshot cohérent et jamais restauré comme autorité persistante.

---

## 11. J — Modbus Adaptation Layer

Architecture :

```text
Modbus RTU Transport / Server
        ↓
Register Model
        ↓
Modbus Adaptation Layer
   ├── codecs
   ├── block projections
   ├── immutable read models
   └── write adapters
        ↓
Domain Services / Snapshots
```

### Sources par bloc

```text
B0 ← IdentitySnapshot
B1 ← SystemStateSnapshot
B2 ← TimeSnapshot
B3 ← SupervisionSnapshot
B4 ← Configuration read-model
B5 ← CommandSnapshot + volatile mailbox
B6 ← CampaignInventorySnapshot + volatile selection
B7 ← DiagnosticSnapshot
```

### Règles

- aucun bloc ne lit un autre bloc pour reconstruire un fait métier ;
- les faits communs sont dérivés des mêmes autorités domaine ;
- une lecture multi-registre capture un seul snapshot stable pour toute la réponse ;
- les writes RW passent par l’adaptation vers le service propriétaire ; aucun snapshot RO n’est muté directement ;
- accès/adresse invalide et valeur métier invalide restent distincts ;
- une valeur sémantiquement invalide dans une zone RW suit la sémantique V1 du bloc et n’est pas automatiquement transformée en exception Modbus.

### Codecs

Les conversions `uint32 MSW/LSW`, `int16`, enum16, bitfields et ASCII fixe sont centralisées et indépendantes du domaine. Les enums domaine restent distincts de leurs codes Modbus.

### CRC

Trois mécanismes indépendants :

```text
Modbus RTU CRC16            → intégrité de trame
B4 CRC-32 normatif          → intégrité de représentation configuration
Persistent record integrity → intégrité technique NVM/storage
```

### Invariants

- **J-LAYER-01** — L2 traduit mais ne possède aucune autorité métier.
- **J-LAYER-02** — B0…B7 sont uniquement des projections.
- **J-LAYER-03** — aucun bloc Modbus n’est source d’un autre bloc.
- **J-REG-01** — le Register Model connaît structure/adresses/RO-RW, pas la logique métier.
- **J-REG-02** — write sur RO/réservé rejeté avant tout appel domaine.
- **J-CODEC-01** — codecs centralisés et purs.
- **J-SNAP-01** — une réponse multi-registre utilise une génération unique de read-model.
- **J-SNAP-02** — aucune transaction globale B0…B7 n’est requise.
- **J-SNAP-03** — un éventuel tableau de registres préencodé n’est qu’un read-model immuable dérivé.
- **J-WRITE-01** — les writes ne mutent jamais directement un read-model RO.
- **J-B6-01** — la sélection B6 reste protocolaire volatile ; l’inventaire reste autorité métier.
- **J-CRC-01** — CRC RTU, CRC B4 et intégrité persistante restent indépendants.
- **J-BOOT-01** — le serveur n’est ouvert qu’après publication de snapshots initiaux cohérents.
- **J-DEP-01** — aucun Domain Service ne dépend d’un registre, codec ou handler Modbus.

---

## 12. Passe transversale A→J

### 12.1 Verdict

Aucune contradiction structurelle majeure ni dépendance circulaire nécessaire n’a été identifiée. Les corrections retenues sont des clarifications de frontières d’autorité.

### 12.2 Autorités uniques

- `AcquisitionService` possède l’état runtime de capture/acquisition.
- `CampaignService` possède le cycle de vie runtime et l’identité de campagne.
- `CampaignRepository` possède l’histoire durable de campagne.
- `ConfigurationService` / `ActiveConfigurationSnapshot` possèdent l’autorité de configuration active.
- `TimeService` possède le temps civil métier ; `MonotonicClock` possède les durées monotones.
- `DiagnosticService` possède les faits diagnostic normalisés ; `SystemStateAggregator` agrège sans devenir autorité des domaines sources.
- `CommandEngine` orchestre les commandes mais ne possède pas leurs effets métier.

### 12.3 Boot et réconciliation

L’ordre logique reste celui du document boot/recovery. Précision : les composants nécessaires au recovery/réconciliation peuvent exister avant l’activation runtime G8 ; G8 désigne leur activation opérationnelle, pas obligatoirement leur première instanciation.

Dans la politique V1 retenue, la réconciliation B5 précède l’ouverture du serveur, tout en conservant conceptuellement deux capacités distinctes : `ModbusReady` et `CommandAcceptanceReady`.

### 12.4 Configuration pendant acquisition

La V1 refuse l’application d’une configuration pendant acquisition active. La règle F de cohérence d’une fenêtre face à un changement de configuration reste néanmoins conservée comme invariant défensif et futur-compatible.

### 12.5 Mode maintenance

Une autorité métier distincte, conceptuellement `SystemModeService` / `OperationalModeService`, porte le mode NORMAL/MAINTENANCE. `CommandEngine` l’invoque ; `SystemStateAggregator` l’observe. La persistance éventuelle de ce mode reste `NOT_DEFINED V1`.

### 12.6 RESET STATISTICS

Les statistiques restent possédées par les domaines qui les produisent. La commande B5 de RAZ orchestre les remises à zéro autorisées mais ne crée pas une autorité globale et ne doit pas toucher les historiques critiques non explicitement concernés.

### 12.7 Campaign ID

La méthode de génération reste ouverte, mais elle doit être crash-safe vis-à-vis de l’inventaire récupéré : aucune création/recovery ne doit conduire à deux campagnes valides présentes portant le même `campaign_id`.

### 12.8 Nouveaux invariants transversaux

- **TR-BOOT-02** — les dépendances de recovery peuvent être construites avant G8 ; G8 signifie activation runtime.
- **TR-BOOT-03** — `ModbusReady` et `CommandAcceptanceReady` sont distincts ; dans le boot V1 actuel, la réconciliation B5 précède néanmoins l’ouverture du serveur.
- **TR-AUTH-01** — tout fait métier possède une autorité unique ; les autres composants n’en portent que des projections dérivées.
- **TR-ACQ-01** — `AcquisitionService` possède l’état runtime d’acquisition ; `CampaignService` possède identité et cycle de vie de campagne.
- **TR-ACQ-02** — `CampaignService` peut orchestrer `AcquisitionService`; la dépendance inverse est interdite.
- **TR-CONF-01** — une nouvelle active ne peut normalement pas être appliquée pendant acquisition active en V1 ; la cohérence de fenêtre reste garantie défensivement.
- **TR-MODE-01** — le mode maintenance possède une autorité domaine indépendante de B5 et de l’agrégateur.
- **TR-STAT-01** — les statistiques restent possédées par leurs domaines producteurs ; `RESET STATISTICS` est une orchestration.
- **TR-CAMP-ID-01** — allocation/récupération de `campaign_id` garantit l’unicité parmi les campagnes valides présentes malgré les resets.
- **TR-STOR-02** — importance métier d’une métadonnée et choix de son média physique sont deux notions distinctes.
- **TR-CRC-01** — CRC RTU, CRC B4 et intégrité persistante restent trois mécanismes indépendants.

---

## 13. Points ouverts après gel A→J

### À résoudre avant implémentation fonctionnelle

- comportement global si identité absente/corrompue ;
- critères de validité temporelle au boot et reconstruction de `time_since_sync` ;
- projection B7 d’un timestamp historique absent et d’un selftest interrompu ;
- politique d’agrégation `system_status` B1 / `system_health_status` B7.

La projection B4 sans active récupérable est désormais confinée par la `FW_POLICY` K2 documentée dans `ARCHITECTURE_FIRMWARE_BOOT_PERSISTENCE_RECOVERY.md`. Sa représentation normative exhaustive reste `NOT_DEFINED V1` et candidate V1.1.

### Limitations transactionnelles V1 identifiées par K1

- cycle de vie normatif, wrap et réutilisation du `transaction_id` ;
- comportement protocolaire explicite en cas d’épuisement des 65535 identifiants ;
- réponse protocolaire pour même `transaction_id` + requête/fingerprint différent ;
- représentation B5 d’une transaction interrompue avant effet ;
- représentation B5 d’un résultat post-crash `INDETERMINATE` ;
- représentation d’un timestamp final indisponible.

### FW_POLICY / IMPLEMENTATION pouvant être traitées plus tard

- algorithme exact de `campaign_id` ;
- format et taille des chunks ;
- média physique, filesystem, layout NVM ;
- représentation physique du `CommandJournal`, indexation et stratégie d’endurance ;
- GC/compaction/wear strategy ;
- DMA, RTOS, synchronisation, publication lock-free/mutex/double-buffer ;
- seuils techniques température/tension/surconsommation ;
- périmètre exact des statistiques de service.

### Candidats V1.1

- cycle de vie / renouvellement du namespace `transaction_id` ;
- indication explicite d’approche ou d’épuisement du namespace ;
- réponse protocolaire explicite pour même `transaction_id` + requête/fingerprint différent ;
- représentation B5 des transactions interrompues avant effet et des résultats `INDETERMINATE` ;
- clarification de certaines projections lorsque le temps historique est indisponible ;
- éventuel journal circulaire de défauts ou statistiques persistantes supplémentaires.

---

## 14. Gel d’architecture

À l’issue de cette passe, l’architecture cible est :

```text
une autorité par fait
+ orchestrateurs sans duplication d’autorité
+ snapshots immuables
+ persistance derrière Stores/Repositories
+ recovery avant publication
+ Modbus entièrement en périphérie
```

Ce document constitue le gel de la passe A→J de l’architecture firmware Modbus RTU V1. Toute évolution ultérieure doit préserver la distinction entre exigence V1, politique firmware, choix d’implémentation et comportement `NOT_DEFINED V1`.
