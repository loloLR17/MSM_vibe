# FT-OBS-05 — Campagnes, commandes et corrélation opératoire

## 1. Objet

FT-OBS-05 vérifie que la centrale dispose des informations normatives nécessaires pour **corréler une commande avec son suivi observable** et pour **identifier sans ambiguïté l'entrée de campagne qu'elle consulte**.

Cette sous-famille ne réaudite pas le moteur de commandes : FT-CMD V1 est gelée et reste propriétaire de la soumission, de l'idempotence, de la concurrence, des transitions, des résultats et de l'historique B5.

## 2. Principe

Deux mécanismes V1 sont directement exploitables côté centrale :

- B5 associe un `transaction_id` à la commande demandée, à la commande active/dernière prise en compte et à la dernière commande terminée ;
- B6 associe un index de sélection à un indicateur explicite `selected_campaign_valid` et à un `campaign_id` unique/non nul pour une campagne valide.

FT-OBS-05 valide la **discriminabilité externe** de ces mécanismes, pas leur mécanique interne.

## 3. Règles anti-confusion

- ne jamais corréler une commande par son seul `cmd_code` si un `transaction_id` est disponible ;
- ne jamais déduire la validité d'une campagne de ses métadonnées lorsque `selected_campaign_valid = 0` ;
- ne jamais confondre `selected_campaign_index` et `campaign_id` ;
- ne jamais supposer que l'historique B5 contient plus que la dernière commande terminée ;
- ne jamais inventer de relation B5↔B6 qui n'est pas explicitement spécifiée.

## 4. Tests actifs

- `TT-OBS-B05-001` — corrélation opératoire par `transaction_id` ;
- `TT-OBS-B05-002` — distinction commande courante / dernière commande terminée ;
- `TT-OBS-B06-001` — sélection et identité de campagne sans heuristique.

## 5. Frontières

- comportement transactionnel B5 : `DELEGATED → FT-CMD` ;
- scénarios métier complets commande→effet : `DELEGATED → FT-SEQ` ;
- cohérences strictes B5↔autres blocs : `DELEGATED → FT-INT` ;
- domaines et types : `DELEGATED → FT-LIM / FT-STR` ;
- persistance historique/corrélation après reboot : `DELEGATED → FT-PER`.

## 6. Statut

Sous-famille reconstruite pour revue. Aucun gel ni merge sans validation explicite.