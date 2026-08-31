# TT-STR-02-B2-006 — Bloc 2 — prepared_time

## Objectif
Valider que le champ logique `prepared_time` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `2`
- Champ logique : `prepared_time`
- Nature : `uint32_from_split_words`
- Champs source : `prepared_time_msw;prepared_time_lsw`
- Offset début : `8`
- Offset fin : `9`
- Adresse début : `2008`
- Adresse fin : `2009`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RW`
- Description : `Temps préparé`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `prepared_time_status`
- Adresse de début du champ suivant attendue : `2010`
- Vérifier l'absence d'empiètement entre `prepared_time` et `prepared_time_status`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `2008`.
2. Vérifier que la lecture couvre la plage `2008` à `2009` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `2010`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `prepared_time` est possible sur la plage `2008` à `2009` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `prepared_time_status` commence à `2010` ;
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
