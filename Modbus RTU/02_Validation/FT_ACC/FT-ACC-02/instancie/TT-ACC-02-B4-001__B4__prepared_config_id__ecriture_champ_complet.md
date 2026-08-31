# TT-ACC-02-B4-001 — Bloc 4 — prepared_config_id

## Objectif
Vérifier le droit d'écriture nominal du champ `prepared_config_id` (`uint32`, `4002..4003`, RW).

## Étapes
1. Sauvegarder la valeur initiale.
2. Écrire une valeur V1 sûre sur le champ complet avec FC16.
3. Vérifier l'absence d'exception.
4. Relire le champ et vérifier la prise en compte.
5. Restaurer si nécessaire.

## Résultat attendu
Écriture autorisée. Toute évolution normative de `config_state` est admise et n'est pas un défaut FT-ACC-02.
