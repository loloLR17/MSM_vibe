# TT-STR-02-B2-010 — Bloc 2 — sync_source

## Objectif
Valider que le champ logique `sync_source` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `2`
- Champ logique : `sync_source`
- Nature : `declared_as_is`
- Champs source : `sync_source`
- Offset début : `13`
- Offset fin : `13`
- Adresse début : `2013`
- Adresse fin : `2013`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Source de synchronisation`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_1`
- Adresse de début du champ suivant attendue : `2014`
- Vérifier l'absence d'empiètement entre `sync_source` et `reserved_1`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2013`.
2. Vérifier que la lecture couvre la plage `2013` à `2013` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `2014`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `enum16`.

## Résultat attendu
- la lecture de `sync_source` est possible sur la plage `2013` à `2013` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved_1` commence à `2014` ;
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
