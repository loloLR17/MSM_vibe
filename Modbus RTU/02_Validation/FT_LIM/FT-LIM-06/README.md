# FT-LIM-06 — Domaines et cohérence fonctionnelle des métadonnées de campagne

## Objet

Valider les domaines normatifs et les invariants fonctionnels des métadonnées RO du Bloc 6 pour une campagne sélectionnée valide.

## Périmètre

- `campaign_state` : domaine 0..5 ;
- `data_integrity_status` : domaine 0..3 ;
- `storage_health_status` : domaine 0..3 ;
- `campaign_id != 0` pour une campagne valide ;
- `end_timestamp = 0` lorsque la campagne est en cours ;
- cohérence de `duration_s` avec les timestamps, tracée sans inventer une formule plus précise que la V1.

## Hors périmètre

- sélection dynamique 6003/6004 : FT-LIM-05 ;
- permissions RO : FT-ACC ;
- atomicité structurelle des uint32 et snapshots : FT-STR ;
- diagnostic Bloc 7 : future FT-LIM-07 ;
- règles uniquement issues des compléments métier informatifs.

## Principe

Les champs sont RO : FT-LIM-06 observe des états réels ou provoqués par des opérations fonctionnelles autorisées. Aucun registre RO n’est écrit pour fabriquer un cas de test.
