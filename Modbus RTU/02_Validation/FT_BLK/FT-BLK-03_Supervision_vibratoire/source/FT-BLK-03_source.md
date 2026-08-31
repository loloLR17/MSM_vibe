# FT-BLK-03 — Exigences source normalisées

## 1. Références normatives

Source principale : `01_Specification_source/bloc3.md` V1 gelé.

Les compléments métier informatifs ne sont pas utilisés comme oracles normatifs.

## 2. Exigences normalisées

### BLK03-B3-001 — RMS global
Le RMS global est le RMS de la norme vectorielle d'accélération sur la fenêtre de calcul. Toutes les grandeurs sont exprimées en mg.

**Classification** : CONDITIONAL — oracle mathématique normatif, stimulation déterministe nécessaire.

### BLK03-B3-002 — Crête globale
La crête globale est le maximum de la norme vectorielle d'accélération observé sur la fenêtre.

**Classification** : CONDITIONAL.

### BLK03-B3-003 — RMS par axe
Les RMS X, Y et Z sont les RMS de l'accélération de chaque axe sur la fenêtre.

**Classification** : CONDITIONAL.

### BLK03-B3-004 — Crête par axe
Les crêtes X, Y et Z sont les valeurs absolues maximales observées sur chaque axe sur la fenêtre.

**Classification** : CONDITIONAL.

### BLK03-B3-005 — Axe dominant
`B3_DOMINANT_AXIS` expose X, Y, Z ou ex aequo / indéterminé, mais la V1 ne précise pas quelle grandeur ou quel critère détermine la dominance.

**Classification** : NOT_DEFINED pour la fonction de dérivation.

### BLK03-B3-006 — Valeur conservée lors d'une indisponibilité temporaire
La V1 autorise le capteur à conserver la dernière valeur calculée en cas d'indisponibilité temporaire, à condition de positionner les états et bits appropriés.

**Classification** : CONDITIONAL — comportement autorisé, non obligatoire.

### BLK03-B3-007 — LAST_VALUE_HELD
Si l'implémentation conserve une dernière valeur sans recalcul récent, le bit `LAST_VALUE_HELD` fournit l'indication normative correspondante.

**Classification** : CONDITIONAL — applicable lorsque la politique de conservation est utilisée et qu'un scénario déterministe peut être provoqué.

### BLK03-B3-008 — Séquence sans nouveau calcul
`B3_CALC_SEQUENCE` est un compteur de calcul. En l'absence de nouvelle fenêtre de calcul validée, aucune nouvelle séquence de calcul ne doit être imputée au bloc.

**Classification** : COVERED sous précondition maîtrisée d'absence de nouvelle fenêtre validée.

### BLK03-B3-009 — Compteur de dépassements
`B3_EXCEED_COUNT` est monotone.

**Classification** : COVERED.

### BLK03-B3-010 — Compteur d'alarmes
`B3_ALARM_COUNT` est monotone.

**Classification** : COVERED.

### BLK03-B3-011 — Saturation des compteurs
La V1 indique que `B3_EXCEED_COUNT` et `B3_ALARM_COUNT` « peuvent saturer à 0xFFFFFFFF ». Cette formulation n'impose pas explicitement une politique obligatoire de saturation ni une interdiction formelle du wrap.

**Classification** : CONDITIONAL / À FORMALISER avant d'utiliser l'absence de wrap comme oracle obligatoire.

### BLK03-B3-012 — Statut global et validité
Les états de `B3_STATUS_GLOBAL` et les bits de `B3_VALIDITY_FLAGS` sont définis, mais aucune table normative exhaustive de dérivation n'est fournie.

**Classification** : NOT_DEFINED pour la dérivation complète. Les redondances fortes déjà identifiées sont traitées dans FT-BLK-01.

### BLK03-B3-013 — Sévérité globale et alarmes
La V1 définit `B3_SEVERITY_GLOBAL` et les flags d'alarme mais ne définit pas la fonction complète reliant ces représentations.

**Classification** : NOT_DEFINED.

### BLK03-B3-014 — Fenêtre et nombre d'échantillons
`B3_WINDOW_DURATION_MS`, `B3_VALID_SAMPLE_COUNT`, `WINDOW_COMPLETE` et `SAMPLE_COUNT_VALID` sont exposés, sans formule normative complète permettant de dériver les deux flags à partir des valeurs numériques.

**Classification** : NOT_DEFINED pour la dérivation intra-bloc ; les relations avec fréquence/configuration B4 sont DELEGATED à FT-INT.

### BLK03-B3-015 — Application des seuils
Les seuils utilisés sont ceux de la configuration B4 active au moment du calcul ; un changement ne rétroagit pas sur les valeurs déjà calculées et prend effet à partir de la prochaine fenêtre validée.

**Classification** : DELEGATED à FT-INT B4↔B3.

### BLK03-B3-016 — Redondances dépassement / alarmes
Les indicateurs `B3_EXCEED_*` et `B3_ALARM_LATCHED` possèdent des représentations redondantes explicites dans `B3_ALARM_FLAGS`.

**Classification** : DELEGATED / déjà COVERED par FT-BLK-01, sans duplication ici.

## 3. Limites d'exécution

Les tests BLK03-B3-001 à 004 ne deviennent exécutables que si le banc peut injecter ou rejouer exactement les échantillons X/Y/Z constituant une fenêtre. Le calcul d'oracle devra être effectué indépendamment du firmware testé.

Aucune tolérance numérique arbitraire n'est fixée dans cette fiche : elle devra être dérivée des règles d'arrondi / quantification lorsqu'elles seront normativement disponibles ou fixées dans le plan de banc.
