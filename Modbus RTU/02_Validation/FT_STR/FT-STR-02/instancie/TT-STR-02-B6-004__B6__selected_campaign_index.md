# TT-STR-02-B6-004 — Bloc 6 — selected_campaign_index

## Objectif
Valider que le champ logique `selected_campaign_index` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `selected_campaign_index`
- Nature : `declared_as_is`
- Champs source : `selected_campaign_index`
- Offset début : `3`
- Offset fin : `3`
- Adresse début : `6003`
- Adresse fin : `6003`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Index sélectionné`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `selected_campaign_valid`
- Adresse de début du champ suivant attendue : `6004`
- Vérifier l'absence d'empiètement entre `selected_campaign_index` et `selected_campaign_valid`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `6003`.
2. Vérifier que la lecture couvre la plage `6003` à `6003` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6004`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `selected_campaign_index` est possible sur la plage `6003` à `6003` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `selected_campaign_valid` commence à `6004` ;
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
