# TT-STR-02-B4-016 — Bloc 4 — acquisition_mode

## Objectif
Valider que le champ logique `acquisition_mode` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `acquisition_mode`
- Nature : `declared_as_is`
- Champs source : `acquisition_mode`
- Offset début : `20`
- Offset fin : `20`
- Adresse début : `4020`
- Adresse fin : `4020`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Mode acquisition`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `window_size_samples`
- Adresse de début du champ suivant attendue : `4021`
- Vérifier l'absence d'empiètement entre `acquisition_mode` et `window_size_samples`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4020`.
2. Vérifier que la lecture couvre la plage `4020` à `4020` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4021`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `acquisition_mode` est possible sur la plage `4020` à `4020` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `window_size_samples` commence à `4021` ;
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
