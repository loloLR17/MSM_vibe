# FT-CMD-04 — Validation détaillée

## TT-CMD-B05-300 — Refus d'une nouvelle commande pendant une commande en cours

**Objectif** : vérifier l'oracle de concurrence explicitement normé.

**Préconditions** :
- disposer d'une commande A dont l'état `en cours` peut être observé suffisamment longtemps ;
- A possède un `transaction_id` frais ;
- préparer une commande B avec un autre `transaction_id` valide.

**Procédure** :
1. soumettre A ;
2. confirmer que le moteur indique qu'une commande est en cours ;
3. pendant cet état, soumettre B par un nouveau front montant de `submit` ;
4. observer le refus fonctionnel associé à la nouvelle demande.

**Oracle** :
- B n'est pas exécutée comme seconde commande concurrente ;
- le refus est exposé avec `cmd_result_code = 13` (`commande déjà en cours`).

**Restriction d'oracle** : ne pas imposer quelle transaction doit apparaître dans `cmd_active_transaction_id` au moment du refus : ce point n'est pas défini par la V1.

---

## TT-CMD-B05-301 — Une seule commande active

**Objectif** : vérifier qu'aucune seconde exécution active n'est créée par la soumission concurrente.

**Préconditions** : identiques à TT-CMD-B05-300.

**Procédure** :
1. lancer A et établir qu'elle est en cours ;
2. soumettre B pendant A ;
3. utiliser les observations disponibles du moteur et, si nécessaire, une instrumentation permettant de distinguer l'exécution effective de B.

**Oracle** :
- le moteur ne traite pas simultanément A et B comme deux commandes actives ;
- B suit le refus de concurrence normé.

**Note** : si le DUT ne fournit aucun moyen externe de prouver l'absence d'exécution de B au-delà du code 13, la preuve d'unicité interne est limitée à l'interface normative.

---

## TT-CMD-B05-302 — Demande d'annulation sur commande signalée non annulable

**Classification** : `CONDITIONAL`.

**Condition d'exécution** : le banc doit pouvoir placer le DUT dans un état où une commande courante est active et `cmd_engine_flags.bit9 = 0`, sans inventer quelle commande doit satisfaire ce cas.

**Procédure** :
1. établir la commande courante ;
2. vérifier `cmd_engine_flags.bit9 = 0` ;
3. demander `cancel_request` ;
4. observer la réponse du moteur.

**Oracle disponible** :
- le vocabulaire normatif prévoit `cmd_result_code = 15` pour `commande non annulable`.

**Limite** : si aucune commande concrète n'est normativement définie comme non annulable et si le comportement du DUT ne permet pas de construire ce cas sans hypothèse, le test reste non exécutable et doit être rapporté comme tel.

---

## Cas non instanciés

### Succès d'annulation

Pas de test V1 : liste des commandes annulables, état final et résultat de succès non définis.

### `clear_request_fields`

Pas de test de contenu V1 : le bit existe, mais la portée et le timing exacts du nettoyage ne sont pas définis.

### Corrélation de B refusée pendant A

Pas d'oracle sur `cmd_active_transaction_id` lors du refus concurrent. Le code 13 est vérifiable ; la représentation transactionnelle exacte reste une dette normative.
