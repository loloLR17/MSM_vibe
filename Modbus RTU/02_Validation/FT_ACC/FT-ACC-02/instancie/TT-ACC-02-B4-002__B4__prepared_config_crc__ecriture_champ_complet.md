# TT-ACC-02-B4-002 — Bloc 4 — prepared_config_crc

## Objectif
Vérifier le droit d'écriture nominal du champ `prepared_config_crc` (`uint32`, `4008..4009`, RW).

## Étapes
1. Sauvegarder la valeur initiale.
2. Écrire le champ complet avec FC16.
3. Vérifier l'absence d'exception.
4. Relire et vérifier la prise en compte.
5. Restaurer si nécessaire.

## Résultat attendu
Accès en écriture accepté. La validité du CRC relève de la logique de configuration, pas de FT-ACC-02.
