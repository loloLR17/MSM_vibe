# TT-STR-06-B2-006 — Bloc 2 — prepared_time

## Objectif
Valider que le champ logique `prepared_time` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `2`
- Champ logique : `prepared_time`
- Champs source : `prepared_time_msw;prepared_time_lsw`
- Offset début : `8`
- Offset fin : `9`
- Adresse début : `2008`
- Adresse fin : `2009`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RW`
- Description : `Temps préparé`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `2` accessible

## Contrôle de frontière
- Champ suivant attendu : `prepared_time_status`
- Adresse de début du champ suivant attendue : `2010`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `2008`.
2. Vérifier que la lecture couvre strictement la plage `2008` à `2009`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `2008`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `2008` à `2009`.
5. Vérifier que le champ suivant `prepared_time_status` commence à `2010` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `2008` à `2009` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `prepared_time_status` commence à `2010` ;
- aucune dépendance implicite de lecture entre `prepared_time` et `prepared_time_status` n'est observée.
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
