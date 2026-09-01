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
      └── dispatch to Domain Services
      ↓
CommandSnapshot
      ↓
B5 projection
```

### Idempotence

Un `transaction_id` déjà traité n’est jamais redispatché. Le journal conserve assez d’identité de requête pour détecter un même txid associé à une requête différente. La réponse protocolaire exacte à ce dernier cas reste `NOT_DEFINED V1`.

### Recovery

Une transaction interne `STARTED` retrouvée au boot n’est jamais rejouée automatiquement. Le moteur reconcile le journal avec les autorités métier déjà récupérées et distingue conceptuellement : effet prouvé, absence d’effet prouvée, résultat indéterminé.

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

Restent ouverts : profondeur/rétention du journal, wrap/réutilisation txid, fingerprint exact, mapping d’un cas indéterminé, timestamp final si WallClock invalide.

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

- profondeur/rétention du `CommandJournal` ;
- wrap/réutilisation du `transaction_id` ;
- comportement global si identité absente/corrompue ;
- projection B4 lorsqu’aucune active n’est récupérable et que le staging est vide ;
- critères de validité temporelle au boot et reconstruction de `time_since_sync` ;
- projection B7 d’un timestamp historique absent et d’un selftest interrompu ;
- politique d’agrégation `system_status` B1 / `system_health_status` B7.

### FW_POLICY / IMPLEMENTATION pouvant être traitées plus tard

- algorithme exact de `campaign_id` ;
- format et taille des chunks ;
- média physique, filesystem, layout NVM ;
- GC/compaction/wear strategy ;
- DMA, RTOS, synchronisation, publication lock-free/mutex/double-buffer ;
- seuils techniques température/tension/surconsommation ;
- périmètre exact des statistiques de service.

### Candidats V1.1

- réponse protocolaire explicite pour même `transaction_id` + requête/fingerprint différent ;
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