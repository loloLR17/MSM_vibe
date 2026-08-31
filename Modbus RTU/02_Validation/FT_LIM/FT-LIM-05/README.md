# FT-LIM-05 — Sélection dynamique de campagne et validité de l’entrée exposée

## Objet

Valider le domaine fonctionnel dynamique de `selected_campaign_index` (6003) et la sémantique de l’entrée de campagne exposée dans le Bloc 6.

Pour `N = total_campaign_count`, le domaine valide est `0..N-1`. Une valeur hors plage reste une écriture Modbus valide sur 6003 : elle est acceptée, `selected_campaign_valid` vaut 0 et les métadonnées exposées ne doivent pas être interprétées comme valides.

## Périmètre

- bornes dynamiques selon N ;
- cas N=0 ;
- première, dernière et valeur intérieure si disponible ;
- première valeur hors plage et borne uint16 ;
- `selected_campaign_valid` comme oracle principal ;
- `campaign_id != 0` pour une campagne valide ;
- cohérence sémantique de l’entrée exposée sans dupliquer FT-STR.

## Hors périmètre

- permissions d’accès : FT-ACC ;
- cohérence structurelle multi-registres générale : FT-STR ;
- domaines RO de `campaign_state`, `data_integrity_status`, `storage_health_status` ;
- contenu exact des métadonnées lorsque `selected_campaign_valid=0`, non défini par la V1.

## Structure

- `source/FT-LIM-05_source.md`
- `detaille/FT-LIM-05_detaille.md`
- `detaille/FT-LIM-05_matrice_selection.csv`
- `instancie/FT-LIM-05_execution.md`
- `instancie/FT-LIM-05_instancie_index.csv`
- `instancie/FT-LIM-05_instancie_overview.md`
