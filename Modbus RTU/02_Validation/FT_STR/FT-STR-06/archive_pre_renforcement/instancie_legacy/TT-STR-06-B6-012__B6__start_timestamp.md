# TT-STR-06-B6-012 — Bloc 6 — start_timestamp

## Objectif
Valider que le champ logique `start_timestamp` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `6`
- Champ logique : `start_timestamp`
- Champs source : `start_timestamp`
- Offset début : `16`
- Offset fin : `17`
- Adresse début : `6016`
- Adresse fin : `6017`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Début campagne`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `6` accessible

## Contrôle de frontière
- Champ suivant attendu : `end_timestamp`
- Adresse de début du champ suivant attendue : `6018`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `6016`.
2. Vérifier que la lecture couvre strictement la plage `6016` à `6017`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `6016`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `6016` à `6017`.
5. Vérifier que le champ suivant `end_timestamp` commence à `6018` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `6016` à `6017` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `end_timestamp` commence à `6018` ;
- aucune dépendance implicite de lecture entre `start_timestamp` et `end_timestamp` n'est observée.
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
