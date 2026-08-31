# FT-LIM-04 — Commandes : valeurs, soumission et préconditions fonctionnelles

## Objet
Valider les valeurs et préconditions fonctionnelles des commandes du Bloc 5 sans dupliquer les essais CRC/application de FT-LIM-03 ni les essais d'accès de FT-ACC.

## Périmètre
- domaine `cmd_request_code` ;
- validité de `transaction_id` et idempotence ;
- `submit` sur front montant ;
- paramètres de commande ;
- confirmation des commandes protégées ;
- préconditions fonctionnelles des commandes 1 à 11 ;
- annulation et nettoyage des champs de requête ;
- cohérence des codes de résultat.

## Hors périmètre
- permissions RW/RO et registres réservés : FT-ACC ;
- CRC et validation détaillée de la configuration : FT-LIM-03 ;
- mécanismes internes non observables et non spécifiés.

## Doctrine
Une valeur fonctionnellement invalide écrite dans un registre RW reste un accès Modbus valide. Le comportement attendu est observé via le moteur de commandes et ses codes de résultat ; aucune exception Modbus n'est inventée.

Les commandes sont prises en compte sur front montant de `submit`. Le firmware pouvant remettre `submit` à zéro après prise en compte, aucun test ne doit exiger `read(submit)==1` après soumission.

## Référentiel
- V1 `bloc5.md` ;
- V1 `bloc4.md` pour la commande 1 ;
- V1 `bloc2.md` pour la commande 2 ;
- mapping unifié dérivé ;
- doctrine GEL-GOV-01 / GEL-GOV-02.
