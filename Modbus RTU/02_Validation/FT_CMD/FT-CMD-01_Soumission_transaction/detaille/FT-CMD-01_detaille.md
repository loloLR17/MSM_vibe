# FT-CMD-01 — Cas de test détaillés

## TT-CMD-B05-001 — Aucune prise en compte sans front montant

**Objectif**  
Vérifier qu'une requête préparée ne déclenche aucune commande tant qu'aucun passage `submit : 0 → 1` n'est produit.

**Source normative**  
Bloc 5 §7.6.

**Préconditions**
- moteur de commande disponible ;
- `submit = 0` ;
- aucune commande active.

**Données d'entrée**
- charger un code de commande valide et non protégé, par exemple `7` ;
- charger un `transaction_id` distinct de la dernière transaction connue ;
- laisser `submit = 0`.

**Étapes**
1. lire et conserver `cmd_active_code`, `cmd_active_transaction_id`, `cmd_status`, `cmd_last_*` ;
2. écrire les champs de requête ;
3. ne pas produire de front montant ;
4. relire les observables B5.

**Résultat attendu**
- aucune nouvelle commande n'est prise en compte ;
- aucune nouvelle transaction ne doit remplacer les observables de commande active/dernière commande du seul fait de la préparation des champs.

**Critère** : PASS si aucune prise en compte n'est observée.

---

## TT-CMD-B05-002 — Prise en compte sur front montant valide

**Objectif**  
Vérifier qu'une requête admissible peut être prise en compte lors du passage de `submit` de `0` à `1`.

**Préconditions**
- moteur prêt ;
- aucune commande active ;
- choisir une commande simple et non protégée, préférentiellement code `7`.

**Étapes**
1. préparer code et transaction ID ;
2. vérifier `submit = 0` ;
3. écrire `submit = 1` sans bit réservé ;
4. observer B5 jusqu'à prise en compte/finalisation.

**Résultat attendu**
- la transaction préparée est prise en compte par le moteur B5 ;
- l'identité de la transaction observée correspond à celle soumise lorsque l'état B5 l'expose.

**Limite**  
Le test ne requiert pas l'observation de tous les états intermédiaires.

---

## TT-CMD-B05-003 — Maintien de submit à 1 sans réexécution

**Classification d'exécution : `CONDITIONAL`.**

**Objectif**  
Vérifier la règle normative selon laquelle un niveau `submit = 1` maintenu ne provoque pas de réexécution répétée.

**Condition d'exécution**  
Le moyen d'essai doit permettre d'observer ou de maintenir ce niveau logique sans créer lui-même un nouveau front `0 → 1`. Sur une interface Modbus où le firmware applique immédiatement l'auto-clear également normatif, cette condition peut ne pas être observable extérieurement.

**Méthode si la condition est satisfaite**
1. soumettre une commande simple avec transaction ID neuf ;
2. maintenir/observer `submit = 1` sans transition intermédiaire à `0` ;
3. surveiller les informations transactionnelles B5 ;
4. vérifier qu'une seule prise en compte a lieu.

**Résultat attendu**
- aucune réexécution tant qu'aucun nouveau front montant n'est intervenu.

**Interdiction**  
Ne pas simuler un « maintien » en réécrivant `submit = 1` après que le firmware l'a déjà remis à `0`, car cela créerait un nouveau front et déplacerait l'oracle vers l'idempotence FT-CMD-02.

---

## TT-CMD-B05-004 — Auto-clear de submit

**Objectif**  
Vérifier la conséquence normative finale de §7.6 : après prise en compte, le firmware remet automatiquement `submit` à `0`.

**Étapes**
1. partir de `submit = 0` ;
2. soumettre une commande admissible ;
3. confirmer sa prise en compte ;
4. relire `cmd_request_control`.

**Résultat attendu**
- bit `submit = 0` après prise en compte.

**Critère** : PASS si le bit est revenu automatiquement à `0` sans écriture de nettoyage de la centrale.

---

## TT-CMD-B05-005 — Code commande nul

**Objectif**  
Vérifier que `cmd_request_code = 0` ne produit aucune prise en compte, même lorsqu'un front montant de `submit` est généré.

**Étapes**
1. placer `cmd_request_code = 0` ;
2. charger un transaction ID distinct ;
3. générer `submit : 0 → 1` ;
4. observer les informations courantes et l'historique B5.

**Résultat attendu**
- aucune nouvelle commande n'est prise en compte à partir du code `0`.

**Limite**  
La V1 n'impose pas ici un code résultat spécifique ; le test ne doit donc pas en inventer un.

---

## TT-CMD-B05-006 — Bit réservé soumis dans cmd_request_control

**Objectif**  
Valider la distinction entre accès Modbus valide à un registre RW et valeur fonctionnellement invalide.

**Préconditions**
- code de commande valide ;
- transaction ID neuf ;
- aucune commande active.

**Étapes**
1. préparer la commande ;
2. écrire `cmd_request_control` avec `submit = 1` et au moins un bit réservé `3..15 = 1` ;
3. observer le résultat B5.

**Résultat attendu**
- l'écriture du registre RW n'est pas traitée comme un accès Modbus interdit ;
- la commande/action associée n'est pas exécutée ;
- `cmd_result_code = 2` (`paramètre invalide`) est exposé pour la soumission invalide.

**Frontière**  
La conformité de l'accès Modbus en lui-même est propriétaire FT-ACC ; le verdict FT-CMD porte sur le refus fonctionnel et l'absence d'exécution.

---

## Dette non instanciée — transaction_id invalide

Aucun test n'est généré pour `cmd_result_code = 14` tant que la V1 ne définit pas une valeur ou une règle déterministe permettant de produire un `transaction_id` invalide.
