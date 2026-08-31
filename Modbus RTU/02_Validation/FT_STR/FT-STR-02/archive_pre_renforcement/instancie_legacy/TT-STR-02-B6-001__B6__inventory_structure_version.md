# TT-STR-02-B6-001 — Bloc 6 — inventory_structure_version

## Objectif
Valider que le champ logique `inventory_structure_version` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `inventory_structure_version`
- Nature : `declared_as_is`
- Champs source : `inventory_structure_version`
- Offset début : `0`
- Offset fin : `0`
- Adresse début : `6000`
- Adresse fin : `6000`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Version structure Bloc 6`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `total_campaign_count`
- Adresse de début du champ suivant attendue : `6001`
- Vérifier l'absence d'empiètement entre `inventory_structure_version` et `total_campaign_count`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `6000`.
2. Vérifier que la lecture couvre la plage `6000` à `6000` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6001`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `inventory_structure_version` est possible sur la plage `6000` à `6000` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `total_campaign_count` commence à `6001` ;
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
