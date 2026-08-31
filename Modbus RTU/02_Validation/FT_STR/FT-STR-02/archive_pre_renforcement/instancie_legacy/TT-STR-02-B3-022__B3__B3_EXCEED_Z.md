# TT-STR-02-B3-022 — Bloc 3 — B3_EXCEED_Z

## Objectif
Valider que le champ logique `B3_EXCEED_Z` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_EXCEED_Z`
- Nature : `declared_as_is`
- Champs source : `B3_EXCEED_Z`
- Offset début : `34`
- Offset fin : `34`
- Adresse début : `3034`
- Adresse fin : `3034`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Dépassement courant axe Z`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_ALARM_LATCHED`
- Adresse de début du champ suivant attendue : `3035`
- Vérifier l'absence d'empiètement entre `B3_EXCEED_Z` et `B3_ALARM_LATCHED`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `3034`.
2. Vérifier que la lecture couvre la plage `3034` à `3034` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `3035`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `B3_EXCEED_Z` est possible sur la plage `3034` à `3034` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `B3_ALARM_LATCHED` commence à `3035` ;
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
