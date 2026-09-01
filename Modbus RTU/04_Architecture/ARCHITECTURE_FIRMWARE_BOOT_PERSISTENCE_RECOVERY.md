# Architecture firmware Modbus RTU V1 — Boot, persistance et recovery

## 1. Statut du document

Ce document formalise les décisions d'architecture firmware TR2 issues de la passe A→J « Boot — Persistance — Recovery ».

Il ne modifie pas la spécification normative Modbus RTU V1 et ne crée aucune nouvelle exigence protocolaire. Toute règle non imposée par la V1 est explicitement considérée comme **politique firmware TR2** ou **choix d'implémentation**.

Baseline normative de référence lors du gel : branche `main`, commit `d881add9b0d5ae0ba884d523cc18d516240dd264` (`Modbus V1: define final baseline freeze record`).

Principes de lecture :

- **V1** : exigence normative existante ;
- **FW_POLICY** : politique firmware TR2 retenue pour obtenir un comportement industriel déterministe ;
- **IMPLEMENTATION** : mécanisme technique non figé par ce document ;
- **NOT_DEFINED V1** : comportement volontairement non inventé lorsque la V1 ne le définit pas.

---

## 2. Architecture générale

La chaîne d'autorité reste strictement descendante :

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

Les services métier ne dépendent jamais de Modbus. Les blocs B0…B7 sont des projections ; ils ne sont jamais l'autorité métier ni l'autorité persistante.

Pour la persistance et le recovery :

```text
HAL / Platform
    ↓
PersistentStorageCore
    ↓
Domain Stores / Repositories
    ↓
Recovered authoritative domain states
    ↓
Runtime Services
    ↓
DiagnosticService / SystemStateAggregator
    ↓
Immutable read-model snapshots
    ↓
Modbus B0..B7
```

---

## 3. A — Classification de persistance

Classes utilisées :

- `P` : persistant par nature / requis par la sémantique existante ;
- `PF` : persistance retenue par politique firmware TR2 ;
- `V` : volatile ;
- `R` : reconstructible.

Une donnée peut être `P/R` si l'autorité persistante permet de la reconstruire sans conserver directement sa projection.

### 3.1 Identité

- `device_id` : `P` ;
- identité provisionnée : `P` ;
- versions HW/FW/protocole : `R` ;
- capabilities : `R`.

### 3.2 Configuration B4

- prepared configuration : `V` ;
- validated prepared configuration : `V` ;
- active configuration : `PF` ;
- `active_config_id` : `P/R` ;
- `active_config_crc` : `R` ;
- `config_revision_counter` : `PF` ;
- `config_state` : `R` ;
- `config_error_code` : `V` sauf justification explicite ultérieure.

### 3.3 Temps B2

- prepared time : `V` ;
- prepared time status : `R/V` ;
- RTC courant : hors copie NVM applicative ;
- dernier succès de synchronisation / source : `PF` ;
- qualité instantanée / drift : `R/V` ;
- `time_since_sync` : `R`.

### 3.4 Campagnes B6

- données de campagne : `P` ;
- inventaire : `P/R` ;
- métadonnées : `P` ;
- `campaign_id` : `P` ;
- contexte/configuration historique : `P` ;
- état d'une campagne ouverte : `P/R` ;
- marqueur d'interruption : `PF` ;
- `selected_campaign_index` : `V` ;
- `selected_campaign_valid` : `R` ;
- compteurs globaux : `R`.

### 3.5 Commandes B5

- mailbox : `V` ;
- submit/cancel request : `V` ;
- état runtime de commande active : `V` avec éventuelle trace de recovery ;
- journal d'exécution durable : `PF` ;
- dernière commande terminée : `PF` ;
- historique minimal / idempotence : `PF` ;
- identité canonique des transactions admises : `PF` ;
- résultat final nécessaire à l'idempotence : `PF` ;
- `CommandRecoveryContext` minimal lorsque requis par la commande : `PF` jusqu'à finalisation/compaction de la transaction.

### 3.6 Diagnostic B7

- conditions actives / fault flags / health : `R` ;
- `last_fault_code` / timestamp associé : `PF` ;
- selftest en cours : `V` ;
- dernier selftest terminé : `PF` candidat retenu ;
- `reset_cause` : `R` depuis le contexte de boot ;
- `uptime_s` : `V/R` depuis MonotonicClock ;
- température / tension instantanées : `V/R`.

