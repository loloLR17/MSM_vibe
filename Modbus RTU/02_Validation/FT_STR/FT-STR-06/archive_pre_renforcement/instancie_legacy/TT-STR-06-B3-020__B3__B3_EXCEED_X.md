# TT-STR-06-B3-020 — Bloc 3 — B3_EXCEED_X

## Objectif
Valider que le champ logique `B3_EXCEED_X` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_EXCEED_X`
- Champs source : `B3_EXCEED_X`
- Offset début : `32`
- Offset fin : `32`
- Adresse début : `3032`
- Adresse fin : `3032`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Dépassement courant axe X`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_EXCEED_Y`
- Adresse de début du champ suivant attendue : `3033`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `3032`.
2. Vérifier que la lecture couvre strictement la plage `3032` à `3032`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `B3_EXCEED_Y` commence à `3033` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `3032` à `3032` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `B3_EXCEED_Y` commence à `3033` ;
- aucune dépendance implicite de lecture entre `B3_EXCEED_X` et `B3_EXCEED_Y` n'est observée.
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
