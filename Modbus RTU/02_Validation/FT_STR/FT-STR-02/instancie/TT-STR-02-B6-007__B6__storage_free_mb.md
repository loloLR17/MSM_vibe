# TT-STR-02-B6-007 — Bloc 6 — storage_free_mb

## Objectif
Valider que le champ logique `storage_free_mb` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `storage_free_mb`
- Nature : `declared_as_is`
- Champs source : `storage_free_mb`
- Offset début : `7`
- Offset fin : `8`
- Adresse début : `6007`
- Adresse fin : `6008`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Espace libre`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `storage_health_status`
- Adresse de début du champ suivant attendue : `6009`
- Vérifier l'absence d'empiètement entre `storage_free_mb` et `storage_health_status`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6007`.
2. Vérifier que la lecture couvre la plage `6007` à `6008` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6009`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `storage_free_mb` est possible sur la plage `6007` à `6008` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `storage_health_status` commence à `6009` ;
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
