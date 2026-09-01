# FT-CMD-01 — Exigences source normalisées

## 1. Références normatives

- `01_Specification_source/bloc5.md` V1 ;
- `02_Validation/FT_BLK/FT-BLK-06_Stabilite_delegations/` ;
- plan maître de validation Modbus TR2.

Les compléments métier explicitement informatifs ne servent pas d'oracle.

## 2. Exigences retenues

### CMD01-R01 — Déclenchement sur front montant
- Source : Bloc 5 §7.6.
- Exigence : une commande n'est évaluée que lors du passage de `submit` de `0` à `1`.
- Classification : `COVERED`.
- Tests : `TT-CMD-B05-001`, `TT-CMD-B05-002`.

### CMD01-R02 — Maintien de submit à 1
- Source : Bloc 5 §7.6.
- Exigence : le maintien de `submit = 1` ne doit pas provoquer de réexécution répétée.
- Classification : `CONDITIONAL`.
- Test : `TT-CMD-B05-003`.
- Condition : disposer d'un moyen d'essai permettant d'observer ou de maintenir le niveau logique `1` sans créer de nouveau front artificiel. L'auto-clear obligatoire peut rendre cette condition non observable depuis la seule interface Modbus.

### CMD01-R03 — Auto-clear de submit
- Source : Bloc 5 §7.6.
- Exigence : le firmware remet automatiquement `submit` à `0` après prise en compte.
- Classification : `COVERED`.
- Test : `TT-CMD-B05-004`.

### CMD01-R04 — Code commande non nul requis
- Source : Bloc 5 §7.6 et §10.1.
- Exigence : une commande n'est prise en compte que si `cmd_request_code != 0`.
- Classification : `COVERED`.
- Test : `TT-CMD-B05-005`.

### CMD01-R05 — transaction_id obligatoire
- Source : Bloc 5 §4, §7.2 et §10.1.
- Exigence documentaire : `transaction_id` est obligatoire pour distinguer les commandes et permettre leur corrélation.
- Classification : `TRACE_ONLY` dans FT-CMD-01.
- Justification : la représentation d'un champ « absent » n'existe pas distinctement dans le registre. La réutilisation et la corrélation sont testées dans FT-CMD-02.

### CMD01-R06 — transaction_id invalide
- Source : Bloc 5 §7.2, §7.6 et `cmd_result_code = 14`.
- Exigence : `transaction_id = 0` est invalide pour une soumission ; les valeurs `1..65535` sont valides. Une soumission avec ID `0` ne doit pas exécuter l'action et doit exposer `cmd_result_code = 14`.
- Classification : `COVERED`.
- Test : `TT-CMD-B05-007`.
- Frontière : l'écriture de `0` dans le registre RW reste un accès Modbus valide ; FT-CMD valide uniquement la conséquence fonctionnelle de la soumission.

### CMD01-R07 — Bits réservés dans cmd_request_control
- Source : Bloc 5 §7.6 et §10.5.
- Exigence : les bits réservés d'un registre RW doivent être écrits à `0`. Si un bit réservé vaut `1` dans une valeur soumise comme commande, l'accès Modbus reste valide mais l'action ne doit pas être exécutée et le refus fonctionnel doit être exposé avec `cmd_result_code = 2`.
- Classification : `COVERED`.
- Test : `TT-CMD-B05-006`.
- Frontière : le fait que l'accès Modbus au registre RW soit autorisé reste couvert par FT-ACC ; FT-CMD juge uniquement la conséquence fonctionnelle lors de la soumission.

## 3. Anti-duplication

FT-CMD-01 ne couvre pas :
- domaine des codes commande : FT-LIM ;
- accès aux registres et exceptions Modbus : FT-ACC ;
- idempotence d'un ID déjà traité : FT-CMD-02 ;
- politique de réutilisation d'un ID après disparition de l'historique d'idempotence : `NOT_DEFINED` en V1 ;
- concurrence : FT-CMD-04 ;
- effets métier inter-blocs : FT-INT.

## 4. Règles anti-fabrication

- ne pas inventer de valeurs invalides autres que `transaction_id = 0` ;
- ne pas imposer une séquence complète des états `cmd_status` pour prouver une prise en compte ;
- ne pas déclarer le maintien à `1` directement testable si l'auto-clear empêche son observation ;
- ne pas utiliser un effet inter-blocs comme unique oracle lorsque la propriété peut être observée dans B5 ;
- ne pas confondre bit réservé dans un registre RW avec écriture sur registre réservé.
