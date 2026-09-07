# Projet MSM — Capteur de vibration TR2

## Gel firmware P8 — Intégration runtime F/G, acquisition, campagnes et supervision B3

## 1. Statut

La tranche firmware **P8 — intégration runtime F/G** est considérée comme implémentée et validée sur la branche `main`.

Baseline de validation fonctionnelle avant le présent gel :

- commit : `49b164bee4a95377b39940ec649d32cc4a7a8813` ;
- validation locale : `./tr2_validate.sh` ;
- résultat : **63/63 tests réussis** ;
- runtime Host : `TR2 P0 host runtime ready`.

Le présent document est un constat de gel d'implémentation. Il ne modifie pas la spécification Modbus RTU V1, ne remplace pas les documents d'architecture F/G, et ne transforme aucune décision V1.1 ou zone `NOT_DEFINED V1` en exigence V1.

---

## 2. Références gelées

P8 s'appuie notamment sur :

- `FREEZE_FIRMWARE_P7_COMMAND_ENGINE_B5.md` ;
- `FREEZE_FIRMWARE_P8B_CAMPAIGN_DATA_CONTRACT.md` ;
- `ARCHITECTURE_FIRMWARE_SERVICES_MODBUS_V1.md` ;
- `ARCHITECTURE_FIRMWARE_BOOT_PERSISTENCE_RECOVERY.md` ;
- `RECOVERY_FAULT_INJECTION_MATRIX.md` ;
- la baseline normative Modbus RTU V1 et ses blocs B3, B5 et B6.

P8 préserve explicitement les séparations d'autorité suivantes :

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

et :

```text
CampaignService
   ├── orchestre AcquisitionService
   └── CampaignRepository
          ├── CampaignMetadata
          └── CampaignDataStore
                 ↓
          PersistentStorageCore
```

Le CommandJournal reste une autorité transactionnelle et ne devient jamais l'autorité métier des campagnes.

---

## 3. Périmètre P8 gelé

P8 couvre les tranches suivantes :

- **P8-A** — confirmation des gaps F/G réels avant intégration ;
- **P8-B** — contrat bulk de campagne et frontière de checkpoint ;
- **P8-C** — composition F/G dans `SystemRuntime` ;
- **P8-D** — driver runtime d'acquisition ;
- **P8-E** — intégration runtime de `SupervisionService` ;
- **P8-F** — append bulk et checkpoint de fin de fenêtre ;
- **P8-G** — convergence après échec partiel de START ;
- **P8-H** — finalisation STOP de la dernière fenêtre ;
- **P8-I** — pont runtime B5 START/STOP vers les autorités F/G ;
- **P8-J** — projection runtime B3 depuis `SupervisionSnapshot` ;
- **P8-K** — tests d'intégration power-loss / boot / recovery ;
- **P8-L** — présente passe transverse et gel.

---

## 4. Contrat bulk gelé

Chaque `VibrationSample` effectivement obtenu avec `read_sample() == TR2_OK` produit exactement un record persistant de **16 octets**, encodé explicitement et indépendant du layout C :

| Offset | Taille | Champ | Encodage |
|---:|---:|---|---|
| 0 | 4 | `x_mg` | `int32` little-endian |
| 4 | 4 | `y_mg` | `int32` little-endian |
| 8 | 4 | `z_mg` | `int32` little-endian |
| 12 | 2 | `flags` | `uint16` little-endian |
| 14 | 2 | `reserved` | zéro |

Flags P8 :

```text
bit 0 = valid
bit 1 = saturated
bits 2..15 = 0
```

Règles gelées :

- sample lu avec succès mais `valid == false` → record persistant avec flag `valid=0` ;
- erreur de lecture source → aucun record fictif ;
- aucun timestamp par sample ;
- `AcquisitionWindow`, `SupervisionSnapshot` et B3 ne sont pas des payloads bulk ;
- `AcquisitionService` ne connaît ni `campaign_id` ni `CampaignDataStore`.

---

## 5. Frontière de fenêtre et ordre de publication

La frontière de checkpoint P8 est la fin de chaque `AcquisitionWindow` effectivement terminée par le runtime.

Ordre gelé :

```text
samples
→ append de chaque record réussi
→ end_window()
→ checkpoint(campaign_id)
→ publication SupervisionSnapshot
→ projection B3
```

Cette règle s'applique aux fenêtres :

- complètes ;
- incomplètes lors d'un STOP explicite ;
- incomplètes lors d'une erreur source lorsque la fenêtre peut être terminée proprement.

Une fenêtre ne doit pas être publiée en supervision avant que son checkpoint bulk ne soit prouvé réussi.

`finish_campaign()` reste la barrière finale du DataStore lors de la fermeture de campagne.

---

## 6. Driver runtime d'acquisition

Le driver P8 expose un pas d'acquisition borné et explicite.

Sémantique gelée :

