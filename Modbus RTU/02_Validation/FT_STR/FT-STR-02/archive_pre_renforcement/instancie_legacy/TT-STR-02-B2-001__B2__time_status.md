# TT-STR-02-B2-001 — Bloc 2 — time_status

## Objectif
Valider que le champ logique `time_status` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `2`
- Champ logique : `time_status`
- Nature : `declared_as_is`
- Champs source : `time_status`
- Offset début : `0`
- Offset fin : `0`
- Adresse début : `2000`
- Adresse fin : `2000`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État global de la base de temps`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `time_flags`
- Adresse de début du champ suivant attendue : `2001`
- Vérifier l'absence d'empiètement entre `time_status` et `time_flags`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2000`.
2. Vérifier que la lecture couvre la plage `2000` à `2000` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `2001`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `time_status` est possible sur la plage `2000` à `2000` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `time_flags` commence à `2001` ;
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
