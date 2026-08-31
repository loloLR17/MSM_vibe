# TT-STR-02-B4-030 — Bloc 4 — reserved_4C

## Objectif
Valider que le champ logique `reserved_4C` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4C`
- Nature : `declared_as_is`
- Champs source : `reserved_4C`
- Offset début : `47`
- Offset fin : `55`
- Adresse début : `4047`
- Adresse fin : `4055`
- Type déclaré : `uint16[9]`
- Taille attendue : `9` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `campaign_context_id`
- Adresse de début du champ suivant attendue : `4056`
- Vérifier l'absence d'empiètement entre `reserved_4C` et `campaign_context_id`.

## Étapes
1. Lire exactement `9` registre(s) à partir de l'adresse `4047`.
2. Vérifier que la lecture couvre la plage `4047` à `4055` sans décalage.
3. Vérifier que la taille observée correspond exactement à `9` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4056`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16[9]`.

## Résultat attendu
- la lecture de `reserved_4C` est possible sur la plage `4047` à `4055` ;
- la taille observée est exactement de `9` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `campaign_context_id` commence à `4056` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint16[9]`.

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
