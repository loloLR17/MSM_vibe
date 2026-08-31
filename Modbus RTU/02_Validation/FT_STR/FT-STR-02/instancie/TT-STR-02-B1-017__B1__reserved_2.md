# TT-STR-02-B1-017 — Bloc 1 — reserved_2

## Objectif
Valider que le champ logique `reserved_2` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `reserved_2`
- Nature : `declared_as_is`
- Champs source : `reserved_2`
- Offset début : `18`
- Offset fin : `18`
- Adresse début : `1018`
- Adresse fin : `1018`
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
- Champ suivant attendu : `reserved_3`
- Adresse de début du champ suivant attendue : `1019`
- Vérifier l'absence d'empiètement entre `reserved_2` et `reserved_3`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1018`.
2. Vérifier que la lecture couvre la plage `1018` à `1018` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1019`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `reserved_2` est possible sur la plage `1018` à `1018` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved_3` commence à `1019` ;
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
