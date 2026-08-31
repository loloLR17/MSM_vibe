# TT-STR-02-B7-007 — Bloc 7 — selftest_result_code

## Objectif
Valider que le champ logique `selftest_result_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `selftest_result_code`
- Nature : `declared_as_is`
- Champs source : `selftest_result_code`
- Offset début : `7`
- Offset fin : `7`
- Adresse début : `7007`
- Adresse fin : `7007`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Résultat autotest`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `selftest_detail`
- Adresse de début du champ suivant attendue : `7008`
- Vérifier l'absence d'empiètement entre `selftest_result_code` et `selftest_detail`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7007`.
2. Vérifier que la lecture couvre la plage `7007` à `7007` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7008`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `selftest_result_code` est possible sur la plage `7007` à `7007` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `selftest_detail` commence à `7008` ;
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
