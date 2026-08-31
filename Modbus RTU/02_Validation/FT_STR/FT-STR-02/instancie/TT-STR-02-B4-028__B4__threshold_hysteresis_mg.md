# TT-STR-02-B4-028 — Bloc 4 — threshold_hysteresis_mg

## Objectif
Valider que le champ logique `threshold_hysteresis_mg` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `threshold_hysteresis_mg`
- Nature : `declared_as_is`
- Champs source : `threshold_hysteresis_mg`
- Offset début : `45`
- Offset fin : `45`
- Adresse début : `4045`
- Adresse fin : `4045`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Hystérésis`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `alarm_hold_time_ms`
- Adresse de début du champ suivant attendue : `4046`
- Vérifier l'absence d'empiètement entre `threshold_hysteresis_mg` et `alarm_hold_time_ms`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4045`.
2. Vérifier que la lecture couvre la plage `4045` à `4045` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4046`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `threshold_hysteresis_mg` est possible sur la plage `4045` à `4045` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `alarm_hold_time_ms` commence à `4046` ;
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
