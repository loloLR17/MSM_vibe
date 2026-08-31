# TT-STR-02-B4-042 — Bloc 4 — active_full_scale_code

## Objectif
Valider que le champ logique `active_full_scale_code` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_full_scale_code`
- Nature : `declared_as_is`
- Champs source : `active_full_scale_code`
- Offset début : `102`
- Offset fin : `102`
- Adresse début : `4102`
- Adresse fin : `4102`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Miroir actif`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_acquisition_mode`
- Adresse de début du champ suivant attendue : `4103`
- Vérifier l'absence d'empiètement entre `active_full_scale_code` et `active_acquisition_mode`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4102`.
2. Vérifier que la lecture couvre la plage `4102` à `4102` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4103`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `active_full_scale_code` est possible sur la plage `4102` à `4102` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_acquisition_mode` commence à `4103` ;
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
