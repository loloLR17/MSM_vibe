# TT-STR-02-B0-008 — Bloc 0 — serial_number

## Objectif
Valider que le champ logique `serial_number` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `0`
- Champ logique : `serial_number`
- Nature : `ascii_fixed_from_register_parts`
- Champs source : `serial_number_r0;serial_number_r1;serial_number_r2;serial_number_r3;serial_number_r4;serial_number_r5;serial_number_r6;serial_number_r7`
- Offset début : `8`
- Offset fin : `15`
- Adresse début : `8`
- Adresse fin : `15`
- Type déclaré : `ASCII fixe`
- Taille attendue : `8` registre(s)
- Accès : `RO`
- Description : `Numéro de série`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `manufacturer`
- Adresse de début du champ suivant attendue : `16`
- Vérifier l'absence d'empiètement entre `serial_number` et `manufacturer`.

## Étapes
1. Lire exactement `8` registre(s) à partir de l'adresse `8`.
2. Vérifier que la lecture couvre la plage `8` à `15` sans décalage.
3. Vérifier que la taille observée correspond exactement à `8` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `16`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `ASCII fixe`.

## Résultat attendu
- la lecture de `serial_number` est possible sur la plage `8` à `15` ;
- la taille observée est exactement de `8` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `manufacturer` commence à `16` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `ASCII fixe`.

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
