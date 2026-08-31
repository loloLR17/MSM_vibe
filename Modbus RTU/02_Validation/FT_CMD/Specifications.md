# FT-CMD — Spécifications de validation

## 1. Principe de propriété

FT-CMD possède l'oracle transactionnel du Bloc 5 : décision de prise en compte, acceptation/refus/échec, unicité d'exécution, état et résultat B5.

Les effets métier observables dans d'autres blocs après une commande réussie restent propriétaires FT-INT. Les comportements après reboot restent FT-PER. Structure/encodage, accès et domaines purs restent respectivement FT-STR, FT-ACC et FT-LIM.

## 2. Doctrine d'oracle

Aucune règle n'est déduite d'une intention architecturale ou d'un complément métier informatif. Lorsqu'une condition d'entrée ou un résultat exact n'est pas défini, l'exigence est classée `CONDITIONAL`, `TRACE_ONLY` ou `NOT_DEFINED`.

La présence d'un code résultat dans une table ne suffit pas, à elle seule, à inventer les conditions précises qui doivent le produire.

## 3. États intermédiaires

La V1 définit les valeurs de `cmd_status`, mais ne fixe pas une séquence temporelle exhaustive imposant l'observation de chaque état intermédiaire. Les tests FT-CMD ne doivent donc pas exiger systématiquement une traversée `reçue → acceptée → en cours → final`.

## 4. Transaction ID

`transaction_id` est obligatoire pour distinguer et corréler les commandes. La V1 ne définit cependant pas le domaine numérique exact d'un ID invalide. Aucun test ne doit donc déclarer arbitrairement `0` ou une autre valeur comme invalide.

## 5. Frontières

- FT-CMD : moteur transactionnel B5 et préconditions/refus des commandes.
- FT-INT : effets inter-blocs après succès.
- FT-BLK : invariants intra-bloc hors moteur B5.
- FT-RBT : pertes/répétitions/timings hostiles dépassant l'oracle transactionnel direct.
- FT-PER : reboot, persistance et reprise.

## 6. Critère de gel

Chaque exigence `COVERED` doit posséder au moins un test, chaque dette normative doit rester visible, et aucun oracle d'une famille déjà gelée ne doit être dupliqué.
