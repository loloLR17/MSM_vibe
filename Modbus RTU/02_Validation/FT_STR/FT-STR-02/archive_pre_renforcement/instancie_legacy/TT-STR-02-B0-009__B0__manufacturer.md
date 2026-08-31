# TT-STR-02-B0-009 — Bloc 0 — manufacturer

## Objectif
Valider que le champ logique `manufacturer` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `0`
- Champ logique : `manufacturer`
- Nature : `ascii_fixed_from_register_parts`
- Champs source : `manufacturer_r0;manufacturer_r1;manufacturer_r2;manufacturer_r3`
- Offset début : `16`
- Offset fin : `19`
- Adresse début : `16`
- Adresse fin : `19`
- Type déclaré : `ASCII fixe`
- Taille attendue : `4` registre(s)
- Accès : `RO`
- Description : `Fabricant`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `0` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved`
- Adresse de début du champ suivant attendue : `20`
- Vérifier l'absence d'empiètement entre `manufacturer` et `reserved`.

## Étapes
1. Lire exactement `4` registre(s) à partir de l'adresse `16`.
2. Vérifier que la lecture couvre la plage `16` à `19` sans décalage.
3. Vérifier que la taille observée correspond exactement à `4` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `20`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `ASCII fixe`.

## Résultat attendu
- la lecture de `manufacturer` est possible sur la plage `16` à `19` ;
- la taille observée est exactement de `4` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved` commence à `20` ;
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
