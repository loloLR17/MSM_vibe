# TT-STR-02-B0-001 — Bloc 0 — device_id

## Objectif
Valider que le champ logique `device_id` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `0`
- Champ logique : `device_id`
- Nature : `uint32_from_split_words`
- Champs source : `device_id_msw;device_id_lsw`
- Offset début : `0`
- Offset fin : `1`
- Adresse début : `0`
- Adresse fin : `1`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Identifiant unique capteur`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `hardware_version`
- Adresse de début du champ suivant attendue : `2`
- Vérifier l'absence d'empiètement entre `device_id` et `hardware_version`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `0`.
2. Vérifier que la lecture couvre la plage `0` à `1` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `2`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `device_id` est possible sur la plage `0` à `1` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `hardware_version` commence à `2` ;
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
