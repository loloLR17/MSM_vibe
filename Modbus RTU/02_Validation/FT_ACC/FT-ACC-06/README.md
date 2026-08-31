# FT-ACC-06 — README

## Objet
Valider le rejet atomique des écritures Modbus composites contenant au moins un registre dont l'écriture est interdite.

FT-ACC-06 complète FT-ACC-02, FT-ACC-03 et FT-ACC-04 : elle ne reteste pas un droit isolé, mais vérifie qu'une requête FC16 mêlant registres autorisés et interdits est rejetée intégralement.

## Doctrine
Une requête FC16 est invalide dès qu'au moins un registre ciblé est non inscriptible selon V1/GEL-MAP-V1.

Le comportement conforme est :
- exception Modbus standard appropriée ;
- aucune modification d'aucun registre de la requête, y compris ceux qui seraient RW isolément ;
- aucun effet interne ;
- aucune exécution partielle ;
- comportement déterministe lors d'une répétition identique.

Une écriture silencieusement partielle est interdite.

## Hors périmètre
- lectures partielles ou traversant des frontières logiques valides : FT-STR-06 ;
- adresses inexistantes en lecture et quantités FC03 invalides : FT-STR-06 ;
- écritures RO isolées : FT-ACC-03 ;
- écritures réservées isolées : FT-ACC-04 ;
- valeurs métier invalides dans un champ RW valide : FT-LIM ;
- trames malformées, CRC et robustesse liaison : autres familles dédiées.

## Hiérarchie documentaire
1. spécification V1 ;
2. GEL-MAP-V1 ;
3. `source/` ;
4. `detaille/` ;
5. `instancie/`.

`archive_pre_renforcement/` est historique et non exécutable en validation V1 active.

## Couverture active
Deux classes génériques :
1. FC16 composite RW + RO ;
2. FC16 composite RW + réservé.

Les cas instanciés ciblent les frontières d'accès réellement pertinentes du mapping courant.
