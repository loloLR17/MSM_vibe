# TT-STR-02-B1-013 — Bloc 1 — active_campaign_id

## Objectif
Valider que le champ logique `active_campaign_id` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `active_campaign_id`
- Nature : `uint32_from_split_words`
- Champs source : `active_campaign_id_msw;active_campaign_id_lsw`
- Offset début : `13`
- Offset fin : `14`
- Adresse début : `1013`
- Adresse fin : `1014`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `ID campagne active`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `error_code`
- Adresse de début du champ suivant attendue : `1015`
- Vérifier l'absence d'empiètement entre `active_campaign_id` et `error_code`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `1013`.
2. Vérifier que la lecture couvre la plage `1013` à `1014` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1015`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `active_campaign_id` est possible sur la plage `1013` à `1014` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `error_code` commence à `1015` ;
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
