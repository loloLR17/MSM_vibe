# TT-STR-02-B4-059 — Bloc 4 — active_mission_id

## Objectif
Valider que le champ logique `active_mission_id` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_mission_id`
- Nature : `declared_as_is`
- Champs source : `active_mission_id`
- Offset début : `130`
- Offset fin : `131`
- Adresse début : `4130`
- Adresse fin : `4131`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Miroir actif`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_campaign_label`
- Adresse de début du champ suivant attendue : `4132`
- Vérifier l'absence d'empiètement entre `active_mission_id` et `active_campaign_label`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4130`.
2. Vérifier que la lecture couvre la plage `4130` à `4131` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4132`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `active_mission_id` est possible sur la plage `4130` à `4131` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_campaign_label` commence à `4132` ;
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
