# FT-BLK-04 — Cas de test détaillés

## TT-BLK-B04-001 — Première écriture préparée : VIDE → BROUILLON
- **Objectif** : vérifier l'entrée dans le cycle de préparation.
- **Source** : Bloc 4 §7.1.
- **Préconditions** : `config_state = VIDE`.
- **Étapes** : écrire un champ RW de la zone préparée 4B/4C/4D ; relire `config_state`.
- **Résultat attendu** : `BROUILLON`.
- **Critère d'acceptation** : aucune autre transition n'est observée.
- **Mode** : automatisable.
- **Criticité** : P0.

## TT-BLK-B04-002 — Modification après VALIDE : retour BROUILLON
- **Objectif** : vérifier l'invalidation implicite d'une configuration validée.
- **Source** : Bloc 4 §7.1.
- **Préconditions** : `config_state = VALIDE`.
- **Étapes** : modifier au moins un champ préparé ; relire `config_state`.
- **Résultat attendu** : `BROUILLON`.
- **Mode** : automatisable si l'état VALIDE peut être établi par le banc.
- **Criticité** : P0.

## TT-BLK-B04-003 — Modification après ERREUR_VALIDATION
- **Objectif** : vérifier la reprise du cycle de préparation.
- **Source** : Bloc 4 §7.1.
- **Préconditions** : `config_state = ERREUR_VALIDATION`.
- **Étapes** : modifier au moins un champ préparé ; relire l'état.
- **Résultat attendu** : `BROUILLON`.
- **Criticité** : P1.

## TT-BLK-B04-004 — Modification après ERREUR_APPLICATION
- **Objectif** : vérifier la reprise du cycle après erreur d'application.
- **Source** : Bloc 4 §7.1.
- **Préconditions** : `config_state = ERREUR_APPLICATION`.
- **Étapes** : modifier au moins un champ préparé ; relire l'état.
- **Résultat attendu** : `BROUILLON`.
- **Criticité** : P1.

## TT-BLK-B04-005 — Nouvelle préparation depuis ACTIF
- **Objectif** : vérifier qu'une configuration active reste distincte d'une nouvelle préparation.
- **Source** : Bloc 4 §7.1 et §9.
- **Préconditions** : `config_state = ACTIF` et image active connue.
- **Étapes** : préparer une configuration distincte en modifiant un champ de 4B/4C/4D ; relire l'état.
- **Résultat attendu** : `BROUILLON`.
- **Criticité** : P0.

## TT-BLK-B04-006 — Préparation sans effet immédiat sur l'image active
- **Objectif** : vérifier la séparation préparé/actif.
- **Source** : Bloc 4 §9.
- **Préconditions** : image active 4E connue ; aucune commande B5 d'application pendant le test.
- **Étapes** : relever 4E et `active_config_id`/`active_config_crc` ; modifier un ou plusieurs champs préparés ; relire 4E et les métadonnées actives.
- **Résultat attendu** : la seule préparation ne modifie pas l'image active ni les métadonnées actives imputables à une application.
- **Critère d'acceptation** : aucune propagation préparé→actif sans commande d'application réussie.
- **Criticité** : P0.

## TT-BLK-B04-007 — Vecteur CRC normatif n°1
- **Objectif** : valider l'oracle CRC V1 commun firmware/centrale/simulateur.
- **Source** : Bloc 4 §6.1 à §6.4.
- **Préconditions** : capacité à constituer exactement la zone préparée de référence offsets 16..99.
- **Étapes** : charger les valeurs normatives ; mettre tous les autres registres du périmètre à `0x0000` ; sérialiser les registres dans l'ordre croissant, MSB puis LSB ; calculer CRC-32/IEEE 802.3.
- **Résultat attendu** : `0x5207CCFC`.
- **Critère d'acceptation** : égalité exacte avec la valeur normative.
- **Mode** : automatisable.
- **Criticité** : P0.

## TT-BLK-B04-008 — Sensibilité du CRC préparé à une modification
- **Objectif** : vérifier que le CRC représente bien le contenu de la zone préparée.
- **Source** : Bloc 4 §6.2 et §6.3.
- **Préconditions** : image préparée connue et oracle CRC indépendant.
- **Étapes** : calculer le CRC d'une image ; modifier un registre inclus dans offsets 16..99 ; recalculer côté centrale/test.
- **Résultat attendu** : le CRC calculé correspond à la nouvelle image selon l'algorithme normatif.
- **Limite** : aucune exigence d'auto-écriture firmware de `prepared_config_crc` n'est imposée.
- **Criticité** : P1.

## TT-BLK-B04-009 — Périmètre et sérialisation CRC préparé
- **Objectif** : vérifier que le calcul couvre exactement offsets 16..99 avec la sérialisation normative.
- **Source** : Bloc 4 §6.2 et §6.3.
- **Préconditions** : calculateur d'oracle indépendant.
- **Étapes** : construire des jeux différenciant ordre registre, ordre octet, inclusion des réservés et limites de périmètre ; comparer les résultats au calcul normatif.
- **Résultat attendu** : seul le calcul conforme à la V1 est accepté.
- **Criticité** : P0.

## TT-BLK-B04-010 — Cohérence active_config_crc avec l'image 4E
- **Objectif** : vérifier le CRC de l'image active.
- **Source** : Bloc 4 §6.2 et §9.
- **Préconditions** : image 4E active connue et stable ; règle de calcul applicable à 4E ; état établi sans introduire d'oracle B5 non couvert.
- **Étapes** : lire 4E et `active_config_crc` ; calculer indépendamment le CRC de l'image active selon la V1 ; comparer.
- **Résultat attendu** : égalité avec l'oracle indépendant.
- **Mode** : CONDITIONAL.
- **Criticité** : P0.

## Traces déléguées

Les transitions `BROUILLON→VALIDE`, `BROUILLON→ERREUR_VALIDATION`, `VALIDE→ACTIF`, `VALIDE→ERREUR_APPLICATION` et l'interdiction d'appliquer un état non `VALIDE` sont conservées dans la matrice mais leur exécution appartient à FT-CMD / FT-INT.
