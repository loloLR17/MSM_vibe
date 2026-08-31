# FT-BLK-01 — Cas de test détaillés

> Chaque cas est autonome et suit le format du plan maître. Les traces minimales à conserver sont les requêtes/réponses Modbus utiles, valeurs lues/écrites, horodatage du banc et verdict.

## TT-BLK-B02-001 — Cohérence état « temps invalide »
- **Objectif** : vérifier l’absence de contradiction entre `time_status` et le bit `Temps valide`.
- **Exigence couverte** : BLK01-B2-001.
- **Source normative** : Bloc 2 §6.1, §6.2, §7.
- **Préconditions** : état `time_status = 1` observable.
- **Entrées** : aucune.
- **Étapes** : lire de manière cohérente `time_status` et `time_flags`.
- **Résultat attendu** : si `time_status = 1`, le bit 0 vaut 0.
- **Critère d’acceptation** : aucune observation `time_status = 1` avec bit 0 = 1.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : valeurs `time_status`, `time_flags`, horodatage et réponse brute.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune combinaison supplémentaire de bits n’est imposée.

## TT-BLK-B02-002 — Cohérence état « temps synchronisé »
- **Objectif** : vérifier la cohérence entre état synchronisé et indication de synchronisation effectuée.
- **Exigence couverte** : BLK01-B2-002.
- **Source normative** : Bloc 2 §6.1, §6.2, §7.
- **Préconditions** : `time_status = 3` observable.
- **Entrées** : aucune.
- **Étapes** : lire `time_status` et `time_flags` dans un même cycle cohérent.
- **Résultat attendu** : bit 1 = 1 lorsque `time_status = 3`.
- **Critère d’acceptation** : aucune contradiction observée.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : valeurs et trame de lecture.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune table de vérité exhaustive supplémentaire n’est imposée.

## TT-BLK-B02-003 — Cohérence temps préparé disponible
- **Objectif** : vérifier la redondance entre `prepared_time_status` et le bit associé.
- **Exigence couverte** : BLK01-B2-003.
- **Source normative** : Bloc 2 §6.2, §6.3.
- **Préconditions** : temps préparé valide présent.
- **Entrées** : état préparé établi par un moyen conforme.
- **Étapes** : lire `prepared_time_status` et `time_flags`.
- **Résultat attendu** : `prepared_time_status = 1` implique bit 3 = 1.
- **Critère d’acceptation** : aucune contradiction observée.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : valeurs et réponse brute.
- **Criticité** : P0.
- **Limites / arbitrages** : la création du temps préparé n’est pas validée ici.

## TT-BLK-B03-001 — Cohérence dépassement global
- **Objectif** : vérifier la redondance fonctionnelle globale.
- **Exigence couverte** : BLK01-B3-001.
- **Source normative** : Bloc 3 §8.3 et §8.6.
- **Préconditions** : Bloc 3 lisible dans un état cohérent.
- **Entrées** : aucune.
- **Étapes** : lire `B3_EXCEED_GLOBAL` et `B3_ALARM_FLAGS` dans un snapshot cohérent.
- **Résultat attendu** : `B3_EXCEED_GLOBAL = 1` si et seulement si bit 4 = 1.
- **Critère d’acceptation** : égalité booléenne des deux représentations.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : valeurs, bit extrait, trame brute.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune règle de seuil B4 n’est testée ici.

## TT-BLK-B03-002 — Cohérence dépassement X
- **Objectif** : vérifier la redondance pour l’axe X.
- **Exigence couverte** : BLK01-B3-002.
- **Source normative** : Bloc 3 §8.3 et §8.6.
- **Préconditions** : Bloc 3 lisible dans un état cohérent.
- **Entrées** : aucune.
- **Étapes** : lire `B3_EXCEED_X` et `B3_ALARM_FLAGS` dans un snapshot cohérent.
- **Résultat attendu** : `B3_EXCEED_X = 1` si et seulement si bit 5 = 1.
- **Critère d’acceptation** : égalité booléenne.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : valeurs, bit extrait, trame brute.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune règle de seuil n’est dérivée.

## TT-BLK-B03-003 — Cohérence dépassement Y
- **Objectif** : vérifier la redondance pour l’axe Y.
- **Exigence couverte** : BLK01-B3-003.
- **Source normative** : Bloc 3 §8.3 et §8.6.
- **Préconditions** : Bloc 3 lisible dans un état cohérent.
- **Entrées** : aucune.
- **Étapes** : lire `B3_EXCEED_Y` et `B3_ALARM_FLAGS` dans un snapshot cohérent.
- **Résultat attendu** : `B3_EXCEED_Y = 1` si et seulement si bit 6 = 1.
- **Critère d’acceptation** : égalité booléenne.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : valeurs, bit extrait, trame brute.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune règle de seuil n’est dérivée.

## TT-BLK-B03-004 — Cohérence dépassement Z
- **Objectif** : vérifier la redondance pour l’axe Z.
- **Exigence couverte** : BLK01-B3-004.
- **Source normative** : Bloc 3 §8.3 et §8.6.
- **Préconditions** : Bloc 3 lisible dans un état cohérent.
- **Entrées** : aucune.
- **Étapes** : lire `B3_EXCEED_Z` et `B3_ALARM_FLAGS` dans un snapshot cohérent.
- **Résultat attendu** : `B3_EXCEED_Z = 1` si et seulement si bit 7 = 1.
- **Critère d’acceptation** : égalité booléenne.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : valeurs, bit extrait, trame brute.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune règle de seuil n’est dérivée.

## TT-BLK-B03-005 — Cohérence alarme mémorisée
- **Objectif** : vérifier la redondance de l’état d’alarme mémorisée.
- **Exigence couverte** : BLK01-B3-005.
- **Source normative** : Bloc 3 §8.3 et §8.7.
- **Préconditions** : Bloc 3 lisible dans un état cohérent.
- **Entrées** : aucune.
- **Étapes** : lire `B3_ALARM_LATCHED` et `B3_ALARM_FLAGS` dans un snapshot cohérent.
- **Résultat attendu** : `B3_ALARM_LATCHED = 1` si et seulement si bit 8 = 1.
- **Critère d’acceptation** : égalité booléenne.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : valeurs, bit extrait, trame brute.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune règle d’acquittement n’est testée ici.

## TT-BLK-B07-001 — Sentinelle « aucun défaut connu »
- **Objectif** : vérifier l’interprétation normative de `last_fault_code = 0`.
- **Exigence couverte** : BLK01-B7-001.
- **Source normative** : Bloc 7 §8.1.
- **Préconditions** : scénario de référence garantissant l’absence de défaut connu.
- **Entrées** : scénario de référence externe au champ lui-même.
- **Étapes** : lire `last_fault_code`.
- **Résultat attendu** : valeur 0.
- **Critère d’acceptation** : `last_fault_code = 0`.
- **Mode d’exécution** : conditionnel au scénario de référence.
- **Automatisation** : lecture automatisable.
- **Traces** : preuve du scénario, valeur lue et trame brute.
- **Criticité** : P1.
- **Limites / arbitrages** : ne dérive pas la relation inverse ni une priorité entre défauts.

## Cas non exécutables dans cette sous-famille
Les exigences `CONDITIONAL` et `NOT_DEFINED` restent dans la matrice ; elles ne deviennent pas des PASS/FAIL sans oracle V1 ou moyen d’injection adéquat.
