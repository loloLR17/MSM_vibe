# TT-STR-02-B4-001 — Bloc 4 — config_structure_version

## Objectif
Valider que le champ logique `config_structure_version` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `config_structure_version`
- Nature : `declared_as_is`
- Champs source : `config_structure_version`
- Offset début : `0`
- Offset fin : `0`
- Adresse début : `4000`
- Adresse fin : `4000`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Version structure Bloc 4`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `config_capabilities_mask`
- Adresse de début du champ suivant attendue : `4001`
- Vérifier l'absence d'empiètement entre `config_structure_version` et `config_capabilities_mask`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4000`.
2. Vérifier que la lecture couvre la plage `4000` à `4000` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4001`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `config_structure_version` est possible sur la plage `4000` à `4000` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `config_capabilities_mask` commence à `4001` ;
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
