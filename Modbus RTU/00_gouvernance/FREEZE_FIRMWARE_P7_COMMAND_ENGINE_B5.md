# Gel firmware P7 — Command Engine transactionnel B5

## 1. Statut

La tranche firmware **P7 — Command Engine / bloc B5** est considérée comme implémentée et validée sur la branche `main`.

Baseline de validation fonctionnelle avant le présent gel :

- commit : `cbcac720b0eadbd6e46db195ccfcefdad1091ed0` ;
- validation locale : `./tr2_validate.sh` ;
- résultat : **60/60 tests réussis** ;
- runtime Host : `TR2 P0 host runtime ready`.

Le présent document est un constat de gel d’implémentation. Il ne modifie pas la spécification Modbus RTU V1 et ne transforme aucune décision V1.1 en exigence V1.

## 2. Périmètre gelé

P7 couvre le chemin transactionnel B5 suivant :

```text
B5 request registers
→ CommandRequestMailbox volatile
→ submit rising edge
→ immutable CommandRequest
→ CommandEngine
→ CommandJournal persistent
→ services métier autorisés
→ CommandSnapshot
→ projection B5
```

Sont gelés dans P7 :

- types domaine de commande et identité canonique de requête ;
- mailbox volatile B5 et capture sur front montant de `submit` ;
- journal transactionnel persistant ;
- admission NEW / RETRY / COLLISION / BUSY ;
- cycle interne `RESERVED → STARTED → COMPLETED` ;
- barrières de persistance avant effet métier et avant publication terminale ;
- contextes de recovery nécessaires aux commandes implémentées ;
- exécution et réconciliation des commandes V1 actuellement raccordées :
  - 1 `APPLY_CONFIG` ;
  - 2 `SYNC_TIME` ;
  - 3 `START_ACQUISITION` ;
  - 4 `STOP_ACQUISITION` ;
- recovery au boot des transactions `RESERVED`, `STARTED` et `COMPLETED` ;
- projection B5 V1 et adaptateurs de lecture/écriture ;
- politique V1 des paramètres, confirmation et annulation non supportée ;
- fault injection aux barrières transactionnelles critiques ;
- intégration du CommandJournal et de la réconciliation B5 dans `SystemRuntime` avant readiness Modbus.

## 3. Invariants transactionnels gelés

### 3.1 Admission et idempotence

- `transaction_id = 0` est invalide.
- Une requête rejetée au niveau Modbus ne crée pas d’entrée d’idempotence.
- Un nouveau transaction_id valide doit devenir `RESERVED` de manière durable avant admission active.
- Même transaction_id + même identité canonique = retry, sans redispatch métier.
- Même transaction_id + identité différente = collision, sans redispatch métier.
- La politique V1 implémentée reste **lifetime-strict** sur les transaction_id 1..65535 : aucune réutilisation, éviction ou remise à zéro implicite n’est introduite.

### 3.2 Barrières de persistance

L’ordre gelé est :

```text
capture
→ RESERVED durable
→ recovery context durable si requis
→ STARTED durable
→ premier effet métier significatif autorisé
→ COMPLETED / résultat durable
→ publication terminale B5
```

Conséquences vérifiées par fault injection :

- échec de persistance `RESERVED` : transaction non admise et non connue après reboot ;
- échec du recovery context : retour durable à `RESERVED` sans contexte ;
- échec de persistance `STARTED` : contexte durable conservé mais aucun franchissement durable de la barrière d’effet ;
- échec de persistance `COMPLETED` : état durable `STARTED`, aucun faux résultat terminal publiable avant recovery et aucun résultat final récupéré.

### 3.3 Boot / recovery

Au boot runtime :

```text
initialisation persistance
→ recovery Time
→ recovery Configuration
→ recovery Campaign
→ recovery CommandJournal
→ réconciliation transactionnelle B5
→ initialisation/restauration CommandEngine
→ projections B4/B5/B6
→ system_ready_for_modbus = true
```

