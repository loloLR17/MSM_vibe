# FT-LIM-02 — Contraintes croisées et dynamiques de la configuration préparée

## Objet

FT-LIM-02 vérifie les contraintes fonctionnelles V1 qui ne peuvent pas être décidées à partir de la valeur d'un seul champ.

Le périmètre actif V1 contient deux contraintes :

1. cohérence entre `sampling_frequency_hz`, `window_size_samples` et `indicator_period_ms` ;
2. compatibilité de `storage_limit_mb` avec la capacité de stockage utilisable déterminée par le firmware.

## Doctrine

FT-LIM-02 ne revalide ni les domaines unitaires de FT-LIM-01, ni les permissions d'accès de FT-ACC, ni les propriétés structurelles de FT-STR.

Une valeur ou combinaison fonctionnellement invalide écrite dans des champs RW valides n'est pas, de ce seul fait, une erreur d'accès Modbus. La configuration préparée ne doit cependant pas pouvoir être appliquée comme configuration active.

Après modification de 4B+4C+4D, le CRC préparé doit être recalculé avant de demander l'application.

## Contraintes normatives couvertes

### LIM02-C01 — Période indicateur / durée de fenêtre

`indicator_period_ms >= 1000 × window_size_samples / sampling_frequency_hz`

Dans les domaines V1 actuellement autorisés, les 4 tailles de fenêtre et les 5 périodes indicateur produisent 20 couples admissibles, et tous satisfont cette contrainte avec `sampling_frequency_hz = 26667`.

FT-LIM-02 vérifie exhaustivement ces 20 combinaisons sans fabriquer de cas négatif utilisant une valeur déjà hors domaine FT-LIM-01.

### LIM02-C02 — Limite de stockage / capacité utilisable

Pour une capacité utilisable connue `C` en MB :

- `storage_limit_mb = C` doit être compatible avec cette contrainte ;
- `storage_limit_mb = C + 1` ne doit pas être validable ;
- `storage_limit_mb = 1` est compatible si `C >= 1`.

Aucune valeur arbitraire de `C` n'est figée dans la validation. `C` est une donnée de l'environnement d'essai déterminée par un moyen fiable et documenté.

## Explicitement non spécifié en V1

FT-LIM-02 n'invente notamment aucune règle normative imposant :

- `rms_warn_threshold_mg < rms_alarm_threshold_mg` ;
- `peak_warn_threshold_mg < peak_alarm_threshold_mg` ;
- une relation numérique entre hystérésis et seuils ;
- une relation entre `supervision_enable_mask` et les seuils ;
- une formule de compatibilité entre durée de campagne et volume de stockage.

Ces relations ne sont pas des critères PASS/FAIL V1 tant qu'elles ne sont pas introduites dans une source normative supérieure.

## Hors périmètre

- domaines unitaires : FT-LIM-01 ;
- CRC, complétude, validation et application de la configuration : future FT-LIM-03 ;
- commandes Bloc 5 : future FT-LIM-04 ;
- préparation/application temporelle Bloc 2 : future sous-famille dédiée ;
- domaines dynamiques Bloc 6 : future sous-famille dédiée.

## Artefacts

- `source/FT-LIM-02_source.md` : exigences dérivées ;
- `detaille/FT-LIM-02_detaille.md` : cas génériques ;
- `detaille/FT-LIM-02_matrice_contraintes.csv` : matrice de traçabilité ;
- `instancie/FT-LIM-02_execution.md` : procédure d'exécution ;
- `instancie/FT-LIM-02_instancie_index.csv` : 23 instances actives ;
- `instancie/FT-LIM-02_instancie_overview.md` : synthèse de couverture.
