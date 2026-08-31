# TT-STR-02-B5-005 — Bloc 5 — cmd_request_param3

## Objectif
Valider que le champ logique `cmd_request_param3` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_request_param3`
- Nature : `uint32_from_split_words`
- Champs source : `cmd_request_param3_msw;cmd_request_param3_lsw`
- Offset début : `4`
- Offset fin : `5`
- Adresse début : `5004`
- Adresse fin : `5005`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RW`
- Description : `Paramètre 3, mot fort`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_request_confirm_key`
- Adresse de début du champ suivant attendue : `5006`
- Vérifier l'absence d'empiètement entre `cmd_request_param3` et `cmd_request_confirm_key`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `5004`.
2. Vérifier que la lecture couvre la plage `5004` à `5005` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `5006`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `cmd_request_param3` est possible sur la plage `5004` à `5005` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `cmd_request_confirm_key` commence à `5006` ;
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
