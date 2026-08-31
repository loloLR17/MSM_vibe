# TT-STR-02-B6-005 — Bloc 6 — selected_campaign_valid

## Objectif
Valider que le champ logique `selected_campaign_valid` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `selected_campaign_valid`
- Nature : `declared_as_is`
- Champs source : `selected_campaign_valid`
- Offset début : `4`
- Offset fin : `4`
- Adresse début : `6004`
- Adresse fin : `6004`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `0 = invalide / 1 = valide`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `storage_used_mb`
- Adresse de début du champ suivant attendue : `6005`
- Vérifier l'absence d'empiètement entre `selected_campaign_valid` et `storage_used_mb`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `6004`.
2. Vérifier que la lecture couvre la plage `6004` à `6004` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6005`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `selected_campaign_valid` est possible sur la plage `6004` à `6004` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `storage_used_mb` commence à `6005` ;
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
