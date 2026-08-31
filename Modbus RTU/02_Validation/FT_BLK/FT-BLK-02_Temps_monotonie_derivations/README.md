# FT-BLK-02 — Temps, monotonie et dérivations internes

## 1. Objet

FT-BLK-02 valide les invariants temporels et les dérivations internes explicitement imposés par la spécification V1, sans retester l'encodage structurel ni les relations inter-blocs.

Elle couvre principalement les Blocs 1, 2, 3 et 7.

## 2. Doctrine

FT-BLK-02 vérifie uniquement les propriétés intra-bloc suivantes lorsqu'un oracle V1 existe :
- monotonie ;
- stabilité hors événement autorisé ;
- dérivation temporelle interne ;
- absence d'effet immédiat d'une valeur préparée sur une valeur appliquée.

Une relation entre deux blocs est déléguée à FT-INT. Un comportement consécutif à un reset ou une coupure est délégué à FT-PER. Les mécaniques de commande B5 sont déléguées à FT-CMD.

## 3. Périmètre actif

FT-BLK-02 couvre :
- Bloc 1 : monotonie de `uptime_s` ;
- Bloc 2 : monotonie de `current_time` hors resynchronisation, absence d'effet immédiat de `prepared_time`, stabilité de `last_sync_time` hors synchronisation effective, cohérence de `time_since_sync` ;
- Bloc 3 : monotonie de `B3_CALC_SEQUENCE`, classification conditionnelle de `B3_VALUE_AGE_MS` ;
- Bloc 7 : monotonie de `uptime_s` hors reset.

## 4. Hors périmètre

Sont exclus :
- MSW/LSW, atomicité et snapshot : FT-STR ;
- droits d'accès : FT-ACC ;
- domaines de valeurs : FT-LIM ;
- égalité B1 uptime ↔ B7 uptime : FT-INT ;
- application effective d'une synchronisation via B5 : FT-CMD / FT-INT ;
- comportement après reboot ou coupure : FT-PER ;
- compteurs `B3_EXCEED_COUNT` et `B3_ALARM_COUNT` : FT-BLK-03.

## 5. Artefacts actifs

- `source/FT-BLK-02_source.md` ;
- `detaille/FT-BLK-02_detaille.md` ;
- `detaille/FT-BLK-02_matrice_couverture.csv`.

## 6. Statut

Sous-famille reconstruite sur la base du cadrage validé. Gel interdit avant audit croisé et validation explicite.
