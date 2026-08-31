# TT-STR-02-B5-004 — Bloc 5 — cmd_request_param2

## Objectif
Valider que le champ logique `cmd_request_param2` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_request_param2`
- Nature : `declared_as_is`
- Champs source : `cmd_request_param2`
- Offset début : `3`
- Offset fin : `3`
- Adresse début : `5003`
- Adresse fin : `5003`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Paramètre 2`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_request_param3`
- Adresse de début du champ suivant attendue : `5004`
- Vérifier l'absence d'empiètement entre `cmd_request_param2` et `cmd_request_param3`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5003`.
2. Vérifier que la lecture couvre la plage `5003` à `5003` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `5004`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `cmd_request_param2` est possible sur la plage `5003` à `5003` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `cmd_request_param3` commence à `5004` ;
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
