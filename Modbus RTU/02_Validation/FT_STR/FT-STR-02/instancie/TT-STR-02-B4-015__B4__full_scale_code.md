# TT-STR-02-B4-015 — Bloc 4 — full_scale_code

## Objectif
Valider que le champ logique `full_scale_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `full_scale_code`
- Nature : `declared_as_is`
- Champs source : `full_scale_code`
- Offset début : `19`
- Offset fin : `19`
- Adresse début : `4019`
- Adresse fin : `4019`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Pleine échelle`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `acquisition_mode`
- Adresse de début du champ suivant attendue : `4020`
- Vérifier l'absence d'empiètement entre `full_scale_code` et `acquisition_mode`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4019`.
2. Vérifier que la lecture couvre la plage `4019` à `4019` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4020`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `full_scale_code` est possible sur la plage `4019` à `4019` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `acquisition_mode` commence à `4020` ;
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