### 3.7 Invariants A

- **A-PERSIST-01** — Les zones de staging Modbus ne constituent jamais par elles-mêmes un état métier persistant.
- **A-PERSIST-02** — On persiste l'autorité métier minimale ; une projection reconstructible n'est pas persistée sans besoin explicite.

Restent ouverts : mécanisme exact de génération `campaign_id`, périmètre exact des statistiques persistantes et représentation physique du stockage transactionnel B5.

La profondeur/rétention logique B5 est désormais définie par la politique firmware `lifetime strict` décrite dans `ARCHITECTURE_FIRMWARE_SERVICES_MODBUS_V1.md` : aucune identité transactionnelle admise n'est volontairement rendue libre en V1.

---

## 4. B — ActiveConfiguration après reboot

**FW_POLICY** : l'`ActiveConfiguration` survit au reboot ; les zones prepared/validated ne survivent pas.

Séquence :

```text
ConfigurationStore
→ lecture des candidats
→ validation structure / version / intégrité
→ validation métier
→ sélection de la dernière génération autoritative valide
→ construction d'un ActiveConfigurationSnapshot immuable
→ publication atomique
→ reconstruction de la projection B4
```

Aucune configuration partiellement récupérée n'est publiée.

Si aucune configuration valide n'existe, le firmware ne transforme pas silencieusement cette situation en configuration active par défaut, sauf politique distincte explicitement définie plus tard.

Le CRC B4 reste calculé sur la représentation normative B4 ; il n'est pas confondu avec l'intégrité physique du record persistant.

---

## 5. C — Campagne interrompue

**FW_POLICY** : une campagne ouverte au moment d'un reset/coupure n'est jamais reprise automatiquement.

Le recovery :

- conserve `campaign_id` ;
- conserve le contexte historique propre à la campagne ;
- récupère uniquement les données durablement validables ;
- identifie le dernier préfixe de données valide ;
- établit un état historique interrompu / dégradé en utilisant uniquement les sémantiques B6 existantes ;
- n'invente ni `end_timestamp` ni `duration_s` lorsqu'ils ne sont pas démontrables ;
- ne transforme jamais la configuration active courante en contexte historique de la campagne.

Le curseur `selected_campaign_index` reste volatile.

---

## 6. D — Idempotence B5 après reboot

Le `CommandJournal` est persistant par politique firmware TR2.

Le détail de l'architecture transactionnelle B5, de la politique `lifetime strict`, de l'identité canonique de requête et des `CommandRecoveryContext` est défini dans la section H de `ARCHITECTURE_FIRMWARE_SERVICES_MODBUS_V1.md`.

Le présent document fixe uniquement les conséquences nécessaires au boot et au recovery.

### 6.1 États transactionnels récupérés

Les états conceptuels internes sont :

```text
RESERVED
STARTED
COMPLETED
```

Ils ne constituent pas de nouveaux états Modbus.

Au boot :

```text
RESERVED
→ absence d'effet significatif prouvée
→ aucune reprise / aucun redispatch
→ finalisation recovery durable

STARTED
→ effet possible
→ réconciliation avec les autorités métier déjà récupérées
→ TERMINAL_EFFECT_PROVEN / ABSENCE_PROVEN / INDETERMINATE
→ aucun redispatch

COMPLETED
→ résultat final transactionnel durable
→ aucune réévaluation métier
→ restauration de l'idempotence et des projections B5
```

Une transaction admise n'est jamais transformée en identité libre par le recovery.

### 6.2 RecoveryContext

Lorsqu'une commande nécessite une preuve de corrélation métier, le `CommandRecoveryContext` correspondant est récupéré avec le journal.

Ce contexte ne constitue jamais l'autorité métier.

Il ne sert qu'à relier la transaction aux faits détenus par les repositories/services métier déjà récupérés.

Un contexte absent, corrompu ou insuffisant ne permet jamais d'inventer une conclusion positive ; le résultat reste au minimum `INDETERMINATE`.

### 6.3 Corruption

`ABSENT`, `CORRUPTED`, `UNAVAILABLE` et `UNSUPPORTED` restent distincts au niveau du stockage transactionnel.

Une preuve transactionnelle corrompue ou indéterminée n'est jamais considérée comme un txid libre.

Si l'identité d'une transaction reste démontrable mais que son résultat final est perdu, elle reste protégée contre tout replay.

