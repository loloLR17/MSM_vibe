# FT-INT-01 — Temps et commandes

## 1. Objet

FT-INT-01 valide les effets temporels inter-blocs explicitement imposés entre le Bloc 2 et le Bloc 5 lors d'une synchronisation horaire réussie.

## 2. Périmètre actif

La sous-famille couvre :
- application effective du temps préparé B2 après commande B5 de synchronisation réussie ;
- mise à jour de `last_sync_time` après synchronisation effective ;
- mise en cohérence observable de l'état temporel B2 après synchronisation réussie, sans inventer de table de transitions non spécifiée.

## 3. Hors périmètre

Sont exclus :
- absence d'effet immédiat de l'écriture de `prepared_time` : déjà couverte par FT-BLK-02 ;
- monotonie de `current_time`, stabilité de `last_sync_time` hors synchronisation et dérivation de `time_since_sync` : FT-BLK-02 ;
- soumission, acceptation, refus, idempotence, `transaction_id`, `cmd_status` et `cmd_result_code` : FT-CMD ;
- comportement après reboot ou coupure : FT-PER ;
- égalités inter-blocs non explicitement imposées par la V1.

## 4. Cas actifs

- `TT-INT-B02B05-001` — application effective du temps préparé ;
- `TT-INT-B02B05-002` — mise à jour de `last_sync_time` ;
- `TT-INT-B02B05-003` — cohérence minimale de l'état temporel après synchronisation.

## 5. Artefacts

- `source/FT-INT-01_source.md` ;
- `detaille/FT-INT-01_detaille.md` ;
- `detaille/FT-INT-01_matrice_couverture.csv`.

## 6. Statut

Sous-famille reconstruite selon le cadrage validé. Gel interdit avant audit croisé et validation explicite.
