# FT-BLK-02 — Source normalisée

## 1. Références normatives

- `01_Specification_source/bloc1.md`
- `01_Specification_source/bloc2.md`
- `01_Specification_source/bloc3.md`
- `01_Specification_source/bloc7.md`
- `01_Specification_source/charte_typage.md`

Les compléments métier sont informatifs et ne servent pas d'oracle.

## 2. Exigences retenues

### B1

**BLK02-B01-R01 — Monotonie uptime**
- Source : Bloc 1 §7.
- Exigence : les compteurs (`uptime`) doivent être monotones.
- Classification : `COVERED`.
- Test : `TT-BLK-B01-001`.
- Limite : aucune cadence exacte d'incrément n'est imposée.

### B2

**BLK02-B02-R01 — Monotonie current_time**
- Source : Bloc 2 §7.
- Exigence : `current_time` est monotone sauf resynchronisation.
- Classification : `COVERED`.
- Test : `TT-BLK-B02-101`.

**BLK02-B02-R02 — Préparation sans application immédiate**
- Source : Bloc 2 §7.
- Exigence : l'écriture de `prepared_time` ne modifie pas immédiatement l'horloge.
- Classification : `COVERED`.
- Test : `TT-BLK-B02-102`.
- Frontière : l'application effective relève de B5 / FT-CMD.

**BLK02-B02-R03 — Stabilité last_sync_time hors synchronisation**
- Source : Bloc 2 §7.
- Exigence : `last_sync_time` n'est mis à jour que lors d'une synchronisation effective.
- Classification : `COVERED` pour l'absence de modification hors synchronisation ; `TRACE_ONLY` pour la mise à jour positive après commande B5.
- Test : `TT-BLK-B02-103`.

**BLK02-B02-R04 — Dérivation time_since_sync**
- Source : Bloc 2 §7.
- Exigence : `time_since_sync` est dérivé de manière cohérente.
- Classification : `COVERED`.
- Test : `TT-BLK-B02-104`.
- Oracle : cohérence avec l'écart `current_time - last_sync_time`, avec tolérance liée au temps de lecture ; aucune égalité stricte inter-requêtes n'est imposée.

### B3

**BLK02-B03-R01 — Monotonie séquence de calcul**
- Source : Bloc 3 §7, mapping `B3_CALC_SEQUENCE`.
- Exigence : compteur monotone de calcul.
- Classification : `COVERED`.
- Test : `TT-BLK-B03-101`.

**BLK02-B03-R02 — Âge de la dernière valeur valide**
- Source : Bloc 3 §6 et mapping `B3_VALUE_AGE_MS` / `B3_LAST_UPDATE_TR2`.
- Exigence : l'âge qualifie la dernière valeur valide.
- Classification : `CONDITIONAL`.
- Motif : absence de formule normative complète et de tolérance imposée entre les deux représentations temporelles.

**BLK02-B03-R03 — Compteurs de dépassements et alarmes**
- Source : Bloc 3 §9.
- Exigence : `B3_EXCEED_COUNT` et `B3_ALARM_COUNT` sont monotones et saturent à `0xFFFFFFFF`.
- Classification : `TRACE_ONLY`.
- Délégation : FT-BLK-03, où l'événement fonctionnel d'incrément pourra être vérifié avec la supervision vibratoire.

### B7

**BLK02-B07-R01 — Monotonie uptime diagnostic**
- Source : Bloc 7 §8.3.
- Exigence : `uptime_s` ne revient jamais en arrière sauf reset.
- Classification : `COVERED`.
- Test : `TT-BLK-B07-001`.
- Frontière : la réaction au reset est déléguée à FT-PER.

## 3. Délégations explicites

- cohérence B1 uptime ↔ B7 uptime : `TRACE_ONLY` → FT-INT ;
- modification de `last_sync_time` après commande réussie B5 : `TRACE_ONLY` → FT-CMD / FT-INT ;
- comportement des uptime après reset : `TRACE_ONLY` → FT-PER ;
- compteurs B3 liés aux dépassements/alarmes : `TRACE_ONLY` → FT-BLK-03.

## 4. Règles anti-fabrication

- ne pas imposer une progression de +1 à chaque lecture d'un uptime ;
- ne pas imposer une égalité exacte entre deux lectures Modbus effectuées à des instants différents ;
- ne pas déduire de relation inter-blocs dans FT-BLK-02 ;
- ne pas utiliser les compléments métier comme exigences V1.
