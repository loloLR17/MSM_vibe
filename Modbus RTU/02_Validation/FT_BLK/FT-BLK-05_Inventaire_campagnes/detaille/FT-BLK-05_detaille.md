# FT-BLK-05 — Cas de test détaillés

> Chaque cas est autonome et suit le format du plan maître. Les traces minimales comprennent index écrit, validité, métadonnées lues, inventaire de référence, trames Modbus et verdict.

## TT-BLK-B06-001 — Sélection de la première campagne
- **Objectif** : vérifier que l’index `0` expose la première campagne logique.
- **Exigences couvertes** : BLK05-B6-001, BLK05-B6-002.
- **Source normative** : Bloc 6 §5, §8.1 et §9.
- **Préconditions** : inventaire non vide et ordre de référence connu.
- **Entrées** : `selected_campaign_index = 0`.
- **Étapes** : écrire 0 ; lire `selected_campaign_valid` puis l’entrée sélectionnée.
- **Résultat attendu** : validité = 1 et métadonnées correspondant à la première campagne de référence.
- **Critère d’acceptation** : concordance de la campagne exposée avec l’index 0.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui avec inventaire déterministe.
- **Traces** : inventaire de référence, index, validité, métadonnées et trames.
- **Criticité** : P0.
- **Limites / arbitrages** : l'accès RW lui-même relève FT-ACC.

## TT-BLK-B06-002 — Sélection d’un index intermédiaire valide
- **Objectif** : vérifier la navigation vers une campagne interne à l’inventaire.
- **Exigences couvertes** : BLK05-B6-001, BLK05-B6-004.
- **Source normative** : Bloc 6 §5, §8.1 et §9.
- **Préconditions** : `total_campaign_count >= 3`, ordre et identifiants de référence connus.
- **Entrées** : index `i` tel que `0 < i < N-1`.
- **Étapes** : écrire `i` ; lire validité et métadonnées.
- **Résultat attendu** : validité = 1 et entrée correspondant à l’index `i`.
- **Critère d’acceptation** : concordance exacte avec l’inventaire de référence.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui sous précondition.
- **Traces** : index, inventaire, valeurs exposées et trames.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune relation non normée entre compteurs n’est utilisée comme oracle.

## TT-BLK-B06-003 — Sélection de la dernière campagne
- **Objectif** : vérifier la borne supérieure valide `N-1`.
- **Exigences couvertes** : BLK05-B6-001, BLK05-B6-003.
- **Source normative** : Bloc 6 §8.1.
- **Préconditions** : `N > 0`, dernière campagne de référence connue.
- **Entrées** : `selected_campaign_index = N-1`.
- **Étapes** : écrire `N-1` ; lire validité et métadonnées.
- **Résultat attendu** : validité = 1 et dernière campagne exposée.
- **Critère d’acceptation** : concordance avec la dernière campagne de référence.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui sous précondition.
- **Traces** : N, index, métadonnées, trames.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune hypothèse supplémentaire sur l'ordre interne.

## TT-BLK-B06-004 — Index hors plage
- **Objectif** : vérifier la conséquence fonctionnelle d’une sélection sémantiquement invalide.
- **Exigence couverte** : BLK05-B6-005.
- **Source normative** : Bloc 6 §8.1.
- **Préconditions** : connaître `N = total_campaign_count`.
- **Entrées** : index hors plage, par exemple `N` lorsque représentable.
- **Étapes** : écrire l’index ; lire `selected_campaign_valid`.
- **Résultat attendu** : `selected_campaign_valid = 0`.
- **Critère d’acceptation** : aucune campagne n’est déclarée valide.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : N, index, validité et trames.
- **Criticité** : P0.
- **Limites / arbitrages** : l’acceptation Modbus de l’écriture et l’absence d’exception sont couvertes hors FT-BLK.

## TT-BLK-B06-005 — Métadonnées non valides après sélection invalide
- **Objectif** : empêcher l’interprétation fonctionnelle des métadonnées lorsque la sélection n’est pas valide.
- **Exigence couverte** : BLK05-B6-006.
- **Source normative** : Bloc 6 §8.1 et §8.2.
- **Préconditions** : `selected_campaign_valid = 0` obtenu par sélection hors plage.
- **Entrées** : sélection invalide établie.
- **Étapes** : lire l’entrée sélectionnée et son indicateur de validité.
- **Résultat attendu** : validité reste 0 ; les autres champs ne sont pas interprétés comme une campagne valide.
- **Critère d’acceptation** : aucune valeur particulière n’est exigée pour les métadonnées.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : index, validité, métadonnées et trames.
- **Criticité** : P1.
- **Limites / arbitrages** : aucun zeroing implicite n’est inventé.

