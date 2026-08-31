# TT-STR-06-B3-001 — Bloc 3 — B3_STATUS_GLOBAL

## Objectif
Valider que le champ logique `B3_STATUS_GLOBAL` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_STATUS_GLOBAL`
- Champs source : `B3_STATUS_GLOBAL`
- Offset début : `0`
- Offset fin : `0`
- Adresse début : `3000`
- Adresse fin : `3000`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Statut global de supervision vibratoire`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_VALIDITY_FLAGS`
- Adresse de début du champ suivant attendue : `3001`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `3000`.
2. Vérifier que la lecture couvre strictement la plage `3000` à `3000`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `B3_VALIDITY_FLAGS` commence à `3001` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `3000` à `3000` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `B3_VALIDITY_FLAGS` commence à `3001` ;
- aucune dépendance implicite de lecture entre `B3_STATUS_GLOBAL` et `B3_VALIDITY_FLAGS` n'est observée.
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
