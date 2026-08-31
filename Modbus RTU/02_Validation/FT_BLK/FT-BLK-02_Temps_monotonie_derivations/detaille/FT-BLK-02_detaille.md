# FT-BLK-02 — Cas de test détaillés

## TT-BLK-B01-001 — Monotonie de l'uptime B1
- **Objectif** : vérifier que `uptime_s` ne diminue pas en fonctionnement normal.
- **Source** : Bloc 1 §7.
- **Préconditions** : aucun reset pendant le test.
- **Étapes** : effectuer plusieurs lectures cohérentes espacées de `uptime_s`.
- **Résultat attendu** : pour toute paire successive, `uptime[n+1] >= uptime[n]`.
- **Critère d'acceptation** : aucune diminution observée.
- **Mode** : automatisable.
- **Criticité** : P1.

## TT-BLK-B02-101 — Monotonie de current_time
- **Objectif** : vérifier la monotonie de l'horloge hors resynchronisation.
- **Source** : Bloc 2 §7.
- **Préconditions** : aucune synchronisation effective pendant le test.
- **Étapes** : lire plusieurs fois `current_time` sur une période suffisante.
- **Résultat attendu** : `current_time[n+1] >= current_time[n]`.
- **Critère d'acceptation** : aucun recul hors événement de resynchronisation identifié.
- **Mode** : automatisable.
- **Criticité** : P0.

## TT-BLK-B02-102 — prepared_time sans effet immédiat
- **Objectif** : vérifier le découplage préparation/application.
- **Source** : Bloc 2 §3 et §7.
- **Préconditions** : aucune commande B5 de synchronisation exécutée pendant le test.
- **Étapes** : lire `current_time`, écrire une valeur préparée volontairement distincte dans `prepared_time`, relire `current_time`.
- **Résultat attendu** : l'horloge continue son évolution normale et n'est pas remplacée immédiatement par `prepared_time`.
- **Critère d'acceptation** : absence de saut vers la valeur préparée imputable à l'écriture seule.
- **Mode** : automatisable.
- **Criticité** : P0.
- **Limite** : le test ne couvre pas l'application via B5.

## TT-BLK-B02-103 — Stabilité de last_sync_time hors synchronisation
- **Objectif** : vérifier que `last_sync_time` ne change pas sans synchronisation effective.
- **Source** : Bloc 2 §7.
- **Préconditions** : aucune synchronisation effective pendant la fenêtre d'observation.
- **Étapes** : lire `last_sync_time`, effectuer des lectures normales et éventuellement préparer un temps sans l'appliquer, relire `last_sync_time`.
- **Résultat attendu** : `last_sync_time` reste inchangé.
- **Critère d'acceptation** : aucune modification hors synchronisation effective.
- **Mode** : automatisable.
- **Criticité** : P0.
- **Limite** : la mise à jour après commande B5 réussie est déléguée à FT-CMD / FT-INT.

## TT-BLK-B02-104 — Cohérence de time_since_sync
- **Objectif** : vérifier la dérivation temporelle interne.
- **Source** : Bloc 2 §7.
- **Préconditions** : temps exploitable et `last_sync_time` significatif ; aucune resynchronisation pendant le test.
- **Étapes** : lire `current_time`, `last_sync_time` et `time_since_sync` dans une séquence de lecture rapprochée ; relever la durée totale de la séquence.
- **Résultat attendu** : `time_since_sync` est compatible avec `current_time - last_sync_time` compte tenu du temps écoulé entre les lectures.
- **Critère d'acceptation** : l'écart observé reste dans une tolérance instrumentale dérivée de la durée réelle de la séquence de lecture ; aucune constante protocolaire non spécifiée n'est imposée.
- **Mode** : automatisable.
- **Criticité** : P0.

## TT-BLK-B03-101 — Monotonie de B3_CALC_SEQUENCE
- **Objectif** : vérifier que le compteur de calcul ne régresse pas.
- **Source** : Bloc 3 mapping `B3_CALC_SEQUENCE`.
- **Préconditions** : aucune réinitialisation du système pendant le test.
- **Étapes** : effectuer plusieurs lectures de `B3_CALC_SEQUENCE` pendant la supervision.
- **Résultat attendu** : `sequence[n+1] >= sequence[n]`.
- **Critère d'acceptation** : aucune diminution observée.
- **Mode** : automatisable.
- **Criticité** : P1.
- **Limite** : aucune cadence minimale d'incrément n'est imposée par la V1.

## TT-BLK-B07-001 — Monotonie de l'uptime B7
- **Objectif** : vérifier l'invariant `uptime_s` du diagnostic.
- **Source** : Bloc 7 §8.3.
- **Préconditions** : aucun reset pendant le test.
- **Étapes** : lire plusieurs fois `uptime_s`.
- **Résultat attendu** : `uptime[n+1] >= uptime[n]`.
- **Critère d'acceptation** : aucune diminution observée.
- **Mode** : automatisable.
- **Criticité** : P1.
- **Limite** : le comportement après reset relève de FT-PER ; l'égalité avec l'uptime B1 relève de FT-INT.
