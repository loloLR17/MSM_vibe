# TT-STR-02-B4-005 — Bloc 4 — config_state

## Objectif
Valider que le champ logique `config_state` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `config_state`
- Nature : `declared_as_is`
- Champs source : `config_state`
- Offset début : `6`
- Offset fin : `6`
- Adresse début : `4006`
- Adresse fin : `4006`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État configuration`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `config_error_code`
- Adresse de début du champ suivant attendue : `4007`
- Vérifier l'absence d'empiètement entre `config_state` et `config_error_code`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4006`.
2. Vérifier que la lecture couvre la plage `4006` à `4006` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4007`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `config_state` est possible sur la plage `4006` à `4006` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `config_error_code` commence à `4007` ;
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
