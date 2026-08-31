# TT-STR-02-B1-010 — Bloc 1 — storage_status

## Objectif
Valider que le champ logique `storage_status` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `storage_status`
- Nature : `declared_as_is`
- Champs source : `storage_status`
- Offset début : `10`
- Offset fin : `10`
- Adresse début : `1010`
- Adresse fin : `1010`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État du stockage`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `storage_usage_percent`
- Adresse de début du champ suivant attendue : `1011`
- Vérifier l'absence d'empiètement entre `storage_status` et `storage_usage_percent`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1010`.
2. Vérifier que la lecture couvre la plage `1010` à `1010` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1011`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `storage_status` est possible sur la plage `1010` à `1010` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `storage_usage_percent` commence à `1011` ;
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
