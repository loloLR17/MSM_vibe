# Recovery fault injection matrix — Firmware TR2 Modbus RTU V1

## 1. Statut

Ce document est le compagnon de :

`ARCHITECTURE_FIRMWARE_BOOT_PERSISTENCE_RECOVERY.md`

Il transforme les invariants Boot / Persistance / Recovery en scénarios de validation falsifiables.

Il ne crée aucune exigence Modbus V1 supplémentaire.

Chaque oracle doit être classé :

- **V1** : exigence normative existante ;
- **FW_POLICY** : politique firmware TR2 gelée ;
- **IMPLEMENTATION** : propriété liée au mécanisme choisi.

Aucun registre Modbus spécifique de test n'est autorisé. Les perturbations doivent être injectées via les abstractions de plateforme/store prévues par l'architecture.

---

## 2. Principe générique d'injection

Pour une opération persistante critique :

```text
candidate built
↓ P1
write begins
↓ P2
payload complete
↓ P3
integrity metadata complete
↓ P4
commit marker / durable commit requested
↓ P5
durability confirmed
↓ P6
domain state published
↓ P7
Modbus/read-model snapshot published
```

L'implémentation doit permettre, lorsque ces frontières existent réellement, d'injecter une interruption :

- avant P1 ;
- P1→P2 ;
- P2→P3 ;
- P3→P4 ;
- P4→P5 ;
- P5→P6 ;
- P6→P7 ;
- après P7.

L'oracle général est : **état ancien valide ou état nouveau complètement committed, jamais état hybride**.

---

## 3. Harness et interfaces

La fault injection doit utiliser les interfaces normales du firmware, par exemple :

```text
IPersistentStore
IStorage
IResetController
IClock
IMonotonicClock
IVibrationSource
ISystemMetrics
IFaultSource
```

Doubles/fakes possibles :

```text
FakePersistentStore
FakeBulkStorage
FakeResetController
FakeClock
FakeFaultSource
```

Capacités utiles :

- failure after N bytes ;
- failure before/after flush ;
- corruption on next read ;
- missing media ;
- forced watchdog ;
- brown-out reset flags ;
- invalid RTC ;
- reset at named commit boundary.

Ces mécanismes sont réservés au harness de test et ne modifient pas le contrat Modbus.

---

## 4. Matrice configuration

### J-CONF-01 — Coupure pendant persistence d'une ActiveConfiguration

**Classe** : FW_POLICY

Précondition :

```text
ActiveConfiguration = A
candidate = B
```

Injection : chaque frontière de persistence de B.

Oracle autorisé après reboot :

```text
A complet et valide
OU
B complet, valide et committed
```

Interdits :

- mélange A/B ;
- payload incomplet publié ;
- CRC/projection B4 incohérent ;
- staging prepared/validated restauré ;
- candidat corrompu choisi uniquement car sa génération est plus récente.

### J-CONF-02 — Coupure après commit métier, avant résultat final B5

**Classe** : FW_POLICY

Scénario :

```text
B5 APPLY CONFIG tx42
→ B committed
→ power loss
→ result B5 not committed
```

Oracle :

- B reste l'autorité active ;
- tx42 n'est pas rejouée aveuglément ;
- le CommandEngine reconcile avec ConfigurationStore ;
- aucune double application ;
- le résultat protocolaire exact n'est pas inventé s'il reste NOT_DEFINED V1.

---

## 5. Matrice campagnes

### J-CAMP-01 — Coupure pendant création

**Classe** : FW_POLICY

Injection : allocation ID, écriture métadonnées initiales, création zone données, commit ouverture.

Oracle :

- campagne inexistante si rien d'autoritatif n'a été committed ;
- campagne cohérente si ouverture committed ;
- éventuellement campagne historiquement interrompue si l'ouverture était durable ;
- jamais objet partiel exposé comme campagne valide ;
- aucune collision de `campaign_id` entre campagnes simultanément présentes dans l'inventaire.

### J-CAMP-02 — Coupure pendant acquisition

**Classe** : FW_POLICY + V1 pour les champs B6 concernés

Scénario :

```text
chunks 1..N committed
chunk N+1 in progress
→ power loss
```

