# FT-BLK-05 — Cas de test détaillés

## TT-BLK-B06-001 — Sélection de la première campagne
- **Objectif** : vérifier que l’index `0` expose la première campagne logique.
- **Source** : Bloc 6 §5, §8.1 et §9.
- **Préconditions** : inventaire non vide et ordre de référence connu.
- **Étapes** : écrire `selected_campaign_index = 0` ; lire `selected_campaign_valid` puis l’entrée sélectionnée.
- **Résultat attendu** : `selected_campaign_valid = 1` et les métadonnées correspondent à la première campagne de l’inventaire de référence.
- **Mode** : automatisable avec inventaire déterministe.
- **Criticité** : P0.

## TT-BLK-B06-002 — Sélection d’un index intermédiaire valide
- **Objectif** : vérifier la navigation vers une campagne interne à l’inventaire.
- **Préconditions** : `total_campaign_count >= 3` et identifiants de référence connus.
- **Étapes** : choisir `i` avec `0 < i < N-1` ; écrire `i` ; lire validité et métadonnées.
- **Résultat attendu** : `selected_campaign_valid = 1` et l’entrée exposée correspond à la campagne logique d’index `i`.
- **Mode** : automatisable sous précondition.
- **Criticité** : P0.

## TT-BLK-B06-003 — Sélection de la dernière campagne
- **Objectif** : vérifier la borne supérieure valide `N-1`.
- **Préconditions** : `N = total_campaign_count > 0` et dernière campagne connue.
- **Étapes** : écrire `N-1` ; lire validité et métadonnées.
- **Résultat attendu** : `selected_campaign_valid = 1` et les métadonnées correspondent à la dernière campagne.
- **Mode** : automatisable sous précondition.
- **Criticité** : P0.

## TT-BLK-B06-004 — Index hors plage
- **Objectif** : vérifier la conséquence fonctionnelle d’une sélection sémantiquement invalide.
- **Source** : Bloc 6 §8.1.
- **Préconditions** : connaître `N = total_campaign_count`.
- **Étapes** : écrire un index hors plage, par exemple `N` lorsque représentable ; lire `selected_campaign_valid`.
- **Résultat attendu** : `selected_campaign_valid = 0`.
- **Critère d’acceptation** : aucune campagne n’est déclarée valide pour cet index.
- **Mode** : automatisable.
- **Criticité** : P0.
- **Limite** : l’acceptation Modbus de l’écriture et l’absence d’exception ne sont pas retestées ici.

## TT-BLK-B06-005 — Métadonnées non valides après sélection invalide
- **Objectif** : empêcher l’interprétation fonctionnelle des métadonnées lorsque la sélection n’est pas valide.
- **Source** : Bloc 6 §8.1 et §8.2.
- **Préconditions** : `selected_campaign_valid = 0` obtenu par une sélection hors plage.
- **Étapes** : lire l’entrée sélectionnée et son indicateur de validité.
- **Résultat attendu** : le bloc continue d’indiquer `selected_campaign_valid = 0`; aucune règle de test ne considère les autres champs comme représentant une campagne valide.
- **Critère d’acceptation** : aucune valeur particulière n’est exigée pour `campaign_id`, timestamps, labels ou tailles.
- **Mode** : automatisable.
- **Criticité** : P1.

## TT-BLK-B06-006 — Identifiant non nul pour une campagne valide
- **Objectif** : vérifier l’invariant d’identification d’une campagne sélectionnée valide.
- **Source** : Bloc 6 §8.3.
- **Préconditions** : `selected_campaign_valid = 1`.
- **Étapes** : lire `campaign_id`.
- **Résultat attendu** : `campaign_id != 0`.
- **Mode** : automatisable.
- **Criticité** : P0.

## TT-BLK-B06-007 — Campagne en cours sans timestamp de fin
- **Objectif** : vérifier l’invariant temporel explicite d’une campagne en cours.
- **Source** : Bloc 6 §6.1 et §8.4.
- **Préconditions** : disposer d’une campagne exposée avec `campaign_state = 2`.
- **Étapes** : sélectionner cette campagne ; lire `campaign_state` et `end_timestamp`.
- **Résultat attendu** : si `campaign_state = 2`, alors `end_timestamp = 0`.
- **Mode** : conditionnel ; automatisable si le banc sait créer ou maintenir une campagne en cours.
- **Criticité** : P0.

## Exigences volontairement sans test exécutable strict

Les relations suivantes restent documentées mais sans faux oracle :
- égalité exacte de `duration_s` avec les timestamps ;
- relation numérique entre compteurs total et valides ;
- relation stockage utilisé/libre/capacité ;
- dérivation de `storage_health_status` ;
- dérivation de `data_integrity_status`.

La cohérence snapshot multi-registres de l’entrée sélectionnée est déléguée à FT-STR.
