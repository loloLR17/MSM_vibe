# TT-STR-02-B4-014 — Bloc 4 — axes_enable_mask

## Objectif
Valider que le champ logique `axes_enable_mask` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `axes_enable_mask`
- Nature : `declared_as_is`
- Champs source : `axes_enable_mask`
- Offset début : `18`
- Offset fin : `18`
- Adresse début : `4018`
- Adresse fin : `4018`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Activation axes`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `full_scale_code`
- Adresse de début du champ suivant attendue : `4019`
- Vérifier l'absence d'empiètement entre `axes_enable_mask` et `full_scale_code`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4018`.
2. Vérifier que la lecture couvre la plage `4018` à `4018` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4019`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `axes_enable_mask` est possible sur la plage `4018` à `4018` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `full_scale_code` commence à `4019` ;
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
