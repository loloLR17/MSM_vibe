# TT-STR-02-B4-049 — Bloc 4 — reserved_4E_A

## Objectif
Valider que le champ logique `reserved_4E_A` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `reserved_4E_A`
- Nature : `declared_as_is`
- Champs source : `reserved_4E_A`
- Offset début : `111`
- Offset fin : `115`
- Adresse début : `4111`
- Adresse fin : `4115`
- Type déclaré : `uint16[5]`
- Taille attendue : `5` registre(s)
- Accès : `RO`
- Description : `Réservé, doit lire 0`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_supervision_enable_mask`
- Adresse de début du champ suivant attendue : `4116`
- Vérifier l'absence d'empiètement entre `reserved_4E_A` et `active_supervision_enable_mask`.

## Étapes
1. Lire exactement `5` registre(s) à partir de l'adresse `4111`.
2. Vérifier que la lecture couvre la plage `4111` à `4115` sans décalage.
3. Vérifier que la taille observée correspond exactement à `5` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4116`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16[5]`.

## Résultat attendu
- la lecture de `reserved_4E_A` est possible sur la plage `4111` à `4115` ;
- la taille observée est exactement de `5` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `active_supervision_enable_mask` commence à `4116` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint16[5]`.

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