Invariants principaux :

- **D-CMD-01** — L'historique nécessaire au recovery/idempotence est persistant.
- **D-CMD-02** — Une transaction capable d'effet métier est durablement identifiée avant le premier effet métier non trivial.
- **D-CMD-03** — Un résultat final B5 n'est publié final qu'après durabilité du résultat d'idempotence correspondant.
- **D-CMD-04** — Une transaction `STARTED` trouvée au boot n'est jamais automatiquement rejouée.
- **D-CMD-05** — Un résultat indéterminé reste protégé contre un replay aveugle.
- **D-CMD-06** — Le journal conserve assez d'identité de requête pour détecter un même txid associé à une requête différente ; la réponse protocolaire exacte reste `NOT_DEFINED V1` lorsqu'elle n'est pas spécifiée.
- **D-CMD-07** — Le CommandEngine reconcile avec les autorités métier persistantes ; il ne déduit pas seul l'effet métier.
- **D-CMD-08** — La mailbox B5 n'est jamais restaurée ni resoumise au boot.
- **D-CMD-10** — Historique corrompu et historique absent restent distincts.
- **D-CMD-11** — Une commande provoquant un reboot laisse une trace durable avant déclenchement du reset.
- **D-CMD-12** — L'objectif est « no blind replay + deterministic recovery when provable », pas une promesse générique « exactly once ».
- **D-CMD-13** — Une transaction `RESERVED` valide trouvée au boot prouve que son effet significatif n'a pas commencé.
- **D-CMD-14** — Une transaction `COMPLETED` valide n'est jamais revalidée depuis l'état métier courant.
- **D-CMD-15** — Aucun txid durablement admis n'est rendu libre par reboot, recovery, corruption ou résultat indéterminé.
- **D-CMD-16** — Les preuves de corrélation requises sont récupérées avant toute conclusion de réconciliation.
- **D-CMD-17** — Un état métier simplement compatible avec la commande ne constitue pas à lui seul une preuve causale.

---

## 7. E — Diagnostic après reboot

- **E-DIAG-01** — `last_fault_code` est conservé comme historique minimal persistant.
- **E-DIAG-02** — `last_fault_code` et son timestamp forment un record cohérent.
- **E-DIAG-03** — aucun timestamp civil n'est inventé si la base temporelle utilisable est absente ; la projection exacte reste `NOT_DEFINED V1` si nécessaire.
- **E-DIAG-04** — les conditions actives sont réévaluées au boot ; l'ancienne projection B7 n'est jamais restaurée comme autorité.
- **E-DIAG-05** — la paire historique est persistée atomiquement : ancien record complet ou nouveau record complet.
- **E-DIAG-06** — selftest `in progress` est volatile.
- **E-DIAG-07** — un selftest interrompu par reboot n'est pas déclaré automatiquement en échec.
- **E-DIAG-08** — les checks techniques de boot ne sont pas automatiquement assimilés au selftest métier B7.
- **E-DIAG-09** — absence d'historique et corruption d'historique restent distinctes.
- **E-DIAG-10** — le dernier selftest complètement terminé peut être restauré ; les catalogues V1 ne sont pas étendus implicitement.
- **E-DIAG-11** — les compteurs runtime purs restent volatils ; les statistiques persistantes restent un sujet séparé.
- **E-DIAG-12** — `reset_cause` et `last_fault_code` sont indépendants.

---

## 8. F — Reset cause et uptime

Le catalogue B7 existant reste la seule projection normative de cause de reset.

### 8.1 Reset cause

- **F-RESET-01** — les indicateurs matériels sont capturés avant toute initialisation susceptible de les modifier/effacer.
- **F-RESET-02** — le HAL expose des faits bruts ; B7 ne lit jamais directement les registres matériels.
- **F-RESET-03** — un `BootIntent` persistant peut raffiner une cause matérielle lorsqu'il est nécessaire de distinguer des causes normatives.
- **F-RESET-04** — le `BootIntent` n'est valable que pour le boot attendu suivant et est consommé/invalide ensuite.
- **F-RESET-05** — le `BootIntent` ne remplace jamais une cause matérielle contradictoire ; il ne fait que raffiner une cause compatible.
- **F-RESET-06** — `reset_cause` ne modifie pas automatiquement l'historique `last_fault`.
- **F-RESET-07** — toute ambiguïté non résoluble produit `UNKNOWN` plutôt qu'une cause supposée.
- **F-RESET-08** — la cause normalisée est immuable pendant l'instance runtime courante.

