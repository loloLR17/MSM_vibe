# TT-STR-02-B4-019 — Bloc 4 — campaign_duration_s

## Objectif
Valider que le champ logique `campaign_duration_s` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `campaign_duration_s`
- Nature : `declared_as_is`
- Champs source : `campaign_duration_s`
- Offset début : `23`
- Offset fin : `24`
- Adresse début : `4023`
- Adresse fin : `4024`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RW`
- Description : `Durée campagne`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `storage_mode`
- Adresse de début du champ suivant attendue : `4025`
- Vérifier l'absence d'empiètement entre `campaign_duration_s` et `storage_mode`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4023`.
2. Vérifier que la lecture couvre la plage `4023` à `4024` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4025`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `campaign_duration_s` est possible sur la plage `4023` à `4024` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `storage_mode` commence à `4025` ;
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
