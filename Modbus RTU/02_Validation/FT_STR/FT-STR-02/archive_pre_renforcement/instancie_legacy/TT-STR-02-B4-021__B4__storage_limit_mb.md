# TT-STR-02-B4-021 — Bloc 4 — storage_limit_mb

## Objectif
Valider que le champ logique `storage_limit_mb` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `storage_limit_mb`
- Nature : `declared_as_is`
- Champs source : `storage_limit_mb`
- Offset début : `26`
- Offset fin : `27`
- Adresse début : `4026`
- Adresse fin : `4027`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RW`
- Description : `Limite stockage`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_4B`
- Adresse de début du champ suivant attendue : `4028`
- Vérifier l'absence d'empiètement entre `storage_limit_mb` et `reserved_4B`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4026`.
2. Vérifier que la lecture couvre la plage `4026` à `4027` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4028`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `storage_limit_mb` est possible sur la plage `4026` à `4027` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved_4B` commence à `4028` ;
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
