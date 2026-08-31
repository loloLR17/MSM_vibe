# FT-BLK-02 — Cas de test détaillés

> Chaque cas est autonome et suit le format du plan maître. Les traces minimales à conserver sont les requêtes/réponses Modbus utiles, valeurs lues/écrites, horodatage du banc et verdict.

## TT-BLK-B01-001 — Monotonie de l'uptime B1
- **Objectif** : vérifier que `uptime_s` ne diminue pas en fonctionnement normal.
- **Exigence couverte** : BLK02-B01-R01.
- **Source normative** : Bloc 1 §7.
- **Préconditions** : aucun reset pendant le test.
- **Entrées** : aucune.
- **Étapes** : effectuer plusieurs lectures cohérentes espacées de `uptime_s`.
- **Résultat attendu** : `uptime[n+1] >= uptime[n]`.
- **Critère d’acceptation** : aucune diminution observée.
- **Mode d’exécution** : fonctionnel temporel intra-bloc.
- **Automatisation** : oui.
- **Traces** : série horodatée des valeurs et trames brutes.
- **Criticité** : P1.
- **Limites / arbitrages** : aucune cadence exacte d’incrément n’est imposée.

## TT-BLK-B02-101 — Monotonie de current_time
- **Objectif** : vérifier la monotonie de l'horloge hors resynchronisation.
- **Exigence couverte** : BLK02-B02-R01.
- **Source normative** : Bloc 2 §7.
- **Préconditions** : aucune synchronisation effective pendant le test.
- **Entrées** : aucune.
- **Étapes** : lire plusieurs fois `current_time` sur une période suffisante.
- **Résultat attendu** : `current_time[n+1] >= current_time[n]`.
- **Critère d’acceptation** : aucun recul hors événement de resynchronisation identifié.
- **Mode d’exécution** : fonctionnel temporel intra-bloc.
- **Automatisation** : oui.
- **Traces** : série horodatée et événements de synchronisation éventuels.
- **Criticité** : P0.
- **Limites / arbitrages** : une resynchronisation effective exclut la fenêtre concernée.

## TT-BLK-B02-102 — prepared_time sans effet immédiat
- **Objectif** : vérifier le découplage préparation/application.
- **Exigence couverte** : BLK02-B02-R02.
- **Source normative** : Bloc 2 §3 et §7.
- **Préconditions** : aucune commande B5 de synchronisation exécutée pendant le test.
- **Entrées** : valeur `prepared_time` volontairement distincte.
- **Étapes** : lire `current_time`, écrire `prepared_time`, relire `current_time`.
- **Résultat attendu** : l’horloge continue son évolution normale sans saut vers la valeur préparée imputable à l’écriture seule.
- **Critère d’acceptation** : absence d’application immédiate.
- **Mode d’exécution** : fonctionnel intra-bloc avec écriture RW valide.
- **Automatisation** : oui.
- **Traces** : valeurs avant/après, écriture et réponses Modbus.
- **Criticité** : P0.
- **Limites / arbitrages** : l’application via B5 est déléguée FT-CMD/FT-INT.

## TT-BLK-B02-103 — Stabilité de last_sync_time hors synchronisation
- **Objectif** : vérifier que `last_sync_time` ne change pas sans synchronisation effective.
- **Exigence couverte** : BLK02-B02-R03A.
- **Source normative** : Bloc 2 §7.
- **Préconditions** : aucune synchronisation effective pendant la fenêtre.
- **Entrées** : éventuellement préparation d’un temps sans application.
- **Étapes** : lire `last_sync_time`, effectuer les opérations normales autorisées, relire `last_sync_time`.
- **Résultat attendu** : valeur inchangée.
- **Critère d’acceptation** : aucune modification hors synchronisation effective.
- **Mode d’exécution** : fonctionnel temporel intra-bloc.
- **Automatisation** : oui.
- **Traces** : lectures avant/après et journal des événements de banc.
- **Criticité** : P0.
- **Limites / arbitrages** : mise à jour après synchronisation réussie déléguée FT-CMD/FT-INT.

## TT-BLK-B02-104 — Cohérence de time_since_sync
- **Objectif** : vérifier la dérivation temporelle interne.
- **Exigence couverte** : BLK02-B02-R04.
- **Source normative** : Bloc 2 §7.
- **Préconditions** : temps exploitable, `last_sync_time` significatif, aucune resynchronisation pendant le test.
- **Entrées** : aucune.
- **Étapes** : lire `current_time`, `last_sync_time`, `time_since_sync_s` en séquence rapprochée et relever la durée réelle de lecture.
- **Résultat attendu** : `time_since_sync_s` compatible avec `current_time - last_sync_time` compte tenu du délai réel de lecture.
- **Critère d’acceptation** : écart compatible avec la durée instrumentale mesurée ; aucune tolérance protocolaire arbitraire.
- **Mode d’exécution** : fonctionnel temporel intra-bloc.
- **Automatisation** : oui.
- **Traces** : valeurs, timestamps du banc et durée de la séquence.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune constante de tolérance non spécifiée n’est introduite.

## TT-BLK-B03-101 — Monotonie de B3_CALC_SEQUENCE
- **Objectif** : vérifier que le compteur de calcul ne régresse pas.
- **Exigence couverte** : BLK02-B03-R01.
- **Source normative** : Bloc 3 mapping `B3_CALC_SEQUENCE`.
- **Préconditions** : aucun reset pendant le test.
- **Entrées** : aucune.
- **Étapes** : effectuer plusieurs lectures pendant la supervision.
- **Résultat attendu** : `sequence[n+1] >= sequence[n]`.
- **Critère d’acceptation** : aucune diminution observée.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : série horodatée des valeurs.
- **Criticité** : P1.
- **Limites / arbitrages** : aucune cadence minimale d’incrément n’est imposée.

## TT-BLK-B07-101 — Monotonie de l'uptime B7
- **Objectif** : vérifier l'invariant `uptime_s` du diagnostic.
- **Exigence couverte** : BLK02-B07-R01.
- **Source normative** : Bloc 7 §8.3.
- **Préconditions** : aucun reset pendant le test.
- **Entrées** : aucune.
- **Étapes** : lire plusieurs fois `uptime_s`.
- **Résultat attendu** : `uptime[n+1] >= uptime[n]`.
- **Critère d’acceptation** : aucune diminution observée.
- **Mode d’exécution** : fonctionnel temporel intra-bloc.
- **Automatisation** : oui.
- **Traces** : série horodatée des valeurs et trames brutes.
- **Criticité** : P1.
- **Limites / arbitrages** : comportement après reset → FT-PER ; égalité avec uptime B1 → FT-INT.
