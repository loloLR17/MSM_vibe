# TT-STR-06-B5-001 — Bloc 5 — cmd_request_code

## Objectif
Valider que le champ logique `cmd_request_code` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_request_code`
- Champs source : `cmd_request_code`
- Offset début : `0`
- Offset fin : `0`
- Adresse début : `5000`
- Adresse fin : `5000`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Code commande demandé par la centrale`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_request_transaction_id`
- Adresse de début du champ suivant attendue : `5001`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5000`.
2. Vérifier que la lecture couvre strictement la plage `5000` à `5000`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `cmd_request_transaction_id` commence à `5001` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `5000` à `5000` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `cmd_request_transaction_id` commence à `5001` ;
- aucune dépendance implicite de lecture entre `cmd_request_code` et `cmd_request_transaction_id` n'est observée.
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
