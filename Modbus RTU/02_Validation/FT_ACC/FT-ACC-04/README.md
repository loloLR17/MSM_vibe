# FT-ACC-04 — Registres réservés

## Objet
Valider l’interdiction d’écriture sur toute zone réservée exposée par le mapping V1.

FT-ACC-04 ne revalide pas la structure des réservés ni leur valeur de lecture nominale : ces aspects relèvent de FT-STR. Elle vérifie exclusivement le comportement d’accès en écriture.

## Référentiel de vérité
1. spécification V1 ;
2. mapping unifié gelé ;
3. `source/` ;
4. `detaille/` ;
5. `instancie/`.

## Doctrine GEL-GOV-02
Toute écriture visant un registre réservé constitue un accès Modbus invalide :
- exception Modbus standard appropriée obligatoire ;
- aucune modification de registre ou d’état interne ;
- aucune exécution partielle ;
- comportement déterministe.

Une écriture « acceptée mais ignorée » est non conforme.

## Périmètre
- zones réservées mono-registre ;
- zones réservées multi-registres ;
- une instanciation par zone réservée logique unique.

Sont hors périmètre : lecture structurale des réservés (FT-STR), champs RO non réservés (FT-ACC-03), adresses inexistantes ou requêtes composites invalides (FT-ACC-06).

## Structure
- `source/` : exigence de validation ;
- `detaille/` : tests génériques sans adresse ;
- `instancie/` : tests dérivés du mapping ;
- `archive_pre_renforcement/` : historique non exécutable.

## Statut
Reconstruction alignée sur GEL-GOV-02.
