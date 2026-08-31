# TT-STR-02-B3-017 — Bloc 3 — B3_PEAK_Z_MG

## Objectif
Valider que le champ logique `B3_PEAK_Z_MG` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_PEAK_Z_MG`
- Nature : `declared_as_is`
- Champs source : `B3_PEAK_Z_MG`
- Offset début : `28`
- Offset fin : `29`
- Adresse début : `3028`
- Adresse fin : `3029`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Crête axe Z`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_DOMINANT_AXIS`
- Adresse de début du champ suivant attendue : `3030`
- Vérifier l'absence d'empiètement entre `B3_PEAK_Z_MG` et `B3_DOMINANT_AXIS`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `3028`.
2. Vérifier que la lecture couvre la plage `3028` à `3029` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `3030`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `B3_PEAK_Z_MG` est possible sur la plage `3028` à `3029` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `B3_DOMINANT_AXIS` commence à `3030` ;
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
