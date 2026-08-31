# TT-STR-02-B0-003 — Bloc 0 — firmware_version_major

## Objectif
Valider que le champ logique `firmware_version_major` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `0`
- Champ logique : `firmware_version_major`
- Nature : `declared_as_is`
- Champs source : `firmware_version_major`
- Offset début : `3`
- Offset fin : `3`
- Adresse début : `3`
- Adresse fin : `3`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Version firmware majeure`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `firmware_version_minor`
- Adresse de début du champ suivant attendue : `4`
- Vérifier l'absence d'empiètement entre `firmware_version_major` et `firmware_version_minor`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `3`.
2. Vérifier que la lecture couvre la plage `3` à `3` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `firmware_version_major` est possible sur la plage `3` à `3` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `firmware_version_minor` commence à `4` ;
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
