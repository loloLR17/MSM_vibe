# FT-LIM-09 — Source normative

## Référence
V1 Bloc 2, avec interaction contrôlée avec la commande 2 du Bloc 5.

## Exigences

### LIM09-RQ-001 — time_status
Domaine normatif : {0,1,2,3,4}. 5..65535 réservés.

### LIM09-RQ-002 — time_flags
Bits 0..7 définis ; bits 8..15 réservés et nuls. Oracle : `(value & 0xFF00)==0`.

### LIM09-RQ-003 — prepared_time_status
Domaine normatif : {0,1,2,3}. 4..65535 réservés.

### LIM09-RQ-004 — sync_source
Domaine normatif : {0,1,2,3,4}. 5..65535 réservés.

### LIM09-RQ-005 — Préparation sans application immédiate
Écrire `prepared_time_msw/lsw` prépare une valeur de synchronisation et ne doit pas modifier immédiatement `current_time`. L’horloge courante continue son évolution normale jusqu’à application effective.

### LIM09-RQ-006 — Application contrôlée
La valeur préparée est appliquée uniquement via la commande 2 du Bloc 5. FT-LIM-09 vérifie l’effet temporel ; FT-LIM-04 reste l’oracle des préconditions/résultats de commande.

### LIM09-RQ-007 — Monotonie horloge courante
`current_time` doit être monotone hors resynchronisation. Une discontinuité est admissible lors d’une synchronisation effective.

### LIM09-RQ-008 — last_sync_time
`last_sync_time` ne doit être mis à jour que lorsqu’une synchronisation est effectivement appliquée.

### LIM09-RQ-009 — time_since_sync
`time_since_sync` doit évoluer de manière cohérente avec le temps écoulé depuis la dernière synchronisation. La V1 ne fixe pas ici une tolérance ni une règle d’arrondi permettant d’imposer une égalité exacte.

## Non défini / à ne pas inventer
- correspondance exhaustive entre `time_status` et les bits de `time_flags` ;
- seuil maximal de `time_accuracy_ms` ;
- plage métier acceptable de `drift_ppm` ;
- durée maximale autorisée depuis dernière synchronisation ;
- tolérance exacte entre `current_time`, `last_sync_time` et `time_since_sync` ;
- relation automatique `sync_source` → valeur particulière de timestamp.
