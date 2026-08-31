# TT-STR-02-B5-001 — Bloc 5 — cmd_request_code

## Objectif
Valider que le champ logique `cmd_request_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_request_code`
- Nature : `declared_as_is`
- Champs source : `cmd_request_code`
- Offset début : `0`
- Offset fin : `0`
- Adresse début : `5000`
- Adresse fin : `5000`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Code commande demandé par la centrale`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_request_transaction_id`
- Adresse de début du champ suivant attendue : `5001`
- Vérifier l'absence d'empiètement entre `cmd_request_code` et `cmd_request_transaction_id`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5000`.
2. Vérifier que la lecture couvre la plage `5000` à `5000` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `5001`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `cmd_request_code` est possible sur la plage `5000` à `5000` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `cmd_request_transaction_id` commence à `5001` ;
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
