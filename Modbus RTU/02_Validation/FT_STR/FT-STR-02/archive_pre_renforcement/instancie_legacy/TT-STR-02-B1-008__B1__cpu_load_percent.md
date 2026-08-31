# TT-STR-02-B1-008 — Bloc 1 — cpu_load_percent

## Objectif
Valider que le champ logique `cpu_load_percent` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `cpu_load_percent`
- Nature : `declared_as_is`
- Champs source : `cpu_load_percent`
- Offset début : `8`
- Offset fin : `8`
- Adresse début : `1008`
- Adresse fin : `1008`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Charge CPU (%)`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `memory_usage_percent`
- Adresse de début du champ suivant attendue : `1009`
- Vérifier l'absence d'empiètement entre `cpu_load_percent` et `memory_usage_percent`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1008`.
2. Vérifier que la lecture couvre la plage `1008` à `1008` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1009`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `cpu_load_percent` est possible sur la plage `1008` à `1008` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `memory_usage_percent` commence à `1009` ;
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
