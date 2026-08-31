# TT-STR-02-B3-003 — Bloc 3 — B3_ALARM_FLAGS

## Objectif
Valider que le champ logique `B3_ALARM_FLAGS` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_ALARM_FLAGS`
- Nature : `declared_as_is`
- Champs source : `B3_ALARM_FLAGS`
- Offset début : `2`
- Offset fin : `2`
- Adresse début : `3002`
- Adresse fin : `3002`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Drapeaux détaillés d’alarme vibratoire`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_SEVERITY_GLOBAL`
- Adresse de début du champ suivant attendue : `3003`
- Vérifier l'absence d'empiètement entre `B3_ALARM_FLAGS` et `B3_SEVERITY_GLOBAL`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `3002`.
2. Vérifier que la lecture couvre la plage `3002` à `3002` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `3003`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `bitfield16`.

## Résultat attendu
- la lecture de `B3_ALARM_FLAGS` est possible sur la plage `3002` à `3002` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `B3_SEVERITY_GLOBAL` commence à `3003` ;
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
