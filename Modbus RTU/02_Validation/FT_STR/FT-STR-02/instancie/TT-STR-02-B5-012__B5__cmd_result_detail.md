# TT-STR-02-B5-012 — Bloc 5 — cmd_result_detail

## Objectif
Valider que le champ logique `cmd_result_detail` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_result_detail`
- Nature : `declared_as_is`
- Champs source : `cmd_result_detail`
- Offset début : `12`
- Offset fin : `12`
- Adresse début : `5012`
- Adresse fin : `5012`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Détail complémentaire spécifique`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_engine_flags`
- Adresse de début du champ suivant attendue : `5013`
- Vérifier l'absence d'empiètement entre `cmd_result_detail` et `cmd_engine_flags`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5012`.
2. Vérifier que la lecture couvre la plage `5012` à `5012` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `5013`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `cmd_result_detail` est possible sur la plage `5012` à `5012` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `cmd_engine_flags` commence à `5013` ;
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
