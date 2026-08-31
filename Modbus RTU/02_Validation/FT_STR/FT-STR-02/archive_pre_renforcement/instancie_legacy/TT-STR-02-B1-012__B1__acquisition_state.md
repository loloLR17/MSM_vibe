# TT-STR-02-B1-012 — Bloc 1 — acquisition_state

## Objectif
Valider que le champ logique `acquisition_state` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `acquisition_state`
- Nature : `declared_as_is`
- Champs source : `acquisition_state`
- Offset début : `12`
- Offset fin : `12`
- Adresse début : `1012`
- Adresse fin : `1012`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État de l’acquisition`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_campaign_id`
- Adresse de début du champ suivant attendue : `1013`
- Vérifier l'absence d'empiètement entre `acquisition_state` et `active_campaign_id`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1012`.
2. Vérifier que la lecture couvre la plage `1012` à `1012` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1013`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `acquisition_state` est possible sur la plage `1012` à `1012` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_campaign_id` commence à `1013` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `enum16`.

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
