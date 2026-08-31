# TT-STR-02-B0-006 — Bloc 0 — protocol_version

## Objectif
Valider que le champ logique `protocol_version` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `0`
- Champ logique : `protocol_version`
- Nature : `declared_as_is`
- Champs source : `protocol_version`
- Offset début : `6`
- Offset fin : `6`
- Adresse début : `6`
- Adresse fin : `6`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Version du protocole Modbus TR2`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `device_capabilities`
- Adresse de début du champ suivant attendue : `7`
- Vérifier l'absence d'empiètement entre `protocol_version` et `device_capabilities`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `6`.
2. Vérifier que la lecture couvre la plage `6` à `6` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `protocol_version` est possible sur la plage `6` à `6` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `device_capabilities` commence à `7` ;
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
