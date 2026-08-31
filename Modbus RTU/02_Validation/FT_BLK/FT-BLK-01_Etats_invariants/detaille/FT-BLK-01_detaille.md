# FT-BLK-01 — Cas de test détaillés

## TT-BLK-B02-001 — Cohérence état « temps invalide »

- **Objectif** : vérifier l’absence de contradiction entre `time_status` et le bit `Temps valide`.
- **Source** : Bloc 2 §6.1, §6.2, §7.
- **Préconditions** : DUT dans un état où `time_status = 1` peut être observé.
- **Entrées** : aucune écriture requise par le test lui-même.
- **Étapes** : lire de manière cohérente `time_status` et `time_flags`.
- **Résultat attendu** : si `time_status = 1`, le bit 0 `Temps valide` vaut 0.
- **Critère d’acceptation** : aucune observation `time_status = 1` avec bit 0 = 1.
- **Mode** : automatisable.
- **Criticité** : P0.

## TT-BLK-B02-002 — Cohérence état « temps synchronisé »

- **Objectif** : détecter une contradiction explicite entre l’état synchronisé et l’indication de synchronisation.
- **Source** : Bloc 2 §6.1, §6.2, §7.
- **Préconditions** : `time_status = 3` observable.
- **Étapes** : lire `time_status` et `time_flags` dans un même cycle cohérent.
- **Résultat attendu** : un état `Temps synchronisé` ne doit pas être accompagné d’une indication explicitement contraire à une synchronisation effectuée.
- **Critère d’acceptation** : bit 1 `Synchronisation effectuée` = 1 lorsque `time_status = 3`.
- **Mode** : automatisable.
- **Criticité** : P0.
- **Limite** : aucune combinaison supplémentaire de bits n’est imposée.

## TT-BLK-B02-003 — Cohérence temps préparé disponible

- **Objectif** : vérifier la redondance entre `prepared_time_status` et le bit associé.
- **Source** : Bloc 2 §6.2, §6.3.
- **Préconditions** : un temps préparé valide est présent.
- **Étapes** : lire `prepared_time_status` et `time_flags`.
- **Résultat attendu** : `prepared_time_status = 1` implique bit 3 `Temps préparé disponible` = 1.
- **Critère d’acceptation** : aucune contradiction observée.
- **Mode** : automatisable.
- **Criticité** : P0.

## TT-BLK-B03-001 — Cohérence dépassement global

- **Objectif** : vérifier la redondance fonctionnelle globale.
- **Source** : Bloc 3 §8.3 et §8.6.
- **Étapes** : lire `B3_EXCEED_GLOBAL` et `B3_ALARM_FLAGS` dans un snapshot cohérent.
- **Résultat attendu** : `B3_EXCEED_GLOBAL = 1` si et seulement si bit 4 `EXCEED_GLOBAL_ACTIVE = 1`.
- **Critère d’acceptation** : égalité booléenne des deux représentations.
- **Mode** : automatisable.
- **Criticité** : P0.

## TT-BLK-B03-002 — Cohérence dépassement X
Même doctrine que TT-BLK-B03-001 entre `B3_EXCEED_X` et bit 5 `EXCEED_X_ACTIVE`.

## TT-BLK-B03-003 — Cohérence dépassement Y
Même doctrine que TT-BLK-B03-001 entre `B3_EXCEED_Y` et bit 6 `EXCEED_Y_ACTIVE`.

## TT-BLK-B03-004 — Cohérence dépassement Z
Même doctrine que TT-BLK-B03-001 entre `B3_EXCEED_Z` et bit 7 `EXCEED_Z_ACTIVE`.

## TT-BLK-B03-005 — Cohérence alarme mémorisée

- **Objectif** : vérifier la redondance de l’état d’alarme mémorisée.
- **Source** : Bloc 3 §8.3 et §8.7.
- **Étapes** : lire `B3_ALARM_LATCHED` et `B3_ALARM_FLAGS` dans un snapshot cohérent.
- **Résultat attendu** : `B3_ALARM_LATCHED = 1` si et seulement si bit 8 `ALARM_LATCHED_PRESENT = 1`.
- **Critère d’acceptation** : égalité booléenne des deux représentations.
- **Mode** : automatisable.
- **Criticité** : P0.

## TT-BLK-B07-001 — Sentinelle « aucun défaut connu »

- **Objectif** : vérifier l’interprétation normative de la valeur sentinelle.
- **Source** : Bloc 7 §8.1.
- **Préconditions** : DUT dans un état sans défaut connu, déterminé par un moyen externe ou un scénario de référence.
- **Étapes** : lire `last_fault_code`.
- **Résultat attendu** : `last_fault_code = 0` lorsque le scénario de référence garantit l’absence de défaut connu.
- **Critère d’acceptation** : valeur 0.
- **Mode** : conditionné par la maîtrise du scénario de référence ; lecture automatisable.
- **Criticité** : P1.

## Cas non exécutables dans cette sous-famille

Les exigences `CONDITIONAL` et `NOT_DEFINED` restent dans la matrice de couverture ; elles ne doivent pas être transformées en tests PASS/FAIL sans évolution normative ou moyen d’injection adéquat.
