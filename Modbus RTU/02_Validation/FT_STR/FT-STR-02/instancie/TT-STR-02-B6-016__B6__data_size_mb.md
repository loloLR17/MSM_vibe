# TT-STR-02-B6-016 — Bloc 6 — data_size_mb

## Objectif
Valider que le champ logique `data_size_mb` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `6`
- Champ logique : `data_size_mb`
- Nature : `declared_as_is`
- Champs source : `data_size_mb`
- Offset début : `23`
- Offset fin : `24`
- Adresse début : `6023`
- Adresse fin : `6024`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Taille données`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `campaign_label`
- Adresse de début du champ suivant attendue : `6025`
- Vérifier l'absence d'empiètement entre `data_size_mb` et `campaign_label`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6023`.
2. Vérifier que la lecture couvre la plage `6023` à `6024` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `6025`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `data_size_mb` est possible sur la plage `6023` à `6024` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `campaign_label` commence à `6025` ;
- aucun chevauchement n'est observé.
- le champ reste décodable conformément au type `uint32`.

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
