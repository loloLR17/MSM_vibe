# FT-BLK-06 — Cas de test détaillés

> Chaque cas est autonome et suit le format du plan maître. Les traces minimales comprennent lectures Modbus brutes, valeurs comparées, contexte de banc, horodatage et verdict.

## TT-BLK-B00-001 — Stabilité des informations d'identification
- **Objectif** : vérifier que les informations B0 restent statiques pendant le fonctionnement normal.
- **Exigence couverte** : BLK06-B0-001.
- **Source normative** : Bloc 0 §7.
- **Préconditions** : fonctionnement normal ; absence de reboot, reprogrammation, remplacement matériel ou opération modifiant légitimement l'identité.
- **Entrées** : aucune.
- **Étapes** : lire les champs non réservés B0 ; faire fonctionner le DUT ; provoquer si possible plusieurs activités normales ; relire B0 ; comparer.
- **Résultat attendu** : aucun champ d'identification ne change.
- **Critère d’acceptation** : égalité des valeurs fonctionnelles avant/après.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : images B0 avant/après, événements du banc et trames.
- **Criticité** : P1.
- **Limites / arbitrages** : ne démontre pas la persistance après reboot.

## TT-BLK-B00-002 — Indépendance vis-à-vis des états dynamiques
- **Objectif** : vérifier qu'un changement d'état dynamique n'altère pas B0.
- **Exigence couverte** : BLK06-B0-002.
- **Source normative** : Bloc 0 §7.
- **Préconditions** : possibilité de faire varier au moins un état dynamique normal sans reboot ni opération modifiant l'identité.
- **Entrées** : scénario dynamique maîtrisé.
- **Étapes** : lire B0 ; faire évoluer l'état dynamique ; relire B0 ; comparer.
- **Résultat attendu** : valeurs B0 inchangées.
- **Critère d’acceptation** : aucune dépendance observable entre état dynamique et B0.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui selon banc.
- **Traces** : états dynamiques, images B0 et trames.
- **Criticité** : P1.
- **Limites / arbitrages** : les commandes utilisées pour provoquer l'état ne sont pas validées ici.

## TT-BLK-B00-003 — Unicité de device_id sur plusieurs équipements
- **Objectif** : vérifier l'unicité de `device_id` sur plusieurs DUT distincts.
- **Exigence couverte** : BLK06-B0-003.
- **Source normative** : Bloc 0 §7.
- **Préconditions** : au moins deux DUT distincts disponibles.
- **Entrées** : ensemble des DUT contrôlés.
- **Étapes** : lire `device_id` sur chaque DUT ; constituer l'ensemble ; rechercher les doublons.
- **Résultat attendu** : aucun doublon.
- **Critère d’acceptation** : nombre d'identifiants distincts égal au nombre de DUT testés.
- **Mode d’exécution** : conditionnel multi-DUT.
- **Automatisation** : oui sur banc multi-DUT.
- **Traces** : liste DUT/identifiants et trames.
- **Criticité** : P0.
- **Limites / arbitrages** : un échantillon fini ne prouve pas l'unicité de toute la production ; un contrôle de fabrication peut être nécessaire.

## Délégation B5 — absence volontaire de TT-BLK-B05-xxx
Les exigences du moteur de commandes sont inventoriées dans la matrice et déléguées à FT-CMD, avec FT-INT lorsque l'oracle porte sur un autre bloc. Cette absence est intentionnelle et évite toute duplication du front `submit`, de l'idempotence, des transactions, statuts, résultats, protections, annulations et effets commandés.
