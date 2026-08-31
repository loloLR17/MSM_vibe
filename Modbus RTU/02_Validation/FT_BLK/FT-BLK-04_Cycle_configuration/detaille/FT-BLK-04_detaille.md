# FT-BLK-04 — Cas de test détaillés

> Chaque cas suit le format du plan maître. Les traces minimales comprennent écritures/lectures Modbus utiles, états avant/après, images préparée/active concernées, calculs d'oracle et verdict.

## TT-BLK-B04-001 — Première écriture préparée : VIDE → BROUILLON
- **Objectif** : vérifier l'entrée dans le cycle de préparation.
- **Exigence couverte** : BLK04-B4-001.
- **Source normative** : Bloc 4 §7.1.
- **Préconditions** : `config_state = VIDE`.
- **Entrées** : valeur valide pour un champ RW de 4B/4C/4D.
- **Étapes** : écrire le champ ; relire `config_state`.
- **Résultat attendu** : `BROUILLON`.
- **Critère d’acceptation** : transition observée sans autre état intermédiaire imposé.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : état initial, écriture, état final, trames.
- **Criticité** : P0.
- **Limites / arbitrages** : les domaines de valeur restent FT-LIM.

## TT-BLK-B04-002 — Modification après VALIDE : retour BROUILLON
- **Objectif** : vérifier l'invalidation d'une configuration validée après modification préparée.
- **Exigence couverte** : BLK04-B4-002.
- **Source normative** : Bloc 4 §7.1.
- **Préconditions** : `config_state = VALIDE`.
- **Entrées** : modification valide d'au moins un champ préparé.
- **Étapes** : modifier le champ ; relire `config_state`.
- **Résultat attendu** : `BROUILLON`.
- **Critère d’acceptation** : état final égal à BROUILLON.
- **Mode d’exécution** : conditionnel à l'établissement de VALIDE.
- **Automatisation** : oui si le banc sait établir VALIDE.
- **Traces** : état avant/après et écriture.
- **Criticité** : P0.
- **Limites / arbitrages** : l'obtention de VALIDE via B5 appartient à FT-CMD.

## TT-BLK-B04-003 — Modification après ERREUR_VALIDATION
- **Objectif** : vérifier la reprise du cycle de préparation.
- **Exigence couverte** : BLK04-B4-003.
- **Source normative** : Bloc 4 §7.1.
- **Préconditions** : `config_state = ERREUR_VALIDATION`.
- **Entrées** : modification valide d'un champ préparé.
- **Étapes** : modifier ; relire l'état.
- **Résultat attendu** : `BROUILLON`.
- **Critère d’acceptation** : état final égal à BROUILLON.
- **Mode d’exécution** : conditionnel à l'établissement de l'état initial.
- **Automatisation** : oui selon banc.
- **Traces** : état avant/après et écriture.
- **Criticité** : P1.
- **Limites / arbitrages** : la génération de l'erreur de validation relève FT-CMD/FT-LIM selon scénario.

## TT-BLK-B04-004 — Modification après ERREUR_APPLICATION
- **Objectif** : vérifier la reprise du cycle après erreur d'application.
- **Exigence couverte** : BLK04-B4-004.
- **Source normative** : Bloc 4 §7.1.
- **Préconditions** : `config_state = ERREUR_APPLICATION`.
- **Entrées** : modification valide d'un champ préparé.
- **Étapes** : modifier ; relire l'état.
- **Résultat attendu** : `BROUILLON`.
- **Critère d’acceptation** : état final égal à BROUILLON.
- **Mode d’exécution** : conditionnel à l'état initial.
- **Automatisation** : oui selon banc.
- **Traces** : état avant/après et écriture.
- **Criticité** : P1.
- **Limites / arbitrages** : la génération de l'erreur d'application appartient à FT-CMD.

## TT-BLK-B04-005 — Nouvelle préparation depuis ACTIF
- **Objectif** : vérifier qu'une configuration active reste distincte d'une nouvelle préparation.
- **Exigence couverte** : BLK04-B4-005.
- **Source normative** : Bloc 4 §7.1 et §9.
- **Préconditions** : `config_state = ACTIF`, image active connue.
- **Entrées** : modification valide distincte de l'image active.
- **Étapes** : modifier un champ préparé ; relire l'état.
- **Résultat attendu** : `BROUILLON`.
- **Critère d’acceptation** : transition vers BROUILLON.
- **Mode d’exécution** : conditionnel à l'état ACTIF.
- **Automatisation** : oui selon banc.
- **Traces** : image active de référence, écriture, état final.
- **Criticité** : P0.
- **Limites / arbitrages** : l'application ayant mené à ACTIF n'est pas validée ici.

