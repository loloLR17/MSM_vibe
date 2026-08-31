# TT-STR-02-B1-018 — Bloc 1 — reserved_3

## Objectif
Valider que le champ logique `reserved_3` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `reserved_3`
- Nature : `declared_as_is`
- Champs source : `reserved_3`
- Offset début : `19`
- Offset fin : `19`
- Adresse début : `1019`
- Adresse fin : `1019`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Réservé (0)`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1019`.
2. Vérifier que la lecture couvre la plage `1019` à `1019` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier qu'aucun registre supplémentaire n'est requis au-delà de l'adresse de fin spécifiée.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `reserved_3` est possible sur la plage `1019` à `1019` ;
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
