# TT-STR-02-B2-003 — Bloc 2 — current_time

## Objectif
Valider que le champ logique `current_time` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `2`
- Champ logique : `current_time`
- Nature : `uint32_from_split_words`
- Champs source : `current_time_msw;current_time_lsw`
- Offset début : `2`
- Offset fin : `3`
- Adresse début : `2002`
- Adresse fin : `2003`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Temps courant`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `last_sync_time`
- Adresse de début du champ suivant attendue : `2004`
- Vérifier l'absence d'empiètement entre `current_time` et `last_sync_time`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `2002`.
2. Vérifier que la lecture couvre la plage `2002` à `2003` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `2004`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `current_time` est possible sur la plage `2002` à `2003` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `last_sync_time` commence à `2004` ;
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
