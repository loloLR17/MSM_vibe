# TT-STR-02-B6-006 — Bloc 6 — storage_used_mb

## Objectif
Valider que le champ logique `storage_used_mb` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `storage_used_mb`
- Nature : `declared_as_is`
- Champs source : `storage_used_mb`
- Offset début : `5`
- Offset fin : `6`
- Adresse début : `6005`
- Adresse fin : `6006`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Espace utilisé`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `storage_free_mb`
- Adresse de début du champ suivant attendue : `6007`
- Vérifier l'absence d'empiètement entre `storage_used_mb` et `storage_free_mb`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6005`.
2. Vérifier que la lecture couvre la plage `6005` à `6006` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6007`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `storage_used_mb` est possible sur la plage `6005` à `6006` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `storage_free_mb` commence à `6007` ;
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
