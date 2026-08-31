# TT-STR-06-B3-002 — Bloc 3 — B3_VALIDITY_FLAGS

## Objectif
Valider que le champ logique `B3_VALIDITY_FLAGS` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_VALIDITY_FLAGS`
- Champs source : `B3_VALIDITY_FLAGS`
- Offset début : `1`
- Offset fin : `1`
- Adresse début : `3001`
- Adresse fin : `3001`
- Type déclaré : `bitfield16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Drapeaux détaillés de validité / fraîcheur / cohérence`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_ALARM_FLAGS`
- Adresse de début du champ suivant attendue : `3002`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `3001`.
2. Vérifier que la lecture couvre strictement la plage `3001` à `3001`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `B3_ALARM_FLAGS` commence à `3002` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `3001` à `3001` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `B3_ALARM_FLAGS` commence à `3002` ;
- aucune dépendance implicite de lecture entre `B3_VALIDITY_FLAGS` et `B3_ALARM_FLAGS` n'est observée.
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
