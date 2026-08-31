# TT-STR-02-B1-007 — Bloc 1 — internal_temp_dC

## Objectif
Valider que le champ logique `internal_temp_dC` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `internal_temp_dC`
- Nature : `declared_as_is`
- Champs source : `internal_temp_dC`
- Offset début : `7`
- Offset fin : `7`
- Adresse début : `1007`
- Adresse fin : `1007`
- Type déclaré : `int16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Température interne (déci °C)`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `cpu_load_percent`
- Adresse de début du champ suivant attendue : `1008`
- Vérifier l'absence d'empiètement entre `internal_temp_dC` et `cpu_load_percent`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `1007`.
2. Vérifier que la lecture couvre la plage `1007` à `1007` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1008`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `int16`.

## Résultat attendu
- la lecture de `internal_temp_dC` est possible sur la plage `1007` à `1007` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `cpu_load_percent` commence à `1008` ;
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
