# FT-CMD-01 — Soumission et transaction

## 1. Objet

FT-CMD-01 valide les règles normatives de prise en compte d'une commande B5 avant toute logique propre à une commande donnée.

## 2. Périmètre actif

La sous-famille couvre :
- déclenchement uniquement sur front montant `submit : 0 → 1` ;
- absence de réexécution lorsque `submit` reste à `1`, avec exécution conditionnelle si ce niveau peut être observé/maintenu par le moyen d'essai ;
- remise automatique de `submit` à `0` après prise en compte ;
- absence de prise en compte lorsque `cmd_request_code = 0` ;
- obligation documentaire de `transaction_id` ;
- refus fonctionnel d'une soumission avec `transaction_id = 0`, avec `cmd_result_code = 14` ;
- traitement fonctionnel d'un `cmd_request_control` contenant un bit réservé à `1` lors d'une soumission.

## 3. Hors périmètre

Sont exclus :
- réutilisation d'un `transaction_id` déjà traité et réemploi du résultat : FT-CMD-02 ;
- corrélation requête/réponse : FT-CMD-02 ;
- politique de réutilisation d'un ID après disparition de l'historique d'idempotence : non définie en V1 ;
- concurrence entre deux commandes : FT-CMD-04 ;
- accès RW/RO et exceptions Modbus : FT-ACC ;
- domaines purs des champs : FT-LIM ;
- effets métier des commandes : FT-CMD-05 à 07 et FT-INT.

## 4. Cas actifs

- `TT-CMD-B05-001` — aucune prise en compte sans front montant de `submit` ;
- `TT-CMD-B05-002` — prise en compte sur front montant valide ;
- `TT-CMD-B05-003` — maintien de `submit = 1` sans nouvelle exécution (`CONDITIONAL`) ;
- `TT-CMD-B05-004` — remise automatique de `submit` à `0` après prise en compte ;
- `TT-CMD-B05-005` — `cmd_request_code = 0` ne déclenche aucune commande ;
- `TT-CMD-B05-006` — bits réservés de `cmd_request_control` à `1` : refus fonctionnel par code résultat `2`, sans exécution de l'action ;
- `TT-CMD-B05-007` — `transaction_id = 0` : refus fonctionnel par code résultat `14`, sans exécution de l'action.

## 5. Dettes / limites conservées

La V1 définit désormais `transaction_id = 0` comme invalide à la soumission et `1..65535` comme identifiants valides. En revanche, la politique de réutilisation/bouclage d'un identifiant après disparition de son historique d'idempotence reste non définie.

La règle « maintien de submit à 1 sans réexécution » est normative, mais son observation directe peut être impossible sur une interface Modbus conforme qui applique immédiatement l'auto-clear également normatif. Le test correspondant reste donc conditionnel à un moyen d'essai permettant d'observer cette condition sans créer artificiellement de nouveaux fronts.

## 6. Artefacts

- `source/FT-CMD-01_source.md` ;
- `detaille/FT-CMD-01_detaille.md` ;
- `detaille/FT-CMD-01_matrice_couverture.csv`.

## 7. Statut

Sous-famille réalignée avec la spécification V1 corrigée lors de l'audit final transversal.
