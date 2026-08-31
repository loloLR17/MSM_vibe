# TT-STR-02-B1-016 — Bloc 1 — reserved_1

## Objectif
Valider que le champ logique `reserved_1` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `reserved_1`
- Nature : `declared_as_is`
- Champs source : `reserved_1`
- Offset début : `17`
- Offset fin : `17`
- Adresse début : `1017`
- Adresse fin : `1017`
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
- Champ suivant attendu : `reserved_2`
- Adresse de début du champ suivant attendue : `1018`
- Vérifier l'absence d'empiètement entre `reserved_1` et `reserved_2`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1017`.
2. Vérifier que la lecture couvre la plage `1017` à `1017` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1018`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `reserved_1` est possible sur la plage `1017` à `1017` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved_2` commence à `1018` ;
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
