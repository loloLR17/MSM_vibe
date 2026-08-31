# TT-STR-02-B2-009 — Bloc 2 — drift_ppm

## Objectif
Valider que le champ logique `drift_ppm` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `2`
- Champ logique : `drift_ppm`
- Nature : `declared_as_is`
- Champs source : `drift_ppm`
- Offset début : `12`
- Offset fin : `12`
- Adresse début : `2012`
- Adresse fin : `2012`
- Type déclaré : `int16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Dérive estimée (ppm)`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `sync_source`
- Adresse de début du champ suivant attendue : `2013`
- Vérifier l'absence d'empiètement entre `drift_ppm` et `sync_source`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2012`.
2. Vérifier que la lecture couvre la plage `2012` à `2012` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `2013`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `int16`.

## Résultat attendu
- la lecture de `drift_ppm` est possible sur la plage `2012` à `2012` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `sync_source` commence à `2013` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `int16`.

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
