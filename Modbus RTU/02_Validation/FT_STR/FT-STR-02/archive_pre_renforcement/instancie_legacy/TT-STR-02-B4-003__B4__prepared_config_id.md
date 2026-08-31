# TT-STR-02-B4-003 — Bloc 4 — prepared_config_id

## Objectif
Valider que le champ logique `prepared_config_id` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `prepared_config_id`
- Nature : `declared_as_is`
- Champs source : `prepared_config_id`
- Offset début : `2`
- Offset fin : `3`
- Adresse début : `4002`
- Adresse fin : `4003`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RW`
- Description : `ID configuration préparée`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_config_id`
- Adresse de début du champ suivant attendue : `4004`
- Vérifier l'absence d'empiètement entre `prepared_config_id` et `active_config_id`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4002`.
2. Vérifier que la lecture couvre la plage `4002` à `4003` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4004`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `prepared_config_id` est possible sur la plage `4002` à `4003` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_config_id` commence à `4004` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint32`.

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
