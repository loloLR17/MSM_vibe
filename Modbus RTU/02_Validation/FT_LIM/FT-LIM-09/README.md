# FT-LIM-09 — Temps et synchronisation

## Objet
Valider les domaines et invariants fonctionnels du sous-système temporel du Bloc 2, en distinguant strictement temps préparé, horloge courante et synchronisation effectivement appliquée.

## Périmètre
- domaines `time_status`, `prepared_time_status`, `sync_source` ;
- bits réservés de `time_flags` ;
- écriture du temps préparé sans effet immédiat sur l’horloge courante ;
- application du temps uniquement par la commande 2 ;
- monotonie de `current_time` hors resynchronisation ;
- mise à jour de `last_sync_time` seulement lors d’une synchronisation effective ;
- cohérence de `time_since_sync` sans inventer de tolérance numérique absente de la V1.

## Frontières
FT-LIM-04 couvre l’acceptation/refus de la commande 2. FT-LIM-09 couvre les effets et invariants temporels avant/après cette commande.

Les accès RW/RO relèvent de FT-ACC et les règles structurelles multi-registres de FT-STR.
