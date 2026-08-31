# TT-STR-06-B6-011 — Bloc 6 — mission_id

## Objectif
Valider que le champ logique `mission_id` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `6`
- Champ logique : `mission_id`
- Champs source : `mission_id`
- Offset début : `14`
- Offset fin : `15`
- Adresse début : `6014`
- Adresse fin : `6015`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Identifiant mission`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `start_timestamp`
- Adresse de début du champ suivant attendue : `6016`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6014`.
2. Vérifier que la lecture couvre strictement la plage `6014` à `6015`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `6014`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `6014` à `6015`.
5. Vérifier que le champ suivant `start_timestamp` commence à `6016` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `6014` à `6015` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `start_timestamp` commence à `6016` ;
- aucune dépendance implicite de lecture entre `mission_id` et `start_timestamp` n'est observée.
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
