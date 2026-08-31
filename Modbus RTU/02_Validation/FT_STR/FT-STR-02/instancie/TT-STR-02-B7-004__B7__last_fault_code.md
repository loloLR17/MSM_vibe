# TT-STR-02-B7-004 — Bloc 7 — last_fault_code

## Objectif
Valider que le champ logique `last_fault_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `last_fault_code`
- Nature : `declared_as_is`
- Champs source : `last_fault_code`
- Offset début : `3`
- Offset fin : `3`
- Adresse début : `7003`
- Adresse fin : `7003`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Dernier défaut détecté`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `last_fault_timestamp`
- Adresse de début du champ suivant attendue : `7004`
- Vérifier l'absence d'empiètement entre `last_fault_code` et `last_fault_timestamp`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7003`.
2. Vérifier que la lecture couvre la plage `7003` à `7003` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7004`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `last_fault_code` est possible sur la plage `7003` à `7003` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `last_fault_timestamp` commence à `7004` ;
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
