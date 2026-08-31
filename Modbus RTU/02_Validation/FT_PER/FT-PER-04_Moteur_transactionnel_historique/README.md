# FT-PER-04 — Moteur transactionnel et historique après reboot

## 1. Objet

FT-PER-04 traite exclusivement le comportement du moteur transactionnel du Bloc 5 lorsqu'une frontière réelle de redémarrage est franchie.

La V1 définit précisément le fonctionnement nominal du moteur B5 : transaction_id, idempotence, état courant et historique minimal. Elle ne définit toutefois pas leur politique de persistance après reboot.

## 2. Frontière avec FT-CMD

FT-CMD reste propriétaire de :

- la soumission et le front montant de `submit` ;
- la corrélation par `transaction_id` ;
- l'idempotence en fonctionnement nominal ;
- `cmd_active_*`, `cmd_status`, `cmd_result_*` ;
- `cmd_last_*` et `cmd_last_timestamp` après terminaison normale.

FT-PER-04 ne réouvre aucune de ces règles hors reboot.

## 3. Conclusion normative V1

Aucune règle V1 explicite n'impose, après reboot :

- la conservation ou l'effacement de la mémoire des `transaction_id` déjà traités ;
- la conservation de `cmd_last_*` ;
- la conservation de `cmd_active_*` ;
- la valeur initiale de `cmd_status` ou `cmd_result_*` ;
- l'état des champs de requête ;
- le comportement d'une commande interrompue par watchdog, brown-out, reset externe ou coupure d'alimentation ;
- la possibilité ou l'interdiction de rejouer après reboot un `transaction_id` traité avant reboot.

Ces propriétés sont donc `NOT_DEFINED` en V1.

## 4. RESET SOFTWARE

Le RESET SOFTWARE accepté exige qu'aucune opération critique inachevée ne soit présente selon le Bloc 5. Cette précondition n'établit cependant aucune politique de persistance du moteur transactionnel après le redémarrage.

La définition exhaustive des opérations critiques et le résultat de refus correspondant restent des dettes FT-CMD/V1.

## 5. Test de caractérisation

- `TT-PER-B05-001` — observation du moteur B5 avant/après RESET SOFTWARE (`TRACE_ONLY`).

Le test peut relever les champs avant et après reboot, mais aucune différence ou conservation ne constitue un PASS/FAIL de persistance tant que V1 ne définit pas l'oracle.

## 6. Doctrine anti-fabrication

Il est interdit de déduire :

- qu'un historique est non volatil parce qu'il est nommé « historique » ;
- que l'idempotence doit survivre au reboot parce qu'elle protège contre les répétitions Modbus ;
- que tous les champs B5 doivent revenir à zéro au boot ;
- qu'une commande interrompue doit être marquée FAILED ;
- qu'une commande interrompue doit être rejouée automatiquement ;
- qu'un transaction_id pré-reboot doit être accepté ou refusé après reboot.

## 7. Statut

Sous-famille reconstruite pour revue. Aucun gel ni merge sans validation explicite.
