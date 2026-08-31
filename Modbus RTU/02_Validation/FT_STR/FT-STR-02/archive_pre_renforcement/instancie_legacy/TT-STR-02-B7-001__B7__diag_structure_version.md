# TT-STR-02-B7-001 — Bloc 7 — diag_structure_version

## Objectif
Valider que le champ logique `diag_structure_version` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `diag_structure_version`
- Nature : `declared_as_is`
- Champs source : `diag_structure_version`
- Offset début : `0`
- Offset fin : `0`
- Adresse début : `7000`
- Adresse fin : `7000`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Version structure Bloc 7`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `system_health_status`
- Adresse de début du champ suivant attendue : `7001`
- Vérifier l'absence d'empiètement entre `diag_structure_version` et `system_health_status`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7000`.
2. Vérifier que la lecture couvre la plage `7000` à `7000` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7001`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `diag_structure_version` est possible sur la plage `7000` à `7000` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `system_health_status` commence à `7001` ;
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
