# TT-STR-02-B1-011 — Bloc 1 — storage_usage_percent

## Objectif
Valider que le champ logique `storage_usage_percent` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `storage_usage_percent`
- Nature : `declared_as_is`
- Champs source : `storage_usage_percent`
- Offset début : `11`
- Offset fin : `11`
- Adresse début : `1011`
- Adresse fin : `1011`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Occupation stockage (%)`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `acquisition_state`
- Adresse de début du champ suivant attendue : `1012`
- Vérifier l'absence d'empiètement entre `storage_usage_percent` et `acquisition_state`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1011`.
2. Vérifier que la lecture couvre la plage `1011` à `1011` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1012`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `storage_usage_percent` est possible sur la plage `1011` à `1011` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `acquisition_state` commence à `1012` ;
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
