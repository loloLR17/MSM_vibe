# TT-STR-02-B3-002 — Bloc 3 — B3_VALIDITY_FLAGS

## Objectif
Valider que le champ logique `B3_VALIDITY_FLAGS` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_VALIDITY_FLAGS`
- Nature : `declared_as_is`
- Champs source : `B3_VALIDITY_FLAGS`
- Offset début : `1`
- Offset fin : `1`
- Adresse début : `3001`
- Adresse fin : `3001`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Drapeaux détaillés de validité / fraîcheur / cohérence`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_ALARM_FLAGS`
- Adresse de début du champ suivant attendue : `3002`
- Vérifier l'absence d'empiètement entre `B3_VALIDITY_FLAGS` et `B3_ALARM_FLAGS`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `3001`.
2. Vérifier que la lecture couvre la plage `3001` à `3001` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `3002`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `bitfield16`.

## Résultat attendu
- la lecture de `B3_VALIDITY_FLAGS` est possible sur la plage `3001` à `3001` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `B3_ALARM_FLAGS` commence à `3002` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `bitfield16`.

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
