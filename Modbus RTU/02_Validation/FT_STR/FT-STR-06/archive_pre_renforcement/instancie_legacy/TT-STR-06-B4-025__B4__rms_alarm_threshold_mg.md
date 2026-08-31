# TT-STR-06-B4-025 — Bloc 4 — rms_alarm_threshold_mg

## Objectif
Valider que le champ logique `rms_alarm_threshold_mg` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `rms_alarm_threshold_mg`
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
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `peak_warn_threshold_mg`
- Adresse de début du champ suivant attendue : `4043`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4042`.
2. Vérifier que la lecture couvre strictement la plage `4042` à `4042`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `peak_warn_threshold_mg` commence à `4043` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4042` à `4042` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `peak_warn_threshold_mg` commence à `4043` ;
- aucune dépendance implicite de lecture entre `rms_alarm_threshold_mg` et `peak_warn_threshold_mg` n'est observée.
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
