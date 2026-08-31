# TT-STR-02-B4-022 — Bloc 4 — reserved_4B

## Objectif
Valider que le champ logique `reserved_4B` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4B`
- Nature : `declared_as_is`
- Champs source : `reserved_4B`
- Offset début : `28`
- Offset fin : `39`
- Adresse début : `4028`
- Adresse fin : `4039`
- Type déclaré : `uint16[12]`
- Taille attendue : `12` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `supervision_enable_mask`
- Adresse de début du champ suivant attendue : `4040`
- Vérifier l'absence d'empiètement entre `reserved_4B` et `supervision_enable_mask`.

## Étapes
1. Lire exactement `12` registre(s) à partir de l'adresse `4028`.
2. Vérifier que la lecture couvre la plage `4028` à `4039` sans décalage.
3. Vérifier que la taille observée correspond exactement à `12` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4040`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16[12]`.

## Résultat attendu
- la lecture de `reserved_4B` est possible sur la plage `4028` à `4039` ;
- la taille observée est exactement de `12` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `supervision_enable_mask` commence à `4040` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint16[12]`.

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
