# TT-STR-02-B7-013 — Bloc 7 — reserved_7A

## Objectif
Valider que le champ logique `reserved_7A` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `reserved_7A`
- Nature : `declared_as_is`
- Champs source : `reserved_7A`
- Offset début : `14`
- Offset fin : `15`
- Adresse début : `7014`
- Adresse fin : `7015`
- Type déclaré : `uint16\[2]`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ terminal du bloc : vérifier uniquement l'absence de débordement interne.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `7014`.
2. Vérifier que la lecture couvre la plage `7014` à `7015` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier qu'aucun registre supplémentaire n'est requis au-delà de l'adresse de fin spécifiée.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16\[2]`.

## Résultat attendu
- la lecture de `reserved_7A` est possible sur la plage `7014` à `7015` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- aucune extension implicite du champ au-delà de l'adresse de fin spécifiée.
- le champ reste décodable conformément au type `uint16\[2]`.

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