- campagne OPEN + acquisition active requises ;
- un pas lit au plus un sample ou termine une fenêtre ;
- lecture réussie → événement `SAMPLE_READ` ;
- lecture en erreur → aucun sample fictif, fermeture immédiate de la fenêtre en `source_error` ;
- le dernier sample réussi d'une fenêtre reste un événement `SAMPLE_READ` ; le pas suivant émet `WINDOW_COMPLETED` ;
- aucune nouvelle fenêtre n'est démarrée automatiquement ;
- aucune cadence scheduler ni politique de retry source n'est inventée par P8.

Le pilotage périodique supérieur reste distinct du contrat fonctionnel de P8.

---

## 7. START acquisition — convergence après effet partiel

P8 conserve le cycle transactionnel gelé de P7 :

```text
RESERVED durable
→ recovery context durable
→ STARTED durable
→ effet métier autorisé
→ COMPLETED durable
```

Pour START, l'ordre métier gelé reste :

```text
réservation campaign_id
→ OPEN durable dans CampaignRepository
→ begin CampaignDataStore
→ démarrage AcquisitionService
```

Si une erreur survient après l'OPEN durable, P8 ne supprime pas l'histoire de campagne et n'invente pas un succès B5.

Politique P8 gelée :

- si la campagne est déjà OPEN, la convergence utilise le chemin normal de fermeture de `CampaignService` ;
- l'historique de campagne reste démontrable ;
- l'erreur originale START reste l'erreur retournée si la convergence réussit ;
- si la convergence échoue elle-même, son erreur est retournée ;
- aucune suppression/rollback imaginaire de l'autorité historique n'est autorisée.

Une réconciliation `TERMINAL_EFFECT_PROVEN` prouve un effet causal durable ; elle ne signifie pas, à elle seule, qu'un résultat B5 `SUCCESS` doit être inventé.

---

## 8. STOP acquisition — finalisation de la dernière fenêtre

P8-H ferme le cas où la dernière fenêtre active était auparavant terminée puis perdue côté supervision.

Ordre gelé d'un STOP avec fenêtre active :

```text
end_window()
→ checkpoint bulk
→ publication supervision de la dernière fenêtre si exploitable
→ finish_campaign()
→ recover durable prefix
→ CampaignRepository CLOSED
→ finalisation B5
```

Une dernière fenêtre sans donnée exploitable peut produire `TR2_ERROR_NOT_AVAILABLE` côté supervision sans empêcher, à elle seule, la fermeture durable de la campagne ; le dernier snapshot live valide reste alors inchangé.

Aucune fenêtre ne doit être publiée si sa barrière bulk n'a pas été prouvée.

---

## 9. Intégration B5 runtime START/STOP

`SystemRuntime` relie maintenant les `CommandRequest` capturées aux autorités F/G pour les seules commandes acquisition déjà raccordées :

- 3 `START_ACQUISITION` ;
- 4 `STOP_ACQUISITION`.

Chaîne gelée :

```text
CommandRequest capturée
→ CommandEngine::admit
→ NEW seulement
→ handler START ou STOP
→ CampaignService / SupervisionService
→ refresh CommandSnapshot / B5
→ refresh CampaignInventorySnapshot / B6
```

Les admissions :

- `RETRY` ;
- `COLLISION` ;
- `BUSY`

ne redispatchent aucun effet métier.

P8 ne raccorde pas artificiellement les commandes 5..11 et ne crée pas d'autorité métier fictive pour elles.

---

## 10. Projection runtime B3

B3 reste une projection du dernier `SupervisionSnapshot` live démontré disponible.

Chaîne gelée :

```text
WINDOW_COMPLETED
→ checkpoint bulk réussi
→ SupervisionService
→ SupervisionSnapshot
→ modbus_project_b3()
→ image B3 runtime
```

Règles :

- aucun B3 live disponible au boot avant publication d'une supervision ;
- aucun B3 reconstruit depuis la NVM ;
- aucun B3 reconstruit depuis les données bulk ou B6 ;
- l'image B3 runtime n'est exposée que si elle correspond au snapshot live courant ;
- un changement d'ActiveConfiguration n'altère pas rétroactivement une fenêtre déjà acquise ;
- B3 ne devient jamais une autorité métier ou historique.

---

## 11. Boot et power-loss

P8 confirme l'ordre de boot :

```text
persistent storage
→ recover Time
→ recover Configuration
→ recover CampaignRepository / CampaignDataStore
→ compose F/G runtime
→ recover CommandJournal et réconciliation B5
→ rebuild B4/B5/B6
→ READY
```

Aucune source vibration n'est démarrée au boot.

Pour une campagne OPEN avant reset :

