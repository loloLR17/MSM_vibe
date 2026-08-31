# FT-RBT-02 — Source normative consolidée

## 1. Références normatives

Sources principales :
- `Modbus RTU/01_Specification_source/bloc5.md` V1 ;
- `Modbus RTU/02_Validation/FT_CMD/FT-CMD-02_Idempotence_correlation/` gelée ;
- plan maître de validation Modbus TR2.

Les compléments métier informatifs ne servent pas d'oracle.

## 2. Exigences retenues

### RBT02-R01 — Perte de réponse suivie d'une retransmission du même transaction_id

**Classification : `CONDITIONAL`.**

La V1 indique que le Bloc 5 est conçu pour être robuste vis-à-vis des répétitions Modbus et des pertes de réponse. Elle impose par ailleurs qu'une commande avec un `transaction_id` déjà traité ne soit jamais exécutée une seconde fois et que le résultat précédent soit réutilisé.

FT-RBT-02 compose ces règles dans le scénario suivant :
1. une transaction `T` est soumise et effectivement traitée côté capteur ;
2. la première réponse n'est pas remise à la centrale ;
3. la centrale retransmet immédiatement la même transaction `T`, avec le même contenu ;
4. le résultat précédemment produit doit pouvoir être récupéré comme résultat de `T`.

Condition d'exécution : le moyen d'essai doit permettre d'injecter ou de simuler de façon contrôlée la perte de la première réponse sans empêcher le traitement initial côté capteur.

Test : `TT-RBT-B05-001`.

### RBT02-R02 — Absence de double exécution sous perte et retransmission

**Classification : `CONDITIONAL`.**

L'absence de seconde exécution est une propriété normative de FT-CMD-02. FT-RBT-02 la vérifie sous perturbation de réponse perdue.

Condition supplémentaire : disposer d'un observable discriminant permettant de distinguer une seconde exécution réelle d'un simple rejeu de résultat.

Test : `TT-RBT-B05-002`.

### RBT02-R03 — Corrélation du résultat récupéré

**Classification : `DELEGATED`.**

La corrélation stricte via `cmd_active_transaction_id` appartient à FT-CMD-02. FT-RBT-02 l'utilise comme jalon pour reconnaître le résultat récupéré après retransmission, sans définir de règle supplémentaire.

### RBT02-R04 — Politique de retry

**Classification : `NOT_DEFINED`.**

La V1 ne définit :
- ni nombre maximal ou minimal de retransmissions ;
- ni délai avant retransmission ;
- ni backoff ;
- ni politique d'abandon.

Le test FT-RBT-02 utilise donc une seule retransmission immédiate comme construction minimale d'essai, sans transformer ce choix en exigence de protocole.

### RBT02-R05 — Fenêtre temporelle d'idempotence

**Classification : `DELEGATED`.**

La profondeur et la durée de mémorisation des `transaction_id` sont déjà `NOT_DEFINED` dans FT-CMD-02. FT-RBT-02 n'invente aucune fenêtre. La retransmission est effectuée immédiatement après la perte de réponse pour rester dans le cas le plus directement supporté par l'oracle V1.

### RBT02-R06 — Même transaction_id avec payload différent après perte

**Classification : `DELEGATED`.**

Ce cas est déjà `NOT_DEFINED` dans FT-CMD-02 et n'est pas rouvert ici. FT-RBT-02 impose, pour ses cas actifs, une retransmission strictement identique au niveau du code et des paramètres de requête.

## 3. Frontières de propriété

- FT-CMD-02 : idempotence, réutilisation du résultat, corrélation et limites de mémoire ;
- FT-RBT-02 : injection de perte de réponse et composition de l'oracle sous perturbation ;
- FT-INT : éventuels effets métier inter-blocs utilisés uniquement comme observables discriminants ;
- FT-PER : tout scénario impliquant reboot/reset/coupure.

## 4. Règles anti-fabrication

- ne pas imposer un timeout de centrale ;
- ne pas imposer un nombre de retries ;
- ne pas imposer un délai de retry ;
- ne pas tester une retransmission tardive comme exigence de rétention ;
- ne pas conclure à l'absence de double exécution sans observable discriminant ;
- ne pas utiliser un payload différent sous le même `transaction_id` comme variante de ce test ;
- ne pas introduire de reboot pour 'récupérer' la communication.
