# FT-CMD-03 — États, résultats et historique

## 1. Objet

FT-CMD-03 valide l'exposition normative des états de traitement, des codes résultat et de l'historique minimal de la dernière commande terminée du Bloc 5.

## 2. Périmètre actif

La sous-famille couvre :
- les valeurs définies de `cmd_status` ;
- la cohérence d'un état final avec l'issue de la commande ;
- les valeurs définies de `cmd_result_code` sans réinventer leurs conditions d'emploi ;
- `cmd_active_code` et `cmd_active_transaction_id` dans les cas non concurrents ;
- la mémorisation de la dernière commande terminée dans `cmd_last_*` ;
- la mise à jour de `cmd_last_timestamp` lors de la terminaison d'une commande.

## 3. Hors périmètre

Sont exclus :
- les conditions métier précises conduisant à chaque code résultat : FT-CMD-05 à FT-CMD-07 selon la commande ;
- la concurrence et l'exposition ambiguë des champs actifs lors d'un refus concurrent : FT-CMD-04 ;
- l'idempotence : FT-CMD-02 ;
- la table sémantique exhaustive de `cmd_result_detail`, non définie en V1 ;
- toute séquence obligatoire `reçu -> accepté -> en cours -> final`, non définie en V1 ;
- la structure MSW/LSW du timestamp, déjà couverte par FT-STR.

## 4. Cas actifs

- `TT-CMD-B05-200` — exposition d'un état final succès ;
- `TT-CMD-B05-201` — exposition d'un état final refus ;
- `TT-CMD-B05-202` — exposition d'un état final échec ;
- `TT-CMD-B05-203` — cohérence nominale des champs `cmd_active_*` ;
- `TT-CMD-B05-204` — mise à jour de l'historique après commande terminée ;
- `TT-CMD-B05-205` — historique de la dernière commande terminée quel que soit son résultat ;
- `TT-CMD-B05-206` — mise à jour du timestamp de fin.

## 5. Dettes V1 conservées

- aucune séquence exhaustive des états intermédiaires n'est imposée ;
- aucune table générique exhaustive ne relie les codes résultat aux commandes ;
- la sémantique exhaustive de `cmd_result_detail` n'est pas définie.

## 6. Statut

Sous-famille reconstruite selon le cadrage validé. Merge interdit avant validation explicite.
