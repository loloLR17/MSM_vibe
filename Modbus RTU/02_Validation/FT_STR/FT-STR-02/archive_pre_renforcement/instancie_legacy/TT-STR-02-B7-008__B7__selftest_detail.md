# TT-STR-02-B7-008 — Bloc 7 — selftest_detail

## Objectif
Valider que le champ logique `selftest_detail` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `7`
- Champ logique : `selftest_detail`
- Nature : `declared_as_is`
- Champs source : `selftest_detail`
- Offset début : `8`
- Offset fin : `8`
- Adresse début : `7008`
- Adresse fin : `7008`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Détail autotest`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `uptime_s`
- Adresse de début du champ suivant attendue : `7009`
- Vérifier l'absence d'empiètement entre `selftest_detail` et `uptime_s`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `7008`.
2. Vérifier que la lecture couvre la plage `7008` à `7008` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `7009`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `selftest_detail` est possible sur la plage `7008` à `7008` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `uptime_s` commence à `7009` ;
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
