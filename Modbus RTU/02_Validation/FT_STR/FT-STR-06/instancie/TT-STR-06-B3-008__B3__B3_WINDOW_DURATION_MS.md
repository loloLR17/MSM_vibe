# TT-STR-06-B3-008 — Bloc 3 — B3_WINDOW_DURATION_MS

## Objectif
Valider que le champ logique `B3_WINDOW_DURATION_MS` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `3`
- Champ logique : `B3_WINDOW_DURATION_MS`
- Champs source : `B3_WINDOW_DURATION_MS`
- Offset début : `10`
- Offset fin : `11`
- Adresse début : `3010`
- Adresse fin : `3011`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Durée de la fenêtre de calcul associée aux valeurs`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `3` accessible

## Contrôle de frontière
- Champ suivant attendu : `B3_VALID_SAMPLE_COUNT`
- Adresse de début du champ suivant attendue : `3012`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `3010`.
2. Vérifier que la lecture couvre strictement la plage `3010` à `3011`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `3010`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `3010` à `3011`.
5. Vérifier que le champ suivant `B3_VALID_SAMPLE_COUNT` commence à `3012` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `3010` à `3011` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `B3_VALID_SAMPLE_COUNT` commence à `3012` ;
- aucune dépendance implicite de lecture entre `B3_WINDOW_DURATION_MS` et `B3_VALID_SAMPLE_COUNT` n'est observée.
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
