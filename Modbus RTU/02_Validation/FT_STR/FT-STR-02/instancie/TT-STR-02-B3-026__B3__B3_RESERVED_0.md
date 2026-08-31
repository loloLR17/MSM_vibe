# TT-STR-02-B3-026 — Bloc 3 — B3_RESERVED_0

## Objectif
Valider que le champ logique `B3_RESERVED_0` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_RESERVED_0`
- Nature : `declared_as_is`
- Champs source : `B3_RESERVED_0`
- Offset début : `40`
- Offset fin : `47`
- Adresse début : `3040`
- Adresse fin : `3047`
- Type déclaré : `réservé`
- Taille attendue : `8` registre(s)
- Accès : `RO`
- Description : `Réserve d’extension future`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `8` registre(s) à partir de l'adresse `3040`.
2. Vérifier que la lecture couvre la plage `3040` à `3047` sans décalage.
3. Vérifier que la taille observée correspond exactement à `8` registre(s).
4. Vérifier qu'aucun registre supplémentaire n'est requis au-delà de l'adresse de fin spécifiée.
5. Vérifier que l'interprétation du champ reste compatible avec le type `réservé`.

## Résultat attendu
- la lecture de `B3_RESERVED_0` est possible sur la plage `3040` à `3047` ;
- la taille observée est exactement de `8` registre(s) ;
- le champ est positionné conformément au mapping ;
- aucune extension implicite du champ au-delà de l'adresse de fin spécifiée.
- le champ reste décodable conformément au type `réservé`.

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
