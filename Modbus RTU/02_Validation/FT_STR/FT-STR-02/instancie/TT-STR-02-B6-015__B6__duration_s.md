# TT-STR-02-B6-015 — Bloc 6 — duration_s

## Objectif
Valider que le champ logique `duration_s` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `duration_s`
- Nature : `declared_as_is`
- Champs source : `duration_s`
- Offset début : `21`
- Offset fin : `22`
- Adresse début : `6021`
- Adresse fin : `6022`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Durée`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `data_size_mb`
- Adresse de début du champ suivant attendue : `6023`
- Vérifier l'absence d'empiètement entre `duration_s` et `data_size_mb`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6021`.
2. Vérifier que la lecture couvre la plage `6021` à `6022` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6023`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `duration_s` est possible sur la plage `6021` à `6022` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `data_size_mb` commence à `6023` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint32`.

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