Oracle :

- chunks 1..N conservés ;
- N+1 rejeté s'il n'est pas démontrablement complet ;
- `campaign_id` conservé ;
- contexte historique conservé ;
- aucune reprise automatique ;
- aucune durée ni timestamp inventé ;
- état B6 limité aux sémantiques V1 existantes.

### J-CAMP-03 — Coupure pendant finalisation

**Classe** : FW_POLICY + V1 pour cohérence B6

Injection : après dernier chunk, pendant end metadata, intégrité, commit terminal.

Oracle :

- soit campagne encore non finalisée/interrompue de manière cohérente ;
- soit campagne complètement finalisée ;
- jamais `completed` avec métadonnées terminales contradictoires ;
- aucun `end_timestamp` fabriqué.

### J-CAMP-04 — Corruption métadonnées campagne

**Classe** : FW_POLICY

Corruptions : header, index chunks, terminal metadata, contexte historique.

Oracle :

- salvage uniquement de ce qui est prouvable ;
- aucune reconstruction imaginative ;
- état dégradé/corrompu conservé lorsque représentable ;
- données brutes valides ne suffisent pas à inventer une finalisation perdue.

---

## 6. Matrice B5 / idempotence

### J-CMD-01 — Reset après STARTED, avant effet métier

**Classe** : FW_POLICY

Oracle :

- transaction connue ;
- aucun replay automatique ;
- reconciliation nécessaire avant toute décision.

### J-CMD-02 — Effet métier durable, résultat B5 absent

**Classe** : FW_POLICY

Oracle :

- l'autorité métier prouve l'effet ;
- le journal incomplet est reconcilié ;
- pas de répétition de l'effet.

### J-CMD-03 — Résultat durable, réponse Modbus perdue

**Classe** : V1/FW_POLICY selon la règle B5 existante et la fenêtre de rétention retenue

Oracle :

- même txid connu → pas de nouvel effet métier ;
- résultat historique cohérent restitué dans la fenêtre d'idempotence effectivement supportée.

### J-CMD-04 — Même txid, requête différente

**Classe** : FW_POLICY pour détection ; NOT_DEFINED V1 pour la réponse exacte si non spécifiée

Oracle :

- contradiction détectée via request fingerprint ;
- jamais interprétée silencieusement comme répétition équivalente.

---

## 7. Matrice reset cause / BootIntent

### J-RST-01 — RESET SOFTWARE B5

**Classe** : V1 pour projection reset cause + FW_POLICY pour chaîne durable

Séquence :

```text
persist command recovery record
→ persist BootIntent SOFTWARE_RESET
→ ensure durability
→ trigger HAL reset
→ boot
```

Oracle :

- pas de reset avant durabilité minimale ;
- `reset_cause = SOFTWARE_RESET` lorsque la preuve plateforme + intent est cohérente ;
- `uptime_s` repart pour le nouveau boot ;
- tx non rejouée aveuglément ;
- BootIntent consommé ;
- reboot suivant sans nouvelle intention non classé à tort SOFTWARE_RESET.

### J-RST-01B — Power loss après BootIntent, avant HAL software reset

**Classe** : FW_POLICY

Scénario :

```text
BootIntent SOFTWARE_RESET committed
→ power loss
→ next boot raw cause POWER_ON
```

Oracle :

- `reset_cause = POWER_ON` ;
- intent contradictoire rejeté/consommé ;
- aucune fausse cause SOFTWARE_RESET.

### J-RST-02 — Watchdog

**Classe** : V1 projection + FW_POLICY mapping plateforme

Oracle :

- `reset_cause = WATCHDOG` si preuve plateforme fiable ;
- uptime repart ;
- pas de conversion automatique vers `last_fault_code`.

### J-RST-03 — Brown-out

**Classe** : V1 projection + FW_POLICY mapping plateforme

Oracle :

- mapping documenté et déterministe ;
- si ambiguïté réellement non résoluble → `UNKNOWN` plutôt qu'une supposition.

### J-RST-04 — Power-on

**Classe** : V1 projection

Oracle :

- `POWER_ON` uniquement sur preuve plateforme de power-on/cold boot ;
- jamais utilisé comme fallback générique.

