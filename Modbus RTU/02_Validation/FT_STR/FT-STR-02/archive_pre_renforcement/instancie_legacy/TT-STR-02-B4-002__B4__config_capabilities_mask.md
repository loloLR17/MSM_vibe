# TT-STR-02-B4-002 — Bloc 4 — config_capabilities_mask

## Objectif
Valider que le champ logique `config_capabilities_mask` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `config_capabilities_mask`
- Nature : `declared_as_is`
- Champs source : `config_capabilities_mask`
- Offset début : `1`
- Offset fin : `1`
- Adresse début : `4001`
- Adresse fin : `4001`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Masque de capacités`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `prepared_config_id`
- Adresse de début du champ suivant attendue : `4002`
- Vérifier l'absence d'empiètement entre `config_capabilities_mask` et `prepared_config_id`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4001`.
2. Vérifier que la lecture couvre la plage `4001` à `4001` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4002`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `config_capabilities_mask` est possible sur la plage `4001` à `4001` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `prepared_config_id` commence à `4002` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint16`.

## Critères d’acceptation
- adresse de début conforme ;
- adresse de fin conforme ;
- taille conforme ;
- absence d’empiètement ;
- type structurellement cohérent ;
- aucune ambiguïté de frontière.

## Classification
- Famille : `FT-STR-02`
- Sous-famille : `Typage des champs`
- Niveau : `instancié`
