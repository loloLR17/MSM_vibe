# TT-STR-02-B0-002 — Bloc 0 — hardware_version

## Objectif
Valider que le champ logique `hardware_version` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `0`
- Champ logique : `hardware_version`
- Nature : `declared_as_is`
- Champs source : `hardware_version`
- Offset début : `2`
- Offset fin : `2`
- Adresse début : `2`
- Adresse fin : `2`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Version hardware`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `firmware_version_major`
- Adresse de début du champ suivant attendue : `3`
- Vérifier l'absence d'empiètement entre `hardware_version` et `firmware_version_major`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2`.
2. Vérifier que la lecture couvre la plage `2` à `2` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `3`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `hardware_version` est possible sur la plage `2` à `2` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `firmware_version_major` commence à `3` ;
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
