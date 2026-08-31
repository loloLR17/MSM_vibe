# TT-STR-02-B0-005 — Bloc 0 — firmware_version_patch

## Objectif
Valider que le champ logique `firmware_version_patch` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `0`
- Champ logique : `firmware_version_patch`
- Nature : `declared_as_is`
- Champs source : `firmware_version_patch`
- Offset début : `5`
- Offset fin : `5`
- Adresse début : `5`
- Adresse fin : `5`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Version firmware patch`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `protocol_version`
- Adresse de début du champ suivant attendue : `6`
- Vérifier l'absence d'empiètement entre `firmware_version_patch` et `protocol_version`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5`.
2. Vérifier que la lecture couvre la plage `5` à `5` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `firmware_version_patch` est possible sur la plage `5` à `5` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `protocol_version` commence à `6` ;
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
