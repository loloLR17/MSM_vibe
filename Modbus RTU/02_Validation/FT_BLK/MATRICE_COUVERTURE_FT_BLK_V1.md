# MATRICE DE COUVERTURE FT-BLK V1 — Consolidation

## 1. Couverture par bloc

| Bloc | Sous-famille(s) | Statut de couverture |
|---|---|---|
| B0 | FT-BLK-06 | Couvert + unicité conditionnelle + persistance déléguée FT-PER |
| B1 | FT-BLK-01 / 02 | États/invariants + uptime |
| B2 | FT-BLK-01 / 02 | États/flags + monotonie et dérivations |
| B3 | FT-BLK-01 / 02 / 03 | États/flags + compteurs/temps + calculs vibratoires |
| B4 | FT-BLK-04 | Cycle de configuration préparée et CRC |
| B5 | FT-BLK-06 | Inventorié et délégué à FT-CMD/FT-INT |
| B6 | FT-BLK-05 | Sélection et cohérence d'inventaire |
| B7 | FT-BLK-01 / 02 | Diagnostic + uptime |

## 2. Délégations principales

- FT-STR : représentation, snapshot, réservés, multi-registres ;
- FT-ACC : accès, permissions, exceptions et effets de bord ;
- FT-LIM : domaines, bornes et valeurs invalides ;
- FT-INT : cohérences B1↔B7, B2↔B5, B4↔B3/B5, B1↔B6, etc. ;
- FT-CMD : moteur B5, idempotence, transaction, transitions commandées ;
- FT-PER : reboot, persistance et reprise.

## 3. Dettes normatives / limitations V1 conservées

### B1
- dérivation exhaustive `system_status` ↔ flags : `NOT_DEFINED` ;
- persistance défaut/avertissement : `CONDITIONAL` selon injection.

### B2
- état dégradé ↔ flags de qualification : `CONDITIONAL` ;
- mise à jour après synchronisation : déléguée FT-CMD/FT-INT.

### B3
- axe dominant : `NOT_DEFINED` ;
- dérivation exacte statut/validité et sévérité : `NOT_DEFINED` ;
- formule complète fenêtre/échantillons valides : `NOT_DEFINED` ;
- RMS/crêtes : `CONDITIONAL` à un banc déterministe ;
- politique de saturation des compteurs : `CONDITIONAL` ;
- politique de conservation de dernière valeur : conditionnelle à l'implémentation autorisée.

### B4
- événement exact d'incrément de `config_revision_counter` : `NOT_DEFINED` ;
- dérivation exhaustive de `config_error_code` : `NOT_DEFINED` ;
- transitions provoquées par commandes : déléguées FT-CMD/FT-INT.

### B5
- ensemble du moteur transactionnel : délégué FT-CMD ; aucun doublon actif FT-BLK.

### B6
- pour une campagne terminée sans discontinuité temporelle, `duration_s = end_timestamp - start_timestamp` est désormais normé et couvert conditionnellement par FT-BLK-05 ;
- pour une campagne en cours ou traversant une discontinuité temporelle, la relation exacte entre `duration_s` et les timestamps reste `NOT_DEFINED` en V1 ;
- unicité locale des `campaign_id` entre campagnes distinctes d'un même TR2 : normée et couverte conditionnellement ;
- invariant numérique `valid_campaign_count <= total_campaign_count` : non explicitement normé ;
- relation stockage utilisé/libre/capacité : `NOT_DEFINED` ;
- dérivation `storage_health_status` : `NOT_DEFINED` ;
- dérivation `data_integrity_status` : `NOT_DEFINED`.

### B7
- dérivation `system_health_status` ↔ flags : `NOT_DEFINED` ;
- relation flags ↔ `last_fault_code` : `NOT_DEFINED` ;
- priorité de défaut multiple : informative uniquement ;
- relation exhaustive autotest status/result/detail : `CONDITIONAL`.

## 4. Règle de conservation

Ces limitations ne sont pas des oublis. Elles constituent une dette normative tracée à reprendre lors d'une évolution formelle de la V1 ou lors de la définition des moyens d'essai requis. Aucun oracle ne doit être inventé pour les masquer.