- `campaign_id` et contexte historique durable sont conservés ;
- seul le dernier préfixe bulk démontré durable est récupéré ;
- une queue non checkpointée peut être perdue ;
- aucun `end_timestamp` ni `duration` n'est inventé ;
- aucune acquisition n'est auto-reprise ;
- aucun B3 live n'est restauré ;
- aucune commande START/STOP n'est automatiquement rejouée.

Le recovery doit être idempotent sur des reboots successifs sans nouvel événement métier.

---

## 12. Couverture de fault injection / intégration

La passe P8 valide explicitement les scénarios suivants :

### 12.1 Campagne en cours

```text
START tx
→ STARTED durable
→ OPEN campagne durable
→ samples
→ checkpoint fenêtre
→ reset avant résultat B5 terminal
```

Après reboot :

- campagne toujours OPEN ;
- préfixe durable exact récupéré ;
- transaction START retrouvée `STARTED` ;
- réconciliation `STARTED_EFFECT_PROVEN` ;
- aucun replay START ;
- aucune reprise source ;
- aucun B3 restauré.

Le même oracle est vérifié sur un second reboot sans événement intermédiaire.

### 12.2 Matrice architecture couverte

P8 participe directement à la couverture de :

- `J-CAMP-01..03` pour les frontières réellement implémentées ;
- `J-CMD-01..03` pour START/STOP et réconciliation ;
- `G-DATA-01..02` ;
- `G-REC-01..03` ;
- `F-SUP-05..07`.

Le gel P8 ne prétend pas que toutes les injections possibles de la matrice globale A–J sont désormais exhaustivement couvertes ; seules les frontières présentes dans l'implémentation P8 et les tests associés sont gelées ici.

---

## 13. Fermeture des gaps P8-A

À l'issue de P8 :

```text
F-GAP-01 composition AcquisitionService runtime        → CLOSED P8-C
F-GAP-02 composition SupervisionService runtime        → CLOSED P8-C/P8-E
F-GAP-03 raccordement SupervisionSnapshot vers B3      → CLOSED P8-J
F-GAP-04 driving acquisition runtime                    → CLOSED P8-D

G-GAP-01 composition CampaignService runtime            → CLOSED P8-C
G-GAP-02 intégration bulk CampaignDataStore             → CLOSED P8-F
G-GAP-03 convergence START après effet partiel          → CLOSED P8-G
G-GAP-04 contrat payload campagne                       → CLOSED P8-B
G-GAP-05 cadence/frontière checkpoint                   → CLOSED P8-B
```

Ces fermetures sont des fermetures d'architecture/implémentation P8 et ne créent pas de nouveaux champs Modbus V1.

---

## 14. Points volontairement non définis ou hors périmètre

P8 ne définit pas silencieusement :

- cadence d'appel du scheduler vers `system_runtime_drive_acquisition_step()` ;
- retry automatique d'une lecture source ;
- politique globale de poursuite/arrêt après erreur d'append ou de checkpoint hors chemins explicitement gelés ;
- mapping B5 final supplémentaire pour une erreur transactionnelle dont le résultat exact reste `NOT_DEFINED V1` ;
- compression, FFT, format de fichier externe ou export/download bulk ;
- politique complète de capacité/rétention média ;
- auto-reprise d'une campagne OPEN après reboot ;
- reconstruction d'une queue bulk perdue ;
- nouvelles commandes métier 5..11 ;
- extensions B5 V1.1, `transaction_epoch`, nouveaux result codes ou nouveaux registres.

Toute évolution future sur ces points exige une tranche ou un arbitrage explicite.

---

## 15. Validation et non-régression

La baseline immédiatement avant le présent document a été validée localement :

```text
100% tests passed, 0 tests failed out of 63
TR2 P0 host runtime ready
TR2 validation complete.
```

Les 63 tests incluent les validations antérieures P1 à P7 ainsi que les tests ajoutés pour P8, notamment :

- composition F/G ;
- driver acquisition ;
- append/checkpoint bulk ;
- convergence START ;
- finalisation STOP ;
- runtime B5 START/STOP ;
- projection runtime B3 ;
- power-loss / boot / recovery idempotent.

---

## 16. Décision de gel

À compter de ce gel :

- P8 constitue la baseline firmware validée pour l'intégration F/G ;
- le chemin `B5 START/STOP → CampaignService → AcquisitionService → CampaignDataStore → SupervisionService → B3/B6` est gelé selon les responsabilités décrites ci-dessus ;
- les invariants P7 transactionnels restent applicables intégralement ;
- le contrat bulk P8-B reste applicable intégralement ;
- aucune évolution ultérieure ne doit faire de B3, B5 ou B6 une autorité métier ;
- aucune évolution ultérieure ne doit introduire une auto-reprise ou un replay transactionnel sans arbitrage explicite ;
- les zones `NOT_DEFINED V1` et les extensions V1.1 restent séparées de la baseline V1.

La tranche suivante devra être définie à partir de cette baseline gelée et de l'état réel de `main`.
