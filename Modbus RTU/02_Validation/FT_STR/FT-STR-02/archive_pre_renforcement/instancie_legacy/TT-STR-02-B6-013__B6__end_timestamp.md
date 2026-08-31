# TT-STR-02-B6-013 — Bloc 6 — end_timestamp

## Objectif
Valider que le champ logique `end_timestamp` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `end_timestamp`
- Nature : `declared_as_is`
- Champs source : `end_timestamp`
- Offset début : `18`
- Offset fin : `19`
- Adresse début : `6018`
- Adresse fin : `6019`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Fin campagne`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `campaign_state`
- Adresse de début du champ suivant attendue : `6020`
- Vérifier l'absence d'empiètement entre `end_timestamp` et `campaign_state`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6018`.
2. Vérifier que la lecture couvre la plage `6018` à `6019` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6020`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `end_timestamp` est possible sur la plage `6018` à `6019` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `campaign_state` commence à `6020` ;
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
