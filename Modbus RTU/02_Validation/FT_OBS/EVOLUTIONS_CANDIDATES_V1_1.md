# FT-OBS — Évolutions candidates V1.1

Ce document consolide les limites observées pendant FT-OBS. Elles ne constituent **pas** des exigences V1 et ne doivent pas être utilisées comme oracle de conformité actuel.

## État système et diagnostic

- définir normativement `B1.system_status` ;
- définir la table détaillée `B1.system_flags` ;
- définir les tables B1 `fault_flags`, `warning_flags`, `error_code`, `warning_code` si leur exploitation distante est souhaitée ;
- définir, seulement si nécessaire, les relations normatives entre l'état synthétique B1 et les diagnostics B7.

## Validité et fraîcheur

- préciser la politique numérique produisant B3 `VALUES_FRESH` à partir de l'âge si une reproduction côté centrale est souhaitée ;
- éventuellement définir les combinaisons obligatoires/interdites entre `B3_STATUS_GLOBAL` et `B3_VALIDITY_FLAGS` ;
- définir une fraîcheur/âge des diagnostics B7 tels que température et tension si nécessaire ;
- ne créer une convention transversale « absent/non renseigné/invalide » que si un besoin réel le justifie.

## Alarmes et défauts

- définir un modèle transversal actif/mémorisé/acquitté uniquement si souhaité ;
- définir la convention de `last_fault_timestamp` lorsque `last_fault_code=0` ;
- ajouter un journal de défauts si le seul dernier défaut devient insuffisant.

## Codes de détail

- table `B4.config_error_code` ;
- catalogue `B5.cmd_result_detail` si nécessaire ;
- catalogue `B7.selftest_result_code` / `selftest_detail`.

## Corrélation et historique B5

- profondeur et durée de rétention de l'historique ;
- comportement d'un `transaction_id` réutilisé avec un payload différent ;
- persistance de l'idempotence/corrélation après reboot : sujet partagé avec les dettes déjà tracées FT-PER/FT-CMD.

## Verdict d'exploitabilité

Un état transversal unique « exploitable/inexploitable » n'est recommandé que si un besoin système clair apparaît. En son absence, conserver les discriminants locaux B2/B3/B6/B7 évite une perte d'information et des dépendances implicites.

## Règle de gouvernance

Toute promotion d'un point de cette liste vers la V1.1 doit entraîner l'analyse d'impact des familles propriétaires concernées et les tests de non-régression correspondants.