La priorité exacte entre plusieurs flags matériels simultanés reste une politique plateforme à documenter ; elle n'est pas une exigence V1.

### 8.2 Uptime

- **F-UPTIME-01** — `uptime_s` repose exclusivement sur MonotonicClock.
- **F-UPTIME-02** — son époque correspond au boot/runtime et non à l'ouverture du serveur Modbus.
- **F-UPTIME-03** — `uptime_s` n'est jamais restauré depuis NVM.
- **F-UPTIME-04** — l'implémentation empêche tout retour arrière dû au wrap interne sur la durée d'exécution supportée.
- **F-BOOT-01** — `reset_cause` et `uptime_s` décrivent la même instance de boot.

---

## 9. G — Ordre logique de boot

Ordre logique retenu :

```text
RESET
│
├─ G0  minimal platform / monotonic clock
├─ G1  capture reset cause + BootContext
├─ G2  initialize PersistentStorageCore
├─ G3  recover identity
├─ G4  initialize TimeService
├─ G5  recover ActiveConfiguration
├─ G6  recover CampaignRepository / StorageService
├─ G7  recover + reconcile B5 CommandJournal
├─ G8  initialize runtime services
├─ G9  restore diagnostic history + evaluate active conditions
├─ G10 build initial coherent snapshots B0..B7
├─ publication barrier: SYSTEM_READY_FOR_MODBUS
├─ start Modbus transport/server
├─ enable B5 new request acceptance
└─ RUN
```

Invariants :

- **G-BOOT-01** — établir d'abord le contexte plateforme minimal, reset cause et base monotone.
- **G-BOOT-02** — initialiser le stockage générique avant les repositories métier persistants.
- **G-BOOT-03** — établir l'identité avant les objets qui dépendent de `device_id`.
- **G-BOOT-04** — un WallClock invalide ne bloque pas le boot global.
- **G-BOOT-05** — l'absence de configuration valide n'empêche pas le système de devenir diagnostiquable.
- **G-BOOT-06** — recovery campagne uniquement depuis son contexte historique propre.
- **G-BOOT-07** — reconciliation B5 après récupération des autorités métier nécessaires.
- **G-BOOT-07A** — les `CommandRecoveryContext` nécessaires sont récupérés et validés avant conclusion de la réconciliation B5.
- **G-BOOT-07B** — aucune transaction durablement admise n'est interprétée comme nouvelle ou libre pendant le recovery.
- **G-BOOT-08** — aucun service runtime ne déclenche automatiquement une action métier lors de son initialisation.
- **G-BOOT-09** — les conditions diagnostiques actives sont évaluées après disponibilité de leurs producteurs.
- **G-BOOT-10** — aucune projection Modbus initiale ne dérive d'un recovery encore incomplet.
- **G-BOOT-11** — aucune nouvelle transaction B5 n'est acceptée avant recovery/idempotence terminé.
- **G-BOOT-12** — une panne métier conduit autant que possible à un état dégradé diagnostiquable plutôt qu'à un blocage total.
- **G-BOOT-13** — une barrière explicite sépare recovery interne et disponibilité Modbus.
- **G-BOOT-14** — le serveur Modbus est ouvert seulement lorsque les snapshots initiaux nécessaires sont cohérents.

L'ordre G exprime des dépendances logiques et des barrières de publication ; il n'interdit pas des lectures/initialisations techniques parallèles indépendantes.

---

## 10. H — Recovery et publication atomique

Pattern général :

```text
PERSIST
→ VALIDATE
→ RECOVER DOMAIN STATE
→ FREEZE
→ ATOMIC PUBLISH
→ DERIVE READ MODELS
→ ATOMIC PUBLISH MODBUS SNAPSHOTS
```

Invariants :

