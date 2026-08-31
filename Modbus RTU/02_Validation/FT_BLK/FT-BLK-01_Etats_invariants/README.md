# FT-BLK-01 — États, flags et invariants d’état

## 1. Objet

FT-BLK-01 valide les relations fonctionnelles intra-bloc entre états synthétiques, flags et indicateurs redondants lorsque la spécification V1 fournit un oracle explicite ou suffisamment déterministe.

Elle couvre principalement les Blocs 1, 2, 3 et 7.

## 2. Doctrine

FT-BLK-01 ne crée aucune règle fonctionnelle absente de la V1.

Une relation n’est exécutable que si son résultat attendu peut être dérivé explicitement du texte normatif. Les compléments métier restent informatifs et ne peuvent pas servir à fabriquer une table de vérité protocolaire.

Les exigences sont classées :
- `COVERED` : oracle V1 exploitable ;
- `CONDITIONAL` : exigence normative réelle mais exécution dépendante d’un moyen d’injection ou d’une précision normative complémentaire ;
- `NOT_DEFINED` : relation attendue conceptuellement mais oracle insuffisant dans la V1 ;
- `TRACE_ONLY` : exigence identifiée mais validation déléguée à une autre famille.

## 3. Périmètre actif

FT-BLK-01 couvre :
- Bloc 1 : persistance conditionnelle des défauts et avertissements ;
- Bloc 2 : cohérence entre états temporels, flags et état du temps préparé ;
- Bloc 3 : cohérence entre indicateurs de dépassement / alarme mémorisée et leurs flags redondants ;
- Bloc 7 : sentinelle `last_fault_code = 0` et classification des relations de diagnostic insuffisamment définies.

## 4. Hors périmètre

Sont exclus :
- structure, typage, atomicité de lecture, MSW/LSW : FT-STR ;
- droits d’accès et exceptions Modbus : FT-ACC ;
- domaines unitaires et valeurs réservées : FT-LIM ;
- relations entre blocs : FT-INT ;
- mécanique de commandes Bloc 5 : FT-CMD ;
- séquences métier complètes : FT-SEQ ;
- robustesse transport / concurrence : FT-RBT ;
- persistance après reboot ou coupure : FT-PER ;
- monotonie de `uptime`, séquences et compteurs : FT-BLK-02 / FT-BLK-03 selon le cas.

## 5. Artefacts actifs

- `source/FT-BLK-01_source.md` : exigences sources normalisées et classification ;
- `detaille/FT-BLK-01_detaille.md` : cas de test génériques ;
- `detaille/FT-BLK-01_matrice_couverture.csv` : matrice exhaustive des relations candidates retenues.

## 6. Statut

Sous-famille reconstruite sur la base du cadrage validé. Gel interdit avant audit croisé et validation explicite.
