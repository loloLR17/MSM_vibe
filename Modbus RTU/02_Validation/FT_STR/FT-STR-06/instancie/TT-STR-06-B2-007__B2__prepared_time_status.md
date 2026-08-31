# TT-STR-06-B2-007 — Bloc 2 — prepared_time_status

## Objectif
Valider que le champ logique `prepared_time_status` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `2`
- Champ logique : `prepared_time_status`
- Champs source : `prepared_time_status`
- Offset début : `10`
- Offset fin : `10`
- Adresse début : `2010`
- Adresse fin : `2010`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `État du temps préparé`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `time_accuracy_ms`
- Adresse de début du champ suivant attendue : `2011`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `2010`.
2. Vérifier que la lecture couvre strictement la plage `2010` à `2010`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `time_accuracy_ms` commence à `2011` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `2010` à `2010` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `time_accuracy_ms` commence à `2011` ;
- aucune dépendance implicite de lecture entre `prepared_time_status` et `time_accuracy_ms` n'est observée.
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
