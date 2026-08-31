# TT-STR-06-B4-028 — Bloc 4 — threshold_hysteresis_mg

## Objectif
Valider que le champ logique `threshold_hysteresis_mg` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `threshold_hysteresis_mg`
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
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `alarm_hold_time_ms`
- Adresse de début du champ suivant attendue : `4046`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4045`.
2. Vérifier que la lecture couvre strictement la plage `4045` à `4045`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `alarm_hold_time_ms` commence à `4046` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4045` à `4045` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `alarm_hold_time_ms` commence à `4046` ;
- aucune dépendance implicite de lecture entre `threshold_hysteresis_mg` et `alarm_hold_time_ms` n'est observée.
- aucune dépendance implicite à un découpage particulier n'est observée sur ce champ.

## Critères d’acceptation
- adresse de début conforme ;
- adresse de fin conforme ;
- taille conforme ;
- lecture valide sur la plage exacte ;
- lecture partielle valide si applicable ;
- absence d’empiètement ;
- aucune dépendance implicite au découpage.

## Classification
- Famille : `FT-STR-06`
- Sous-famille : `Accessibilité lecture`
- Niveau : `instancié`
