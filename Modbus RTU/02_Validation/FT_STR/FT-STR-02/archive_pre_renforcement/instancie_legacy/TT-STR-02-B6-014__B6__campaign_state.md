# TT-STR-02-B6-014 — Bloc 6 — campaign_state

## Objectif
Valider que le champ logique `campaign_state` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `campaign_state`
- Nature : `declared_as_is`
- Champs source : `campaign_state`
- Offset début : `20`
- Offset fin : `20`
- Adresse début : `6020`
- Adresse fin : `6020`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État campagne`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `duration_s`
- Adresse de début du champ suivant attendue : `6021`
- Vérifier l'absence d'empiètement entre `campaign_state` et `duration_s`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `6020`.
2. Vérifier que la lecture couvre la plage `6020` à `6020` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6021`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `campaign_state` est possible sur la plage `6020` à `6020` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `duration_s` commence à `6021` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `enum16`.

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
