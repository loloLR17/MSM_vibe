# TT-STR-02-B6-018 — Bloc 6 — mission_label

## Objectif
Valider que le champ logique `mission_label` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `mission_label`
- Nature : `declared_as_is`
- Champs source : `mission_label`
- Offset début : `41`
- Offset fin : `56`
- Adresse début : `6041`
- Adresse fin : `6056`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RO`
- Description : `Label mission`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `data_integrity_status`
- Adresse de début du champ suivant attendue : `6057`
- Vérifier l'absence d'empiètement entre `mission_label` et `data_integrity_status`.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `6041`.
2. Vérifier que la lecture couvre la plage `6041` à `6056` sans décalage.
3. Vérifier que la taille observée correspond exactement à `16` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6057`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `ASCII fixe`.

## Résultat attendu
- la lecture de `mission_label` est possible sur la plage `6041` à `6056` ;
- la taille observée est exactement de `16` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `data_integrity_status` commence à `6057` ;
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
