# TT-STR-02-B4-038 — Bloc 4 — sea_state_code

## Objectif
Valider que le champ logique `sea_state_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `sea_state_code`
- Nature : `declared_as_is`
- Champs source : `sea_state_code`
- Offset début : `95`
- Offset fin : `95`
- Adresse début : `4095`
- Adresse fin : `4095`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `État mer`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_4D`
- Adresse de début du champ suivant attendue : `4096`
- Vérifier l'absence d'empiètement entre `sea_state_code` et `reserved_4D`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4095`.
2. Vérifier que la lecture couvre la plage `4095` à `4095` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4096`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `sea_state_code` est possible sur la plage `4095` à `4095` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved_4D` commence à `4096` ;
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