- **H-ATOM-01** — aucun record persistant ne devient autorité avant validation complète.
- **H-ATOM-02** — tout état logique multi-champs est publié de manière cohérente.
- **H-ATOM-03** — les read-models sont remplacés par snapshots complets, jamais mutés progressivement sous les lecteurs.
- **H-ATOM-04** — l'atomicité porte sur les objets/projections concernés ; aucune transaction globale B0…B7 n'est requise.
- **H-ATOM-05** — le premier état Modbus post-boot n'est publié qu'après la barrière de recovery.
- **H-ATOM-06** — la sélection d'une génération se fait seulement entre candidats complètement valides.
- **H-ATOM-07** — une ambiguïté persistante contradictoire n'est jamais résolue arbitrairement.
- **H-ATOM-08** — pas de transaction globale multi-services obligatoire ; commits locaux atomiques + reconciliation.
- **H-ATOM-09** — l'état métier est publié avant toute projection Modbus dérivée.
- **H-ATOM-10** — en RUN, l'ancien snapshot reste visible jusqu'à complétion du nouveau.
- **H-ATOM-11** — une réponse Modbus multi-registres utilise un seul snapshot stable.
- **H-REC-01** — le recovery est idempotent face à des resets répétés pendant le recovery.
- **H-REC-02** — aucune destruction de la seule copie exploitable avant commit d'un remplacement sûr.
- **H-REC-03** — un recovery techniquement réussi ne masque pas l'état historiquement interrompu/dégradé.
- **H-RT-01** — les opérations lentes de persistance/recovery ne maintiennent pas de verrou durable sur les chemins temps réel ou Modbus.

---

## 11. I — Architecture du stockage persistant

Architecture logique :

```text
                 DOMAIN
                   │
      ┌────────────┼─────────────┐
      │            │             │
Configuration   Command       Campaign
   Store        Journal      Repository
      │            │             │
      ├────────────┼─────────────┤
      │ Diagnostic │ Time        │
      │ History    │ History     │
      └────────────┼─────────────┘
                   │
        PersistentStorageCore
             /           \
   CriticalRecordMedia   BulkDataMedia
           │                  │
          HAL                HAL
```

`CriticalRecordMedia` et `BulkDataMedia` sont des rôles architecturaux, pas des composants imposés.

### 11.1 PersistentStorageCore

Fournit uniquement des mécanismes génériques : lecture, écriture, erase, flush/sync, primitives d'intégrité/atomicité, état du média. Il ne connaît aucune sémantique métier.

### 11.2 Stores métier

- `IdentityStore` ;
- `ConfigurationStore` ;
- `CampaignRepository` ;
- `CommandJournal` ;
- `DiagnosticHistoryStore` ;
- `TimeHistoryStore` ;
- `StatisticsStore` seulement si son périmètre est défini ultérieurement.

### 11.3 Invariants I

- **I-STOR-01** — aucun service métier n'accède directement au média physique.
- **I-STOR-02** — `PersistentStorageCore` fournit des mécanismes, jamais des règles métier.
- **I-STOR-03** — les autorités persistantes restent transactionnellement séparées sauf besoin explicite contraire.
- **I-STOR-04** — état critique compact et données volumineuses de campagne sont deux classes distinctes architecturalement.
- **I-STOR-05** — la perte du stockage de campagne ne doit pas entraîner par construction la perte de toute persistance critique.
- **I-STOR-06** — tout record critique possède type, version et protection d'intégrité explicites.
- **I-STOR-07** — version de stockage, version firmware et version Modbus sont indépendantes.
- **I-STOR-08** — les familles persistantes peuvent évoluer avec leurs propres versions.
- **I-STOR-09** — une version inconnue n'est jamais interprétée comme le format courant.
- **I-STOR-10** — migration `copy-before-retire`.
- **I-STOR-11** — aucune struct RAM native n'est utilisée comme format NVM implicite.
- **I-STOR-12** — toute persistance haute fréquence doit être justifiée par un besoin réel de recovery.
- **I-STOR-13** — la corruption est confinée autant que possible à la famille concernée.
- **I-STOR-14** — santé physique du support et validité logique du record sont distinctes.
- **I-STOR-15** — le commit possède une sémantique de durabilité réelle, pas seulement « copié dans un buffer ».
- **I-STOR-16** — garbage collection/nettoyage recovery-safe.
- **I-STOR-17** — `EMPTY`, `CORRUPTED`, `UNAVAILABLE` et, lorsqu'applicable, `UNSUPPORTED` restent distincts.
- **I-CONF-01** — `ConfigurationStore` ne persiste que l'autorité active, jamais le staging B4.
- **I-CAMP-01** — identité/métadonnées de campagne et flux volumineux sont logiquement distincts.
- **I-CAMP-02** — les données campagne sont structurées en unités permettant de retrouver le dernier préfixe durable valide.
- **I-CAMP-03** — progression append/checkpoint plutôt que réécriture globale permanente.
- **I-CMD-01** — `CommandJournal` indépendant de la mailbox et de l'état métier commandé.
- **I-DIAG-01** — seuls les faits diagnostiques historiques retenus sont persistés.
- **I-TIME-01** — seules les informations historiques de synchronisation utiles sont persistées, jamais une copie périodique de l'heure courante.

