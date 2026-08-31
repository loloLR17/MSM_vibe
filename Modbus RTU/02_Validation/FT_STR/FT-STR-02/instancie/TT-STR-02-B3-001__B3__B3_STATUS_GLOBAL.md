# TT-STR-02-B3-001 — Bloc 3 — B3_STATUS_GLOBAL

## Objectif
Valider que le champ logique `B3_STATUS_GLOBAL` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_STATUS_GLOBAL`
- Nature : `declared_as_is`
- Champs source : `B3_STATUS_GLOBAL`
- Offset début : `0`
- Offset fin : `0`
- Adresse début : `3000`
- Adresse fin : `3000`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Statut global de supervision vibratoire`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_VALIDITY_FLAGS`
- Adresse de début du champ suivant attendue : `3001`
- Vérifier l'absence d'empiètement entre `B3_STATUS_GLOBAL` et `B3_VALIDITY_FLAGS`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `3000`.
2. Vérifier que la lecture couvre la plage `3000` à `3000` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `3001`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `B3_STATUS_GLOBAL` est possible sur la plage `3000` à `3000` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `B3_VALIDITY_FLAGS` commence à `3001` ;
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
