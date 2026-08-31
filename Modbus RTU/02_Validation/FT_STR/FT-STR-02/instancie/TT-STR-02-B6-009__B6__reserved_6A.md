# TT-STR-02-B6-009 — Bloc 6 — reserved_6A

## Objectif
Valider que le champ logique `reserved_6A` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `reserved_6A`
- Nature : `declared_as_is`
- Champs source : `reserved_6A`
- Offset début : `10`
- Offset fin : `11`
- Adresse début : `6010`
- Adresse fin : `6011`
- Type déclaré : `uint16\[2]`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Réservé`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `campaign_id`
- Adresse de début du champ suivant attendue : `6012`
- Vérifier l'absence d'empiètement entre `reserved_6A` et `campaign_id`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6010`.
2. Vérifier que la lecture couvre la plage `6010` à `6011` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6012`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16\[2]`.

## Résultat attendu
- la lecture de `reserved_6A` est possible sur la plage `6010` à `6011` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `campaign_id` commence à `6012` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint16\[2]`.

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
