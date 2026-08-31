# FT-LIM-09 — Vue d’ensemble

## Couverture
13 cas génériques et 13 instances.

## Oracles directs
- domaines des trois enums temporels ;
- bits réservés de `time_flags` à zéro ;
- préparation sans effet immédiat ;
- monotonie de l’horloge hors synchronisation ;
- invariance de `last_sync_time` sans synchronisation.

## Oracles conditionnels
- application effective via commande 2 ;
- correction de l’horloge lors de la synchronisation ;
- mise à jour de `last_sync_time` ;
- évolution cohérente de `time_since_sync`.

## Doctrine
FT-LIM-09 ne rejuge pas les codes résultat et préconditions de la commande 2 déjà couverts par FT-LIM-04. Il juge l’effet temporel observable.

Aucune tolérance, plage de dérive, précision maximale ou relation exhaustive status/flags n’est inventée lorsque la V1 ne la définit pas.
