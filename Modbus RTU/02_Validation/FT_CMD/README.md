# FT-CMD — Moteur de commandes Bloc 5

## 1. Objet

FT-CMD valide le comportement transactionnel normatif du Bloc 5 : soumission, transaction, idempotence, corrélation, concurrence, états, résultats, historique et règles propres aux commandes.

## 2. Référentiel

Source normative principale : `01_Specification_source/bloc5.md` V1, complétée uniquement par les règles normatives explicites des autres blocs et par les délégations des familles gelées.

Les compléments métier explicitement informatifs ne sont pas utilisés comme oracles.

## 3. Décomposition validée

- `FT-CMD-01_Soumission_transaction`
- `FT-CMD-02_Idempotence_correlation`
- `FT-CMD-03_Etats_resultats_historique`
- `FT-CMD-04_Concurrence_controles`
- `FT-CMD-05_Configuration_temps`
- `FT-CMD-06_Acquisition_diagnostic`
- `FT-CMD-07_Maintenance_commandes_protegees`

## 4. Convention des tests

Convention retenue : `TT-CMD-B05-<numéro>` avec numérotation globale à FT-CMD.

## 5. Dettes normatives V1 à conserver

Sont notamment à tracer sans fabrication d'oracle :
- domaine exact d'un `transaction_id` invalide ;
- profondeur/durée de mémoire d'idempotence ;
- comportement exact si un `transaction_id` déjà traité est réutilisé avec un contenu de requête différent ;
- exposition exacte de `cmd_active_*` lors du refus d'une commande concurrente ;
- liste des commandes annulables et état final d'une annulation réussie ;
- effet exact de `clear_request_fields` ;
- table sémantique exhaustive de `cmd_result_detail` ;
- politique recommandée d'entrée en maintenance acquisition arrêtée, non normative en V1.

## 6. Statut

- FT-CMD-01 : validée et mergée dans `main` ;
- FT-CMD-02 : reconstruite, en attente de validation ;
- FT-CMD-03 à FT-CMD-07 : non démarrées.

Aucun gel ni merge global sans validation explicite.
