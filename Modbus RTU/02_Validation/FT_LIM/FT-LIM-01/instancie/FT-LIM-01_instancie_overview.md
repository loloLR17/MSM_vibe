# FT-LIM-01 — Overview d'instanciation

## Couverture active

FT-LIM-01 contient **68 instanciations**, toutes sur le Bloc 4 et pilotées par un index CSV unique.

Répartition :
- identifiants requis : 6 ;
- `sampling_frequency_hz` : 3 ;
- `axes_enable_mask` : 21 ;
- `full_scale_code` : 6 ;
- `acquisition_mode` : 4 ;
- `window_size_samples` : 8 ;
- `indicator_period_ms` : 9 ;
- `campaign_duration_s` : 5 ;
- `storage_mode` : 4 ;
- `storage_limit_mb` : 2.

Les 68 instances comprennent **30 valeurs candidates valides/conditionnellement valides** et **38 valeurs invalides ou non validables**.

## Couverture de la matrice des RW Bloc 4

Les 26 champs RW logiques du Bloc 4 sont tous classifiés :

- 11 champs avec domaine statique directement testable par FT-LIM-01 ;
- 1 champ avec domaine statique partiel + contrainte dynamique (`storage_limit_mb`) ;
- 1 champ différé vers la validation CRC (`prepared_config_crc`) ;
- 2 champs relevant de FT-STR pour leur contrainte connue (`campaign_label`, `mission_label`) ;
- 11 champs sans domaine fonctionnel V1 suffisamment défini pour produire un test de limites.

Aucun champ RW du Bloc 4 n'est orphelin de classification.

## Règle d'interprétation

`NOT_DEFINED` ne signifie pas « toute valeur est fonctionnellement valide ». Il signifie uniquement qu'aucune restriction métier supplémentaire au domaine représentationnel ne peut être affirmée à partir de la V1 sans invention.

Les valeurs des « Compléments métier » ne sont pas utilisées comme codes protocole.
