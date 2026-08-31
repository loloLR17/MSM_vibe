# TT-STR-02-B6-020 — Bloc 6 — reserved_6B

## Objectif
Valider que le champ logique `reserved_6B` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `reserved_6B`
- Nature : `declared_as_is`
- Champs source : `reserved_6B`
- Offset début : `58`
- Offset fin : `63`
- Adresse début : `6058`
- Adresse fin : `6063`
- Type déclaré : `uint16\[6]`
- Taille attendue : `6` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `6` registre(s) à partir de l'adresse `6058`.
2. Vérifier que la lecture couvre la plage `6058` à `6063` sans décalage.
3. Vérifier que la taille observée correspond exactement à `6` registre(s).
4. Vérifier qu'aucun registre supplémentaire n'est requis au-delà de l'adresse de fin spécifiée.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16\[6]`.

## Résultat attendu
- la lecture de `reserved_6B` est possible sur la plage `6058` à `6063` ;
- la taille observée est exactement de `6` registre(s) ;
- le champ est positionné conformément au mapping ;
- aucune extension implicite du champ au-delà de l'adresse de fin spécifiée.
- le champ reste décodable conformément au type `uint16\[6]`.

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
