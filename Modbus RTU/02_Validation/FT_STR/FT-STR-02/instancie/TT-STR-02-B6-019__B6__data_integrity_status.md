# TT-STR-02-B6-019 — Bloc 6 — data_integrity_status

## Objectif
Valider que le champ logique `data_integrity_status` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `data_integrity_status`
- Nature : `declared_as_is`
- Champs source : `data_integrity_status`
- Offset début : `57`
- Offset fin : `57`
- Adresse début : `6057`
- Adresse fin : `6057`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Intégrité données`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_6B`
- Adresse de début du champ suivant attendue : `6058`
- Vérifier l'absence d'empiètement entre `data_integrity_status` et `reserved_6B`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `6057`.
2. Vérifier que la lecture couvre la plage `6057` à `6057` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6058`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `data_integrity_status` est possible sur la plage `6057` à `6057` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved_6B` commence à `6058` ;
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
