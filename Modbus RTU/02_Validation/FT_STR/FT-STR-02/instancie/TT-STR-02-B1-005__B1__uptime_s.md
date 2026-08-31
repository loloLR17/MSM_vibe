# TT-STR-02-B1-005 — Bloc 1 — uptime_s

## Objectif
Valider que le champ logique `uptime_s` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `1`
- Champ logique : `uptime_s`
- Nature : `uint32_from_split_words`
- Champs source : `uptime_s_msw;uptime_s_lsw`
- Offset début : `4`
- Offset fin : `5`
- Adresse début : `1004`
- Adresse fin : `1005`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Temps de fonctionnement`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `1` accessible

## Contrôle de frontière
- Champ suivant attendu : `last_reset_cause`
- Adresse de début du champ suivant attendue : `1006`
- Vérifier l'absence d'empiètement entre `uptime_s` et `last_reset_cause`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `1004`.
2. Vérifier que la lecture couvre la plage `1004` à `1005` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `1006`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `uptime_s` est possible sur la plage `1004` à `1005` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `last_reset_cause` commence à `1006` ;
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
