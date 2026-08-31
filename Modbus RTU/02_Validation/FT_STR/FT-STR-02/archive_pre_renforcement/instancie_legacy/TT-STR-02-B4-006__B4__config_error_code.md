# TT-STR-02-B4-006 — Bloc 4 — config_error_code

## Objectif
Valider que le champ logique `config_error_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `config_error_code`
- Nature : `declared_as_is`
- Champs source : `config_error_code`
- Offset début : `7`
- Offset fin : `7`
- Adresse début : `4007`
- Adresse fin : `4007`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Code erreur configuration`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `prepared_config_crc`
- Adresse de début du champ suivant attendue : `4008`
- Vérifier l'absence d'empiètement entre `config_error_code` et `prepared_config_crc`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4007`.
2. Vérifier que la lecture couvre la plage `4007` à `4007` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4008`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `config_error_code` est possible sur la plage `4007` à `4007` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `prepared_config_crc` commence à `4008` ;
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
