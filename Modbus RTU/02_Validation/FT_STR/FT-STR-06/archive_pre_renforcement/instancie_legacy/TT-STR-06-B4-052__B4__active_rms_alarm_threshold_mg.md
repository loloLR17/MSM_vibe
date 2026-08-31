# TT-STR-06-B4-052 — Bloc 4 — active_rms_alarm_threshold_mg

## Objectif
Valider que le champ logique `active_rms_alarm_threshold_mg` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `4`
- Champ logique : `active_rms_alarm_threshold_mg`
- Champs source : `active_rms_alarm_threshold_mg`
- Offset début : `118`
- Offset fin : `118`
- Adresse début : `4118`
- Adresse fin : `4118`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Miroir actif`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `4` accessible

## Contrôle de frontière
- Champ suivant attendu : `active_peak_warn_threshold_mg`
- Adresse de début du champ suivant attendue : `4119`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `4118`.
2. Vérifier que la lecture couvre strictement la plage `4118` à `4118`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `active_peak_warn_threshold_mg` commence à `4119` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `4118` à `4118` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `active_peak_warn_threshold_mg` commence à `4119` ;
- aucune dépendance implicite de lecture entre `active_rms_alarm_threshold_mg` et `active_peak_warn_threshold_mg` n'est observée.
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
