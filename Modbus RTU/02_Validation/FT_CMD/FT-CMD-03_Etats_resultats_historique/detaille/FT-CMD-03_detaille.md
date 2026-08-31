# FT-CMD-03 — Validation détaillée

## 1. Principe

Les essais utilisent une commande dont l'issue attendue est explicitement normée par sa sous-famille métier ou une injection de banc maîtrisée. FT-CMD-03 valide alors la représentation Bloc 5 de cette issue ; il ne redéfinit pas la cause métier.

## 2. Cas de test

### TT-CMD-B05-200 — État final succès

Précondition : disposer d'une commande dont le succès est obtenu de façon déterministe.

Action : soumettre une transaction nouvelle et attendre sa terminaison.

Oracle :
- `cmd_status = 4` ;
- `cmd_result_code = 0` si la commande se termine avec succès.

Ne pas exiger l'observation préalable de `1`, `2` ou `3`.

### TT-CMD-B05-201 — État final refus

Précondition : disposer d'une condition de refus explicitement normée pour la commande utilisée.

Action : soumettre la transaction.

Oracle :
- état final cohérent avec un refus, notamment `cmd_status = 5` lorsque la règle de commande prévoit un refus ;
- `cmd_result_code` égal au code de refus explicitement normé par la sous-famille métier.

### TT-CMD-B05-202 — État final échec

Précondition : disposer d'une condition d'échec explicitement normée ou d'une injection de banc autorisée.

Oracle : `cmd_status = 6` avec le code résultat normé correspondant lorsque cette situation est spécifiée.

Ce cas ne doit pas inventer une panne uniquement pour rendre le test exécutable.

### TT-CMD-B05-203 — Cohérence nominale des champs actifs

Précondition : aucune commande concurrente.

Action : soumettre une nouvelle transaction valide.

Oracle : `cmd_active_code` et `cmd_active_transaction_id` correspondent à la commande prise en compte / dernière prise en compte selon la définition V1.

La concurrence est exclue de ce cas.

### TT-CMD-B05-204 — Mise à jour de l'historique

Action : terminer une commande avec un résultat final déterminé.

Oracle après terminaison :
- `cmd_last_code` = code de cette commande ;
- `cmd_last_transaction_id` = transaction ID de cette commande ;
- `cmd_last_status_final` = état final observé ;
- `cmd_last_result_code` = résultat final observé.

### TT-CMD-B05-205 — Dernière commande terminée quel que soit le résultat

Action : terminer successivement deux commandes A puis B avec des issues finales éventuellement différentes.

Oracle : après terminaison de B, les champs `cmd_last_*` représentent B, y compris si B est refusée ou échouée. L'historique n'est pas défini comme « dernier succès ».

### TT-CMD-B05-206 — Timestamp de fin

Action : relever le contexte temporel, terminer une commande, lire `cmd_last_timestamp`.

Oracle : le timestamp est mis à jour comme timestamp de fin de la dernière commande terminée et utilise l'Epoch TR2.

La reconstruction MSW/LSW et l'atomicité relèvent de FT-STR.

## 3. Exigences non transformées en tests positifs

### Séquence complète d'états

`NOT_DEFINED` : aucune obligation de voir successivement 1, 2, 3 avant l'état final.

### `cmd_result_detail`

`NOT_DEFINED` pour une table générique exhaustive. Les exemples V1 ne sont pas transformés en obligations universelles.

### Tous les codes résultat sur toutes les commandes

Interdit : la table de codes est un vocabulaire, pas une matrice universelle commande × résultat.

## 4. Critère de validation de la sous-famille

FT-CMD-03 est satisfaite si les représentations finales, les champs actifs nominaux et l'historique sont conformes aux règles V1, sans imposer d'automate intermédiaire ou de sémantique de détail absente de la spécification.
