# TT-STR-02-B6-003 — Bloc 6 — valid_campaign_count

## Objectif
Valider que le champ logique `valid_campaign_count` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `valid_campaign_count`
- Nature : `declared_as_is`
- Champs source : `valid_campaign_count`
- Offset début : `2`
- Offset fin : `2`
- Adresse début : `6002`
- Adresse fin : `6002`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Nombre campagnes valides`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `selected_campaign_index`
- Adresse de début du champ suivant attendue : `6003`
- Vérifier l'absence d'empiètement entre `valid_campaign_count` et `selected_campaign_index`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `6002`.
2. Vérifier que la lecture couvre la plage `6002` à `6002` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6003`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `valid_campaign_count` est possible sur la plage `6002` à `6002` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `selected_campaign_index` commence à `6003` ;
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
