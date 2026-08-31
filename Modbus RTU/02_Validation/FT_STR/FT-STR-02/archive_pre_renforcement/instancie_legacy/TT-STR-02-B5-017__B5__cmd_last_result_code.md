# TT-STR-02-B5-017 — Bloc 5 — cmd_last_result_code

## Objectif
Valider que le champ logique `cmd_last_result_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_last_result_code`
- Nature : `declared_as_is`
- Champs source : `cmd_last_result_code`
- Offset début : `17`
- Offset fin : `17`
- Adresse début : `5017`
- Adresse fin : `5017`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Résultat final de la dernière commande`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_last_timestamp`
- Adresse de début du champ suivant attendue : `5018`
- Vérifier l'absence d'empiètement entre `cmd_last_result_code` et `cmd_last_timestamp`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5017`.
2. Vérifier que la lecture couvre la plage `5017` à `5017` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `5018`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `cmd_last_result_code` est possible sur la plage `5017` à `5017` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `cmd_last_timestamp` commence à `5018` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `enum16`.

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
