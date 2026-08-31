# TT-STR-02-B2-008 — Bloc 2 — time_accuracy_ms

## Objectif
Valider que le champ logique `time_accuracy_ms` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `2`
- Champ logique : `time_accuracy_ms`
- Nature : `declared_as_is`
- Champs source : `time_accuracy_ms`
- Offset début : `11`
- Offset fin : `11`
- Adresse début : `2011`
- Adresse fin : `2011`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Précision estimée de l’horloge (ms)`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `drift_ppm`
- Adresse de début du champ suivant attendue : `2012`
- Vérifier l'absence d'empiètement entre `time_accuracy_ms` et `drift_ppm`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2011`.
2. Vérifier que la lecture couvre la plage `2011` à `2011` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `2012`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `time_accuracy_ms` est possible sur la plage `2011` à `2011` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `drift_ppm` commence à `2012` ;
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
