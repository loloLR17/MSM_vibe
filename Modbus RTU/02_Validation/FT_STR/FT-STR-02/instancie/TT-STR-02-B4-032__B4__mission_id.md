# TT-STR-02-B4-032 — Bloc 4 — mission_id

## Objectif
Valider que le champ logique `mission_id` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `mission_id`
- Nature : `declared_as_is`
- Champs source : `mission_id`
- Offset début : `58`
- Offset fin : `59`
- Adresse début : `4058`
- Adresse fin : `4059`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RW`
- Description : `ID mission`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `campaign_label`
- Adresse de début du champ suivant attendue : `4060`
- Vérifier l'absence d'empiètement entre `mission_id` et `campaign_label`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4058`.
2. Vérifier que la lecture couvre la plage `4058` à `4059` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4060`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `mission_id` est possible sur la plage `4058` à `4059` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `campaign_label` commence à `4060` ;
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
