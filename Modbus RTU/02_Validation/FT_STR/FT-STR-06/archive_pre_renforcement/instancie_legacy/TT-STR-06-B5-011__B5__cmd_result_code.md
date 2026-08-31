# TT-STR-06-B5-011 — Bloc 5 — cmd_result_code

## Objectif
Valider que le champ logique `cmd_result_code` est lisible de manière standard, sans dépendance implicite à la taille de requête ni au découpage de lecture.

## Référence mapping
- Bloc : `5`
- Champ logique : `cmd_result_code`
- Champs source : `cmd_result_code`
- Offset début : `11`
- Offset fin : `11`
- Adresse début : `5011`
- Adresse fin : `5011`
- Type déclaré : `enum16`
- Taille attendue : `1` registre(s)
- Accès : `RO`
- Description : `Code résultat détaillé`

## Préconditions
- FT-STR-01 validée
- FT-STR-02 validée
- Accès Modbus opérationnel
- Mapping unifié logique validé
- Bloc `5` accessible

## Contrôle de frontière
- Champ suivant attendu : `cmd_result_detail`
- Adresse de début du champ suivant attendue : `5012`
- Vérifier l'absence d'empiètement et la lisibilité indépendante des deux champs.

## Étapes
1. Lire exactement `1` registre(s) à partir de l'adresse `5011`.
2. Vérifier que la lecture couvre strictement la plage `5011` à `5011`.
3. Relire le registre unitaire de manière répétée avec le même adressage.
5. Vérifier que le champ suivant `cmd_result_detail` commence à `5012` et reste lisible indépendamment.
6. Vérifier que la lecture du champ reste possible sans imposer de taille de requête implicite autre que la plage demandée.

## Résultat attendu
- la lecture exacte de la plage `5011` à `5011` est possible ;
- la taille observée est exactement de `1` registre(s) ;
- la lecture unitaire est acceptée de manière répétée sans contrainte implicite ;
- le champ suivant `cmd_result_detail` commence à `5012` ;
- aucune dépendance implicite de lecture entre `cmd_result_code` et `cmd_result_detail` n'est observée.
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
