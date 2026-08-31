# TT-STR-02-B0-010 — Bloc 0 — reserved

## Objectif
Valider que le champ logique `reserved` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `0`
- Champ logique : `reserved`
- Nature : `declared_as_is`
- Champs source : `reserved`
- Offset début : `20`
- Offset fin : `20`
- Adresse début : `20`
- Adresse fin : `20`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Réservé (0)`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `20`.
2. Vérifier que la lecture couvre la plage `20` à `20` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier qu'aucun registre supplémentaire n'est requis au-delà de l'adresse de fin spécifiée.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `reserved` est possible sur la plage `20` à `20` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- aucune extension implicite du champ au-delà de l'adresse de fin spécifiée.
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
