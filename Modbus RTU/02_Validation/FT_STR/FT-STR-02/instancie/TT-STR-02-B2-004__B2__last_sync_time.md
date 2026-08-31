# TT-STR-02-B2-004 — Bloc 2 — last_sync_time

## Objectif
Valider que le champ logique `last_sync_time` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `2`
- Champ logique : `last_sync_time`
- Nature : `uint32_from_split_words`
- Champs source : `last_sync_time_msw;last_sync_time_lsw`
- Offset début : `4`
- Offset fin : `5`
- Adresse début : `2004`
- Adresse fin : `2005`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Dernière synchronisation`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `time_since_sync_s`
- Adresse de début du champ suivant attendue : `2006`
- Vérifier l'absence d'empiètement entre `last_sync_time` et `time_since_sync_s`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `2004`.
2. Vérifier que la lecture couvre la plage `2004` à `2005` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `2006`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `last_sync_time` est possible sur la plage `2004` à `2005` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `time_since_sync_s` commence à `2006` ;
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