---

## 8. Matrice corruption NVM

### J-CORR-01 — Record critique corrompu

**Classe** : FW_POLICY

Corruptions à tester séparément :

- payload ;
- integrity field ;
- format version ;
- generation ;
- commit marker ;
- length.

Oracles :

```text
new generation invalid + previous valid
→ previous valid recovered
```

```text
all generations invalid
→ no authoritative state + diagnostic
```

Interdit : record corrompu publié comme autorité.

### J-CORR-02 — Générations contradictoires

**Classe** : FW_POLICY

```text
slot A gen 16 valid payload X
slot B gen 16 valid payload Y
```

Oracle :

- aucun winner arbitraire ;
- fallback vers une preuve antérieure seulement si cette politique est démontrable ;
- sinon absence d'autorité + diagnostic.

### J-CORR-03 — Format inconnu

**Classe** : FW_POLICY / IMPLEMENTATION

Oracle :

- version inconnue jamais interprétée comme format courant ;
- classée `UNSUPPORTED` ou équivalent interne.

---

## 9. Matrice stockage et temps

### J-STOR-01 — Stockage campagne absent au boot

**Classe** : FW_POLICY

Oracle :

- bulk storage = unavailable ;
- identité/configuration/journal critique restent disponibles si leur média est distinct ;
- Modbus peut atteindre un état READY dégradé lorsque le fonctionnement sûr minimal le permet ;
- `UNAVAILABLE` n'est jamais transformé implicitement en `EMPTY` en interne.

### J-TIME-01 — RTC invalide

**Classe** : FW_POLICY + V1 pour projections temporelles existantes

Oracle :

- boot non bloqué ;
- MonotonicClock opérationnelle ;
- uptime cohérent ;
- reset cause cohérente ;
- aucun timestamp civil inventé ;
- aucune campagne récupérée ne reçoit arbitrairement une heure de fin ;
- aucun `last_fault_timestamp` fabriqué.

### J-TIME-02 — RTC utilisable + historique sync valide + continuité prouvée

**Classe** : FW_POLICY + V1 pour la cohérence des projections existantes

Préconditions :

```text
WallClock technically usable
LastSyncHistory = VALID
continuity proof = positive
```

Oracle :

- `CONTINUITY_PROVEN` ;
- `civil_time_usable = true` si aucun autre fait technique ne l'interdit ;
- `sync_continuity_proven = true` ;
- `time_since_sync` reconstructible de manière cohérente ;
- `time_status = SYNCHRONIZED` autorisé sous les autres règles temporelles applicables ;
- aucun fait historique n'est recalculé depuis une projection B2 antérieure.

### J-TIME-03 — RTC utilisable + historique sync valide + continuité indéterminée

**Classe** : FW_POLICY

Préconditions :

```text
WallClock technically usable
LastSyncHistory = VALID
no positive continuity proof
no explicit rupture proof
```

Oracle :

- `CONTINUITY_INDETERMINATE` ;
- `civil_time_usable` peut rester vrai si l'RTC est techniquement utilisable ;
- `sync_continuity_proven = false` ;
- `TimeSinceSync = UNAVAILABLE` ;
- aucune soustraction `WallClock - last_sync_time` ;
- `time_status = 3` interdit ;
- `time_status = 2` est la projection FW_POLICY attendue lorsque le temps civil est utilisable et qu'aucune dégradation positive distincte n'impose un autre état ;
- `SYNC_PERFORMED = 1` si l'historique prouve une synchronisation effective ;
- `CONTINUITY_INDETERMINATE` seul n'impose jamais `DEGRADED`.

### J-TIME-04 — Historique sync valide + rupture temporelle prouvée

**Classe** : FW_POLICY

Préconditions :

```text
LastSyncHistory = VALID
RTC loss/reset/backup-domain rupture positively proven
```

Oracle :

- `CONTINUITY_BROKEN` ;
- l'historique de synchronisation reste conservé comme fait passé ;
- `sync_continuity_proven = false` ;
- `TimeSinceSync = UNAVAILABLE` ;
- aucun calcul depuis le RTC courant ;
- aucun timestamp historique durable n'est modifié rétroactivement ;
- toute éventuelle utilisabilité du temps civil courant est réévaluée indépendamment de l'ancien historique.

