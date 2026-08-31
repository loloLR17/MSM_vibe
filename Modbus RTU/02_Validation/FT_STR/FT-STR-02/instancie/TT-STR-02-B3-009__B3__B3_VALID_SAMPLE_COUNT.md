# TT-STR-02-B3-009 — Bloc 3 — B3_VALID_SAMPLE_COUNT

## Objectif
Valider que le champ logique `B3_VALID_SAMPLE_COUNT` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_VALID_SAMPLE_COUNT`
- Nature : `declared_as_is`
- Champs source : `B3_VALID_SAMPLE_COUNT`
- Offset début : `12`
- Offset fin : `13`
- Adresse début : `3012`
- Adresse fin : `3013`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Nombre d’échantillons valides utilisés dans la fenêtre`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_RMS_GLOBAL_MG`
- Adresse de début du champ suivant attendue : `3014`
- Vérifier l'absence d'empiètement entre `B3_VALID_SAMPLE_COUNT` et `B3_RMS_GLOBAL_MG`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `3012`.
2. Vérifier que la lecture couvre la plage `3012` à `3013` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `3014`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `B3_VALID_SAMPLE_COUNT` est possible sur la plage `3012` à `3013` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `B3_RMS_GLOBAL_MG` commence à `3014` ;
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
