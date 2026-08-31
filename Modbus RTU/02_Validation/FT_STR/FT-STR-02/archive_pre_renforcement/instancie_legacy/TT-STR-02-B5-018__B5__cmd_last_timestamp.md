# TT-STR-02-B5-018 — Bloc 5 — cmd_last_timestamp

## Objectif
Valider que le champ logique `cmd_last_timestamp` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_last_timestamp`
- Nature : `uint32_from_split_words`
- Champs source : `cmd_last_timestamp_msw;cmd_last_timestamp_lsw`
- Offset début : `18`
- Offset fin : `19`
- Adresse début : `5018`
- Adresse fin : `5019`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Timestamp de fin de dernière commande, mot fort`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `5018`.
2. Vérifier que la lecture couvre la plage `5018` à `5019` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier qu'aucun registre supplémentaire n'est requis au-delà de l'adresse de fin spécifiée.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `cmd_last_timestamp` est possible sur la plage `5018` à `5019` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- aucune extension implicite du champ au-delà de l'adresse de fin spécifiée.
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