### J-TIME-05 — Aucune synchronisation historique

**Classe** : FW_POLICY + NOT_DEFINED V1 pour les sentinelles

Précondition :

```text
LastSyncHistory = NONE
```

Oracle interne :

- état distinct de toute corruption d'historique ;
- `SYNC_PERFORMED = 0` ;
- `TimeSinceSync = UNAVAILABLE` ;
- aucun faux `last_sync_time` n'est créé.

Projection firmware attendue tant que V1 ne définit pas d'indisponibilité explicite :

```text
last_sync_time    = 0x00000000
time_since_sync_s = 0xFFFFFFFF
```

Ces valeurs restent des conventions `FW_POLICY` de projection et non des sentinelles normatives V1.

### J-TIME-06 — Synchronisation réussie dans le boot courant puis ajustement WallClock

**Classe** : FW_POLICY

Scénario :

```text
boot
→ successful B5 SYNCHRONIZE
→ establish monotonic reference M0
→ later WallClock correction / resynchronization adjustment
```

Oracle :

- la durée depuis la synchronisation courante est suivie depuis `MonotonicClock` ;
- un ajustement de WallClock ne provoque ni retour arrière ni saut artificiel de `time_since_sync` ;
- `last_sync_time` ne change que sur une synchronisation effective conformément aux règles B2/B5 ;
- `MonotonicClock` n'est jamais modifiée par `WallClock.set`.

### J-TIME-07 — Stabilité du recovery temporel au second reboot

**Classe** : FW_POLICY

Pour chacun des scénarios J-TIME-02 à J-TIME-05 :

```text
establish recovered temporal state S
→ normal reboot without new temporal event/fault
→ establish S'
```

Oracle :

- aucun nouveau fait historique n'est inventé entre S et S' ;
- l'historique durable reste stable ;
- une absence de preuve ne devient pas spontanément une preuve positive ;
- aucune indisponibilité interne n'est remplacée par une valeur numérique interprétée comme autorité métier ;
- les projections B1/B2 restent cohérentes avec les faits du nouveau boot.

---

## 10. Matrice diagnostic

### J-DIAG-01 — Historique last fault corrompu

**Classe** : FW_POLICY

Oracle interne :

```text
NO_HISTORY ≠ CORRUPTED_HISTORY
```

La projection B7 doit rester limitée aux possibilités réellement définies par V1.

### J-DIAG-02 — Selftest interrompu

**Classe** : FW_POLICY + V1 états selftest existants

Scénario :

```text
selftest in progress
→ reset
```

Oracle :

- `in progress` non restauré ;
- pas de conversion automatique en failure ;
- dernier selftest complètement terminé éventuellement restauré ;
- checks techniques de boot non assimilés automatiquement au selftest B7.

---

## 11. Recovery du recovery

### J-REC-01 — Reset pendant recovery

**Classe** : FW_POLICY

Exemple :

```text
campaign recovery
→ recovered metadata write
→ second power loss
→ boot again
```

Oracle :

- convergence vers un même état sûr ;
- aucune nouvelle campagne ;
- aucun nouvel ID arbitraire ;
- aucune double finalisation ;
- aucune perte de la seule copie antérieurement exploitable avant commit du remplacement.

Propriété recherchée :

```text
recovery(recovery(state)) ≃ recovery(state)
```

---

## 12. Migration et garbage collection

### J-MIG-01 — Migration de format interrompue

**Classe** : IMPLEMENTATION

Applicable dès qu'une migration existe.

Oracle :

- ancienne version valide conservée jusqu'au commit de la nouvelle ;
- coupure pendant migration → au moins une version récupérable ;
- jamais destruction simultanée des deux.

### J-GC-01 — Garbage collection interrompue

**Classe** : IMPLEMENTATION

Oracle :

- dernière génération autoritative récupérable après toute interruption de GC ;
- nettoyage seulement après établissement d'un état sûr de remplacement.

---

## 13. Publication atomique

