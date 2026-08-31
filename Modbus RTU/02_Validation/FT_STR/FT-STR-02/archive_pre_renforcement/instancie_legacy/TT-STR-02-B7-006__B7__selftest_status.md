# TT-STR-02-B7-006 — Bloc 7 — selftest_status

## Objectif
Valider que le champ logique `selftest_status` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `selftest_status`
- Nature : `declared_as_is`
- Champs source : `selftest_status`
- Offset début : `6`
- Offset fin : `6`
- Adresse début : `7006`
- Adresse fin : `7006`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État autotest`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `selftest_result_code`
- Adresse de début du champ suivant attendue : `7007`
- Vérifier l'absence d'empiètement entre `selftest_status` et `selftest_result_code`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7006`.
2. Vérifier que la lecture couvre la plage `7006` à `7006` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7007`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `selftest_status` est possible sur la plage `7006` à `7006` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `selftest_result_code` commence à `7007` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `enum16`.

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
