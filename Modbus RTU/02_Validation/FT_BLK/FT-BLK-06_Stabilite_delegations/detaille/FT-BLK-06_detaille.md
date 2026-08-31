# FT-BLK-06 — Cas de test détaillés

## TT-BLK-B00-001 — Stabilité des informations d'identification

- **Objectif** : vérifier que les informations B0 restent statiques pendant le fonctionnement normal.
- **Source normative** : Bloc 0 §7.
- **Préconditions** : DUT en fonctionnement normal ; absence de reboot, reprogrammation firmware, remplacement matériel ou opération de fabrication/maintenance modifiant légitimement l'identité.
- **Entrées** : aucune.
- **Étapes** :
  1. lire l'ensemble des champs non réservés du Bloc 0 et enregistrer l'image de référence ;
  2. laisser fonctionner le DUT pendant une durée représentative ;
  3. provoquer si disponible plusieurs activités normales du capteur sans reboot ;
  4. relire le Bloc 0 ;
  5. comparer champ par champ à l'image initiale.
- **Résultat attendu** : aucun champ d'identification ne change pendant le fonctionnement normal.
- **Critère d'acceptation** : égalité des valeurs fonctionnelles B0 entre les lectures de référence et finales.
- **Mode** : automatisable.
- **Criticité** : P1.
- **Limite** : ce test ne démontre pas la persistance après reboot.

## TT-BLK-B00-002 — Indépendance vis-à-vis des états dynamiques

- **Objectif** : vérifier qu'un changement d'état dynamique du capteur n'altère pas les informations d'identification B0.
- **Source normative** : Bloc 0 §7.
- **Préconditions** : possibilité de faire varier au moins un état dynamique normal du capteur sans reboot ni opération modifiant légitimement son identité.
- **Entrées** : scénario dynamique maîtrisé, par exemple changement d'état d'acquisition ou de supervision lorsque le banc le permet.
- **Étapes** :
  1. lire B0 dans l'état dynamique initial ;
  2. faire évoluer le DUT vers un autre état dynamique normal ;
  3. relire B0 ;
  4. comparer les champs fonctionnels.
- **Résultat attendu** : les valeurs B0 restent inchangées malgré l'évolution de l'état dynamique.
- **Critère d'acceptation** : aucune dépendance observable entre l'état dynamique et les champs B0.
- **Mode** : automatisable selon banc.
- **Criticité** : P1.
- **Limite** : les moyens utilisés pour provoquer les transitions peuvent appartenir à FT-CMD/FT-SEQ ; le présent test n'en valide pas la sémantique.

## TT-BLK-B00-003 — Unicité de device_id sur plusieurs équipements

- **Objectif** : vérifier l'exigence d'unicité de `device_id` sur un échantillon de plusieurs équipements distincts.
- **Source normative** : Bloc 0 §7.
- **Préconditions** : au moins deux DUT physiquement distincts disponibles.
- **Entrées** : ensemble des DUT soumis au contrôle.
- **Étapes** :
  1. lire `device_id` sur chaque DUT ;
  2. constituer l'ensemble des identifiants observés ;
  3. rechercher les doublons.
- **Résultat attendu** : aucun doublon parmi les DUT contrôlés.
- **Critère d'acceptation** : cardinalité des `device_id` distincts égale au nombre de DUT testés.
- **Mode** : conditionnel, automatisable sur banc multi-DUT.
- **Criticité** : P0.
- **Limite** : un échantillon fini ne prouve pas mathématiquement l'unicité de toute la production ; la garantie globale peut nécessiter un contrôle de fabrication/provisionnement.

## Délégation B5 — absence volontaire de cas TT-BLK-B05-xxx

Aucun cas de test exécutable du moteur de commandes n'est créé dans FT-BLK-06. Les exigences B5 sont inventoriées dans la matrice et déléguées à FT-CMD, avec FT-INT lorsque l'oracle porte sur l'effet observable dans un autre bloc.

Cette absence est intentionnelle : elle évite de dupliquer les tests de front `submit`, idempotence, transaction, états, résultats, historique, protection, annulation et effets de commandes.
