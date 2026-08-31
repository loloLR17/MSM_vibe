# FT-BLK-04 — Cycle de vie de configuration préparée

## 1. Objet

FT-BLK-04 valide les règles fonctionnelles internes du Bloc 4 relatives au cycle de vie d'une configuration préparée, à la séparation préparé/actif et aux CRC explicitement définis par la V1.

## 2. Doctrine

FT-BLK-04 vérifie uniquement les règles intra-bloc disposant d'un oracle normatif. Elle ne duplique ni les domaines de valeurs (FT-LIM), ni les droits d'accès (FT-ACC), ni la mécanique des commandes B5 (FT-CMD), ni les effets B4→B3 (FT-INT).

Les transitions dépendant d'une commande B5 restent tracées mais déléguées.

## 3. Périmètre actif

- transitions induites par l'écriture/modification de la zone préparée ;
- invalidation d'un état `VALIDE` après modification ;
- séparation préparé / actif ;
- absence d'effet immédiat de la configuration préparée ;
- CRC préparé : algorithme, périmètre, sérialisation et vecteur normatif `0x5207CCFC` ;
- cohérence conditionnelle de `active_config_crc` avec l'image active 4E.

## 4. Limitations conservées

- `config_revision_counter` : politique d'incrément non définie par la V1 ;
- `config_error_code` : dérivation détaillée non définie ici ;
- application/validation via B5 : déléguée à FT-CMD / FT-INT ;
- aucune exigence d'auto-recalcul firmware de `prepared_config_crc` après écriture : la V1 impose à la centrale de recalculer et mettre à jour ce champ ;
- aucune règle de domaine métier n'est retestée ici.

## 5. Hors périmètre

- structure, atomicité, snapshot, MSW/LSW : FT-STR ;
- permissions RO/RW et accès réservés : FT-ACC ;
- domaines et valeurs invalides : FT-LIM ;
- exécution des commandes de validation/application : FT-CMD ;
- effets de la configuration active sur B3 : FT-INT ;
- persistance après reboot/coupure : FT-PER.

## 6. Artefacts

- `source/FT-BLK-04_source.md` ;
- `detaille/FT-BLK-04_detaille.md` ;
- `detaille/FT-BLK-04_matrice_couverture.csv`.

## 7. Statut

Sous-famille reconstruite sur le cadrage validé. Gel interdit avant audit croisé et validation explicite.
