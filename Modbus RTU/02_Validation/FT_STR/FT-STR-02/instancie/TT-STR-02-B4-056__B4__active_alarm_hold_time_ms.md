# TT-STR-02-B4-056 — Bloc 4 — active_alarm_hold_time_ms

## Objectif
Valider que le champ logique `active_alarm_hold_time_ms` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_alarm_hold_time_ms`
- Nature : `declared_as_is`
- Champs source : `active_alarm_hold_time_ms`
- Offset début : `122`
- Offset fin : `122`
- Adresse début : `4122`
- Adresse fin : `4122`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Miroir actif`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `reserved_4E_B`
- Adresse de début du champ suivant attendue : `4123`
- Vérifier l'absence d'empiètement entre `active_alarm_hold_time_ms` et `reserved_4E_B`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4122`.
2. Vérifier que la lecture couvre la plage `4122` à `4122` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4123`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `active_alarm_hold_time_ms` est possible sur la plage `4122` à `4122` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `reserved_4E_B` commence à `4123` ;
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
