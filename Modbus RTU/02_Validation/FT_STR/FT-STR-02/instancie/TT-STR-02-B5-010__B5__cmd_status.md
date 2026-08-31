# TT-STR-02-B5-010 — Bloc 5 — cmd_status

## Objectif
Valider que le champ logique `cmd_status` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_status`
- Nature : `declared_as_is`
- Champs source : `cmd_status`
- Offset début : `10`
- Offset fin : `10`
- Adresse début : `5010`
- Adresse fin : `5010`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Statut de traitement`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_result_code`
- Adresse de début du champ suivant attendue : `5011`
- Vérifier l'absence d'empiètement entre `cmd_status` et `cmd_result_code`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5010`.
2. Vérifier que la lecture couvre la plage `5010` à `5010` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `5011`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `cmd_status` est possible sur la plage `5010` à `5010` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `cmd_result_code` commence à `5011` ;
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
