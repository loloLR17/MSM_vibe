# TT-STR-02-B7-009 — Bloc 7 — uptime_s

## Objectif
Valider que le champ logique `uptime_s` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `uptime_s`
- Nature : `declared_as_is`
- Champs source : `uptime_s`
- Offset début : `9`
- Offset fin : `10`
- Adresse début : `7009`
- Adresse fin : `7010`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Temps fonctionnement`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `reset_cause`
- Adresse de début du champ suivant attendue : `7011`
- Vérifier l'absence d'empiètement entre `uptime_s` et `reset_cause`.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `7009`.
2. Vérifier que la lecture couvre la plage `7009` à `7010` sans décalage.
3. Vérifier que la taille observée correspond exactement à `2` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7011`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint32`.

## Résultat attendu
- la lecture de `uptime_s` est possible sur la plage `7009` à `7010` ;
- la taille observée est exactement de `2` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reset_cause` commence à `7011` ;
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
