# TT-STR-02-B4-046 — Bloc 4 — active_campaign_duration_s

## Objectif
Valider que le champ logique `active_campaign_duration_s` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_campaign_duration_s`
- Nature : `declared_as_is`
- Champs source : `active_campaign_duration_s`
- Offset début : `106`
- Offset fin : `107`
- Adresse début : `4106`
- Adresse fin : `4107`
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
- Champ suivant attendu : `active_storage_mode`
- Adresse de début du champ suivant attendue : `4108`
- Vérifier l'absence d'empiètement entre `active_campaign_duration_s` et `active_storage_mode`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4106`.
2. Vérifier que la lecture couvre la plage `4106` à `4107` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4108`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `active_campaign_duration_s` est possible sur la plage `4106` à `4107` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_storage_mode` commence à `4108` ;
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
