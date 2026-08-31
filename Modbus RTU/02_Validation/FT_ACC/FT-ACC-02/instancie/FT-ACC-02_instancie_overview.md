# FT-ACC-02 — Tests instanciés

## Objet

Instanciations des droits d'écriture `RW` dérivées de GEL-MAP-V1 et contrôlées contre la spécification V1.

## Règle d'instanciation

- une fiche par champ logique RW ;
- aucune pseudo-plage de bloc n'est créée si elle traverse des registres RO ou réservés ;
- les champs `uint32` et `ASCII fixe` sont écrits comme unités logiques complètes ;
- la relecture stricte `valeur lue = valeur écrite` n'est utilisée que pour les champs dont la sémantique V1 est stable ;
- les registres du Bloc 5 sont testés avec `submit = 0` sauf scénario fonctionnel dédié hors FT-ACC-02 ;
- `cmd_request_control` est testé avec une valeur non déclenchante et n'impose pas une persistance incompatible avec sa sémantique de front montant.

## Couverture active

- Bloc 2 : 1 champ logique RW ;
- Bloc 4 : 26 champs logiques RW ;
- Bloc 5 : 7 champs logiques RW ;
- Bloc 6 : 1 champ logique RW.

**Total : 35 champs logiques RW.**

## Références

Ordre de vérité : V1 → GEL-MAP-V1 → source FT-ACC-02 → génériques → instanciés.

Les anciens tests de plage et l'ancienne génération sont conservés sous `archive_pre_renforcement/` à titre historique uniquement.
