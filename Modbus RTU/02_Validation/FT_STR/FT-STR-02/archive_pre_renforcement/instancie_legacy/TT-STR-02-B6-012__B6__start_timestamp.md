# TT-STR-02-B6-012 — Bloc 6 — start_timestamp

## Objectif
Valider que le champ logique `start_timestamp` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `start_timestamp`
- Nature : `declared_as_is`
- Champs source : `start_timestamp`
- Offset début : `16`
- Offset fin : `17`
- Adresse début : `6016`
- Adresse fin : `6017`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Début campagne`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `end_timestamp`
- Adresse de début du champ suivant attendue : `6018`
- Vérifier l'absence d'empiètement entre `start_timestamp` et `end_timestamp`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6016`.
2. Vérifier que la lecture couvre la plage `6016` à `6017` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6018`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `start_timestamp` est possible sur la plage `6016` à `6017` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `end_timestamp` commence à `6018` ;
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
