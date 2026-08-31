# TT-STR-02-B1-014 — Bloc 1 — error_code

## Objectif
Valider que le champ logique `error_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `error_code`
- Nature : `declared_as_is`
- Champs source : `error_code`
- Offset début : `15`
- Offset fin : `15`
- Adresse début : `1015`
- Adresse fin : `1015`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Code erreur principal`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `warning_code`
- Adresse de début du champ suivant attendue : `1016`
- Vérifier l'absence d'empiètement entre `error_code` et `warning_code`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1015`.
2. Vérifier que la lecture couvre la plage `1015` à `1015` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1016`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `error_code` est possible sur la plage `1015` à `1015` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `warning_code` commence à `1016` ;
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
