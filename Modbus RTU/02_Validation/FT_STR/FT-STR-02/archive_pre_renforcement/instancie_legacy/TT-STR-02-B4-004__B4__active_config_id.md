# TT-STR-02-B4-004 — Bloc 4 — active_config_id

## Objectif
Valider que le champ logique `active_config_id` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_config_id`
- Nature : `declared_as_is`
- Champs source : `active_config_id`
- Offset début : `4`
- Offset fin : `5`
- Adresse début : `4004`
- Adresse fin : `4005`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `ID configuration active`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `config_state`
- Adresse de début du champ suivant attendue : `4006`
- Vérifier l'absence d'empiètement entre `active_config_id` et `config_state`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4004`.
2. Vérifier que la lecture couvre la plage `4004` à `4005` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4006`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `active_config_id` est possible sur la plage `4004` à `4005` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `config_state` commence à `4006` ;
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