## TT-BLK-B04-006 — Préparation sans effet immédiat sur l'image active
- **Objectif** : vérifier la séparation préparé/actif.
- **Exigences couvertes** : BLK04-B4-012, BLK04-B4-013.
- **Source normative** : Bloc 4 §9.
- **Préconditions** : image active 4E connue ; aucune commande B5 d'application pendant le test.
- **Entrées** : modification valide d'un ou plusieurs champs préparés.
- **Étapes** : relever 4E et métadonnées actives ; modifier la préparation ; relire 4E et métadonnées actives.
- **Résultat attendu** : aucune modification de l'image active imputable à la préparation seule.
- **Critère d’acceptation** : image active et métadonnées actives inchangées hors évolution explicitement permise.
- **Mode d’exécution** : fonctionnel intra-bloc.
- **Automatisation** : oui.
- **Traces** : snapshots avant/après et écritures.
- **Criticité** : P0.
- **Limites / arbitrages** : l'application B5 reste hors périmètre.

## TT-BLK-B04-007 — Vecteur CRC normatif n°1
- **Objectif** : valider l'oracle CRC V1 commun firmware/centrale/simulateur.
- **Exigence couverte** : BLK04-B4-015.
- **Source normative** : Bloc 4 §6.1 à §6.4.
- **Préconditions** : capacité à constituer exactement la zone préparée de référence offsets 16..99.
- **Entrées** : vecteur normatif V1 ; autres registres du périmètre à `0x0000`.
- **Étapes** : sérialiser dans l'ordre croissant, MSB puis LSB ; calculer CRC-32/IEEE 802.3.
- **Résultat attendu** : `0x5207CCFC`.
- **Critère d’acceptation** : égalité exacte.
- **Mode d’exécution** : oracle logiciel indépendant.
- **Automatisation** : oui.
- **Traces** : image sérialisée, paramètres CRC, résultat.
- **Criticité** : P0.
- **Limites / arbitrages** : aucun autre variant CRC n'est accepté.

## TT-BLK-B04-008 — Sensibilité du CRC préparé à une modification
- **Objectif** : vérifier que le CRC représente le contenu préparé sans imposer une auto-écriture firmware.
- **Exigence couverte** : BLK04-B4-016, trace associée.
- **Source normative** : Bloc 4 §6.2 et §6.3.
- **Préconditions** : image préparée connue et oracle indépendant.
- **Entrées** : image initiale puis image avec un registre inclus modifié.
- **Étapes** : calculer les deux CRC côté test.
- **Résultat attendu** : chaque CRC correspond à son image selon l'algorithme normatif.
- **Critère d’acceptation** : calcul indépendant conforme.
- **Mode d’exécution** : trace/oracle côté centrale.
- **Automatisation** : oui.
- **Traces** : deux images et deux CRC.
- **Criticité** : P1.
- **Limites / arbitrages** : aucune exigence d'auto-mise-à-jour firmware de `prepared_config_crc`.

## TT-BLK-B04-009 — Périmètre et sérialisation CRC préparé
- **Objectif** : vérifier le périmètre exact et l'ordre de sérialisation.
- **Exigence couverte** : BLK04-B4-014.
- **Source normative** : Bloc 4 §6.2 et §6.3.
- **Préconditions** : calculateur d'oracle indépendant.
- **Entrées** : jeux différenciant ordre registres, ordre octets, réservés et limites.
- **Étapes** : calculer les variantes et comparer au calcul normatif.
- **Résultat attendu** : seul le calcul offsets 16..99, ordre croissant, MSB puis LSB est conforme.
- **Critère d’acceptation** : concordance exacte avec la règle V1.
- **Mode d’exécution** : oracle logiciel.
- **Automatisation** : oui.
- **Traces** : jeux d'essai et résultats.
- **Criticité** : P0.
- **Limites / arbitrages** : aucune extension de périmètre implicite.

## TT-BLK-B04-010 — Cohérence active_config_crc avec l'image 4E
- **Objectif** : vérifier le CRC de l'image active.
- **Exigence couverte** : BLK04-B4-018.
- **Source normative** : Bloc 4 §6.2 et §9.
- **Préconditions** : image 4E active connue et stable ; état établi sans introduire un oracle B5 non couvert.
- **Entrées** : image active lue.
- **Étapes** : lire 4E et `active_config_crc` ; calculer l'oracle ; comparer.
- **Résultat attendu** : égalité avec l'oracle indépendant.
- **Critère d’acceptation** : CRC identique.
- **Mode d’exécution** : CONDITIONAL.
- **Automatisation** : oui lorsque l'image active est déterministe.
- **Traces** : image 4E, CRC lu, CRC calculé.
- **Criticité** : P0.
- **Limites / arbitrages** : établissement de l'image active via B5 non validé ici.

## Traces déléguées
Les transitions commandées `BROUILLON→VALIDE`, `BROUILLON→ERREUR_VALIDATION`, `VALIDE→ACTIF`, `VALIDE→ERREUR_APPLICATION` et l'interdiction d'appliquer hors `VALIDE` restent dans la matrice et relèvent FT-CMD/FT-INT.
