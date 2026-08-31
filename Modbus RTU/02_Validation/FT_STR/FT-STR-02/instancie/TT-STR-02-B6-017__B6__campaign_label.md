# TT-STR-02-B6-017 — Bloc 6 — campaign_label

## Objectif
Valider que le champ logique `campaign_label` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `campaign_label`
- Nature : `declared_as_is`
- Champs source : `campaign_label`
- Offset début : `25`
- Offset fin : `40`
- Adresse début : `6025`
- Adresse fin : `6040`
- Type déclaré : `ASCII fixe`
- Taille attendue : `16` registre(s)
- Accès : `RO`
- Description : `Label campagne`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `mission_label`
- Adresse de début du champ suivant attendue : `6041`
- Vérifier l'absence d'empiètement entre `campaign_label` et `mission_label`.

## Étapes
1. Lire exactement `16` registre(s) à partir de l'adresse `6025`.
2. Vérifier que la lecture couvre la plage `6025` à `6040` sans décalage.
3. Vérifier que la taille observée correspond exactement à `16` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6041`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `ASCII fixe`.

## Résultat attendu
- la lecture de `campaign_label` est possible sur la plage `6025` à `6040` ;
- la taille observée est exactement de `16` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `mission_label` commence à `6041` ;
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
