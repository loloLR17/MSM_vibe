# TT-STR-02-B3-023 — Bloc 3 — B3_ALARM_LATCHED

## Objectif
Valider que le champ logique `B3_ALARM_LATCHED` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_ALARM_LATCHED`
- Nature : `declared_as_is`
- Champs source : `B3_ALARM_LATCHED`
- Offset début : `35`
- Offset fin : `35`
- Adresse début : `3035`
- Adresse fin : `3035`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Présence d’une alarme mémorisée`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_EXCEED_COUNT`
- Adresse de début du champ suivant attendue : `3036`
- Vérifier l'absence d'empiètement entre `B3_ALARM_LATCHED` et `B3_EXCEED_COUNT`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `3035`.
2. Vérifier que la lecture couvre la plage `3035` à `3035` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `3036`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `B3_ALARM_LATCHED` est possible sur la plage `3035` à `3035` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `B3_EXCEED_COUNT` commence à `3036` ;
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
