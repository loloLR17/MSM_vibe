# TT-STR-02-B5-006 — Bloc 5 — cmd_request_confirm_key

## Objectif
Valider que le champ logique `cmd_request_confirm_key` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_request_confirm_key`
- Nature : `declared_as_is`
- Champs source : `cmd_request_confirm_key`
- Offset début : `6`
- Offset fin : `6`
- Adresse début : `5006`
- Adresse fin : `5006`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Clé de confirmation pour commande protégée`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_request_control`
- Adresse de début du champ suivant attendue : `5007`
- Vérifier l'absence d'empiètement entre `cmd_request_confirm_key` et `cmd_request_control`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5006`.
2. Vérifier que la lecture couvre la plage `5006` à `5006` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `5007`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `cmd_request_confirm_key` est possible sur la plage `5006` à `5006` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `cmd_request_control` commence à `5007` ;
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
