# TT-STR-06-B7-005 — Bloc 7 — last_fault_timestamp

## Objectif
Valider que le champ logique `last_fault_timestamp` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `7`
- Champ logique : `last_fault_timestamp`
- Champs source : `last_fault_timestamp`
- Offset début : `4`
- Offset fin : `5`
- Adresse début : `7004`
- Adresse fin : `7005`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RO`
- Description : `Timestamp dernier défaut`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `7` accessible

## Contrôle de frontière
- Champ suivant attendu : `selftest_status`
- Adresse de début du champ suivant attendue : `7006`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `7004`.
2. Vérifier que la lecture couvre strictement la plage `7004` à `7005`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `7004`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `7004` à `7005`.
5. Vérifier que le champ suivant `selftest_status` commence à `7006` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `7004` à `7005` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `selftest_status` commence à `7006` ;
- aucune dépendance implicite de lecture entre `last_fault_timestamp` et `selftest_status` n'est observée.
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
