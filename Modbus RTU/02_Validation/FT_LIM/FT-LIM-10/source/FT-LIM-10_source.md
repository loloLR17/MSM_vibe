# FT-LIM-10 — Source normative

## Référence
V1 Bloc 3 — Supervision vibratoire.

## Exigences

### LIM10-RQ-001 — Statut global
`B3_STATUS_GLOBAL` ∈ {0,1,2,3,4,5}. 6..65535 réservés.

### LIM10-RQ-002 — Validity flags
Bits 0..12 définis ; bits 13..15 réservés. Oracle : `(value & 0xE000)==0`.

### LIM10-RQ-003 — Alarm flags
Bits 0..8 définis ; bits 9..15 réservés. Oracle : `(value & 0xFE00)==0`.

### LIM10-RQ-004 — Sévérité
`B3_SEVERITY_GLOBAL` ∈ {0..5}. 6..65535 réservés.

### LIM10-RQ-005 — Axe dominant
`B3_DOMINANT_AXIS` ∈ {0..4}. 5..65535 réservés.

### LIM10-RQ-006 — Dépassements
`B3_EXCEED_GLOBAL`, `X`, `Y`, `Z` ∈ {0,1}. 2..65535 réservés.

### LIM10-RQ-007 — Alarme mémorisée
`B3_ALARM_LATCHED` ∈ {0,1}. 2..65535 réservés.

### LIM10-RQ-008 — Compteurs
`B3_EXCEED_COUNT` et `B3_ALARM_COUNT` sont monotones et peuvent saturer à `0xFFFFFFFF`.

### LIM10-RQ-009 — Réserves
Les registres 3040..3047 sont maintenus à zéro.

### LIM10-RQ-010 — Configuration active
Les seuils appliqués à une fenêtre sont ceux de la configuration active au moment du calcul.

### LIM10-RQ-011 — Non-rétroactivité
Un changement de configuration ne modifie pas les valeurs déjà calculées. Il prend effet à partir de la prochaine fenêtre de calcul validée.

### LIM10-RQ-012 — Valeur conservée et qualification
Une dernière valeur calculée peut rester exposée en cas d’indisponibilité temporaire ; sa confiance doit être qualifiée via statut, flags, âge et horodatage. La présence numérique seule ne constitue pas un oracle de validité.

### LIM10-RQ-013 — Grandeurs V1
Toutes les grandeurs vibratoires du Bloc 3 sont des accélérations en mg ; aucune valeur en vitesse mm/s n’est exposée en V1.

## Non défini / à ne pas inventer
- relation exhaustive `STATUS_GLOBAL` ↔ masque exact de validity flags ;
- relation obligatoire `ALARM_FLAGS` ↔ `SEVERITY_GLOBAL` ;
- relation exhaustive entre `EXCEED_*`, flags d’alarme et `ALARM_LATCHED` ;
- seuil numérique de fraîcheur ;
- égalité exacte `VALUE_AGE_MS` ↔ `LAST_UPDATE_TR2` ;
- plage fonctionnelle RMS/crête ;
- relation algébrique imposée entre valeurs globales et valeurs par axe au niveau des registres exposés.

La cohérence atomique des snapshots et des uint32 reste principalement couverte par FT-STR.
