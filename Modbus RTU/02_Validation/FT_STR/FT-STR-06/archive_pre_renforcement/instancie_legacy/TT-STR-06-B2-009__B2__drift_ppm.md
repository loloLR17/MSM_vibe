# TT-STR-06-B2-009 — Bloc 2 — drift_ppm

## Objectif
Valider que le champ logique `drift_ppm` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `2`
- Champ logique : `drift_ppm`
- Champs source : `drift_ppm`
- Offset début : `12`
- Offset fin : `12`
- Adresse début : `2012`
- Adresse fin : `2012`
- Type déclaré : `int16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Dérive estimée (ppm)`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `sync_source`
- Adresse de début du champ suivant attendue : `2013`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2012`.
2. Vérifier que la lecture couvre strictement la plage `2012` à `2012`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `sync_source` commence à `2013` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `2012` à `2012` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `sync_source` commence à `2013` ;
- aucune dépendance implicite de lecture entre `drift_ppm` et `sync_source` n'est observée.
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
