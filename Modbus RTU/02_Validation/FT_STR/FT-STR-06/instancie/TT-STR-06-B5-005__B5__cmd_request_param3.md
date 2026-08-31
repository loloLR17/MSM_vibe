# TT-STR-06-B5-005 — Bloc 5 — cmd_request_param3

## Objectif
Valider que le champ logique `cmd_request_param3` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_request_param3`
- Champs source : `cmd_request_param3_msw;cmd_request_param3_lsw`
- Offset début : `4`
- Offset fin : `5`
- Adresse début : `5004`
- Adresse fin : `5005`
- Type déclaré : `uint32`
- Taille attendue : `2` registre(s)
- Accès : `RW`
- Description : `Paramètre 3, mot fort`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_request_confirm_key`
- Adresse de début du champ suivant attendue : `5006`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `2` registre(s) à partir de l'adresse `5004`.
2. Vérifier que la lecture couvre strictement la plage `5004` à `5005`.
3. Lire partiellement le champ sur sa première sous-plage valide, à partir de `5004`, avec une longueur strictement inférieure à `2`.
4. Relire ensuite le champ complet de `5004` à `5005`.
5. Vérifier que le champ suivant `cmd_request_confirm_key` commence à `5006` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `5004` à `5005` est possible ;
- la taille observée est exactement de `2` registre(s) ;
- la lecture partielle est acceptée sur toute sous-plage valide ;
- la lecture complète reste cohérente après lecture partielle ;
- le champ suivant `cmd_request_confirm_key` commence à `5006` ;
- aucune dépendance implicite de lecture entre `cmd_request_param3` et `cmd_request_confirm_key` n'est observée.
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
