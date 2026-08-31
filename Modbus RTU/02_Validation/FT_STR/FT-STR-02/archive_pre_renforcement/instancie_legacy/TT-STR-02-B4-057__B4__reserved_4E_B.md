# TT-STR-02-B4-057 — Bloc 4 — reserved_4E_B

## Objectif
Valider que le champ logique `reserved_4E_B` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4E_B`
- Nature : `declared_as_is`
- Champs source : `reserved_4E_B`
- Offset début : `123`
- Offset fin : `127`
- Adresse début : `4123`
- Adresse fin : `4127`
- Type déclaré : `uint16[5]`
- Taille attendue : `5` registre(s)
- Accès : `RO`
- Description : `Réservé, doit lire 0`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_campaign_context_id`
- Adresse de début du champ suivant attendue : `4128`
- Vérifier l'absence d'empiètement entre `reserved_4E_B` et `active_campaign_context_id`.

## Étapes
1. Lire exactement `5` registre(s) à partir de l'adresse `4123`.
2. Vérifier que la lecture couvre la plage `4123` à `4127` sans décalage.
3. Vérifier que la taille observée correspond exactement à `5` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4128`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16[5]`.

## Résultat attendu
- la lecture de `reserved_4E_B` est possible sur la plage `4123` à `4127` ;
- la taille observée est exactement de `5` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_campaign_context_id` commence à `4128` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint16[5]`.

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