### J-PUB-01 — ActiveConfiguration concurrente aux lectures Modbus

**Classe** : FW_POLICY

Méthode : provoquer des lectures multi-registres pendant publication N→N+1.

Oracle :

- réponse entièrement issue du snapshot N ou N+1 ;
- jamais mélange de générations ;
- CRC/config ID/champs cohérents entre eux.

### J-PUB-02 — B5/B6/B7

**Classe** : FW_POLICY

Même principe pour :

- historique/résultat B5 ;
- métadonnées/inventaire B6 ;
- diagnostic B7.

---

## 14. Publication initiale au boot

### J-BOOT-01 — Barrière SYSTEM_READY_FOR_MODBUS

**Classe** : FW_POLICY

Oracle :

```text
private recovery
→ build initial snapshots
→ publication barrier
→ Modbus available
```

Interdit : client capable de lire simultanément des blocs à des stades de recovery incompatibles.

### J-BOOT-02 — Boot dégradé

**Classe** : FW_POLICY

Combinaisons minimales :

- RTC invalide + configuration valide ;
- configuration absente + storage valide ;
- bulk storage absent + état critique valide ;
- diagnostic history corrompu + autres autorités valides.

Oracle : une panne métier n'entraîne pas un blocage global si un mode sûr et diagnostiquable reste possible.

---

## 15. Second reboot systématique

Pour chaque scénario critique :

```text
inject fault
→ reboot/recovery
→ establish recovered state S
→ normal reboot without new fault
→ establish S' equivalent to S
```

Le second reboot doit confirmer que le recovery a produit un état persistant stable et non une réparation transitoire.

---

## 16. Critères transversaux

Tous les scénarios doivent vérifier, selon applicabilité :

```text
No torn authority
No partial publication
No blind replay
No automatic campaign resume
No fabricated timestamp
No stale BootIntent
No arbitrary corruption winner
No hidden CORRUPTED → EMPTY conversion
No Modbus exposure before initial barrier
No WallClock dependency for uptime/recovery timeout logic
No implicit continuity proof
No hidden unavailable → numeric authority conversion
```

---

## 17. Priorisation de développement

### P0 — avant campagnes réelles

- J-CONF-01 / 02 ;
- J-CAMP-01 / 02 / 03 ;
- J-CMD-01 / 02 / 03 ;
- J-RST-01 / 01B / 02 / 03 / 04 ;
- J-CORR-01 ;
- J-STOR-01 ;
- J-TIME-01 / 02 / 03 / 04 / 05 / 06 / 07 ;
- J-DIAG-01 / 02 ;
- J-REC-01 ;
- J-PUB-01 ;
- J-BOOT-01 / 02.

### P1 — industrialisation

- J-CORR-02 / 03 ;
- J-MIG-01 lorsqu'applicable ;
- J-GC-01 lorsqu'applicable ;
- repeated brown-out / repeated reset sequences ;
- faults composés.

### P2 — endurance

- cycles répétés power-cut/reboot ;
- injection aléatoire sur frontières persistantes ;
- longues campagnes ;
- wrap/GC du journal une fois sa politique définie.

Cette priorisation est une stratégie de développement, pas une exigence normative V1.

---

## 18. Points volontairement hors oracle

Ne pas inventer comme critères V1 :

- nombre minimal de cycles d'endurance ;
- délai maximal de boot/recovery ;
- nombre exact de retries ;
- profondeur/durée du journal B5 ;
- taille des chunks ;
- fréquence des checkpoints ;
- algorithme exact de GC ;
- priorité normative entre flags reset concurrents ;
- résultat normatif « même txid + requête différente » si la V1 ne le définit pas ;
- sentinelle normative d'indisponibilité temporelle ;
- machine exhaustive `time_status` au-delà des règles V1 existantes.

---

## 19. Gabarit d'automatisation

Conceptuellement :

```text
for each scenario:
    restore_known_initial_state()
    configure_fault_injection()
    execute_operation()
    reboot()
    assert_recovery_invariants()
    normal_reboot()
    assert_stable_recovered_state()
```

La réussite de la campagne J repose sur les invariants de sûreté, non sur la coïncidence accidentelle de timings d'implémentation.
