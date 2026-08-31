# TT-STR-02-B2-007 — Bloc 2 — prepared_time_status

## Objectif
Valider que le champ logique `prepared_time_status` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `2`
- Champ logique : `prepared_time_status`
- Nature : `declared_as_is`
- Champs source : `prepared_time_status`
- Offset début : `10`
- Offset fin : `10`
- Adresse début : `2010`
- Adresse fin : `2010`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État du temps préparé`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `time_accuracy_ms`
- Adresse de début du champ suivant attendue : `2011`
- Vérifier l'absence d'empiètement entre `prepared_time_status` et `time_accuracy_ms`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2010`.
2. Vérifier que la lecture couvre la plage `2010` à `2010` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `2011`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `prepared_time_status` est possible sur la plage `2010` à `2010` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `time_accuracy_ms` commence à `2011` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `enum16`.

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