Une transaction incomplète récupérée reste consommée et ne peut pas être contournée par l’admission d’une nouvelle transaction.

Aucune commande métier n’est automatiquement rejouée au boot.

Un résultat de réconciliation `INDETERMINATE` ne libère jamais le transaction_id et ne provoque jamais de redispatch automatique.

P7 n’invente aucun code résultat ou statut Modbus V1 pour les situations dont la représentation exacte reste `NOT_DEFINED` dans la baseline V1.

## 4. Autorités métier

Le CommandEngine et le CommandJournal ne deviennent pas des copies d’autorité métier.

Les preuves de recovery sont corrélées aux autorités existantes :

- configuration active : `ConfigurationService` ;
- synchronisation temporelle : `TimeService` ;
- campagne/acquisition : `CampaignService` et `CampaignRepository` ;
- persistance transactionnelle : `CommandJournal`.

Les `CommandRecoveryContext` servent uniquement de preuve/corrélation de transaction.

## 5. Projection B5 V1

La projection B5 couvre les registres 5000..5019 de la baseline V1 :

- 5000..5007 : mailbox/request ;
- 5008..5013 : transaction active / acquittement / flags moteur ;
- 5014..5019 : dernière transaction logiquement complétée.

Les écritures sont limitées aux registres RW prévus. Les bits réservés du contrôle sont rejetés par l’adaptateur B5.

La projection masque les bits `cmd_engine_flags` réservés V1.

Le timestamp terminal est celui de l’événement original lorsqu’il existe ; aucun timestamp civil n’est inventé si l’autorité temps n’était pas utilisable au moment causal.

## 6. Frontière V1 / V1.1

P7 ne publie pas dans le protocole V1 :

- `transaction_epoch` ;
- registres B5 étendus 5020..5028 ;
- commande 12 ;
- `cmd_status = 9` ;
- result codes 23..29 ;
- `LAST_TIMESTAMP_VALID` en bit 11 ;
- résultat V1.1 `TRANSACTION_IDENTITY_COLLISION`.

Les documents V1.1 restent des extensions séparées et ne remplacent pas la baseline V1.

## 7. Commandes non raccordées

Les codes de commande V1 5..11 restent reconnus par le modèle de commande lorsque nécessaire au protocole, mais P7 ne leur attribue pas artificiellement une réussite métier.

En particulier, aucune autorité métier fictive n’est ajoutée pour :

- SELFTEST ;
- ACK_FAULT ;
- REFRESH_INDICATORS ;
- ENTER_MAINTENANCE ;
- EXIT_MAINTENANCE ;
- SOFTWARE_RESET ;
- RESET_STATISTICS.

Leur raccordement métier relève d’une tranche ultérieure avec contrat d’autorité explicite.

## 8. Validation et non-régression

La validation finale de la baseline précédant ce document a donné :

```text
100% tests passed, 0 tests failed out of 60
Total Test time (real) = 13.84 sec
TR2 P0 host runtime ready
TR2 validation complete.
```

Cette validation inclut les tests P1 à P6 existants ainsi que P7-A à P7-O.

Le test exhaustif de fault injection Configuration conserve la couverture des écritures partielles, mais la boucle exhaustive travaille au niveau `PersistentStorageCore → ConfigurationStore → ConfigurationService` afin de ne pas répéter artificiellement un scan complet du journal B5 à chaque octet. Les scénarios de reboot d’intégration restent exécutés via `SystemRuntime`.

## 9. Décision de gel

À compter de ce gel :

- P7 est une baseline firmware validée ;
- toute évolution transactionnelle ultérieure doit préserver les invariants ci-dessus ou faire l’objet d’un arbitrage explicite ;
- les extensions V1.1 ne doivent pas être introduites silencieusement dans le protocole V1 ;
- la poursuite recommandée est le raccordement des autorités métier manquantes et/ou la tranche firmware suivante définie par le plan d’implémentation courant.