Le support exact, filesystem, taille de slots/chunks, algorithme de GC et format binaire détaillé restent des choix d'implémentation.

---

## 12. J — Stratégie de validation fault injection

La stratégie détaillée est portée par le document compagnon :

`RECOVERY_FAULT_INJECTION_MATRIX.md`

Principes :

- chaque oracle est étiqueté `V1`, `FW_POLICY` ou `IMPLEMENTATION` ;
- injection aux frontières de write/commit/publication/résultat ;
- aucun registre Modbus spécial de test ;
- injection via les interfaces HAL/store ;
- vérification d'ensembles d'états sûrs autorisés plutôt que de timings arbitraires ;
- second reboot normal après recovery dans les scénarios critiques.

---

## 13. Invariants transversaux gelés

- **TR-REC-01** — Une information n'est autoritative après reboot que si sa durabilité et son intégrité sont démontrables ; le recovery ne déduit jamais un effet métier depuis une ancienne projection Modbus ou un état volatile.
- **TR-REC-02** — Le `CommandJournal` apporte preuve transactionnelle et idempotence B5 mais n'est jamais l'autorité de l'état métier commandé.
- **TR-REC-03** — Le recovery d'un objet historique utilise uniquement son propre contexte durable ; aucun état courant post-reboot ne reconstruit rétroactivement son histoire.
- **TR-TIME-01** — La capacité de recovery ne dépend jamais de la validité du WallClock, sauf lorsqu'une donnée métier exige intrinsèquement une information civile déjà persistée.
- **TR-REC-04** — `EMPTY`, `CORRUPTED`, `UNAVAILABLE` et `UNSUPPORTED` ne sont jamais normalisés silencieusement vers un même état vide.
- **TR-BOOT-01** — L'ordre G exprime les dépendances logiques/barrières, pas une obligation de sérialisation CPU.
- **TR-PUB-01** — La cohérence transversale repose sur les autorités et snapshots stables, pas sur une transaction globale B0…B7.

---

## 14. Points volontairement ouverts

Les sujets suivants restent explicitement non gelés par cette architecture :

- cycle de vie normatif, réutilisation et wrap des transaction IDs ;
- comportement protocolaire explicite en cas d'épuisement des 65535 identifiants ;
- comportement protocolaire exact « même txid, requête différente » lorsqu'il n'est pas défini par V1 ;
- représentation B5 d'une transaction interrompue avant effet ;
- représentation B5 d'un résultat post-crash `INDETERMINATE` ;
- représentation d'un timestamp final indisponible ;
- mécanisme exact de génération/récupération de `campaign_id` ;
- périmètre exact des statistiques persistantes et de `RESET STATISTICS` ;
- priorité précise entre plusieurs causes matérielles de reset concurrentes ;
- comportement exact à saturation de certains compteurs si la V1 ne le définit pas ;
- type de NVM, filesystem, layout physique, taille des slots/chunks, fréquence des checkpoints, algorithme de GC ;
- politique cryptographique éventuelle de confidentialité/authenticité, hors baseline V1 actuelle.

La profondeur/rétention logique du `CommandJournal` n'est plus ouverte : la politique firmware V1 `lifetime strict` conserve toute identité transactionnelle admise. La représentation physique de cette politique reste un choix d'implémentation.

Ces points doivent rester `NOT_DEFINED V1`, `FW_POLICY à définir` ou `IMPLEMENTATION` selon leur nature.

---

## 15. Conséquence pour l'implémentation

Le firmware doit pouvoir être implémenté et testé autour du pattern suivant :

```text
BOOT:
  capture BootContext
  initialize persistence
  recover authoritative states
  reconcile B5
  evaluate diagnostics
  build immutable snapshots
  cross publication barrier
  enable Modbus

RUN:
  build candidate privately
  validate
  persist/commit if authoritative state is durable
  atomically publish domain state
  rebuild affected read models
  atomically publish snapshots
```

Aucune décision de ce document n'autorise à étendre silencieusement la spécification Modbus RTU V1.
