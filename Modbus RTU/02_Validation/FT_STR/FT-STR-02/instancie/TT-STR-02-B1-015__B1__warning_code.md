# TT-STR-02-B1-015 — Bloc 1 — warning_code

## Objectif
Valider que le champ logique `warning_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `warning_code`
- Nature : `declared_as_is`
- Champs source : `warning_code`
- Offset début : `16`
- Offset fin : `16`
- Adresse début : `1016`
- Adresse fin : `1016`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Code avertissement principal`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_1`
- Adresse de début du champ suivant attendue : `1017`
- Vérifier l'absence d'empiètement entre `warning_code` et `reserved_1`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1016`.
2. Vérifier que la lecture couvre la plage `1016` à `1016` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1017`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `warning_code` est possible sur la plage `1016` à `1016` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved_1` commence à `1017` ;
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
