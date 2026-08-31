# TT-STR-02-B4-007 — Bloc 4 — prepared_config_crc

## Objectif
Valider que le champ logique `prepared_config_crc` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `prepared_config_crc`
- Nature : `declared_as_is`
- Champs source : `prepared_config_crc`
- Offset début : `8`
- Offset fin : `9`
- Adresse début : `4008`
- Adresse fin : `4009`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RW`
- Description : `CRC configuration préparée`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_config_crc`
- Adresse de début du champ suivant attendue : `4010`
- Vérifier l'absence d'empiètement entre `prepared_config_crc` et `active_config_crc`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4008`.
2. Vérifier que la lecture couvre la plage `4008` à `4009` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4010`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `prepared_config_crc` est possible sur la plage `4008` à `4009` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_config_crc` commence à `4010` ;
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
