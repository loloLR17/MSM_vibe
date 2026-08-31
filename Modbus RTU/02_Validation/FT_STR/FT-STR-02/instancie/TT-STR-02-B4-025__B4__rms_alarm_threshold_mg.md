# TT-STR-02-B4-025 — Bloc 4 — rms_alarm_threshold_mg

## Objectif
Valider que le champ logique `rms_alarm_threshold_mg` est exposé au bon emplacement, avec le bon type et la bonne taille, conformément au mapping unifié.

## Référence mapping
- Bloc : `4`
- Champ logique : `rms_alarm_threshold_mg`
- Nature : `declared_as_is`
- Champs source : `rms_alarm_threshold_mg`
- Offset début : `42`
- Offset fin : `42`
- Adresse début : `4042`
- Adresse fin : `4042`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Seuil RMS alarme`

## Préconditions
- FT-STR-01 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `peak_warn_threshold_mg`
- Adresse de début du champ suivant attendue : `4043`
- Vérifier l'absence d'empiètement entre `rms_alarm_threshold_mg` et `peak_warn_threshold_mg`.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4042`.
2. Vérifier que la lecture couvre la plage `4042` à `4042` sans décalage.
3. Vérifier que la taille observée correspond exactement à `1` registre(s).
4. Vérifier que le champ logique suivant commence à l'adresse `4043`.
5. Vérifier que l'interprétation du champ reste compatible avec le type `uint16`.

## Résultat attendu
- la lecture de `rms_alarm_threshold_mg` est possible sur la plage `4042` à `4042` ;
- la taille observée est exactement de `1` registre(s) ;
- le champ est positionné conformément au mapping ;
- le champ suivant `peak_warn_threshold_mg` commence à `4043` ;
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
