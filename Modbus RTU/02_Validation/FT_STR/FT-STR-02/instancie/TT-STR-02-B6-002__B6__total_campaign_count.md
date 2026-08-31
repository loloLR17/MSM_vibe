# TT-STR-02-B6-002 — Bloc 6 — total_campaign_count

## Objectif
Valider que le champ logique `total_campaign_count` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `total_campaign_count`
- Nature : `declared_as_is`
- Champs source : `total_campaign_count`
- Offset début : `1`
- Offset fin : `1`
- Adresse début : `6001`
- Adresse fin : `6001`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Nombre total de campagnes`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `valid_campaign_count`
- Adresse de début du champ suivant attendue : `6002`
- Vérifier l'absence d'empiètement entre `total_campaign_count` et `valid_campaign_count`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `6001`.
2. Vérifier que la lecture couvre la plage `6001` à `6001` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6002`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `total_campaign_count` est possible sur la plage `6001` à `6001` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `valid_campaign_count` commence à `6002` ;
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
