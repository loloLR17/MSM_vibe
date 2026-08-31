# TT-STR-02-B2-005 — Bloc 2 — time_since_sync_s

## Objectif
Valider que le champ logique `time_since_sync_s` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `2`
- Champ logique : `time_since_sync_s`
- Nature : `uint32_from_split_words`
- Champs source : `time_since_sync_s_msw;time_since_sync_s_lsw`
- Offset début : `6`
- Offset fin : `7`
- Adresse début : `2006`
- Adresse fin : `2007`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Temps écoulé depuis dernière sync`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `prepared_time`
- Adresse de début du champ suivant attendue : `2008`
- Vérifier l'absence d'empiètement entre `time_since_sync_s` et `prepared_time`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `2006`.
2. Vérifier que la lecture couvre la plage `2006` à `2007` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `2008`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `time_since_sync_s` est possible sur la plage `2006` à `2007` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `prepared_time` commence à `2008` ;
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
