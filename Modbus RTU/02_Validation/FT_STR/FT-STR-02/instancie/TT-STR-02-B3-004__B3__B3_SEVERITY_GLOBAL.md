# TT-STR-02-B3-004 — Bloc 3 — B3_SEVERITY_GLOBAL

## Objectif
Valider que le champ logique `B3_SEVERITY_GLOBAL` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_SEVERITY_GLOBAL`
- Nature : `declared_as_is`
- Champs source : `B3_SEVERITY_GLOBAL`
- Offset début : `3`
- Offset fin : `3`
- Adresse début : `3003`
- Adresse fin : `3003`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Niveau de sévérité global courant`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_LAST_UPDATE_TR2`
- Adresse de début du champ suivant attendue : `3004`
- Vérifier l'absence d'empiètement entre `B3_SEVERITY_GLOBAL` et `B3_LAST_UPDATE_TR2`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `3003`.
2. Vérifier que la lecture couvre la plage `3003` à `3003` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `3004`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `B3_SEVERITY_GLOBAL` est possible sur la plage `3003` à `3003` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `B3_LAST_UPDATE_TR2` commence à `3004` ;
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
