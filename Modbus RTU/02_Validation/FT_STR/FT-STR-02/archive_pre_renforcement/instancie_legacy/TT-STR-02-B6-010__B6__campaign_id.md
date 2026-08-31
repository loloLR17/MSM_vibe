# TT-STR-02-B6-010 — Bloc 6 — campaign_id

## Objectif
Valider que le champ logique `campaign_id` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `campaign_id`
- Nature : `declared_as_is`
- Champs source : `campaign_id`
- Offset début : `12`
- Offset fin : `13`
- Adresse début : `6012`
- Adresse fin : `6013`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Identifiant campagne`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `mission_id`
- Adresse de début du champ suivant attendue : `6014`
- Vérifier l'absence d'empiètement entre `campaign_id` et `mission_id`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6012`.
2. Vérifier que la lecture couvre la plage `6012` à `6013` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6014`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `campaign_id` est possible sur la plage `6012` à `6013` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `mission_id` commence à `6014` ;
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
