# TT-STR-02-B4-066 — Bloc 4 — reserved_4E_C

## Objectif
Valider que le champ logique `reserved_4E_C` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4E_C`
- Nature : `declared_as_is`
- Champs source : `reserved_4E_C`
- Offset début : `168`
- Offset fin : `175`
- Adresse début : `4168`
- Adresse fin : `4175`
- Type déclaré : `uint16[8]`
- Taille attendue : `8` registre(s)
- Accès : `RO`
- Description : `Réservé, doit lire 0`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `8` registre(s) à partir de l'adresse `4168`.
2. Vérifier que la lecture couvre la plage `4168` à `4175` sans décalage.
3. Vérifier que la taille observée correspond exactement à `8` registre(s).
4. Vérifier qu'aucun registre supplémentaire n'est requis au-delà de l'adresse de fin spécifiée.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16[8]`.

## Résultat attendu
- la lecture de `reserved_4E_C` est possible sur la plage `4168` à `4175` ;
- la taille observée est exactement de `8` registre(s) ;
- le champ est positionné conformément au mapping ;
- aucune extension implicite du champ au-delà de l'adresse de fin spécifiée.
- le champ reste décodable conformément au type `uint16[8]`.

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