## TT-BLK-B06-006 — Identifiant non nul pour une campagne valide
- **Objectif** : vérifier l’invariant d’identification d’une campagne sélectionnée valide.
- **Exigence couverte** : BLK05-B6-007.
- **Source normative** : Bloc 6 §8.3.
- **Préconditions** : `selected_campaign_valid = 1`.
- **Entrées** : sélection d'une campagne valide.
- **Étapes** : lire `campaign_id`.
- **Résultat attendu** : `campaign_id != 0`.
- **Critère d’acceptation** : identifiant strictement non nul.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : validité, identifiant et trame.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune unicité inter-capteurs n’est testée ici.

## TT-BLK-B06-007 — Campagne en cours sans timestamp de fin
- **Objectif** : vérifier l’invariant temporel explicite d’une campagne en cours.
- **Exigence couverte** : BLK05-B6-008.
- **Source normative** : Bloc 6 §6.1 et §8.4.
- **Préconditions** : campagne exposée avec `campaign_state = 2`.
- **Entrées** : sélection de cette campagne.
- **Étapes** : lire `campaign_state` et `end_timestamp`.
- **Résultat attendu** : si état = 2, `end_timestamp = 0`.
- **Critère d’acceptation** : implication respectée.
- **Mode d’exécution** : conditionnel.
- **Automatisation** : oui si le banc sait maintenir une campagne en cours.
- **Traces** : index, état, timestamp et trames.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune équation exacte de `duration_s` n’est exigée pour une campagne en cours.

## TT-BLK-B06-008 — Unicité locale des campaign_id
- **Objectif** : vérifier que deux campagnes distinctes présentes dans l’inventaire du même TR2 n’exposent pas le même `campaign_id`.
- **Exigence couverte** : BLK05-B6-015.
- **Source normative** : Bloc 6 §8.3.
- **Préconditions** : inventaire contenant au moins deux campagnes distinctes et navigables.
- **Entrées** : sélectionner successivement au moins deux index valides distincts.
- **Étapes** : pour chaque campagne sélectionnée, lire `selected_campaign_valid` et `campaign_id` ; comparer les identifiants.
- **Résultat attendu** : chaque campagne distincte sélectionnée possède un `campaign_id` non nul et distinct des autres campagnes présentes dans l’inventaire examiné.
- **Critère d’acceptation** : aucune collision d’identifiant entre campagnes distinctes du même TR2.
- **Mode d’exécution** : conditionnel.
- **Automatisation** : oui si l’inventaire de référence contient au moins deux campagnes.
- **Traces** : index, identifiants, validité et trames.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune unicité entre deux TR2 différents n’est requise.

## TT-BLK-B06-009 — Durée nominale d’une campagne terminée
- **Objectif** : vérifier l’égalité normative de durée dans le seul cas où la base de temps est continue sur toute la campagne.
- **Exigence couverte** : BLK05-B6-009.
- **Source normative** : Bloc 6 §8.5.
- **Préconditions** : campagne terminée ; `end_timestamp != 0` ; aucune resynchronisation temporelle n’a affecté l’intervalle entre début et fin.
- **Entrées** : sélectionner la campagne terminée de référence.
- **Étapes** : lire `start_timestamp`, `end_timestamp` et `duration_s` dans un état cohérent.
- **Résultat attendu** : `duration_s = end_timestamp - start_timestamp`.
- **Critère d’acceptation** : égalité exacte en secondes.
- **Mode d’exécution** : conditionnel.
- **Automatisation** : oui si le banc maîtrise l’historique de synchronisation de la campagne.
- **Traces** : timestamps, durée, preuve de précondition temporelle et trames.
- **Criticité** : P1.
- **Limites / arbitrages** : aucune égalité n’est extrapolée aux campagnes en cours ni aux campagnes traversant une discontinuité de temps.

## Exigences sans test exécutable strict
Restent tracées sans faux oracle : relation total/valides, relation stockage utilisé/libre/capacité, dérivation `storage_health_status`, dérivation `data_integrity_status`. La cohérence snapshot multi-registres reste FT-STR.
