# MATRICE DE COUVERTURE FT-SEQ V1

## 1. Objet

Cette matrice consolide l'audit V1 de la famille FT-SEQ — séquences métier et scénarios fonctionnels multi-actions.

FT-SEQ valide la continuité de chaînes complètes sans reprendre les oracles élémentaires des familles gelées.

## 2. Synthèse globale

| Sous-famille | COVERED | CONDITIONAL | DELEGATED | TRACE_ONLY | NOT_DEFINED |
|---|---:|---:|---:|---:|---:|
| FT-SEQ-01 Qualification initiale / contexte | 0 | 0 | 7 | 1 | 4 |
| FT-SEQ-02 Préparation / activation configuration | 1 | 1 | 4 | 0 | 3 |
| FT-SEQ-03 Préparation / synchronisation temporelle | 1 | 0 | 5 | 1 | 4 |
| FT-SEQ-04 Démarrage / établissement campagne | 1 | 0 | 5 | 0 | 4 |
| FT-SEQ-05 Arrêt / clôture / consultation | 1 | 0 | 6 | 2 | 4 |
| FT-SEQ-06 Cycle nominal complet | 2 | 0 | 6 | 0 | 5 |
| FT-SEQ-07 Refus puis reprise | 4 | 0 | 0* | 3 | 4 |
| **TOTAL** | **10** | **1** | **33** | **7** | **28** |

\* FT-SEQ-07 compose de nombreuses délégations FT-CMD/FT-INT/FT-SEQ-02..05, mais elles sont décrites comme délégations communes et non comme exigences numérotées autonomes.

Total d'entrées classifiées : **79**.

## 3. Scénarios propriétaires FT-SEQ

| Test | Sous-famille | Chaîne couverte |
|---|---|---|
| `TT-SEQ-CONFIG-001` | FT-SEQ-02 | préparation B4 → absence d'effet immédiat → APPLY CONFIG → configuration active cohérente |
| `TT-SEQ-TIME-001` | FT-SEQ-03 | préparation B2 → absence d'effet immédiat → SYNC TIME → temps appliqué/cohérent |
| `TT-SEQ-CAMP-001` | FT-SEQ-04 | contexte START valide → START → acquisition active → nouvelle campagne en cours |
| `TT-SEQ-CAMP-002` | FT-SEQ-05 | campagne en cours → STOP → acquisition arrêtée → campagne clôturée/cohérente → consultation B6 |
| `TT-SEQ-SYS-001` | FT-SEQ-06 | cycle E2E nominal complet |
| `TT-SEQ-REC-001` | FT-SEQ-07 | START refus 22 → correction configuration → START réussi |
| `TT-SEQ-REC-002` | FT-SEQ-07 | SYNC TIME refus 19 → préparation temps → succès |
| `TT-SEQ-REC-003` | FT-SEQ-07 | STOP refus 21 → START valide → STOP réussi |
| `TT-SEQ-REC-004` | FT-SEQ-07 | APPLY CONFIG refus 5 → STOP → APPLY CONFIG réussi |

FT-SEQ-01 ne possède pas de test PASS/FAIL autonome : la V1 ne définit aucun handshake initial obligatoire.

## 4. Propriétés FT-SEQ effectivement couvertes

### FT-SEQ-02
- `SEQ02-R01` : préparer puis activer une configuration valide.

### FT-SEQ-03
- `SEQ03-R01` : préparer puis appliquer une synchronisation temporelle.

### FT-SEQ-04
- `SEQ04-R01` : START puis établissement d'une campagne en cours.

### FT-SEQ-05
- `SEQ05-R01` : STOP puis clôture et consultation de la campagne.

### FT-SEQ-06
- `SEQ06-R01` : cycle nominal complet de campagne.
- `SEQ06-R10` : cohérence E2E sans renforcer les oracles élémentaires.

### FT-SEQ-07
- `SEQ07-R01` : START sans configuration active valide → correction → succès.
- `SEQ07-R02` : SYNC TIME sans temps préparé → correction → succès.
- `SEQ07-R03` : STOP acquisition inactive → établissement d'un contexte valide → succès.
- `SEQ07-R04` : APPLY CONFIG pendant acquisition → STOP → nouvelle application réussie.

## 5. Délégations structurantes

| Nature de propriété | Propriétaire |
|---|---|
| structure, tailles, encodage, snapshots | FT-STR |
| droits RO/RW et exceptions d'accès | FT-ACC |
| domaines, limites, valeurs réservées | FT-LIM |
| invariants internes de blocs | FT-BLK |
| effets et cohérences inter-blocs élémentaires | FT-INT |
| moteur transactionnel B5, résultats, idempotence, corrélation, concurrence | FT-CMD |
| succession et réussite de chaînes fonctionnelles complètes | FT-SEQ |
| pertes, répétitions agressives, timing hostile | FT-RBT |
| reboot, persistance, reprise après redémarrage | FT-PER |

## 6. Limites V1 majeures conservées

Les éléments suivants ne sont pas transformés en oracles :
- handshake initial et ordre obligatoire B0→B1→B2→B4→B5→B6→B7 ;
- observation obligatoire d'un état intermédiaire `VALIDE` de configuration ;
- délai maximal global préparation→activation ou préparation→synchronisation ;
- égalité temporelle stricte entre transactions séparées ;
- égalité `B1.active_campaign_id == B6.campaign_id` ;
- `total_campaign_count` augmentant exactement de 1 immédiatement après START ;
- égalité universelle `duration_s = end_timestamp - start_timestamp` ;
- témoin Modbus direct de flush buffers / fermeture fichiers ;
- obligation de SYNC TIME avant chaque START ;
- ordre universel SYNC TIME / APPLY CONFIG ;
- lecture B7 obligatoire après STOP ;
- nombre de retries, backoff ou délai de reprise ;
- procédure générique de récupération d'horloge indisponible, SD absente/mémoire insuffisante ou défaut critique.

## 7. Conclusion de couverture

La V1 fournit un périmètre FT-SEQ cohérent et testable pour les chaînes nominales et quatre reprises fonctionnelles explicitement fondées.

Les `NOT_DEFINED` ne constituent pas des trous cachés : ils matérialisent des comportements que la V1 n'a pas normés et qui ne doivent pas être inventés par la validation.
