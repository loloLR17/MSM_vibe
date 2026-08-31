# FT-INT-01 — Source normalisée

## 1. Références normatives

- `01_Specification_source/bloc2.md`
- `01_Specification_source/bloc5.md`
- `02_Validation/FT_BLK/FT-BLK-02_Temps_monotonie_derivations/`

Les compléments métier explicitement informatifs ne servent pas d'oracle.

## 2. Exigences retenues

### INT01-R01 — Application effective du temps préparé

- Blocs : B2 ↔ B5.
- Source : B2, règles de cohérence temporelle ; B5, commande de synchronisation horaire.
- Exigence : l'écriture du temps préparé ne règle pas l'horloge ; après exécution réussie de la commande B5 de synchronisation, le temps préparé est effectivement appliqué à l'horloge B2.
- Classification : `COVERED`.
- Test : `TT-INT-B02B05-001`.
- Frontière : FT-INT ne juge pas le moteur transactionnel B5 ; la réussite de la commande constitue un prérequis du test.
- Oracle : la valeur observée de `current_time` après synchronisation doit être cohérente avec le temps préparé et le temps réellement écoulé. Aucune égalité stricte entre lectures séparées n'est imposée.

### INT01-R02 — Mise à jour de last_sync_time

- Blocs : B2 ↔ B5.
- Source : B2, `last_sync_time` mis à jour uniquement lors d'une synchronisation effective ; B5, effet de la commande de synchronisation.
- Exigence : après synchronisation B5 réussie, `last_sync_time` est mis à jour vers la nouvelle référence temporelle appliquée.
- Classification : `COVERED`.
- Test : `TT-INT-B02B05-002`.
- Délégation entrante : FT-BLK-02 ne couvre que la stabilité hors synchronisation et délègue explicitement la branche positive à FT-CMD / FT-INT.

### INT01-R03 — Cohérence minimale de l'état temporel après synchronisation

- Blocs : B2 ↔ B5.
- Source : B5, effet de synchronisation incluant la mise à jour de l'état de synchronisation ; B2, représentation de l'état temporel.
- Exigence : après une synchronisation réussie, les observables B2 ne doivent pas rester dans un état contradictoire avec la synchronisation effectivement matérialisée.
- Classification : `COVERED` avec oracle borné.
- Test : `TT-INT-B02B05-003`.
- Limite : la V1 ne fournit pas une table exhaustive imposant un triplet exact `time_status` / `time_flags` / `prepared_time_status` pour toutes les transitions. Le test n'invente donc aucune combinaison numérique supplémentaire.

### INT01-R04 — Base temporelle de cmd_last_timestamp

- Blocs : B2 ↔ B5.
- Source : B5, timestamp de commande exprimé dans l'Epoch TR2 défini par B2.
- Exigence : `cmd_last_timestamp` partage la base temporelle B2.
- Classification : `TRACE_ONLY`.
- Justification : la base commune est normative, mais aucune tolérance ou relation numérique précise avec une lecture B2 simultanée n'est imposée. Un verdict d'égalité serait fabriqué.

### INT01-R05 — Disponibilité de la synchronisation préparée

- Blocs : B2 ↔ B5.
- Observation : B2 expose l'état/disponibilité du temps préparé et B5 expose un indicateur de synchronisation préparée disponible.
- Classification : `NOT_DEFINED`.
- Justification : aucune équivalence normative explicite du type `B5 flag == B2 flag/status` n'est définie. La relation ne devient pas un oracle FT-INT.

### INT01-R06 — Refus en absence de synchronisation préparée

- Blocs impliqués fonctionnellement : B2 ↔ B5.
- Exigence : le moteur de commande peut refuser la synchronisation lorsque son prérequis n'est pas satisfait.
- Classification : `DELEGATED`.
- Propriétaire : FT-CMD.
- Justification : le verdict porte sur l'acceptation/refus et le code résultat B5, donc sur le moteur transactionnel, pas sur un effet inter-blocs après succès.

## 3. Anti-duplication

FT-INT-01 ne répète pas :
- `prepared_time` sans effet immédiat : FT-BLK-02 ;
- monotonie `current_time` : FT-BLK-02 ;
- stabilité `last_sync_time` hors synchronisation : FT-BLK-02 ;
- cohérence interne `time_since_sync` : FT-BLK-02 ;
- mécanique et résultats de commande : FT-CMD.

## 4. Règles anti-fabrication

- ne pas exiger une égalité temporelle bit-à-bit entre lectures non simultanées ;
- ne pas inventer de tolérance chiffrée absente de la V1 ;
- ne pas transformer une base temporelle commune en égalité de valeurs ;
- ne pas déduire une égalité entre flags B2 et B5 sans exigence explicite ;
- ne pas imposer une table de transitions d'état B2 plus précise que la V1.
