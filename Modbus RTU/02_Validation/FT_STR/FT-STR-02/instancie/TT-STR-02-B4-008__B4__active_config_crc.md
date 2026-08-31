# TT-STR-02-B4-008 — Bloc 4 — active_config_crc

## Objectif
Valider que le champ logique `active_config_crc` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_config_crc`
- Nature : `declared_as_is`
- Champs source : `active_config_crc`
- Offset début : `10`
- Offset fin : `11`
- Adresse début : `4010`
- Adresse fin : `4011`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `CRC configuration active`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `config_revision_counter`
- Adresse de début du champ suivant attendue : `4012`
- Vérifier l'absence d'empiètement entre `active_config_crc` et `config_revision_counter`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `4010`.
2. Vérifier que la lecture couvre la plage `4010` à `4011` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4012`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `active_config_crc` est possible sur la plage `4010` à `4011` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `config_revision_counter` commence à `4012` ;
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
