# TT-STR-02-B5-014 — Bloc 5 — cmd_last_code

## Objectif
Valider que le champ logique `cmd_last_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_last_code`
- Nature : `declared_as_is`
- Champs source : `cmd_last_code`
- Offset début : `14`
- Offset fin : `14`
- Adresse début : `5014`
- Adresse fin : `5014`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Dernière commande terminée`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_last_transaction_id`
- Adresse de début du champ suivant attendue : `5015`
- Vérifier l'absence d'empiètement entre `cmd_last_code` et `cmd_last_transaction_id`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5014`.
2. Vérifier que la lecture couvre la plage `5014` à `5014` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `5015`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `cmd_last_code` est possible sur la plage `5014` à `5014` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `cmd_last_transaction_id` commence à `5015` ;
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
