# TT-STR-06-B5-002 — Bloc 5 — cmd_request_transaction_id

## Objectif
Valider que le champ logique `cmd_request_transaction_id` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_request_transaction_id`
- Champs source : `cmd_request_transaction_id`
- Offset début : `1`
- Offset fin : `1`
- Adresse début : `5001`
- Adresse fin : `5001`
- Type déclaré : `uint16`
- Taille attendue : `1` registre(s)
- Accès : `RW`
- Description : `Identifiant de transaction de la commande`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_request_param1`
- Adresse de début du champ suivant attendue : `5002`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5001`.
2. Vérifier que la lecture couvre strictement la plage `5001` à `5001`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `cmd_request_param1` commence à `5002` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `5001` à `5001` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `cmd_request_param1` commence à `5002` ;
- aucune dépendance implicite de lecture entre `cmd_request_transaction_id` et `cmd_request_param1` n'est observée.
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
