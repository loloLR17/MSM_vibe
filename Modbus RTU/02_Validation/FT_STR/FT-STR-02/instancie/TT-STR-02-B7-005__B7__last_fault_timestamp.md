# TT-STR-02-B7-005 — Bloc 7 — last_fault_timestamp

## Objectif
Valider que le champ logique `last_fault_timestamp` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `last_fault_timestamp`
- Nature : `declared_as_is`
- Champs source : `last_fault_timestamp`
- Offset début : `4`
- Offset fin : `5`
- Adresse début : `7004`
- Adresse fin : `7005`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Timestamp dernier défaut`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `selftest_status`
- Adresse de début du champ suivant attendue : `7006`
- Vérifier l'absence d'empiètement entre `last_fault_timestamp` et `selftest_status`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `7004`.
2. Vérifier que la lecture couvre la plage `7004` à `7005` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7006`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `last_fault_timestamp` est possible sur la plage `7004` à `7005` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `selftest_status` commence à `7006` ;
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
