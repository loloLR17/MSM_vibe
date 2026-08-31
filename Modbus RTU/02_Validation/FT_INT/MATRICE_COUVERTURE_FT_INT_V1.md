# FT-INT — Matrice de couverture consolidée V1

## 1. Synthèse

La famille FT-INT couvre les relations fonctionnelles inter-blocs normatives V1, sans reprendre les responsabilités FT-STR, FT-ACC, FT-LIM, FT-BLK, FT-CMD, FT-SEQ, FT-RBT ou FT-PER.

Bilan consolidé :
- 44 exigences/classifications inventoriées ;
- 19 `COVERED` ;
- 6 `CONDITIONAL` ;
- 5 `DELEGATED` ;
- 5 `TRACE_ONLY` ;
- 9 `NOT_DEFINED` ;
- 22 identifiants de tests FT-INT distincts.

## 2. Couverture par sous-famille

| Sous-famille | Périmètre | COVERED | CONDITIONAL | DELEGATED | TRACE_ONLY | NOT_DEFINED |
|---|---|---:|---:|---:|---:|---:|
| FT-INT-01 | B2 ↔ B5 temps/commandes | 3 | 0 | 1 | 1 | 1 |
| FT-INT-02 | B4 ↔ B5 configuration/application | 5 | 1 | 3 | 0 | 0 |
| FT-INT-03 | B4 ↔ B3 configuration/supervision | 1 | 3 | 0 | 1 | 3 |
| FT-INT-04 | B0/B1/B2/B4/B5/B6 acquisition/campagnes/RAZ | 8 | 0 | 1 | 0 | 2 |
| FT-INT-05 | B1/B2/B5/B6/B7 état/diagnostic | 2 | 2 | 0 | 3 | 3 |
| **Total** |  | **19** | **6** | **5** | **5** | **9** |

## 3. Tests FT-INT distincts

### FT-INT-01
- `TT-INT-B02B05-001`
- `TT-INT-B02B05-002`
- `TT-INT-B02B05-003`

### FT-INT-02
- `TT-INT-B04B05-001`
- `TT-INT-B04B05-002`
- `TT-INT-B04B05-003`
- `TT-INT-B04B05-004`

### FT-INT-03
- `TT-INT-B03B04-001`
- `TT-INT-B03B04-002`
- `TT-INT-B03B04-003`
- `TT-INT-B03B04-004`

### FT-INT-04
- `TT-INT-B01B05-001`
- `TT-INT-B05B06-001`
- `TT-INT-B05B06-002`
- `TT-INT-B01B05B06-001`
- `TT-INT-B02B06-001`
- `TT-INT-B00B04B05B06-001`

### FT-INT-05
- `TT-INT-B05B07-001`
- `TT-INT-B05B07-002`
- `TT-INT-B01B05B07-001`
- `TT-INT-B02B07-001`
- `TT-INT-B01B07-001`

Aucun identifiant n'est dupliqué entre les sous-familles.

## 4. Audit B0 → B7

| Bloc | Relations inter-blocs V1 pertinentes | Propriétaire FT-INT / statut |
|---|---|---|
| B0 | identité conservée après RAZ statistiques B5 | FT-INT-04 `COVERED` ; persistance reboot → FT-PER |
| B1 | acquisition après START/STOP ; observables dupliqués B7 ; défaut maintenu après ACK | FT-INT-04 / FT-INT-05 |
| B2 | synchronisation via B5 ; base temporelle campagnes B6 et défaut B7 | FT-INT-01 / FT-INT-04 / FT-INT-05 |
| B3 | seuils issus de la configuration B4 active | FT-INT-03 |
| B4 | activation via B5 ; effet sur B3 ; conservation après RAZ statistiques | FT-INT-02 / FT-INT-03 / FT-INT-04 |
| B5 | stimulus de commandes ; moteur transactionnel exclu | Effets inter-blocs FT-INT ; moteur → FT-CMD |
| B6 | campagnes START/STOP ; base B2 ; conservation RAZ ; stockage↔B7 non défini | FT-INT-04 / FT-INT-05 |
| B7 | SELFTEST, ACK, timestamp défaut, duplications B1 | FT-INT-05 |

## 5. Limites V1 conservées explicitement

Restent volontairement sans oracle PASS/FAIL inventé :
- égalité exhaustive B1 `system_status` ↔ B7 `system_health_status` ;
- égalité exhaustive des bitfields défaut B1 ↔ B7 ;
- égalité B1 `active_campaign_id` ↔ B6 `campaign_id` ;
- correspondance exhaustive acquisition B1 ↔ flags B3 ;
- correspondance état stockage B6 ↔ diagnostic B7 ;
- chronologie exhaustive hystérésis/hold time B4→B3 ;
- équivalence exacte `CONFIG_VALID` B3 ↔ état B4 ;
- égalité temporelle stricte entre lectures Modbus séparées lorsqu'aucune tolérance V1 n'est définie.

## 6. Délégations finales

- structure/encodage/snapshot : FT-STR ;
- permissions : FT-ACC ;
- domaines/codes/unités : FT-LIM ;
- invariants intra-bloc : FT-BLK ;
- moteur B5, refus, idempotence, transaction : FT-CMD ;
- scénarios complets multi-actions : FT-SEQ ;
- timings hostiles/robustesse : FT-RBT ;
- reboot/persistance : FT-PER.

## 7. Conclusion d'audit

Après normalisation des matrices, correction de l'identifiant du scénario RAZ statistiques et contrôle croisé B0→B7, aucun trou fonctionnel inter-blocs V1 supplémentaire n'a été identifié sans sortir du référentiel normatif.

La famille est **candidate au gel V1**, sous réserve de validation explicite finale avant merge de la présente passe.